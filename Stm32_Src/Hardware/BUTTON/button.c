#include "button.h"
#include "beep.h"
#include "oled.h"

// ��������������λ�����룩
#define DEBOUNCE_INTERVAL 200

// ��¼ÿ���������ϴΰ���ʱ��
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
                 // �л���ǰҳ��
                currentPage = (currentPage + 1) % PAGE_COUNT; // ѭ���л�ҳ��
                OLED_Update(); // ���� OLED ��ʾ
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
