#ifndef __BUTTON_H
#define __BUTTON_H

#include "main.h"  // ȷ��������Ҫ��ͷ�ļ�

// ȫ�ֱ���������
extern _Bool warring;
extern uint32_t lastButtonPressTime;
extern uint32_t debounceInterval;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin); // �����ص�����

#endif
