#include "beep.h"
#include "main.h"

// 定义全局变量
uint32_t beepStartTime = 0;
uint8_t beepState = 0;  // 0: 停止, 1: 蜂鸣器发声, 2: 停止蜂鸣器
_Bool warring = 0;

void CheckThreshold(void) {
    // 检查各项数据是否超过设定的阈值
    if (temperature > tempThreshold || 
        humidity > humiThreshold || 
        airQuality > airQThreshold || 
        density > pressThreshold || 
        density > pm25Threshold) {
        warring = 1;  // 触发报警
    } else {
        warring = 0;  // 恢复正常状态
    }
}


void BEEP_1S(void) {
    if (!warring) {
        HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET); 
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
        return;
    }

    uint32_t currentTime = HAL_GetTick();  // 获取当前系统时间

    switch (beepState) {
        case 0:  // 停止蜂鸣器
            if (currentTime - beepStartTime >= 300) {  // 0.3秒
                HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_SET);  // 开始发声
                HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);  // LED 熄灭

                beepStartTime = currentTime;  // 记录发声开始时间
                beepState = 1;
            }
            break;

        case 1:  // 蜂鸣器发声
            if (currentTime - beepStartTime >= 200) {  // 0.2秒
                HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET);  // 停止发声
                HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);  // LED 亮起

                beepStartTime = currentTime;  // 记录停止发声时间
                beepState = 2;
            }
            break;

        case 2:  // 停止蜂鸣器
            if (currentTime - beepStartTime >= 100) {  // 0.1秒
                beepState = 0;  // 重新进入停止状态，准备下一轮操作
            }
            break;
    }
}
