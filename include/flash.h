#ifndef FLASH_H
#define FLASH_H

#include <SPI.h>
#include <SPIMemory.h>

#define FLASH_CS_PIN 15    // W25Q80的片选引脚
// 最大用户数量和每个用户记录大小
const int USER_RECORD_SIZE = 20;  // 每个用户的记录大小：用户名(16字节) + UID(4字节)
const int MAX_USERS = 255;        // 最多存储255个用户

// 函数声明
void initFlash();
void addUserToFlash(const char* username, const byte* uid);
void deleteUserFromFlash(const byte* uid);
bool verifyUserFromFlash(const byte* uid);
void printUsersInFlash();

#endif // FLASH_H
