/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "OLED.h"
#include <stdlib.h>
#include "string.h"
#include "dht11.h"
#include "button.h"
#include "beep.h"
#include "mqt135.h"
#include <string.h>
#include "My_esp8266.h"
#include "cjson.h"
#include "bmp180.h"
#include "pm25_sensor.h"

//#include "DATA.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */



/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

// ����Ļ�������
float temperature = 1.0;
float density = 1.0;
uint8_t humidity = 1;
uint16_t airQuality = 0;

// ����ı�����ֵ
uint8_t selectedThresholdIndex = 0; // Ĭ��ѡ�е� 0 �У��¶ȣ�

float tempThreshold = 30.0;
uint8_t humiThreshold = 60;
int airQThreshold = 200;
int pressThreshold = 1010;
float pm25Threshold = 75.0;


//BMP180Data_T g_tBMP180 = {0};

uint8_t not_init = 1; // �Ƿ��ʼ����ɣ���ʼ����ɲ���ʾ����ҳ��
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

// ------- ESP8266 -------------
uint8_t esp_buff[1024] = {0};
uint16_t esp_cnt = 0;
// ------- ESP8266 -------------

volatile OLED_Page_t currentPage = PAGE_SENSOR_DATA; // OLED ҳ�� ��������

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
typedef struct {
    float temp;
    uint8_t humi;
    int airque;
} SensorData;

void Ali_Yun_Send(SensorData *sensor);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

#define SYS_VOLTAGE 5.0
float adc1_value,adc2_value;

void Test_ADC_Readings(void) {
    uint32_t adc1_value, adc2_value;
    float voltage1, voltage2;

    // ��ȡ ADC1
    HAL_ADC_Start(&hadc1);  // ���� ADC1
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);  // �ȴ�ת�����
    adc1_value = HAL_ADC_GetValue(&hadc1);  // ��ȡ ADC1 ֵ
    HAL_ADC_Stop(&hadc1);  // ֹͣ ADC1

    // ��ȡ ADC2
    HAL_ADC_Start(&hadc2);  // ���� ADC2
    HAL_ADC_PollForConversion(&hadc2, HAL_MAX_DELAY);  // �ȴ�ת�����
    adc2_value = HAL_ADC_GetValue(&hadc2);  // ��ȡ ADC2 ֵ
    HAL_ADC_Stop(&hadc2);  // ֹͣ ADC2

    // �����ѹ
    voltage1 = (SYS_VOLTAGE / 4096.0) * adc1_value;
    voltage2 = (SYS_VOLTAGE / 4096.0) * adc2_value;

    // ��ӡ ADC ���
    printf("ADC1 Value: %lu, Voltage: %.3f V\n", adc1_value, voltage1);
    printf("ADC2 Value: %lu, Voltage: %.3f V\n", adc2_value, voltage2);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
    
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_TIM3_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_I2C2_Init();
  MX_ADC2_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim2); // �����ж�
  HAL_TIM_Base_Start_IT(&htim3); // �����ж�
  BMP180_Init();
  PM25_Init(); // ��ʼ�� PM2.5 ������
  OLED_Init(); // OLED ��ʼ��
  HAL_Delay(1000);
  OLED_Clear();
  OLED_ShowString(2, 0, "Init WIFI ...", OLED_8X16);
  OLED_Update();
  // -------������MQTT��ʼ��-----------
  /*
  ESP8266_Init();
  Ali_Yun_Init();
  SensorData sensor = {25.5, 60, 0.8}; // ����������
  */
  // -------������MQTT��ʼ��-----------
  HAL_Delay(3000);

 //---------------------
   not_init = 0; // ��ʼ������� ����������ʾ

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    readData();
    MQ135_Init();
    MQ135_ReadData(&airQuality);
    Test_ADC_Readings();
    density = PM25_ReadDensity();
    CheckThreshold();
      // ��ȡ ADC ֵ


    //printf("�¶�%.2f",g_tBMP180.fPressure);
      /*
    // ���´���������
    sensor.temp = temperature; // �����¶�
    sensor.humi = humidity;     // ����ʪ��
    sensor.airque = airQStr; // ���¿�������

    // ��������
    Ali_Yun_Send(&sensor);
      */
    HAL_Delay(1000);
      

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

