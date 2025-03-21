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
#include "iwdg.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"
#include "usbd_cdc_if.h"

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
uint16_t airQThreshold = 200;
int pressThreshold = 1010;
float pm25Threshold = 75.0;


//BMP180Data_T g_tBMP180 = {0};
uint8_t ThresholdsChanged = 1; // 阈值是否被改变
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

// 发送到阿里云的数据模型
typedef struct {
    float temp;
    uint8_t humi;
    uint16_t airque;
    uint16_t airpress;
    float PM;
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
  MX_IWDG_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim2); // 开启中断
  HAL_TIM_Base_Start_IT(&htim3); // 开启中断
  BMP180_Init();
  PM25_Init(); // 初始化 PM2.5 传感器
  OLED_Init(); // OLED 初始化
  HAL_Delay(300);

  OLED_Clear();
  OLED_ShowString(2, 0, "Init WIFI ...", OLED_8X16);
  
  OLED_Update();
  // -------阿里云MQTT初始化-----------
  ESP8266_Init();
  Ali_Yun_Init();
  SensorData sensor = {25.5, 20, 88, 99, 0.7}; // 传感器数据
  // -------阿里云MQTT初始化-----------
  HAL_GPIO_WritePin(LED_STATE_GPIO_Port, LED_STATE_Pin, GPIO_PIN_SET); // 状态灯显示
  HAL_GPIO_WritePin(LED_POWER_GPIO_Port, LED_POWER_Pin, GPIO_PIN_SET); // 传输指示显示
  HAL_Delay(500);

 //---------------------
   not_init = 0; // 初始化都完成 进行数据显示

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    HAL_GPIO_TogglePin(LED_POWER_GPIO_Port, LED_POWER_Pin);
    readData();  // 读取传感器数据
    CheckThreshold(); // 阈值检测开启报警

    // 更新传感器数据
    sensor.temp = temperature; 
    sensor.humi = humidity;     
    sensor.airque = airQuality; 
    sensor.airpress = (int)(g_tBMP180.fPressure / 100);
    sensor.PM = density;
       // 检查新数据标志位
    if (newDataFlag)
    {
        // 调用解析函数
        Ali_Yun_GetRCV();
        newDataFlag = 0;  // 重置标志位
    }
    // 发送传感器数据到云端
    Ali_Yun_Send(&sensor);
    HAL_Delay(1000);  // 控制发送频率
    Send_Thresholds(); // 发送当前阈值设置

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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI|RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.LSIState = RCC_LSI_ON;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC|RCC_PERIPHCLK_USB;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

