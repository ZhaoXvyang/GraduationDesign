#ifndef __BEEP_H
#define __BEEP_H

#include "main.h"
#include "button.h"

// ����ȫ�ֱ���
extern uint32_t beepStartTime;
extern uint8_t beepState;  // 0: ֹͣ, 1: ����������, 2: ֹͣ������
extern _Bool warring;
// ��������
void BEEP_1S(void);

#endif // __BEEP_H
