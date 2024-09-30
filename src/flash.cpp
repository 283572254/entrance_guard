#include "flash.h"
#include "serial_bluetooth.h"  // 为了反馈消息

SPIFlash flash(FLASH_CS_PIN);

// 初始化Flash模块
void initFlash() {
    if (flash.begin()) {
        sendFeedback("W25Q80 Flash初始化成功");
    } else {
        sendFeedback("W25Q80 Flash初始化失败");
        while (1);  // 如果初始化失败，停止运行
    }
}

// 添加用户到Flash存储
void addUserToFlash(const char* username, const byte* uid) {
    for (int i = 0; i < MAX_USERS; i++) {
        uint32_t address = i * USER_RECORD_SIZE;
        byte tempUID[4];
        flash.readByteArray(address + 16, tempUID, 4);

        if (tempUID[0] == 0xFF) {  // 如果UID为空，表示这个位置未使用
            flash.writeByteArray(address, (byte*)username, 16);  // 写入用户名
            flash.writeByteArray(address + 16, (byte*)uid, 4);   // 写入UID
            sendFeedback("用户添加成功！");
            return;
        }
    }
    sendFeedback("用户存储已满，无法添加更多用户！");
}

// 删除用户
void deleteUserFromFlash(const byte* uid) {
    for (int i = 0; i < MAX_USERS; i++) {
        uint32_t address = i * USER_RECORD_SIZE;
        byte tempUID[4];

        // 读取用户的 UID
        flash.readByteArray(address + 16, tempUID, 4);

        // 检查 UID 是否匹配
        if (memcmp(tempUID, uid, 4) == 0) {  // 找到匹配的UID
            // 计算这个用户数据所在的扇区地址
            uint32_t sectorStartAddress = (address / 4096) * 4096;  // 4096字节为一个扇区
            
            // 创建一个缓冲区用于存储整个扇区的数据
            byte sectorData[4096];  // 扇区大小通常是4KB
            flash.readByteArray(sectorStartAddress, sectorData, 4096);  // 读取整个扇区

            // 在缓冲区中找到对应用户的记录位置，并将其清空为0xFF
            memset(&sectorData[address % 4096], 0xFF, USER_RECORD_SIZE);

            // 擦除该扇区
            if (flash.eraseSector(sectorStartAddress / 4096)) {  // 擦除整个扇区
                // 擦除成功后，重新将修改后的扇区数据写回
                if (flash.writeByteArray(sectorStartAddress, sectorData, 4096)) {
                    sendFeedback("用户删除成功！");
                } else {
                    sendFeedback("用户删除失败，无法写回扇区数据！");
                }
            } else {
                sendFeedback("用户删除失败，无法擦除扇区！");
            }

            // 验证清除是否成功
            byte verifyBuffer[USER_RECORD_SIZE];
            flash.readByteArray(address, verifyBuffer, USER_RECORD_SIZE);
            bool cleared = true;
            for (int j = 0; j < USER_RECORD_SIZE; j++) {
                if (verifyBuffer[j] != 0xFF) {
                    cleared = false;
                    break;
                }
            }

            // 检查清除是否成功
            if (cleared) {
                sendFeedback("用户删除成功！");
            } else {
                sendFeedback("用户删除失败，未能清除数据！");
            }

            delay(50);  // 延迟以防止过度操作
            return;
        }
    }
    sendFeedback("未找到匹配的用户！");
}




// 验证UID是否存在于Flash中
bool verifyUserFromFlash(const byte* uid) {
    for (int i = 0; i < MAX_USERS; i++) {
        uint32_t address = i * USER_RECORD_SIZE;
        byte tempUID[4];
        flash.readByteArray(address + 16, tempUID, 4);

        if (memcmp(tempUID, uid, 4) == 0) {  // 找到匹配的UID
            return true;
        }
    }
    return false;
}

// 打印所有用户
// 打印所有用户
// 打印所有用户
void printUsersInFlash() {
    sendFeedback("当前存储的用户:");

    for (int i = 0; i < MAX_USERS; i++) {
        uint32_t address = i * USER_RECORD_SIZE;
        char username[17] = {0};  // 16字节用户名 + 1字节结尾
        byte uid[4];

        // 读取用户名和UID
        flash.readByteArray(address, (byte*)username, 16);
        flash.readByteArray(address + 16, uid, 4);

        // 检查是否为有效的 UID（排除全 0xFF 的 UID）
        bool isValidUID = false;
        for (int j = 0; j < 4; j++) {
            if (uid[j] != 0xFF) {
                isValidUID = true;
                break;
            }
        }

        // 如果 UID 有效，则输出用户信息
        if (isValidUID) {
            String message = "用户名: " + String(username) + ", UID: ";
            for (int j = 0; j < 4; j++) {
                // 确保以两位十六进制形式输出
                if (uid[j] < 0x10) message += "0";
                message += String(uid[j], HEX);
                if (j < 3) message += ":";
            }
            sendFeedback(message);

            // 为每个用户输出后增加一点延迟，避免系统负载过大
            delay(50);
        }
    }
    sendFeedback("用户列表打印完毕");
}
