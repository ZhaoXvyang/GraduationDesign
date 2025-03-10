#ifndef __BMP_180_H
#define __BMP_180_H

#include "main.h"

typedef struct
{
	float fTemp;		/*�¶ȣ����϶�*/
	float fPressure;	/*ѹ����pa*/
	float fAltitude;	/*�߶ȣ���*/
}BMP180Data_T;

extern BMP180Data_T g_tBMP180;
 
void BMP180_Init(void);

void BMP180_StartTempSample(void);	/*��ʼ�¶Ȳɼ�*/
void BMP180_GetTemp(BMP180Data_T *tpBMP180);			/*���ٵȴ�4.5ms�����ܻ�ȡ�¶�*/

void BMP180_StartPressureSample(void);
void BMP180_GetPressureAltitude(BMP180Data_T *tpBMP180);
#endif
