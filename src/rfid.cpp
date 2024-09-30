#include "rfid.h"


MFRC522 mfrc522(SS_PIN, RST_PIN);
// ��ʼ��RFIDģ��
void initRFID() {
    mfrc522.PCD_Init();
    Serial.println("RFIDģ���ʼ�����");
}

// ��ȡIC��UID
bool readCardUID(byte* uidBuffer) {
    // ����Ƿ����µ� RFID ��Ƭ
    if (!mfrc522.PICC_IsNewCardPresent()) {
        return false;
    }

    // ���Զ�ȡ��Ƭ���кţ�UID��
    if (!mfrc522.PICC_ReadCardSerial()) {
        return false;
    }

    // �� UID ���Ƶ��ṩ�Ļ�����������ӡΪʮ�����Ƹ�ʽ
    Serial.print("��Ƭ UID: ");
    String uidStr = "";  // ���ڴ洢 UID �ַ���
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        uidBuffer[i] = mfrc522.uid.uidByte[i];
        
        // ��ÿ���ֽ�ת��Ϊ��λʮ�����Ʋ�ƴ��
        if (mfrc522.uid.uidByte[i] < 0x10) {
            uidStr += "0";  // ��֤����λ�������㣩
        }
        uidStr += String(mfrc522.uid.uidByte[i], HEX);  // ת��Ϊʮ�������ַ���
    }

    // ��ӡ UID ����
    Serial.println(uidStr);

    return true;
}
