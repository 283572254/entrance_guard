#include <Arduino.h>
#include "flash.h"
#include "rfid.h"
#include "serial_bluetooth.h"
#include <Servo.h>
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
    Servo_Init(PWM_CHANNEL,PWM_FREQUENCY,PWM_RESOLUTION,SERVO_PIN);              // ��ʼ�����
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
            Servo_control(PWM_CHANNEL,90);  // ���ƶ��(PWM_CHANNELΪͨ����
            digitalWrite(RELAY_CON, HIGH);  // �򿪼̵���
            delay(5000);                    // �ӳ�5���رռ̵���
            digitalWrite(RELAY_CON, LOW);   // �رռ̵���
            Servo_control(PWM_CHANNEL,0);  // ���ƶ��(PWM_CHANNELΪͨ����
        } else {
            sendFeedback("�û���֤ʧ��");
        }
    }
    
    delay(100);  // �ӳ�1��
}
