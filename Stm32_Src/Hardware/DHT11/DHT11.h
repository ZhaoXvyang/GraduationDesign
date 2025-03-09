#ifndef __DHT11_H
#define __DHT11_H

#define DHT11_HIGH     HAL_GPIO_WritePin(GPIOA, DHT11_Pin,	GPIO_PIN_SET)
#define DHT11_LOW      HAL_GPIO_WritePin(GPIOA, DHT11_Pin, GPIO_PIN_RESET)
#define DHT11_IO_IN    HAL_GPIO_ReadPin(GPIOA, DHT11_Pin)


void SystemClock_Config(void);
 
void Delay_us(uint32_t delay);                //us����ʱ����
 
//DHT11_DATA ,IO������Ϊ���ģʽ�������IO��ָ����STM32CubeMX�����õ�IO�ڣ�
void DHT11_OUT(void);	
												
void DHT11_IN(void);                          //DHT11_Data IO����Ϊ����ģʽ
void DHT11_Strat(void);				          //����������ʼ�ź�
uint8_t DHT11_Check(void);                    //DHT11������Ӧ�ź�
uint8_t DHT11_Read_Bit(void);                 //��ȡDHT11һ��BIT������
uint8_t DHT11_Read_Byte(void);                //��ȡDHT11һ��Byte������
uint8_t DHT11_Read_Data(float* temp , uint8_t* humi);  //��ȡDHT11ʪ�Ⱥ��¶ȵ�����
             

#endif
