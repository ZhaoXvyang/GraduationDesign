#include "beep.h"

// 定义全局变量
uint32_t beepStartTime = 0;
uint8_t beepState = 0;  // 0: 停止, 1: 蜂鸣器发声, 2: 停止蜂鸣器ADC1
_Bool warring = 0;

void BEEP_1S(void) {
    if (!warring) {
        HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET); 
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
        return;
    }

    uint32_t currentTime = HAL_GetTick();  // 获取当前系统时间

    switch (beepState) {
        case 0:  // 停止蜂鸣器
            if (currentTime - beepStartTime >= 1000) {  // 1秒
                HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_SET);  // 开始发声
                // 熄灭 LED（假设连接在 PC13）
                HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);  // 高电平熄灭 LED

                beepStartTime = currentTime;  // 记录发声开始时间
                beepState = 1;
            }
            break;

        case 1:  // 蜂鸣器发声
            if (currentTime - beepStartTime >= 500) {  // 0.5秒
                HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET);  // 停止发声
                // 点亮 LED（假设连接在 PC13）
                HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);  // 低电平点亮 LED（如果 LED 是接地引脚时）

                beepStartTime = currentTime;  // 记录停止发声时间
                beepState = 2;
            }
            break;

        case 2:  // 停止蜂鸣器
            if (currentTime - beepStartTime >= 500) {  // 0.5秒
                beepState = 0;  // 重新进入停止状态，准备下一轮操作
            }
            break;
    }
}
