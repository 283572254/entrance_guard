#ifndef __SERVO_H
#define __SERVO_H
#include <Arduino.h>
#define SERVO_PIN 4        // ������ӵ�����
#define PWM_CHANNEL 0    // PWM ͨ��
#define PWM_FREQUENCY 50 // PWM Ƶ�ʣ�Hz��
#define PWM_RESOLUTION 10

void Servo_Init(uint8_t channel,uint32_t frequency,uint8_t bit,uint8_t servo_pin);
void Servo_control(uint8_t channel,uint8_t degree);
#endif