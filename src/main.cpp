// 导入所需库
#include <SPI.h>
#include <MFRC522.h>

#define RELAY_CON 5

// 定义RC522模块的引脚
#define SS_PIN    21 // SDA
#define RST_PIN   22 // RST

// 创建MFRC522对象
MFRC522 mfrc522(SS_PIN, RST_PIN);

// 定义目标扇区和块号
int sector = 1;
int blockAddr = 4; // 块4是第一扇区的数据块

// 密钥A
MFRC522::MIFARE_Key keyA;


//用户A
int userA[] = {0x32, 0x30, 0x31, 0x39, 0x31, 0x30, 0x30, 0x38, 0x38, 0x30, 0x34, 0x37, 0x00, 0x1C, 0xFC, 0x3F};
void setup() {
  pinMode(RELAY_CON, OUTPUT);
  Serial.begin(115200); // 初始化串口通信
  SPI.begin(); // 初始化SPI总线
  mfrc522.PCD_Init(); // 初始化MFRC522模块
  
  // 初始化密钥A（全为0xFF）
  for (byte i = 0; i < 6; i++) {
    keyA.keyByte[i] = 0xFF;
  }
  
  Serial.println("RFID读写控制器初始化完成 - 羡林i");
}

void readRFID() {
  // 检查是否有新的RFID标签
  if (!mfrc522.PICC_IsNewCardPresent()) {
    Serial.println("未检测到新标签");
    return;
  }

  // 选择其中一个RFID标签
  if (!mfrc522.PICC_ReadCardSerial()) {
    Serial.println("读取标签失败");
    return;
  }

  // 打印标签的UID
  Serial.print("标签UID: ");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
    Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();

  // 认证指定扇区
  MFRC522::StatusCode status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockAddr, &keyA, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("认证失败: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  // 读取扇区数据
  byte buffer[18];
  byte size = sizeof(buffer);
  status = mfrc522.MIFARE_Read(blockAddr, buffer, &size);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("读取失败: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  // 修改部分：输出扇区数据为数组形式
  Serial.print("扇区数据: {");
  for (byte i = 0; i < 16; i++) {
    Serial.print("0x");
    if (buffer[i] < 0x10) Serial.print("0"); // 补零
    Serial.print(buffer[i], HEX);
    if (i != 15) {
      Serial.print(", ");
    }
  }
  Serial.println("}");

  // 停止与标签的通信
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

void writeRFID(String data) {
  // 检查是否有新的RFID标签
  if (!mfrc522.PICC_IsNewCardPresent()) {
    Serial.println("未检测到新标签");
    return;
  }

  // 选择其中一个RFID标签
  if (!mfrc522.PICC_ReadCardSerial()) {
    Serial.println("读取标签失败");
    return;
  }

  // 将数据转化为字节数组（最多16字节）
  byte dataBlock[16];
  data.getBytes(dataBlock, 16);

  // 认证指定扇区
  MFRC522::StatusCode status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, blockAddr, &keyA, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.print("认证失败: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
    return;
  }

  // 写入数据到指定块
  status = mfrc522.MIFARE_Write(blockAddr, dataBlock, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.print("写入失败: ");
    Serial.println(mfrc522.GetStatusCodeName(status));
  } else {
    Serial.println("数据写入成功！");
  }

  // 停止与标签的通信
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}

void loop() {
  if (Serial.available() > 0) {
    String command = Serial.readStringUntil('\n');
    Serial.print("收到命令: ");
    Serial.println(command);
   
    if (command == "READ") {
      readRFID();
    } else if (command.startsWith("WRITE ")) {
      String data = command.substring(6);
      writeRFID(data);
    } else {
      Serial.println("无效命令");
    }
    
    // 发送确认消息
    Serial.println("命令执行完毕 - 羡林i");
  }
}
