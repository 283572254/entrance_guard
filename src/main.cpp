// ���������
#include <SPI.h>
#include <MFRC522.h>

#define RELAY_CON 5

// ����RC522ģ�������
#define SS_PIN    21 // SDA
#define RST_PIN   22 // RST

// ����MFRC522����
MFRC522 mfrc522(SS_PIN, RST_PIN);

// ����Ŀ�������Ϳ��
int sector = 1;
int blockAddr = 4; // ��4�ǵ�һ���������ݿ�

// ��ԿA
MFRC522::MIFARE_Key keyA;


//�û�A
int userA[] = {0x32, 0x30, 0x31, 0x39, 0x31, 0x30, 0x30, 0x38, 0x38, 0x30, 0x34, 0x37, 0x00, 0x1C, 0xFC, 0x3F};
void setup() {
  pinMode(RELAY_CON, OUTPUT);
  Serial.begin(115200); // ��ʼ������ͨ��
  SPI.begin(); // ��ʼ��SPI����
  mfrc522.PCD_Init(); // ��ʼ��MFRC522ģ��
  
  // ��ʼ����ԿA��ȫΪ0xFF��
  for (byte i = 0; i < 6; i++) {
    keyA.keyByte[i] = 0xFF;
  }
  
  Serial.println("RFID��д��������ʼ����� - ����i");
}

void readRFID() {
  // ����Ƿ����µ�RFID��ǩ
  if (!mfrc522.PICC_IsNewCardPresent()) {
    Serial.println("δ��⵽�±�ǩ");
    return;
  }

  // ѡ������һ��RFID��ǩ
  if (!mfrc522.PICC_ReadCardSerial()) {
    Serial.println("��ȡ��ǩʧ��");
    return;
  }

  // ��ӡ��ǩ��UID
  Serial.print("��ǩUID: ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();

  // ��ָ֤������
  MFRC522::StatusCode status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockAddr, &keyA, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("��֤ʧ��: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  // ��ȡ��������
  byte buffer[18];
  byte size = sizeof(buffer);
  status = mfrc522.MIFARE_Read(blockAddr, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("��ȡʧ��: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  // �޸Ĳ��֣������������Ϊ������ʽ
  Serial.print("��������: {");
  for (byte i = 0; i < 16; i++) {
    Serial.print("0x");
    if (buffer[i] < 0x10) Serial.print("0"); // ����
    Serial.print(buffer[i], HEX);
    if (i != 15) {
      Serial.print(", ");
    }
  }
  Serial.println("}");

  // ֹͣ���ǩ��ͨ��
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

void writeRFID(String data) {
  // ����Ƿ����µ�RFID��ǩ
  if (!mfrc522.PICC_IsNewCardPresent()) {
    Serial.println("δ��⵽�±�ǩ");
    return;
  }

  // ѡ������һ��RFID��ǩ
  if (!mfrc522.PICC_ReadCardSerial()) {
    Serial.println("��ȡ��ǩʧ��");
    return;
  }

  // ������ת��Ϊ�ֽ����飨���16�ֽڣ�
  byte dataBlock[16];
  data.getBytes(dataBlock, 16);

  // ��ָ֤������
  MFRC522::StatusCode status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockAddr, &keyA, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("��֤ʧ��: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  // д�����ݵ�ָ����
  status = mfrc522.MIFARE_Write(blockAddr, dataBlock, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("д��ʧ��: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
  } else {
    Serial.println("����д��ɹ���");
  }

  // ֹͣ���ǩ��ͨ��
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    Serial.print("�յ�����: ");
    Serial.println(command);
   
    if (command == "READ") {
      readRFID();
    } else if (command.startsWith("WRITE ")) {
      String data = command.substring(6);
      writeRFID(data);
    } else {
      Serial.println("��Ч����");
    }
    
    // ����ȷ����Ϣ
    Serial.println("����ִ����� - ����i");
  }
}
