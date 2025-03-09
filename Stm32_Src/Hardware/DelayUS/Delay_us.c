#include "Delay_us.h"
 
extern TIM_HandleTypeDef htim3;
 
void Delay_us(uint16_t us)
{     
	uint16_t differ = 0xffff-us-5;				
	__HAL_TIM_SET_COUNTER(&htim3,differ);	//设定TIM1计数器起始值
	HAL_TIM_Base_Start(&htim3);		//启动定时器	
	
	while(differ < 0xffff-5)//判断
	{	
		differ = __HAL_TIM_GET_COUNTER(&htim3);		//查询计数器的计数值
	}
	HAL_TIM_Base_Stop(&htim3);//关闭定时器
}
