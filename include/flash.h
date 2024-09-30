#ifndef FLASH_H
#define FLASH_H

#include <SPI.h>
#include <SPIMemory.h>

#define FLASH_CS_PIN 15    // W25Q80��Ƭѡ����
// ����û�������ÿ���û���¼��С
const int USER_RECORD_SIZE = 20;  // ÿ���û��ļ�¼��С���û���(16�ֽ�) + UID(4�ֽ�)
const int MAX_USERS = 255;        // ���洢255���û�

// ��������
void initFlash();
void addUserToFlash(const char* username, const byte* uid);
void deleteUserFromFlash(const byte* uid);
bool verifyUserFromFlash(const byte* uid);
void printUsersInFlash();

#endif // FLASH_H
