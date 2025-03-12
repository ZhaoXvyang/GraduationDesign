#ifndef PM25_SENSOR_H
#define PM25_SENSOR_H

#include "main.h"  // ������� STM32 ϵ�е���ͷ�ļ�

#define SAMPLE_COUNT 10 // ��������
#define SYS_VOLTAGE 5.0          // STM32 �����ѹ��5V��
#define ADC_RESOLUTION 4096.0    // ADC �ֱ���

#define COV_RATIO 50.0         // ���е���ת��ϵ����ԭֵ 20.0��
#define GAIN_FACTOR 3.5  // ���ӷŴ���
#define NO_DUST_VOLTAGE 1   // ��΢�����޳���ѹ��׼��ԭֵ 1.0��



void PM25_Init(void);           // ��ʼ������
float PM25_ReadDensity(void);   // ��ȡ PM2.5 Ũ�ȵĺ���

#endif // PM25_SENSOR_H
