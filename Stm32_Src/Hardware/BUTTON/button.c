#include "button.h"
#include "beep.h"
#include "oled.h"

// 定义防抖间隔（单位：毫秒）
#define DEBOUNCE_INTERVAL 200
#define DOUBLE_CLICK_INTERVAL 300  // 双击间隔（300ms）

// 记录每个按键的上次按下时间
static uint32_t lastPressTime_KEY1 = 0;
static uint32_t lastPressTime_KEY2 = 0;
static uint32_t lastPressTime_KEY3 = 0;
static uint32_t lastKey1PressTime = 0; // KEY1 的双击检测时间

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    uint32_t currentTime = HAL_GetTick();

    if (GPIO_Pin == KEY1_Pin) // KEY1 处理
    {
        if (currentTime - lastPressTime_KEY1 >= DEBOUNCE_INTERVAL) // 消抖
        {
            if (HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_RESET)
            {
                uint32_t timeSinceLastPress = currentTime - lastKey1PressTime;
                lastKey1PressTime = currentTime;
                lastPressTime_KEY1 = currentTime; 

                if (currentPage == 1) // 在 Page 2
                {
                    if (timeSinceLastPress < DOUBLE_CLICK_INTERVAL) // 300ms 内双击
                    {
                        currentPage = 2; // 跳转到 Page 3
                    }
                    else // 单击，仅切换选中行
                    {
                        selectedThresholdIndex = (selectedThresholdIndex + 1) % 5;
                    }
                }
                else // 其他页面正常换页
                {
                    currentPage = (currentPage + 1) % PAGE_COUNT;
                }
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
