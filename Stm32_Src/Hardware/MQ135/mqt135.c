/* mq135.c - 酒精浓度检测版 */
#include "mqt135.h"
#include "math.h"
#include "stm32f1xx_hal.h"  // 根据你的 HAL 版本调整

#define RLOAD 1000.0f                  // 负载电阻（单位：欧姆）
#define PARA 77.255f                   // 曲线参数 A（适用于酒精）
#define PARB -1.638f                   // 曲线参数 B（适用于酒精）
#define MQ135_CLEAN_AIR_FACTOR 60.0f  // 清洁空气下 Rs/R0 比值（酒精为约 60）
#define ADC_VREF 5.0f                  // ADC 参考电压，5V 供电时为 5.0f，3.3V 时改为 3.3f

// 注：
// - 室内空气中的酒精浓度一般为 0~5 ppm；
// - 饮酒后呼气中酒精浓度可能达到 200~800 ppm 甚至更高；
// - MQ135 传感器需预热 24 小时才能达到稳定状态，
//   未预热时其读数通常偏高但不稳定，可用于调试但不可作为有效数据；
// - 初次使用建议通过 Calibrate 函数在清洁空气中校准 R0
// - 供电为 5V 时，ADC 电压参考需配合设定；若使用 3.3V 供电则保持当前计算方式。

extern ADC_HandleTypeDef hadc1;

static float R0 = 1000.0f; // 默认初始值，可通过校准动态获取

void MQ135_Init(void)
{
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 50);
}

// 清洁空气中校准 R0
void MQ135_CalibrateR0(void)
{
    float vol, Rs;
    int val;

    if (HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC)) {
        val = HAL_ADC_GetValue(&hadc1);
    } else {
        return; // 读取失败，跳过
    }

    vol = (float)val * ADC_VREF / 4096.0f;

    if (vol > 0) {
        Rs = (ADC_VREF - vol) / vol * RLOAD;
    } else {
        return;
    }

    R0 = Rs / MQ135_CLEAN_AIR_FACTOR;
}

void MQ135_ReadData(uint16_t *concentration)
{
    float vol, Rs = 0, ratio;
    int val;
    const int SAMPLE_COUNT = 10;

    for (int i = 0; i < SAMPLE_COUNT; i++) {
        if (HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC)) {
            val = HAL_ADC_GetValue(&hadc1);
        } else {
            *concentration = 0;
            return;
        }

        vol = (float)val * ADC_VREF / 4096.0f;

        if (vol > 0) {
            Rs += (ADC_VREF - vol) / vol * RLOAD;
        }
        HAL_Delay(10); // 每次采样间隔
    }

    Rs /= SAMPLE_COUNT;
    ratio = Rs / R0;

    *concentration = (uint16_t)(PARA * powf(ratio, PARB)); // 返回 ppm 浓度
}