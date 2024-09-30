#ifndef SERIAL_BLUETOOTH_H
#define SERIAL_BLUETOOTH_H

#include <BluetoothSerial.h>
#include <HardwareSerial.h>

// º¯ÊýÉùÃ÷
void initBluetooth();
void handleCommand(String input);
void sendFeedback(String message);
extern BluetoothSerial SerialBT;
#endif // SERIAL_BLUETOOTH_H
