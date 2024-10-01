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
 * Blinker 库下载地址:
 * https://github.com/blinker-iot/blinker-library/archive/master.zip
 * 
 * Blinker 是一套跨硬件、跨平台的物联网解决方案，提供APP端、设备端、
 * 服务器端支持，使用公有云服务进行数据传输存储。可用于智能家居、
 * 数据监测等领域，可以帮助用户更好更快地搭建物联网项目。
 * 
 * 如果使用 ESP8266 接入 Blinker,
 * 请确保安装了 2.7.4 或更新的 ESP8266/Arduino 支持包。
 * https://github.com/esp8266/Arduino/releases
 * 
 * 如果使用 ESP32 接入 Blinker,
 * 请确保安装了 1.0.5 或更新的 ESP32/Arduino 支持包。
 * https://github.com/espressif/arduino-esp32/releases
 * 
 * 文档: https://diandeng.tech/doc
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
#include <Servo.h>


// 新建组件对象
BlinkerButton Button1("Button1");
BlinkerNumber numInput("num-input");           // 输入框 (用于接收用户输入)
BlinkerButton btnAddUser("btn-adduser");       // 添加用户按钮
BlinkerButton btnDelUser("btn-deluser");       // 删除用户按钮
BlinkerButton btnQuickRecord("btn-record");    // 快速录入按钮
BlinkerText txtFeedback("txt-feedback");       // 反馈文本

int counter = 0;
bool door_flag = 0;
// 按下按键即会执行该函数
void button1_callback(const String & state) {
    door_flag = 1;
    if(door_flag==1)
    {
        Servo_control(PWM_CHANNEL,90);
    }

    BLINKER_LOG("get button state: ", state);
    BLINKER_LOG("get door_flag state: ", door_flag);
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

}

// 如果未绑定的组件被触发，则会执行其中内容
void dataRead(const String & data) {
    // 打印接收到的数据，用于调试
    Serial.println("Received data: " + data);
    String blinkr_command = data;
    handleCommand(blinkr_command);  // 处理命令
}

void setup() {
    // 初始化串口
    Serial.begin(115200);

    #if defined(BLINKER_PRINT)
        BLINKER_DEBUG.stream(BLINKER_PRINT);
    #endif
    
    // 初始化有LED的IO
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    // 初始化blinker
    Blinker.begin(auth, ssid, pswd);
    Blinker.attachData(dataRead);
    Button1.attach(button1_callback);
    Servo_Init(PWM_CHANNEL,PWM_FREQUENCY,PWM_RESOLUTION,SERVO_PIN);
}

void loop() {
    Blinker.run();
    delay(100);
    if(door_flag==1)
    {
        delay(5000);
        Servo_control(PWM_CHANNEL,0);
        door_flag = 0;
    }
}
