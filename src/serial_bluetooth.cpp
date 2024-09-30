#include "serial_bluetooth.h"
#include "flash.h"  // ��Ҫ����Flash����
#include "rfid.h"   // ��Ҫ����RFID����

BluetoothSerial SerialBT;
extern SPIFlash flash;
extern MFRC522 mfrc522;
// �������¼ȡ�����ȴ�ʱ�䣨1���� = 60�룩
#define RECORD_TIMEOUT 60000  // 60000 ���� = 1����

// ��ʼ������
void initBluetooth() {
    SerialBT.begin("ESP32-RFID-Manager");  // ���������豸����
    Serial.println("�����ѳ�ʼ�����豸����Ϊ��ESP32-RFID-Manager");
}
// ����û����Ƿ��Ѿ�����
bool isUsernameExists(const char* username) {
    for (int i = 0; i < MAX_USERS; i++) {
        uint32_t address = i * USER_RECORD_SIZE;
        char storedUsername[17] = {0};  // 16�ֽ��û��� + 1�ֽڽ�β

        // ��Flash�ж�ȡ�û���
        flash.readByteArray(address, (byte*)storedUsername, 16);
        if (strcmp(storedUsername, username) == 0) {
            return true;  // �û����Ѵ���
        }
    }
    return false;
}

// ���UID�Ƿ��Ѿ�����
bool isUIDExists(const byte* uid) {
    for (int i = 0; i < MAX_USERS; i++) {
        uint32_t address = i * USER_RECORD_SIZE;
        byte storedUID[4];

        // ��Flash�ж�ȡUID
        flash.readByteArray(address + 16, storedUID, 4);
        if (memcmp(storedUID, uid, 4) == 0) {
            return true;  // UID�Ѵ���
        }
    }
    return false;
}
// ����¼ȡģʽ
void quickRecord() {
    sendFeedback("�������¼ȡģʽ������1�����ڽ�IC�������ڶ�������...");

    unsigned long startTime = millis();  // ��¼��ʼʱ��
    byte uid[10];
    bool cardRecorded = false;
    bool waitingForUserInput = false;
    String username;

    // ѭ���ȴ�1���ӻ�ֱ����Ƭ¼ȡ�ɹ�
    while (millis() - startTime < RECORD_TIMEOUT) {
        // �����û�н���ȴ��û�����׶Σ��������⿨Ƭ
        if (!waitingForUserInput && readCardUID(uid)) {
            String uidStr = "";  // ��UIDת��Ϊ�ַ���
            for (int i = 0; i < mfrc522.uid.size; i++) {
                if (mfrc522.uid.uidByte[i] < 0x10) uidStr += "0";
                uidStr += String(mfrc522.uid.uidByte[i], HEX);
            }

            // ���Ϳ�ƬUID
            sendFeedback("��⵽IC����UID: " + uidStr);
            cardRecorded = true;

            // ����ȴ��û�����¼��׶�
            sendFeedback("�������û�������¼ȡ��");
            waitingForUserInput = true;  // ���ñ�־��ʾ�����ڵȴ��û�������
        }

        // ��������ʽ����û�����
        if (waitingForUserInput) {
            if (Serial.available()) {
                username = Serial.readStringUntil('\n');  // �Ӵ��ڶ�ȡ
                waitingForUserInput = false;  // ��ȡ���˳�����ȴ�ģʽ
            } else if (SerialBT.available()) {
                username = SerialBT.readStringUntil('\n');  // ��������ȡ
                waitingForUserInput = false;  // ��ȡ���˳�����ȴ�ģʽ
            }

            if (!waitingForUserInput) {
                // ��������󣬼���û�����UID�Ƿ��ظ�
                char usernameArray[16];
                username.toCharArray(usernameArray, 16);
                if (isUsernameExists(usernameArray)) {
                    sendFeedback("�û����Ѵ��ڣ�¼ȡʧ�ܣ�");
                } else if (isUIDExists(uid)) {
                    sendFeedback("UID�Ѵ��ڣ�¼ȡʧ�ܣ�");
                } else {
                    addUserToFlash(usernameArray, uid);  // ¼���û���Ϣ
                    sendFeedback("�û� " + username + " ¼ȡ�ɹ���");
                }
                break;  // ��Ƭ¼ȡ���˳�ѭ��
            }
        }

        delay(100);  // ÿ��100������һ���Ƿ����¿�Ƭ
    }

    if (!cardRecorded) {
        sendFeedback("¼ȡ��ʱ��1������δ��⵽�κ�IC����¼ȡģʽ������");
    }
}




// ����������
// ����������
void handleCommand(String input) {
    if (input.startsWith("ADDUSER ")) {
        int commaIndex = input.indexOf(',');
        if (commaIndex > 0 && input.length() > commaIndex + 1) {
            String newUsername = input.substring(8, commaIndex);
            String newUID = input.substring(commaIndex + 1);

            // ���UID��ʽ�Ƿ���Ч��ӦΪ8��16�����ַ���
            if (newUID.length() != 8) {
                sendFeedback("��Ч��UID��ʽ��ӦΪ8��16�����ַ���");
                return;
            }

            char username[16];
            newUsername.toCharArray(username, 16);
            byte uid[4];

            // ��UIDת��Ϊ�ֽ�����
            for (int i = 0; i < 4; i++) {
                uid[i] = (byte)strtoul(newUID.substring(i * 2, i * 2 + 2).c_str(), NULL, 16);
            }

            // ����û�����UID�Ƿ��ظ�
            if (isUsernameExists(username)) {
                sendFeedback("�û����Ѵ��ڣ��޷���ӣ�");
            } else if (isUIDExists(uid)) {
                sendFeedback("UID�Ѵ��ڣ��޷���ӣ�");
            } else {
                addUserToFlash(username, uid);
                sendFeedback("�û� " + newUsername + " ��ӳɹ���");
            }
        } else {
            sendFeedback("��Ч�������ʽ��ӦΪ��ADDUSER <�û���>,<UID>");
        }
    } else if (input.startsWith("DELUSER ")) {
        String uidStr = input.substring(8);

        // ���UID��ʽ�Ƿ���Ч��ӦΪ8��16�����ַ���
        if (uidStr.length() != 8) {
            sendFeedback("��Ч��UID��ʽ��ӦΪ8��16�����ַ���");
            return;
        }

        byte uid[4];
        for (int i = 0; i < 4; i++) {
            uid[i] = (byte)strtoul(uidStr.substring(i * 2, i * 2 + 2).c_str(), NULL, 16);
        }

        deleteUserFromFlash(uid);
    } else if (input.startsWith("PRINTUSERS")) {
        printUsersInFlash();
    } else if (input.startsWith("RECORD")) {
        // ��������¼ȡģʽ
        quickRecord();
    } else {
        sendFeedback("��Ч�����");
    }
}


// ͨ�÷�������
void sendFeedback(String message) {
    // ���͵����ߴ���
    Serial.println(message);

    // ������������ӣ����͵���������
    if (SerialBT.hasClient()) {
        SerialBT.println(message);
    }
}
