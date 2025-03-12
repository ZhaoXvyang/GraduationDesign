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
    int samples = 20; // ��߲�������
    uint32_t sum_adc = 0;
    int valid_samples = 0;

    HAL_GPIO_WritePin(ILED_GPIO_Port, ILED_Pin, GPIO_PIN_RESET);
    delayXus(500);

    for (int i = 0; i < samples; i++) {
        HAL_ADC_Start(&hadc2);
        HAL_ADC_PollForConversion(&hadc2, HAL_MAX_DELAY);
        uint32_t value = HAL_ADC_GetValue(&hadc2);

        if (value > 0) { 
            sum_adc += value;
            valid_samples++;
        }

        delayXus(50);
    }

    HAL_GPIO_WritePin(ILED_GPIO_Port, ILED_Pin, GPIO_PIN_SET);

    if (valid_samples == 0) { 
        return 1.0; // �����в���ֵ��Ϊ 0������ 1.0��������ȫΪ 0��
    }

    adcvalue = sum_adc / valid_samples;

    voltage = (SYS_VOLTAGE / ADC_RESOLUTION) * adcvalue * GAIN_FACTOR;

    if (voltage >= NO_DUST_VOLTAGE - 0.02) {  // ���� NO_DUST_VOLTAGE
        voltage -= (NO_DUST_VOLTAGE - 0.02);
        density = voltage * COV_RATIO;
    } else {
        density = 0.5; // ���� 0������ 0.5����ʾ����Ũ��
    }

    // ƽ���˲�����ֹ����
    static float last_density = 1.0;
    density = 0.8 * last_density + 0.2 * density; // ����ֵ�仯��ƽ��
    last_density = density;

    return density;
}
