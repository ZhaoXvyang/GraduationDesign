#include "mqt135.h"
#include "math.h"

#define RLOAD 1500.0  // ���ص���ֵ����λΪ kOhm
#define ATMOCO2 400.0 // ������ CO2 ��Ũ�ȣ���λΪ ppm
#define PARA 116.6020682 // ����Ũ�ȼ���Ĳ���
#define PARB 2.738407 // ����Ũ�ȼ���Ĳ���
#define CORA 0.01 // �¶�У��ϵ��
#define CORB 0.01 // �¶�У��ϵ��
#define CORC 0.01 // �¶�У��ϵ��
#define CORD 0.01 // ʪ��У��ϵ��

static float R0 = 1500.0; // ��ʼ R0 ֵ

void MQ135_Init(void)
{
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 50);
}

// **��̬���� R0������������У׼��**
void MQ135_CalibrateR0(void) {
    float vol, Rs;
    int val;

    if (HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC)) {
        val = HAL_ADC_GetValue(&hadc1);
    } else {
        return; // ��ȡʧ�ܣ�����
    }

    // �����ѹ
    vol = (float)val * 5.0 / 4096;

    // ���� Rs
    if (vol > 0) {
        Rs = (5.0 - vol) / vol * RLOAD; // Rs = (RLOAD * Vout) / (Vin - Vout)
    } else {
        return;
    }

    // ���������м��� Rs/R0 �� 10����У׼��
    R0 = Rs / 10.0;
}

// **��ȡ����Ũ��**
void MQ135_ReadData(uint16_t *concentration) {
    float vol, Rs, ratio;
    int val;
    const int SAMPLE_COUNT = 10;  // �����������

    Rs = 0;  // ��ʼ�� Rs

    // ���ж�β���
    for (int i = 0; i < SAMPLE_COUNT; i++) {
        if (HAL_IS_BIT_SET(HAL_ADC_GetState(&hadc1), HAL_ADC_STATE_REG_EOC)) {
            val = HAL_ADC_GetValue(&hadc1);
        } else {
            *concentration = 0; // ��ȡʧ��
            return;
        }

        // �����ѹ
        vol = (float)val * 5.0 / 4096;

        // ���� Rs
        if (vol > 0) {
            Rs += (5.0 - vol) / vol * RLOAD;  // �ۼ� Rs ֵ
        }
    }

    // ����ƽ�� Rs
    Rs = Rs / SAMPLE_COUNT;

    // ���� Rs/R0 ��ֵ
    ratio = Rs / R0;

    // ��������Ũ�ȣ�ppm�������� 100 ��
    *concentration = PARA * pow(ratio, -PARB) * 100.0; // ��Ũ������ 100 ��
}
