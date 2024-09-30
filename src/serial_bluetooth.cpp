#include "serial_bluetooth.h"
#include "flash.h"  // 需要访问Flash函数
#include "rfid.h"   // 需要访问RFID函数

BluetoothSerial SerialBT;
extern SPIFlash flash;
extern MFRC522 mfrc522;
// 定义快速录取的最大等待时间（1分钟 = 60秒）
#define RECORD_TIMEOUT 60000  // 60000 毫秒 = 1分钟

// 初始化蓝牙
void initBluetooth() {
    SerialBT.begin("ESP32-RFID-Manager");  // 设置蓝牙设备名称
    Serial.println("蓝牙已初始化，设备名称为：ESP32-RFID-Manager");
}
// 检查用户名是否已经存在
bool isUsernameExists(const char* username) {
    for (int i = 0; i < MAX_USERS; i++) {
        uint32_t address = i * USER_RECORD_SIZE;
        char storedUsername[17] = {0};  // 16字节用户名 + 1字节结尾

        // 从Flash中读取用户名
        flash.readByteArray(address, (byte*)storedUsername, 16);
        if (strcmp(storedUsername, username) == 0) {
            return true;  // 用户名已存在
        }
    }
    return false;
}

// 检查UID是否已经存在
bool isUIDExists(const byte* uid) {
    for (int i = 0; i < MAX_USERS; i++) {
        uint32_t address = i * USER_RECORD_SIZE;
        byte storedUID[4];

        // 从Flash中读取UID
        flash.readByteArray(address + 16, storedUID, 4);
        if (memcmp(storedUID, uid, 4) == 0) {
            return true;  // UID已存在
        }
    }
    return false;
}
// 快速录取模式
void quickRecord() {
    sendFeedback("进入快速录取模式，请在1分钟内将IC卡放置在读卡器上...");

    unsigned long startTime = millis();  // 记录开始时间
    byte uid[10];
    bool cardRecorded = false;
    bool waitingForUserInput = false;
    String username;

    // 循环等待1分钟或直到卡片录取成功
    while (millis() - startTime < RECORD_TIMEOUT) {
        // 如果还没有进入等待用户输入阶段，则继续检测卡片
        if (!waitingForUserInput && readCardUID(uid)) {
            String uidStr = "";  // 将UID转换为字符串
            for (int i = 0; i < mfrc522.uid.size; i++) {
                if (mfrc522.uid.uidByte[i] < 0x10) uidStr += "0";
                uidStr += String(mfrc522.uid.uidByte[i], HEX);
            }

            // 发送卡片UID
            sendFeedback("检测到IC卡，UID: " + uidStr);
            cardRecorded = true;

            // 进入等待用户数据录入阶段
            sendFeedback("请输入用户名进行录取：");
            waitingForUserInput = true;  // 设置标志表示现在在等待用户名输入
        }

        // 非阻塞方式检查用户输入
        if (waitingForUserInput) {
            if (Serial.available()) {
                username = Serial.readStringUntil('\n');  // 从串口读取
                waitingForUserInput = false;  // 读取后退出输入等待模式
            } else if (SerialBT.available()) {
                username = SerialBT.readStringUntil('\n');  // 从蓝牙读取
                waitingForUserInput = false;  // 读取后退出输入等待模式
            }

            if (!waitingForUserInput) {
                // 输入结束后，检查用户名或UID是否重复
                char usernameArray[16];
                username.toCharArray(usernameArray, 16);
                if (isUsernameExists(usernameArray)) {
                    sendFeedback("用户名已存在，录取失败！");
                } else if (isUIDExists(uid)) {
                    sendFeedback("UID已存在，录取失败！");
                } else {
                    addUserToFlash(usernameArray, uid);  // 录入用户信息
                    sendFeedback("用户 " + username + " 录取成功！");
                }
                break;  // 卡片录取后退出循环
            }
        }

        delay(100);  // 每隔100毫秒检测一次是否有新卡片
    }

    if (!cardRecorded) {
        sendFeedback("录取超时，1分钟内未检测到任何IC卡，录取模式结束。");
    }
}




// 处理串口命令
// 处理串口命令
void handleCommand(String input) {
    if (input.startsWith("ADDUSER ")) {
        int commaIndex = input.indexOf(',');
        if (commaIndex > 0 && input.length() > commaIndex + 1) {
            String newUsername = input.substring(8, commaIndex);
            String newUID = input.substring(commaIndex + 1);

            // 检查UID格式是否有效（应为8个16进制字符）
            if (newUID.length() != 8) {
                sendFeedback("无效的UID格式！应为8个16进制字符。");
                return;
            }

            char username[16];
            newUsername.toCharArray(username, 16);
            byte uid[4];

            // 将UID转换为字节数组
            for (int i = 0; i < 4; i++) {
                uid[i] = (byte)strtoul(newUID.substring(i * 2, i * 2 + 2).c_str(), NULL, 16);
            }

            // 检查用户名或UID是否重复
            if (isUsernameExists(username)) {
                sendFeedback("用户名已存在，无法添加！");
            } else if (isUIDExists(uid)) {
                sendFeedback("UID已存在，无法添加！");
            } else {
                addUserToFlash(username, uid);
                sendFeedback("用户 " + newUsername + " 添加成功！");
            }
        } else {
            sendFeedback("无效的命令格式！应为：ADDUSER <用户名>,<UID>");
        }
    } else if (input.startsWith("DELUSER ")) {
        String uidStr = input.substring(8);

        // 检查UID格式是否有效（应为8个16进制字符）
        if (uidStr.length() != 8) {
            sendFeedback("无效的UID格式！应为8个16进制字符。");
            return;
        }

        byte uid[4];
        for (int i = 0; i < 4; i++) {
            uid[i] = (byte)strtoul(uidStr.substring(i * 2, i * 2 + 2).c_str(), NULL, 16);
        }

        deleteUserFromFlash(uid);
    } else if (input.startsWith("PRINTUSERS")) {
        printUsersInFlash();
    } else if (input.startsWith("RECORD")) {
        // 启动快速录取模式
        quickRecord();
    } else {
        sendFeedback("无效的命令！");
    }
}


// 通用反馈函数
void sendFeedback(String message) {
    // 发送到有线串口
    Serial.println(message);

    // 如果蓝牙已连接，则发送到蓝牙串口
    if (SerialBT.hasClient()) {
        SerialBT.println(message);
    }
}
