/* *****************************************************************
 *
 * Download latest Blinker library here:
 * https://github.com/blinker-iot/blinker-library/archive/master.zip
 * 
 * 
 * Blinker is a cross-hardware, cross-platform solution for the IoT. 
 * It provides APP, device and server support, 
 * and uses public cloud services for data transmission and storage.
 * It can be used in smart home, data monitoring and other fields 
 * to help users build Internet of Things projects better and faster.
 * 
 * Make sure installed 2.7.4 or later ESP8266/Arduino package,
 * if use ESP8266 with Blinker.
 * https://github.com/esp8266/Arduino/releases
 * 
 * Make sure installed 1.0.5 or later ESP32/Arduino package,
 * if use ESP32 with Blinker.
 * https://github.com/espressif/arduino-esp32/releases
 * 
 * Docs: https://diandeng.tech/doc
 *       
 * 
 * *****************************************************************
 * 
 * Blinker �����ص�ַ:
 * https://github.com/blinker-iot/blinker-library/archive/master.zip
 * 
 * Blinker ��һ�׿�Ӳ������ƽ̨������������������ṩAPP�ˡ��豸�ˡ�
 * ��������֧�֣�ʹ�ù����Ʒ���������ݴ���洢�����������ܼҾӡ�
 * ���ݼ������򣬿��԰����û����ø���ش��������Ŀ��
 * 
 * ���ʹ�� ESP8266 ���� Blinker,
 * ��ȷ����װ�� 2.7.4 ����µ� ESP8266/Arduino ֧�ְ���
 * https://github.com/esp8266/Arduino/releases
 * 
 * ���ʹ�� ESP32 ���� Blinker,
 * ��ȷ����װ�� 1.0.5 ����µ� ESP32/Arduino ֧�ְ���
 * https://github.com/espressif/arduino-esp32/releases
 * 
 * �ĵ�: https://diandeng.tech/doc
 *       
 * 
 * *****************************************************************/
#include <Arduino.h>
#include "serial_bluetooth.h"
#include "rfid.h"
#include "flash.h"
char auth[] = "9bf44e098d3f";
char ssid[] = "507";
char pswd[] = "507507507507";

#define BLINKER_PRINT Serial
#define BLINKER_WIFI

#include <Blinker.h>



// �½��������
BlinkerButton Button1("Button1");
BlinkerNumber numInput("num-input");           // ����� (���ڽ����û�����)
BlinkerButton btnAddUser("btn-adduser");       // ����û���ť
BlinkerButton btnDelUser("btn-deluser");       // ɾ���û���ť
BlinkerButton btnQuickRecord("btn-record");    // ����¼�밴ť
BlinkerText txtFeedback("txt-feedback");       // �����ı�

int counter = 0;

// ���°�������ִ�иú���
void button1_callback(const String & state) {
    BLINKER_LOG("get button state: ", state);
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
}

// ���δ�󶨵���������������ִ����������
void dataRead(const String & data) {
    String command = data;  // �������� Blinker ����������
    handleCommand(command);  // �� Blinker �����ݴ��ݸ��������
    
    // ��� command �� data
    command = "";  // ���������ַ���
    // ע�⣺data �Ǵ���ĳ������ã���˲���ֱ���޸ġ�
}


void setup() {
    // ��ʼ������
    Serial.begin(115200);

    #if defined(BLINKER_PRINT)
        BLINKER_DEBUG.stream(BLINKER_PRINT);
    #endif
    
    // ��ʼ����LED��IO
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    // ��ʼ��blinker
    Blinker.begin(auth, ssid, pswd);
    Blinker.attachData(dataRead);
    Button1.attach(button1_callback);
}

void loop() {
    Blinker.run();
}
