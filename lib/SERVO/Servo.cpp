#include <Servo.h>



void Servo_Init(uint8_t channel,uint32_t frequency,uint8_t bit,uint8_t servo_pin)
{
    ledcSetup(channel,frequency,bit);
    ledcAttachPin(servo_pin,channel);
}

void Servo_control(uint8_t channel,uint8_t degree)
{
    uint32_t duty;
    duty = (uint32_t)((degree/90+0.5)*(1024/20));
    ledcWrite(channel,duty);
}

