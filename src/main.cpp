#include <Arduino.h>
#include "flash.h"
#include "rfid.h"
#include "serial_bluetooth.h"
#include <Servo.h>
#define RELAY_CON 5        // 继电器控制引脚
bool isAdminMode = false;  // 是否处于管理者模式

// 检查用户名和密码是否正确


// 初始化函数
void setup() {
    Serial.begin(115200);     // 初始化串口
    SPI.begin();              // 初始化SPI总线

    initRFID();               // 初始化RFID模块
    initFlash();              // 初始化Flash模块
    initBluetooth();          // 初始化蓝牙模块
    Servo_Init(PWM_CHANNEL,PWM_FREQUENCY,PWM_RESOLUTION,SERVO_PIN);              // 初始化舵机
}

// 主循环：处理串口命令和门禁逻辑
void loop() {
    byte currentUID[10];  // 当前读取的UID

    // 处理有线串口输入
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        handleCommand(input);
    }

    // 处理蓝牙串口输入
    if (SerialBT.available()) {
        String input = SerialBT.readStringUntil('\n');
        handleCommand(input);
    }

    // 读取RFID卡片并验证用户
    if (readCardUID(currentUID)) {
        if (verifyUserFromFlash(currentUID)) {
            sendFeedback("用户验证成功，打开继电器");
            Servo_control(PWM_CHANNEL,90);  // 控制舵机(PWM_CHANNEL为通道号
            digitalWrite(RELAY_CON, HIGH);  // 打开继电器
            delay(5000);                    // 延迟5秒后关闭继电器
            digitalWrite(RELAY_CON, LOW);   // 关闭继电器
            Servo_control(PWM_CHANNEL,0);  // 控制舵机(PWM_CHANNEL为通道号
        } else {
            sendFeedback("用户验证失败");
        }
    }
    
    delay(100);  // 延迟1秒
}