//�����������ϴ�

void Ali_Yun_Send(SensorData *sensor) {
    char msg_buf[1024];

    // �ֶ�ƴ���ַ���������������������
    sprintf(msg_buf, "AT+MQTTPUB=0,\"/sys/k282yy0B25v/ESP8266_dev/thing/event/property/post\",\"{params:{\\\"temp\\\":%.1f\\,\\\"humi\\\":%d\\,\\\"airque\\\":%.2f}}\",0,0\r\n", sensor->temp, sensor->humi, sensor->airque);

    // ��ӡ�ͷ�������
    printf("��ʼ��������: %s\r\n", msg_buf);
    ESP8266_SendCmd(msg_buf, "OK");
    ESP8266_Clear();
}

//uint8_t cjson_err_num = 0;  // cJSON ��������Ĵ����������жϽ���ʧ�ܵĴ������������޽���ʧ��

// ���ղ��������ݵĺ���

/*
void Ali_Yun_GetRCV(void)
{
    cJSON *cjson = NULL;  // ���ڴ洢������� cJSON ����
    int num;  // ���ڴ洢������Ϣ���ֽ���
    char topic_buff[256];  // �洢���ĵ�����
    char recv_buffer[ESPBUFF_MAX_SIZE];  // �洢���յ�����Ϣ����
    char *ptr_recv = strstr((const char *)esp_buff,"+MQTTSUBRECV");  // ���ҽ��յ��Ķ�����Ϣ��ʶ�� "+MQTTSUBRECV"
    
    // ����ҵ��˶�����Ϣ��ʶ��
    if(ptr_recv != NULL)  // ���ڽ��յ���MQTT������Ϣ
    {
        memset(topic_buff, 0, sizeof(topic_buff));  // ��� topic_buff
        // �������յ������ݣ���ȡ���⡢�ֽ�������Ϣ����
        sscanf((char *)esp_buff,"+MQTTSUBRECV:0,%[^,],%d,%s", topic_buff, &num, recv_buffer);
        
        // �ж������Ƿ�ƥ��Ԥ��������⣨ALI_TOPIC_SET��
        if(strstr(topic_buff, ALI_TOPIC_SET))  // �жϽ��յ��������Ƿ���ָ��������
        {
            printf("========================���ݽ�����ʼ===========================\r\n");
            printf("�������ݳɹ�����ʼ����  %s\r\n", recv_buffer);
            
            // ʹ�� cJSON �������յ�����Ϣ����
            cjson = cJSON_Parse(recv_buffer);  
            
            // ��� cJSON ����ʧ��
            if(cjson == NULL)
            {
                printf("cjson ��������\r\n");
                cjson_err_num++;  // ���ӽ����������
                if(cjson_err_num > 3)  // ��������������󳬹� 3 ��
                {
                    ESP8266_Clear();  // ��ս��ջ���
                    cjson_err_num = 0;  // ���ý����������
                }            
                printf("========================���ݽ���ʧ��===========================\r\n");
            }
            else
            {
                // �����ɹ�����ȡ JSON �����е� "params" ����
                cJSON *json_data = NULL;
                json_data = cJSON_GetObjectItem(cjson, "params");
                cjson_err_num = 0;  // �����ɹ������ý����������
                
                // ��� "params" ���󲻴���
                if(json_data == NULL)
                {
                    printf("cjson û������\r\n");
                    return;  // ���û�����ݣ���ֱ�ӷ���
                }
                else
                {
                    // ���������� JSON �ڴ��С����������������ֻ���ڴ�ռ��С��
                    printf("cjson �ڴ��СΪ = %d\r\n", sizeof(cjson));
                    
                    // �������ݲ���
                    if(cJSON_GetObjectItem(json_data, "temperature") != NULL)  // �ж��Ƿ���� "temperature" �ֶ�
                    {
                        temp_value = cJSON_GetObjectItem(json_data, "temperature")->valuedouble;  // ��ȡ�¶�ֵ
                        printf("cjson �����ɹ� temp_value = %f\r\n", temp_value);  // ����¶�ֵ
                    }
                    
                    if(cJSON_GetObjectItem(json_data, "Humidity") != NULL)  // �ж��Ƿ���� "Humidity" �ֶ�
                    {
                        humi_value = cJSON_GetObjectItem(json_data, "Humidity")->valuedouble;  // ��ȡʪ��ֵ
                        printf("cjson �����ɹ� Humidity = %f\r\n", humi_value);  // ���ʪ��ֵ
                    }       
                }
                ESP8266_Clear();  // ��ս��ջ���
                cJSON_Delete(cjson);  // �ͷ� cJSON ����
                printf("========================���ݽ����ɹ�===========================\r\n");
            }
        }
    }
}
*/


