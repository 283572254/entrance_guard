#ifndef RFID_H
#define RFID_H

#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN    21       // RC522��SDA���ţ�Ƭѡ��
#define RST_PIN   22       // RC522��RST����
// ��������
void initRFID();
bool readCardUID(byte* uidBuffer);

#endif // RFID_H
