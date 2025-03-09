#ifndef __BEEP_H
#define __BEEP_H

#include "main.h"
#include "button.h"

// 声明全局变量
extern uint32_t beepStartTime;
extern uint8_t beepState;  // 0: 停止, 1: 蜂鸣器发声, 2: 停止蜂鸣器
extern _Bool warring;
// 函数声明
void BEEP_1S(void);

#endif // __BEEP_H
