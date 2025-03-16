#include "main.h"
#include "dht11.h"

/**
  * @brief  us级延时函数
  * @param  delay 控制延时的时长
  * @retval 无
  */
void Delay_us(uint32_t udelay)
{
  uint32_t startval,tickn,delays,wait;
 
  startval = SysTick->VAL;
  tickn = HAL_GetTick();
  delays =udelay * 72;
  if(delays > startval)
  {
      while(HAL_GetTick() == tickn)
      {
 
      }
      wait = 72000 + startval - delays;
      while(wait < SysTick->VAL)
      {
 
      }
  }
  else
  {
      wait = startval - delays;
      while(wait < SysTick->VAL && HAL_GetTick() == tickn)
     {
 
     }
  }
}
 
void DHT11_OUT(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct = {0};
 
	GPIO_InitStruct.Pin = GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}
 
void DHT11_IN(void)
{
	GPIO_InitTypeDef  GPIO_InitStruct = {0};
 
	GPIO_InitStruct.Pin  = GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}
 
 
void DHT11_Strat(void)
{
	DHT11_OUT();
	DHT11_LOW;
	HAL_Delay(20);
	DHT11_HIGH;
	Delay_us(30);
}
 
 
uint8_t DHT11_Check(void)
{
	uint8_t retry = 0 ;
	DHT11_IN();
		
	while(DHT11_IO_IN && retry <100) 
	{
		retry++;
		Delay_us(1);//1us
	}
	if(retry>=100) 
	{return  1;}
	 else retry =  0 ;
		
	while(!DHT11_IO_IN && retry<100)
	{
		retry++;
		Delay_us(1);//1us
	}
		
	if(retry>=100)
	{return 1;}
	return 0 ;
}
 
 
uint8_t DHT11_Read_Bit(void)
{
	uint8_t retry = 0 ;
	while(DHT11_IO_IN && retry <100)
	{
		retry++;
		Delay_us(1);
	}
		retry = 0 ;
	while(!DHT11_IO_IN && retry<100)
	{
		retry++;
		Delay_us(1);
	}
	Delay_us(40);
	if(DHT11_IO_IN) return 1;
	else 
	return 0 ;
}
 
 
uint8_t DHT11_Read_Byte(void)
{
	uint8_t i , dat ;
	dat = 0 ;
	for(i=0; i<8; i++)
	{
		dat <<= 1;
		dat |= DHT11_Read_Bit();
	}
	return dat ; 
}
 

uint8_t DHT11_Read_Data(float* temp, uint8_t* humi) {
    uint8_t buf[5];
    uint8_t i;

    DHT11_Strat();
    if (DHT11_Check() == 0) {
        for (i = 0; i < 5; i++) {
            buf[i] = DHT11_Read_Byte();
        }

        // 校验和检查
        if (buf[0] + buf[1] + buf[2] + buf[3] == buf[4]) {
            // 湿度数据，只取整数部分
            *humi = buf[0];

            // 组合温度数据，整数部分和小数部分
            // 这里需要注意DHT11温度小数部分是在buf[3]里，buf[2]是整数部分
            *temp = (float)buf[2] + (float)(buf[3] & 0xFF) / 10.0;
        } else {
            return 1;
        }
    } else {
        return 1;
    }

    return 0;
}
