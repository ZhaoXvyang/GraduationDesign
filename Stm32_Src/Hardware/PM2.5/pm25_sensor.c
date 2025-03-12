#include "pm25_sensor.h"
#include "stdio.h"

extern ADC_HandleTypeDef hadc2; // ADC ���



/**
 * @brief us����ʱ����
 * @param udelay �ӳٵ�ʱ�䣨΢�룩
 */
void delayXus(uint32_t udelay) {
    uint32_t startval, tickn, delays, wait;
    startval = SysTick->VAL;
    tickn = HAL_GetTick();
    delays = udelay * 72;

    if (delays > startval) {
        while (HAL_GetTick() == tickn) {} // �ȴ� Tick �仯
        wait = 72000 + startval - delays;
        while (wait < SysTick->VAL) {} // �ȴ���ʱ����
    } else {
        wait = startval - delays;
        while (wait < SysTick->VAL && HAL_GetTick() == tickn) {}
    }
}

/**
 * @brief PM2.5 ��������ʼ��
 */
void PM25_Init(void) {
    HAL_ADC_Start(&hadc2); // ���� ADC
}

/**
 * @brief ��ȡ PM2.5 �������� ADC ֵ����ת��ΪŨ��
 * @return PM2.5 Ũ�ȣ���λ����g/m3��
 */
float PM25_ReadDensity(void) {
    uint32_t adcvalue = 0;
    float voltage, density;
    int samples = 10; // ���� 10 ����ƽ��ֵ
    uint32_t sum_adc = 0;
    int valid_samples = 0; // ��¼��Ч���ݸ���

    HAL_GPIO_WritePin(ILED_GPIO_Port, ILED_Pin, GPIO_PIN_SET);
    delayXus(500); // �ӳ�Ԥ��ʱ�䣬��֤ LED ������ȶ�

    for (int i = 0; i < samples; i++) {
        HAL_ADC_Start(&hadc2);
        HAL_ADC_PollForConversion(&hadc2, HAL_MAX_DELAY);
        uint32_t value = HAL_ADC_GetValue(&hadc2);

        if (value > 0) { // ֻͳ�Ʒ���ֵ
            sum_adc += value;
            valid_samples++;
        }

        delayXus(50); // �����������
    }

    HAL_GPIO_WritePin(ILED_GPIO_Port, ILED_Pin, GPIO_PIN_RESET);

    if (valid_samples == 0) { 
        return 0; // �����в���ֵ��Ϊ 0����ֱ�ӷ��� 0
    }

    adcvalue = sum_adc / valid_samples; // ������Ч��ֵ

    voltage = (SYS_VOLTAGE / ADC_RESOLUTION) * adcvalue * GAIN_FACTOR;

    if (voltage >= NO_DUST_VOLTAGE) {
        voltage -= NO_DUST_VOLTAGE;
        density = voltage * COV_RATIO;
    } else {
        density = 0;
    }
    
    return density;
}
