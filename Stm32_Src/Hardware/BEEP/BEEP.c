#include "beep.h"
#include "main.h"
#include "BMP180.h"

// 全局变量
uint32_t beepStartTime = 0;
uint8_t beepState = 0;  // 0: 停止, 1: 蜂鸣器发声, 2: 停止蜂鸣器
uint8_t beep_enabled = 1;  // 1: 启用蜂鸣器, 0: 关闭蜂鸣器（不管报警与否）
_Bool warring = 0;  // 报警状态

void CheckThreshold(void) {
    // 检查数据是否超过设定阈值
    if (temperature > tempThreshold || 
        humidity > humiThreshold || 
        airQuality > airQThreshold || 
        (int)(g_tBMP180.fPressure / 100) > pressThreshold || 
        density > pm25Threshold) {
        warring = 1;  // 触发报警
    } else {
        warring = 0;  // 恢复正常状态
    }
}

void BEEP_1S(void) {
    // 如果手动关闭蜂鸣器，直接关闭蜂鸣器并返回
    if (!beep_enabled) {
        HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET); 
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
        return;
    }

    // 如果没有警报且 `beep_enabled` 为 1，则关闭蜂鸣器
    if (!warring) {
        HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET); 
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
        return;
    }

    uint32_t currentTime = HAL_GetTick();  // 获取当前时间

    switch (beepState) {
        case 0:  // 停止蜂鸣器
            if (currentTime - beepStartTime >= 300) {  // 0.3秒
                HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_SET);  // 蜂鸣器发声
                HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);  // LED 亮起

                beepStartTime = currentTime;
                beepState = 1;
            }
            break;

        case 1:  // 蜂鸣器发声
            if (currentTime - beepStartTime >= 200) {  // 0.2秒
                HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET);  // 停止发声
                HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);  // LED 熄灭

                beepStartTime = currentTime;
                beepState = 2;
            }
            break;

        case 2:  // 停止蜂鸣器
            if (currentTime - beepStartTime >= 100) {  // 0.1秒
                beepState = 0;  // 重新进入停止状态
            }
            break;
    }
}
