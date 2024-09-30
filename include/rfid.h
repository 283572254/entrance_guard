#ifndef RFID_H
#define RFID_H

#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN    21       // RC522的SDA引脚（片选）
#define RST_PIN   22       // RC522的RST引脚
// 函数声明
void initRFID();
bool readCardUID(byte* uidBuffer);

#endif // RFID_H
