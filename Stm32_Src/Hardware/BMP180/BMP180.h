#ifndef __BMP_180_H
#define __BMP_180_H

#include "main.h"

typedef struct
{
	float fTemp;		/*温度，摄氏度*/
	float fPressure;	/*压力，pa*/
	float fAltitude;	/*高度，米*/
}BMP180Data_T;

extern BMP180Data_T g_tBMP180;
 
void BMP180_Init(void);

void BMP180_StartTempSample(void);	/*开始温度采集*/
void BMP180_GetTemp(BMP180Data_T *tpBMP180);			/*最少等待4.5ms，才能获取温度*/

void BMP180_StartPressureSample(void);
void BMP180_GetPressureAltitude(BMP180Data_T *tpBMP180);
#endif
