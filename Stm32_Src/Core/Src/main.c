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

// 共享的环境数据
float temperature = 1.0;
float density = 1.0;
uint8_t humidity = 1;
uint16_t airQuality = 0;

// 共享的报警阈值
uint8_t selectedThresholdIndex = 0; // 默认选中第 0 行（温度）

float tempThreshold = 30.0;
uint8_t humiThreshold = 60;
int airQThreshold = 200;
int pressThreshold = 1010;
float pm25Threshold = 75.0;


//BMP180Data_T g_tBMP180 = {0};

uint8_t not_init = 1; // 是否初始化完成，初始化完成才显示数据页面
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

// ------- ESP8266 -------------
uint8_t esp_buff[1024] = {0};
uint16_t esp_cnt = 0;
// ------- ESP8266 -------------

volatile OLED_Page_t currentPage = PAGE_SENSOR_DATA; // OLED 页数 变量定义

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

    // 读取 ADC1
    HAL_ADC_Start(&hadc1);  // 启动 ADC1
    HAL_ADC_PollForConversion(&hadc1, HAL_MAX_DELAY);  // 等待转换完成
    adc1_value = HAL_ADC_GetValue(&hadc1);  // 获取 ADC1 值
    HAL_ADC_Stop(&hadc1);  // 停止 ADC1

    // 读取 ADC2
    HAL_ADC_Start(&hadc2);  // 启动 ADC2
    HAL_ADC_PollForConversion(&hadc2, HAL_MAX_DELAY);  // 等待转换完成
    adc2_value = HAL_ADC_GetValue(&hadc2);  // 获取 ADC2 值
    HAL_ADC_Stop(&hadc2);  // 停止 ADC2

    // 计算电压
    voltage1 = (SYS_VOLTAGE / 4096.0) * adc1_value;
    voltage2 = (SYS_VOLTAGE / 4096.0) * adc2_value;

    // 打印 ADC 结果
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
  HAL_TIM_Base_Start_IT(&htim2); // 开启中断
  HAL_TIM_Base_Start_IT(&htim3); // 开启中断
  BMP180_Init();
  PM25_Init(); // 初始化 PM2.5 传感器
  OLED_Init(); // OLED 初始化
  HAL_Delay(1000);
  OLED_Clear();
  OLED_ShowString(2, 0, "Init WIFI ...", OLED_8X16);
  OLED_Update();
  // -------阿里云MQTT初始化-----------
  /*
  ESP8266_Init();
  Ali_Yun_Init();
  SensorData sensor = {25.5, 60, 0.8}; // 传感器数据
  */
  // -------阿里云MQTT初始化-----------
  HAL_Delay(3000);

 //---------------------
   not_init = 0; // 初始化都完成 进行数据显示

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
      // 获取 ADC 值


    //printf("温度%.2f",g_tBMP180.fPressure);
      /*
    // 更新传感器数据
    sensor.temp = temperature; // 更新温度
    sensor.humi = humidity;     // 更新湿度
    sensor.airque = airQStr; // 更新空气质量

    // 发送数据
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

//阿里云数据上传

void Ali_Yun_Send(SensorData *sensor) {
    char msg_buf[1024];

    // 手动拼接字符串，包含空气质量数据
    sprintf(msg_buf, "AT+MQTTPUB=0,\"/sys/k282yy0B25v/ESP8266_dev/thing/event/property/post\",\"{params:{\\\"temp\\\":%.1f\\,\\\"humi\\\":%d\\,\\\"airque\\\":%.2f}}\",0,0\r\n", sensor->temp, sensor->humi, sensor->airque);

    // 打印和发送数据
    printf("开始发送数据: %s\r\n", msg_buf);
    ESP8266_SendCmd(msg_buf, "OK");
    ESP8266_Clear();
}

//uint8_t cjson_err_num = 0;  // cJSON 解析错误的次数，用于判断解析失败的次数，避免无限解析失败

// 接收并解析数据的函数

/*
void Ali_Yun_GetRCV(void)
{
    cJSON *cjson = NULL;  // 用于存储解析后的 cJSON 对象
    int num;  // 用于存储订阅消息的字节数
    char topic_buff[256];  // 存储订阅的主题
    char recv_buffer[ESPBUFF_MAX_SIZE];  // 存储接收到的消息内容
    char *ptr_recv = strstr((const char *)esp_buff,"+MQTTSUBRECV");  // 查找接收到的订阅消息标识符 "+MQTTSUBRECV"
    
    // 如果找到了订阅消息标识符
    if(ptr_recv != NULL)  // 存在接收到的MQTT订阅消息
    {
        memset(topic_buff, 0, sizeof(topic_buff));  // 清空 topic_buff
        // 解析接收到的数据，提取主题、字节数和消息内容
        sscanf((char *)esp_buff,"+MQTTSUBRECV:0,%[^,],%d,%s", topic_buff, &num, recv_buffer);
        
        // 判断主题是否匹配预定义的主题（ALI_TOPIC_SET）
        if(strstr(topic_buff, ALI_TOPIC_SET))  // 判断接收到的主题是否是指定的主题
        {
            printf("========================数据解析开始===========================\r\n");
            printf("接收数据成功，开始解析  %s\r\n", recv_buffer);
            
            // 使用 cJSON 解析接收到的消息内容
            cjson = cJSON_Parse(recv_buffer);  
            
            // 如果 cJSON 解析失败
            if(cjson == NULL)
            {
                printf("cjson 解析错误\r\n");
                cjson_err_num++;  // 增加解析错误次数
                if(cjson_err_num > 3)  // 如果连续解析错误超过 3 次
                {
                    ESP8266_Clear();  // 清空接收缓存
                    cjson_err_num = 0;  // 重置解析错误次数
                }            
                printf("========================数据解析失败===========================\r\n");
            }
            else
            {
                // 解析成功，获取 JSON 数据中的 "params" 对象
                cJSON *json_data = NULL;
                json_data = cJSON_GetObjectItem(cjson, "params");
                cjson_err_num = 0;  // 解析成功，重置解析错误次数
                
                // 如果 "params" 对象不存在
                if(json_data == NULL)
                {
                    printf("cjson 没有数据\r\n");
                    return;  // 如果没有数据，则直接返回
                }
                else
                {
                    // 输出解析后的 JSON 内存大小（不代表数据量，只是内存空间大小）
                    printf("cjson 内存大小为 = %d\r\n", sizeof(cjson));
                    
                    // 解析数据部分
                    if(cJSON_GetObjectItem(json_data, "temperature") != NULL)  // 判断是否包含 "temperature" 字段
                    {
                        temp_value = cJSON_GetObjectItem(json_data, "temperature")->valuedouble;  // 获取温度值
                        printf("cjson 解析成功 temp_value = %f\r\n", temp_value);  // 输出温度值
                    }
                    
                    if(cJSON_GetObjectItem(json_data, "Humidity") != NULL)  // 判断是否包含 "Humidity" 字段
                    {
                        humi_value = cJSON_GetObjectItem(json_data, "Humidity")->valuedouble;  // 获取湿度值
                        printf("cjson 解析成功 Humidity = %f\r\n", humi_value);  // 输出湿度值
                    }       
                }
                ESP8266_Clear();  // 清空接收缓存
                cJSON_Delete(cjson);  // 释放 cJSON 对象
                printf("========================数据解析成功===========================\r\n");
            }
        }
    }
}
*/


