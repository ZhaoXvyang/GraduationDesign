#include "button.h"
#include "beep.h"
#include "oled.h"

// ��������������λ�����룩
#define DEBOUNCE_INTERVAL 200
#define DOUBLE_CLICK_INTERVAL 300  // ˫�������300ms��

// ��¼ÿ���������ϴΰ���ʱ��
static uint32_t lastPressTime_KEY1 = 0;
static uint32_t lastPressTime_KEY2 = 0;
static uint32_t lastPressTime_KEY3 = 0;
static uint32_t lastKey1PressTime = 0; // KEY1 ��˫�����ʱ��

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    uint32_t currentTime = HAL_GetTick();

    if (GPIO_Pin == KEY1_Pin) // KEY1 ����
    {
        if (currentTime - lastPressTime_KEY1 >= DEBOUNCE_INTERVAL) // ����
        {
            if (HAL_GPIO_ReadPin(KEY1_GPIO_Port, KEY1_Pin) == GPIO_PIN_RESET)
            {
                uint32_t timeSinceLastPress = currentTime - lastKey1PressTime;
                lastKey1PressTime = currentTime;
                lastPressTime_KEY1 = currentTime; 

                if (currentPage == 1) // �� Page 2
                {
                    if (timeSinceLastPress < DOUBLE_CLICK_INTERVAL) // 300ms ��˫��
                    {
                        currentPage = 2; // ��ת�� Page 3
                    }
                    else // ���������л�ѡ����
                    {
                        selectedThresholdIndex = (selectedThresholdIndex + 1) % 5;
                    }
                }
                else // ����ҳ��������ҳ
                {
                    currentPage = (currentPage + 1) % PAGE_COUNT;
                }
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
