#include "button.h"
#include "beep.h"
#include "oled.h"

// 定义防抖间隔（单位：毫秒）
#define DEBOUNCE_INTERVAL 200

// 记录每个按键的上次按下时间
static uint32_t lastPressTime_KEY1 = 0;
static uint32_t lastPressTime_KEY2 = 0;
static uint32_t lastPressTime_KEY3 = 0;


void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    uint32_t currentTime = HAL_GetTick();

    if (GPIO_Pin == KEY1_Pin) // KEY1
    {
        if (currentTime - lastPressTime_KEY1 >= DEBOUNCE_INTERVAL)
        {
            if (HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_RESET)
            {
                lastPressTime_KEY1 = currentTime;
                 // 切换当前页面
                currentPage = (currentPage + 1) % PAGE_COUNT; // 循环切换页面
                OLED_Update(); // 更新 OLED 显示
            }
        }
    }
    else if (GPIO_Pin == KEY2_Pin) // KEY2
    {
        if (currentTime - lastPressTime_KEY2 >= DEBOUNCE_INTERVAL)
        {
            if (HAL_GPIO_ReadPin(KEY2_GPIO_Port, KEY2_Pin) == GPIO_PIN_RESET)
            {
                lastPressTime_KEY2 = currentTime;
                warring = !warring;
            }
        }
    }
    else if (GPIO_Pin == KEY3_Pin) // KEY3
    {
        if (currentTime - lastPressTime_KEY3 >= DEBOUNCE_INTERVAL)
        {
            if (HAL_GPIO_ReadPin(KEY3_GPIO_Port, KEY3_Pin) == GPIO_PIN_RESET)
            {
                lastPressTime_KEY3 = currentTime;
                warring = !warring;
            }
        }
    }
}
