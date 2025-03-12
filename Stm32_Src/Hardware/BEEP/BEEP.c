#include "beep.h"
#include "main.h"

// ����ȫ�ֱ���
uint32_t beepStartTime = 0;
uint8_t beepState = 0;  // 0: ֹͣ, 1: ����������, 2: ֹͣ������
_Bool warring = 0;

void CheckThreshold(void) {
    // �����������Ƿ񳬹��趨����ֵ
    if (temperature > tempThreshold || 
        humidity > humiThreshold || 
        airQuality > airQThreshold || 
        density > pressThreshold || 
        density > pm25Threshold) {
        warring = 1;  // ��������
    } else {
        warring = 0;  // �ָ�����״̬
    }
}


void BEEP_1S(void) {
    if (!warring) {
        HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET); 
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
        return;
    }

    uint32_t currentTime = HAL_GetTick();  // ��ȡ��ǰϵͳʱ��

    switch (beepState) {
        case 0:  // ֹͣ������
            if (currentTime - beepStartTime >= 300) {  // 0.3��
                HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_SET);  // ��ʼ����
                HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);  // LED Ϩ��

                beepStartTime = currentTime;  // ��¼������ʼʱ��
                beepState = 1;
            }
            break;

        case 1:  // ����������
            if (currentTime - beepStartTime >= 200) {  // 0.2��
                HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET);  // ֹͣ����
                HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);  // LED ����

                beepStartTime = currentTime;  // ��¼ֹͣ����ʱ��
                beepState = 2;
            }
            break;

        case 2:  // ֹͣ������
            if (currentTime - beepStartTime >= 100) {  // 0.1��
                beepState = 0;  // ���½���ֹͣ״̬��׼����һ�ֲ���
            }
            break;
    }
}
