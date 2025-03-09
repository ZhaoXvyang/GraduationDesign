#include "mqt135.h"
#include "stdio.h"
#include <math.h>
#include "mqt135.h"

float R0 = 15000.0; // ����У׼���� R0
float k = 0.30;     // ����ʵ����� k ֵ
float m = 1.5;      // ����ʵ����� m ֵ

void MQ135_Init(void)
{
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 50);
}

void MQ135_ReadData(float *concentration)
{
    float vol;             // ʵ�����õĵ�ѹֵ
    int val;               // ����ת����������
    float Rs;

    if (HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC))
    {
        val = HAL_ADC_GetValue(&hadc1);
    }
    else
    {
        *concentration = -1; // ��� ADC ��ȡʧ�ܣ�����һ������ֵ
        return;
    }

    vol = (float)val * 3.3 / 4096; // ����ʹ�� 3V ����

    // ���� Rs ֵ
    Rs = (3.3 - vol) / vol * R0;

    // ����Ũ��
    if (Rs > 0) // ȷ�� Rs ������
    {
        *concentration = k * pow((R0 / Rs), m);
    }
    else
    {
        *concentration = 0; // ��� Rs ����������Ũ��Ϊ 0
    }
}