//阿里云数据上传
/*
    sensor.temp = temperature; // 四舍五入到一位小数
    sensor.humi = humidity;     // 更新湿度
    sensor.airque = airQuality; // 更新空气质量
    sensor.airpress =  (int)(g_tBMP180.fPressure / 100);
    sensor.PM = density;
*/
void Send_Thresholds(void) {
    if(!ThresholdsChanged) return;
    char json_buf[256];  // JSON 数据缓冲区
    char msg_buf[512];   // AT 指令缓冲区

    // 1. 创建 cJSON 对象
    cJSON *root = cJSON_CreateObject();
    if (root == NULL) {
        printf("Error creating JSON object.\n");
        return;  // 错误处理
    }

    cJSON *params = cJSON_CreateObject();
    if (params == NULL) {
        cJSON_Delete(root);
        printf("Error creating JSON params object.\n");
        return;  // 错误处理
    }
    
    cJSON_AddItemToObject(root, "params", params);
    
    // 2. 添加阈值数据
    cJSON_AddNumberToObject(params, "tempThreshold", tempThreshold); // 温度阈值
    cJSON_AddNumberToObject(params, "humiThreshold", humiThreshold); // 湿度阈值
    cJSON_AddNumberToObject(params, "airQThreshold", airQThreshold); // 空气质量阈值
    cJSON_AddNumberToObject(params, "pressThreshold", pressThreshold); // 压力阈值
    cJSON_AddNumberToObject(params, "pm25Threshold", pm25Threshold); // PM2.5 阈值
    
    // 3. 生成 JSON 字符串
    char *json_str = cJSON_PrintUnformatted(root);
    cJSON_Delete(root); // 释放 JSON 对象

    if (json_str == NULL) {
        printf("Error creating JSON string.\n");
        return;  // 错误处理
    }

    // 4. 手动替换逗号 "," 和双引号 "\"" 为 "\,"
    int i, j = 0;
    for (i = 0; json_str[i] != '\0'; i++) {
        if (json_str[i] == ',' || json_str[i] == '\"') {
            json_buf[j++] = '\\';  // 插入转义字符 "\"
        }
        json_buf[j++] = json_str[i];
    }
    json_buf[j] = '\0'; // 确保字符串以 null 结尾

    // 5. 组合 AT 指令
    snprintf(msg_buf, sizeof(msg_buf),
             "AT+MQTTPUB=0,\"/sys/k282yy0B25v/ESP8266_dev/thing/event/property/post\",\"%s\",0,0\r\n",
             json_buf);

    // 6. 调试输出 & 发送数据
    //printf("调试 JSON: %s\n", json_buf);
    //printf("开始发送数据: %s\r\n", msg_buf);
    ESP8266_SendCmd(msg_buf, "OK");
    ESP8266_Clear();

    // 7. 释放 JSON 字符串
    free(json_str);
    ThresholdsChanged = 0;
}

void Ali_Yun_Send(SensorData *sensor) {
    char json_buf[128];
    char msg_buf[256];
    char tempStr[10];
    char PMStr[10];

    // 1. 格式化温度，保留一位小数
    snprintf(tempStr, sizeof(tempStr), "%.1f", sensor->temp);
    // 1. 格式化PM，保留两位小数
    snprintf(PMStr, sizeof(PMStr), "%.2f", sensor->PM);
    
    // 2. 创建 cJSON 对象
    cJSON *root = cJSON_CreateObject();
    cJSON *params = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "params", params);
    
    // 使用格式化后的浮点数值而非字符串
    cJSON_AddNumberToObject(params, "temp", atof(tempStr)); // 转换为浮点数
    cJSON_AddNumberToObject(params, "humi", sensor->humi);
    cJSON_AddNumberToObject(params, "airque", sensor->airque);
    cJSON_AddNumberToObject(params, "airpress", sensor->airpress);
    cJSON_AddNumberToObject(params, "PM", atof(PMStr));
    
    // 3. 生成 JSON 字符串
    char *json_str = cJSON_PrintUnformatted(root);
    cJSON_Delete(root); // 释放 JSON 对象

    // 4. 手动替换逗号 "," 和双引号 "\"" 为 "\,"
    int i, j = 0;
    for (i = 0; json_str[i] != '\0'; i++) {
        if (json_str[i] == ',' || json_str[i] == '\"') {
            json_buf[j++] = '\\';  // 插入转义字符 "\"
        }
        json_buf[j++] = json_str[i];
    }
    json_buf[j] = '\0'; // 确保字符串以 null 结尾

    // 5. 组合 AT 指令
    snprintf(msg_buf, sizeof(msg_buf),
             "AT+MQTTPUB=0,\"/sys/k282yy0B25v/ESP8266_dev/thing/event/property/post\",\"%s\",0,0\r\n",
             json_buf);

    // 6. 调试输出 & 发送数据
    //printf("调试 JSON: %s\n", json_buf);
    //printf("开始发送数据: %s\r\n", msg_buf);
    ESP8266_SendCmd(msg_buf, "OK");
    ESP8266_Clear();

    // 7. 释放 JSON 字符串
    free(json_str);
}

/// 定义最大尝试次数
#define MAX_RETRY_COUNT 3

