#include "rfid.h"


MFRC522 mfrc522(SS_PIN, RST_PIN);
// 初始化RFID模块
void initRFID() {
    mfrc522.PCD_Init();
    Serial.println("RFID模块初始化完成");
}

// 读取IC卡UID
bool readCardUID(byte* uidBuffer) {
    // 检查是否有新的 RFID 卡片
    if (!mfrc522.PICC_IsNewCardPresent()) {
        return false;
    }

    // 尝试读取卡片序列号（UID）
    if (!mfrc522.PICC_ReadCardSerial()) {
        return false;
    }

    // 将 UID 复制到提供的缓冲区，并打印为十六进制格式
    Serial.print("卡片 UID: ");
    String uidStr = "";  // 用于存储 UID 字符串
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        uidBuffer[i] = mfrc522.uid.uidByte[i];
        
        // 将每个字节转换为两位十六进制并拼接
        if (mfrc522.uid.uidByte[i] < 0x10) {
            uidStr += "0";  // 保证是两位数（补零）
        }
        uidStr += String(mfrc522.uid.uidByte[i], HEX);  // 转换为十六进制字符串
    }

    // 打印 UID 数据
    Serial.println(uidStr);

    return true;
}
