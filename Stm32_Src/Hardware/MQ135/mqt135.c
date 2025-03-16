#include "mqt135.h"
#include "math.h"

#define RLOAD 1500.0  // 负载电阻值，单位为 kOhm
#define ATMOCO2 400.0 // 大气中 CO2 的浓度，单位为 ppm
#define PARA 116.6020682 // 烟雾浓度计算的参数
#define PARB 2.738407 // 烟雾浓度计算的参数
#define CORA 0.01 // 温度校正系数
#define CORB 0.01 // 温度校正系数
#define CORC 0.01 // 温度校正系数
#define CORD 0.01 // 湿度校正系数

static float R0 = 1500.0; // 初始 R0 值

void MQ135_Init(void)
{
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 50);
}

// **动态计算 R0（在清洁空气中校准）**
void MQ135_CalibrateR0(void) {
    float vol, Rs;
    int val;

    if (HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC)) {
        val = HAL_ADC_GetValue(&hadc1);
    } else {
        return; // 读取失败，跳过
    }

    // 计算电压
    vol = (float)val * 5.0 / 4096;

    // 计算 Rs
    if (vol > 0) {
        Rs = (5.0 - vol) / vol * RLOAD; // Rs = (RLOAD * Vout) / (Vin - Vout)
    } else {
        return;
    }

    // 在清洁空气中假设 Rs/R0 ≈ 10（需校准）
    R0 = Rs / 10.0;
}

// **读取烟雾浓度**
void MQ135_ReadData(uint16_t *concentration) {
    float vol, Rs, ratio;
    int val;
    const int SAMPLE_COUNT = 10;  // 定义采样次数

    Rs = 0;  // 初始化 Rs

    // 进行多次采样
    for (int i = 0; i < SAMPLE_COUNT; i++) {
        if (HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC)) {
            val = HAL_ADC_GetValue(&hadc1);
        } else {
            *concentration = 0; // 读取失败
            return;
        }

        // 计算电压
        vol = (float)val * 5.0 / 4096;

        // 计算 Rs
        if (vol > 0) {
            Rs += (5.0 - vol) / vol * RLOAD;  // 累加 Rs 值
        }
    }

    // 计算平均 Rs
    Rs = Rs / SAMPLE_COUNT;

    // 计算 Rs/R0 比值
    ratio = Rs / R0;

    // 计算烟雾浓度（ppm）并扩大 100 倍
    *concentration = PARA * pow(ratio, -PARB) * 100.0; // 将浓度扩大 100 倍
}