// MQTT 数据解析函数
void Ali_Yun_GetRCV(void)
{
    cJSON *cjson = NULL;
    int num;
    char topic_buff[256];
    char recv_buffer[ESPBUFF_MAX_SIZE];
    char *ptr_recv = strstr((const char *)esp_buff, "+MQTTSUBRECV");

    if (ptr_recv != NULL)  // 存在接收到的 MQTT 订阅消息
    {
        memset(topic_buff, 0, sizeof(topic_buff));
        sscanf((char *)esp_buff, "+MQTTSUBRECV:0,%[^,],%d,%s", topic_buff, &num, recv_buffer);

        if (strstr(topic_buff, ALI_TOPIC_SET))  // 确认接收到的主题是指定主题
        {
            printf("========================数据解析开始===========================\r\n");
            printf("接收数据成功，开始解析  %s\r\n", recv_buffer);

            int retry_count = 0;
            while (retry_count < MAX_RETRY_COUNT)
            {
                cjson = cJSON_Parse(recv_buffer);
                if (cjson != NULL)  // 解析成功
                {
                    // 获取 items 对象
                    cJSON *items = cJSON_GetObjectItem(cjson, "items");
                    if (items != NULL)  // 如果找到 items 对象
                    {
                        // 解析温度阈值
                        cJSON *tempThresholdItem = cJSON_GetObjectItem(items, "tempThreshold");
                        if (tempThresholdItem != NULL)
                        {
                            cJSON *value = cJSON_GetObjectItem(tempThresholdItem, "value");
                            if (value != NULL && cJSON_IsNumber(value))
                            {
                                tempThreshold = (float)value->valuedouble;
                                printf("更新成功 tempThreshold = %f\r\n", tempThreshold);
                            }
                        }

                        // 解析湿度阈值
                        cJSON *humiThresholdItem = cJSON_GetObjectItem(items, "humiThreshold");
                        if (humiThresholdItem != NULL)
                        {
                            cJSON *value = cJSON_GetObjectItem(humiThresholdItem, "value");
                            if (value != NULL && cJSON_IsNumber(value))
                            {
                                humiThreshold = (uint8_t)value->valueint;
                                printf("更新成功 humiThreshold = %d\r\n", humiThreshold);
                            }
                        }

                        // 解析其他阈值
                        // ...

                        break;  // 成功解析，跳出重试循环
                    }
                    else
                    {
                        printf("未找到 items 数据，重试中...\r\n");
                    }
                }
                else
                {
                    printf("cjson 解析错误，重试中...\r\n");
                }

                retry_count++;
                // 可选择在每次重试之间增加延迟
                HAL_Delay(100);  // 等待一段时间再重试
            }

            // 如果到达最大尝试次数，打印提示
            if (retry_count >= MAX_RETRY_COUNT)
            {
                printf("达到最大重试次数，解析失败。\r\n");
            }

            ESP8266_Clear();   // 清空接收缓存
            cJSON_Delete(cjson);  // 释放 cJSON 对象
            printf("========================数据解析结束===========================\r\n");
        }
    }
}

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
    sprintf(buffer[1], "Humi:   %u RH%%", humidity);       // 湿度（百分比）
    sprintf(buffer[2], "AirQ:   %u ppm", airQuality);    // 空气质量（ppm）
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
    sprintf(buffer[1], "Humi Th:  %u RH%%", humiThreshold);     // 湿度阈值
    sprintf(buffer[2], "AirQ Th:  %u ppm", airQThreshold);    // 空气质量阈值
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
    // 气体质量
    MQ135_Init();
    MQ135_ReadData(&airQuality);
    // 温\湿度
    DHT11_Read_Data(&temperature, &humidity);
    // 气压
    BMP180_StartPressureSample();
    HAL_Delay(10);
    BMP180_GetTemp(&g_tBMP180);
    BMP180_StartPressureSample();
    HAL_Delay(100);
    BMP180_GetPressureAltitude(&g_tBMP180);
    // PM2.5检测
    density = PM25_ReadDensity();
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
