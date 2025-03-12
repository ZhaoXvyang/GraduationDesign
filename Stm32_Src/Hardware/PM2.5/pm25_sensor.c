#include "pm25_sensor.h"
#include "stdio.h"

extern ADC_HandleTypeDef hadc2; // ADC 句柄



/**
 * @brief us级延时函数
 * @param udelay 延迟的时间（微秒）
 */
void delayXus(uint32_t udelay) {
    uint32_t startval, tickn, delays, wait;
    startval = SysTick->VAL;
    tickn = HAL_GetTick();
    delays = udelay * 72;

    if (delays > startval) {
        while (HAL_GetTick() == tickn) {} // 等待 Tick 变化
        wait = 72000 + startval - delays;
        while (wait < SysTick->VAL) {} // 等待延时结束
    } else {
        wait = startval - delays;
        while (wait < SysTick->VAL && HAL_GetTick() == tickn) {}
    }
}

/**
 * @brief PM2.5 传感器初始化
 */
void PM25_Init(void) {
    HAL_ADC_Start(&hadc2); // 启动 ADC
}

/**
 * @brief 读取 PM2.5 传感器的 ADC 值，并转换为浓度
 * @return PM2.5 浓度（单位：μg/m3）
 */
float PM25_ReadDensity(void) {
    uint32_t adcvalue = 0;
    float voltage, density;
    int samples = 20; // 提高采样次数
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
        return 1.0; // 若所有采样值都为 0，返回 1.0（避免完全为 0）
    }

    adcvalue = sum_adc / valid_samples;

    voltage = (SYS_VOLTAGE / ADC_RESOLUTION) * adcvalue * GAIN_FACTOR;

    if (voltage >= NO_DUST_VOLTAGE - 0.02) {  // 降低 NO_DUST_VOLTAGE
        voltage -= (NO_DUST_VOLTAGE - 0.02);
        density = voltage * COV_RATIO;
    } else {
        density = 0.5; // 不是 0，而是 0.5，表示极低浓度
    }

    // 平滑滤波，防止波动
    static float last_density = 1.0;
    density = 0.8 * last_density + 0.2 * density; // 让数值变化更平滑
    last_density = density;

    return density;
}
