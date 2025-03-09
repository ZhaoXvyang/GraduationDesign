#ifndef __BUTTON_H
#define __BUTTON_H

#include "main.h"  // 确保包含必要的头文件

// 全局变量的声明
extern _Bool warring;
extern uint32_t lastButtonPressTime;
extern uint32_t debounceInterval;

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin); // 声明回调函数

#endif
