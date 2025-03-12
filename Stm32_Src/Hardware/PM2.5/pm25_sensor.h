#ifndef PM25_SENSOR_H
#define PM25_SENSOR_H

#include "main.h"  // 根据你的 STM32 系列调整头文件

#define SAMPLE_COUNT 10 // 采样次数
#define SYS_VOLTAGE 5.0          // STM32 供电电压（5V）
#define ADC_RESOLUTION 4096.0    // ADC 分辨率

#define COV_RATIO 50.0         // 适中调整转换系数（原值 20.0）
#define GAIN_FACTOR 3.5  // 增加放大倍数
#define NO_DUST_VOLTAGE 1   // 稍微降低无尘电压基准（原值 1.0）



void PM25_Init(void);           // 初始化函数
float PM25_ReadDensity(void);   // 读取 PM2.5 浓度的函数

#endif // PM25_SENSOR_H
