#include "Delay_us.h"
 
extern TIM_HandleTypeDef htim3;
 
void Delay_us(uint16_t us)
{     
	uint16_t differ = 0xffff-us-5;				
	__HAL_TIM_SET_COUNTER(&htim3,differ);	//�趨TIM1��������ʼֵ
	HAL_TIM_Base_Start(&htim3);		//������ʱ��	
	
	while(differ < 0xffff-5)//�ж�
	{	
		differ = __HAL_TIM_GET_COUNTER(&htim3);		//��ѯ�������ļ���ֵ
	}
	HAL_TIM_Base_Stop(&htim3);//�رն�ʱ��
}