// PAGE 1������ʾ
void Display_SensorData(void) {
    char buffer[5][20]; // �洢��ʽ������ַ���

    sprintf(buffer[0], "Temp:   %.1f C", temperature);   // �¶ȣ����϶ȣ�
    sprintf(buffer[1], "Humi:   %u %%", humidity);       // ʪ�ȣ��ٷֱȣ�
    sprintf(buffer[2], "AirQ:   %d ppm", airQuality);    // ����������ppm��
    sprintf(buffer[3], "Press:  %d hPa", (int)(g_tBMP180.fPressure / 100)); // ��ѹ��hPa��
    sprintf(buffer[4], "PM2.5:  %.2f ug/m3", density);   // PM2.5 Ũ�ȣ�ug/m3��

    // ��� OLED ��ʾ���������
    OLED_Clear();

    // ������ʾ����������
    for (int i = 0; i < 5; i++) {
        OLED_ShowString(2, i * 10, buffer[i], OLED_6X8);
    }
}

// ��ֵ��ʾ
// PAGE 2 ��ʾ��ֵ�趨
// PAGE 2 ��ʾ��ֵ�趨��֧����˸��ʾ
void Display_Thresholds(void) {
    char buffer[5][20]; // �洢��ʽ������ַ���

    sprintf(buffer[0], "Temp Th:  %.1f C", tempThreshold);    // �¶���ֵ
    sprintf(buffer[1], "Humi Th:  %u %%", humiThreshold);     // ʪ����ֵ
    sprintf(buffer[2], "AirQ Th:  %d ppm", airQThreshold);    // ����������ֵ
    sprintf(buffer[3], "Press Th: %d hPa", pressThreshold);   // ��ѹ��ֵ
    sprintf(buffer[4], "PM2.5 Th: %.2f ug/m3", pm25Threshold); // PM2.5 ��ֵ

    // ��� OLED ��ʾ���������
    OLED_Clear();

    // ��ȡ��ǰϵͳʱ��
    uint32_t currentTime = HAL_GetTick();

    // ������ʾ��ֵ�趨
    for (int i = 0; i < 5; i++) {
        if (i == selectedThresholdIndex) {
            // �õ�ǰѡ�е���ÿ 500ms ��˸һ��
            if ((currentTime / 500) % 2 == 0) {
                OLED_ShowString(2, i * 10, buffer[i], OLED_6X8);
            }
        } else {
            OLED_ShowString(2, i * 10, buffer[i], OLED_6X8);
        }
    }
}


void showData(void) {
    if (not_init == 1) return;
    char tempStr[16];  
    char humiStr[16];  
    char airQueStr[16];  
    char airPress[16];
    char densityStr[16];

    OLED_Clear();

    switch (currentPage) {
        case PAGE_SENSOR_DATA:  // ����������ҳ��
            Display_SensorData();
            break;

        case PAGE_ALARM_THRESHOLDS:  // ������ֵҳ��
            Display_Thresholds();
            break;

        case PAGE_THREE:  // ����ҳ
            OLED_ShowString(2, 0, "Page Three", OLED_6X8);
            OLED_ShowString(2, 10, "More data here", OLED_6X8);
            break;
    }

    OLED_Update();
}

void readData(void){
    DHT11_Read_Data(&temperature, &humidity);
    BMP180_StartPressureSample();
    HAL_Delay(10);
    BMP180_GetTemp(&g_tBMP180);
    
    BMP180_StartPressureSample();
    HAL_Delay(100);
    BMP180_GetPressureAltitude(&g_tBMP180);
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
