#ifndef SERIAL_BLUETOOTH_H
#define SERIAL_BLUETOOTH_H

#include <BluetoothSerial.h>
#include <HardwareSerial.h>

// º¯ÊýÉùÃ÷

void handleCommand(String input);
void sendFeedback(String message);

#endif // SERIAL_BLUETOOTH_H
