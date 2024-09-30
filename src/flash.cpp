#include "flash.h"
#include "serial_bluetooth.h"  // Ϊ�˷�����Ϣ

SPIFlash flash(FLASH_CS_PIN);

// ��ʼ��Flashģ��
void initFlash() {
    if (flash.begin()) {
        sendFeedback("W25Q80 Flash��ʼ���ɹ�");
    } else {
        sendFeedback("W25Q80 Flash��ʼ��ʧ��");
        while (1);  // �����ʼ��ʧ�ܣ�ֹͣ����
    }
}

// ����û���Flash�洢
void addUserToFlash(const char* username, const byte* uid) {
    for (int i = 0; i < MAX_USERS; i++) {
        uint32_t address = i * USER_RECORD_SIZE;
        byte tempUID[4];
        flash.readByteArray(address + 16, tempUID, 4);

        if (tempUID[0] == 0xFF) {  // ���UIDΪ�գ���ʾ���λ��δʹ��
            flash.writeByteArray(address, (byte*)username, 16);  // д���û���
            flash.writeByteArray(address + 16, (byte*)uid, 4);   // д��UID
            sendFeedback("�û���ӳɹ���");
            return;
        }
    }
    sendFeedback("�û��洢�������޷���Ӹ����û���");
}

// ɾ���û�
void deleteUserFromFlash(const byte* uid) {
    for (int i = 0; i < MAX_USERS; i++) {
        uint32_t address = i * USER_RECORD_SIZE;
        byte tempUID[4];

        // ��ȡ�û��� UID
        flash.readByteArray(address + 16, tempUID, 4);

        // ��� UID �Ƿ�ƥ��
        if (memcmp(tempUID, uid, 4) == 0) {  // �ҵ�ƥ���UID
            // ��������û��������ڵ�������ַ
            uint32_t sectorStartAddress = (address / 4096) * 4096;  // 4096�ֽ�Ϊһ������
            
            // ����һ�����������ڴ洢��������������
            byte sectorData[4096];  // ������Сͨ����4KB
            flash.readByteArray(sectorStartAddress, sectorData, 4096);  // ��ȡ��������

            // �ڻ��������ҵ���Ӧ�û��ļ�¼λ�ã����������Ϊ0xFF
            memset(&sectorData[address % 4096], 0xFF, USER_RECORD_SIZE);

            // ����������
            if (flash.eraseSector(sectorStartAddress / 4096)) {  // ������������
                // �����ɹ������½��޸ĺ����������д��
                if (flash.writeByteArray(sectorStartAddress, sectorData, 4096)) {
                    sendFeedback("�û�ɾ���ɹ���");
                } else {
                    sendFeedback("�û�ɾ��ʧ�ܣ��޷�д���������ݣ�");
                }
            } else {
                sendFeedback("�û�ɾ��ʧ�ܣ��޷�����������");
            }

            // ��֤����Ƿ�ɹ�
            byte verifyBuffer[USER_RECORD_SIZE];
            flash.readByteArray(address, verifyBuffer, USER_RECORD_SIZE);
            bool cleared = true;
            for (int j = 0; j < USER_RECORD_SIZE; j++) {
                if (verifyBuffer[j] != 0xFF) {
                    cleared = false;
                    break;
                }
            }

            // �������Ƿ�ɹ�
            if (cleared) {
                sendFeedback("�û�ɾ���ɹ���");
            } else {
                sendFeedback("�û�ɾ��ʧ�ܣ�δ��������ݣ�");
            }

            delay(50);  // �ӳ��Է�ֹ���Ȳ���
            return;
        }
    }
    sendFeedback("δ�ҵ�ƥ����û���");
}




// ��֤UID�Ƿ������Flash��
bool verifyUserFromFlash(const byte* uid) {
    for (int i = 0; i < MAX_USERS; i++) {
        uint32_t address = i * USER_RECORD_SIZE;
        byte tempUID[4];
        flash.readByteArray(address + 16, tempUID, 4);

        if (memcmp(tempUID, uid, 4) == 0) {  // �ҵ�ƥ���UID
            return true;
        }
    }
    return false;
}

// ��ӡ�����û�
// ��ӡ�����û�
// ��ӡ�����û�
void printUsersInFlash() {
    sendFeedback("��ǰ�洢���û�:");

    for (int i = 0; i < MAX_USERS; i++) {
        uint32_t address = i * USER_RECORD_SIZE;
        char username[17] = {0};  // 16�ֽ��û��� + 1�ֽڽ�β
        byte uid[4];

        // ��ȡ�û�����UID
        flash.readByteArray(address, (byte*)username, 16);
        flash.readByteArray(address + 16, uid, 4);

        // ����Ƿ�Ϊ��Ч�� UID���ų�ȫ 0xFF �� UID��
        bool isValidUID = false;
        for (int j = 0; j < 4; j++) {
            if (uid[j] != 0xFF) {
                isValidUID = true;
                break;
            }
        }

        // ��� UID ��Ч��������û���Ϣ
        if (isValidUID) {
            String message = "�û���: " + String(username) + ", UID: ";
            for (int j = 0; j < 4; j++) {
                // ȷ������λʮ��������ʽ���
                if (uid[j] < 0x10) message += "0";
                message += String(uid[j], HEX);
                if (j < 3) message += ":";
            }
            sendFeedback(message);

            // Ϊÿ���û����������һ���ӳ٣�����ϵͳ���ع���
            delay(50);
        }
    }
    sendFeedback("�û��б��ӡ���");
}
