#include "mqt135.h"
#include "stdio.h"
#include <math.h>
#include "mqt135.h"

float R0 = 15000.0; // 根据校准调整 R0
float k = 0.30;     // 根据实验调整 k 值
float m = 1.5;      // 根据实验调整 m 值

void MQ135_Init(void)
{
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 50);
}

void MQ135_ReadData(float *concentration)
{
    float vol;             // 实际所得的电压值
    int val;               // 输入转换的数字量
    float Rs;

    if (HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC))
    {
        val = HAL_ADC_GetValue(&hadc1);
    }
    else
    {
        *concentration = -1; // 如果 ADC 读取失败，返回一个错误值
        return;
    }

    vol = (float)val * 3.3 / 4096; // 假设使用 3V 供电

    // 计算 Rs 值
    Rs = (3.3 - vol) / vol * R0;

    // 计算浓度
    if (Rs > 0) // 确保 Rs 大于零
    {
        *concentration = k * pow((R0 / Rs), m);
    }
    else
    {
        *concentration = 0; // 如果 Rs 不合理，设置浓度为 0
    }
}