// PAGE 1数据显示
void Display_SensorData(void) {
    char buffer[5][20]; // 存储格式化后的字符串

    sprintf(buffer[0], "Temp:   %.1f C", temperature);   // 温度（摄氏度）
    sprintf(buffer[1], "Humi:   %u %%", humidity);       // 湿度（百分比）
    sprintf(buffer[2], "AirQ:   %d ppm", airQuality);    // 空气质量（ppm）
    sprintf(buffer[3], "Press:  %d hPa", (int)(g_tBMP180.fPressure / 100)); // 气压（hPa）
    sprintf(buffer[4], "PM2.5:  %.2f ug/m3", density);   // PM2.5 浓度（ug/m3）

    // 清除 OLED 显示，避免残留
    OLED_Clear();

    // 逐行显示传感器数据
    for (int i = 0; i < 5; i++) {
        OLED_ShowString(2, i * 10, buffer[i], OLED_6X8);
    }
}

// 阈值显示
// PAGE 2 显示阈值设定
// PAGE 2 显示阈值设定，支持闪烁显示
void Display_Thresholds(void) {
    char buffer[5][20]; // 存储格式化后的字符串

    sprintf(buffer[0], "Temp Th:  %.1f C", tempThreshold);    // 温度阈值
    sprintf(buffer[1], "Humi Th:  %u %%", humiThreshold);     // 湿度阈值
    sprintf(buffer[2], "AirQ Th:  %d ppm", airQThreshold);    // 空气质量阈值
    sprintf(buffer[3], "Press Th: %d hPa", pressThreshold);   // 气压阈值
    sprintf(buffer[4], "PM2.5 Th: %.2f ug/m3", pm25Threshold); // PM2.5 阈值

    // 清除 OLED 显示，避免残留
    OLED_Clear();

    // 获取当前系统时间
    uint32_t currentTime = HAL_GetTick();

    // 逐行显示阈值设定
    for (int i = 0; i < 5; i++) {
        if (i == selectedThresholdIndex) {
            // 让当前选中的行每 500ms 闪烁一次
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
        case PAGE_SENSOR_DATA:  // 传感器数据页面
            Display_SensorData();
            break;

        case PAGE_ALARM_THRESHOLDS:  // 报警阈值页面
            Display_Thresholds();
            break;

        case PAGE_THREE:  // 第三页
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
