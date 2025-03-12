#ifndef __MQ135_H
#define __MQ135_H
#include "usart.h"
#include "main.h"
#include "adc.h"
 
 
void MQ135_Init(void);
void MQ135_ReadData(uint16_t *concentration);
 
 
#endif
