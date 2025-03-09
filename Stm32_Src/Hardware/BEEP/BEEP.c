#include "beep.h"

// ����ȫ�ֱ���
uint32_t beepStartTime = 0;
uint8_t beepState = 0;  // 0: ֹͣ, 1: ����������, 2: ֹͣ������ADC1
_Bool warring = 0;

void BEEP_1S(void) {
    if (!warring) {
        HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET); 
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
        return;
    }

    uint32_t currentTime = HAL_GetTick();  // ��ȡ��ǰϵͳʱ��

    switch (beepState) {
        case 0:  // ֹͣ������
            if (currentTime - beepStartTime >= 1000) {  // 1��
                HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_SET);  // ��ʼ����
                // Ϩ�� LED������������ PC13��
                HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);  // �ߵ�ƽϨ�� LED

                beepStartTime = currentTime;  // ��¼������ʼʱ��
                beepState = 1;
            }
            break;

        case 1:  // ����������
            if (currentTime - beepStartTime >= 500) {  // 0.5��
                HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET);  // ֹͣ����
                // ���� LED������������ PC13��
                HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);  // �͵�ƽ���� LED����� LED �ǽӵ�����ʱ��

                beepStartTime = currentTime;  // ��¼ֹͣ����ʱ��
                beepState = 2;
            }
            break;

        case 2:  // ֹͣ������
            if (currentTime - beepStartTime >= 500) {  // 0.5��
                beepState = 0;  // ���½���ֹͣ״̬��׼����һ�ֲ���
            }
            break;
    }
}
