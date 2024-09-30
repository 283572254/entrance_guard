#include <Arduino.h>
#include "flash.h"
#include "rfid.h"
#include "serial_bluetooth.h"
#define RELAY_CON 5        // �̵�����������
bool isAdminMode = false;  // �Ƿ��ڹ�����ģʽ

// ����û����������Ƿ���ȷ


// ��ʼ������
void setup() {
    Serial.begin(115200);     // ��ʼ������
    SPI.begin();              // ��ʼ��SPI����

    initRFID();               // ��ʼ��RFIDģ��
    initFlash();              // ��ʼ��Flashģ��
    initBluetooth();          // ��ʼ������ģ��
}

// ��ѭ����������������Ž��߼�
void loop() {
    byte currentUID[10];  // ��ǰ��ȡ��UID

    // �������ߴ�������
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        handleCommand(input);
    }

    // ����������������
    if (SerialBT.available()) {
        String input = SerialBT.readStringUntil('\n');
        handleCommand(input);
    }

    // ��ȡRFID��Ƭ����֤�û�
    if (readCardUID(currentUID)) {
        if (verifyUserFromFlash(currentUID)) {
            sendFeedback("�û���֤�ɹ����򿪼̵���");
            digitalWrite(RELAY_CON, HIGH);  // �򿪼̵���
            delay(5000);                    // �ӳ�5���رռ̵���
            digitalWrite(RELAY_CON, LOW);   // �رռ̵���
        } else {
            sendFeedback("�û���֤ʧ��");
        }
    }

    delay(100);  // �ӳ�1��
}
