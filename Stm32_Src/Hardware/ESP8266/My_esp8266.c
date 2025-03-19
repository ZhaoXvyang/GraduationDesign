#include "my_esp8266.h"
#include "usart.h"
#include "stdio.h"
#include "string.h"
#include "oled.h"

// WiFi名称和密码
#define WIFI_NAME    "Xiaomi_14"
#define WIFI_PASS    "z12345678"

// ESP8266初始化
void ESP8266_Init(void)
{
    HAL_UART_Receive_DMA(&huart2, esp_buff, ESPBUFF_MAX_SIZE);  // 开启DMA接收
    __HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);  // 开启串口空闲中断

    // 重置ESP8266
    while(ESP8266_SendCmd("AT+RST\r\n", "ready")) {}
    while(ESP8266_SendCmd("AT\r\n", "OK")) {}
    while(ESP8266_SendCmd("AT+CWMODE=1\r\n", "OK")) {}

    // 连接WiFi
    while(ESP8266_SendCmd("AT+CWJAP=\"" WIFI_NAME "\",\"" WIFI_PASS "\"\r\n", "OK")) {}
    printf("success\n");
}

// 清空接收缓冲区
void ESP8266_Clear()
{
    memset(esp_buff, 0, sizeof(esp_buff));
    esp_cnt = 0;
}

// 发送字符串
void ESP8266_SendString(uint8_t *str, uint8_t len)
{
    uint8_t i = 0;
    for (i = 0; i < len; i++) {
        USART2->DR = *str;
        str++;
        HAL_Delay(1);
    }
}

// 发送AT命令并等待响应
uint8_t ESP8266_SendCmd(uint8_t *cmd, uint8_t *res)
{
    uint8_t num = 200;
    ESP8266_Clear();
    
    // 发送AT命令
    ESP8266_SendString(cmd, strlen((const char *)cmd));
    // 等待响应
    while (num--) {
        if (strstr((const char *)esp_buff, (const char *)res) != NULL) {
            ESP8266_Clear();
            return 0;  // 成功找到响应
        }
        HAL_Delay(10);  // 等待10ms
    }
    
    return 1;  // 超时或未找到响应
}

void Ali_Yun_Init(void)
{
    
	while(ESP8266_SendCmd("AT+MQTTUSERCFG=0,1,\"NULL\",\""ALI_USERNAME"\",\""ALI_PASSWD"\",0,0,\"\"\r\n","OK")){}
	HAL_Delay(10);
        
	while(ESP8266_SendCmd("AT+MQTTCLIENTID=0,\""ALICLIENTLD"\"\r\n","OK")){}
		
	while(ESP8266_SendCmd("AT+MQTTCONN=0,\""ALI_MQTT_HOSTURL"\",1883,1\r\n","OK")){}
    printf("Ali_Yun_Init OK!\r\n");
	Ali_Yun_Topic();
}

void Ali_Yun_Topic(void)
{

	while(ESP8266_SendCmd("AT+MQTTSUB=0,\""ALI_TOPIC_SET"\",0\r\n","OK")){}
		
	while(ESP8266_SendCmd("AT+MQTTSUB=0,\""ALI_TOPIC_POST"\",0\r\n","OK")){}
        
    printf("Ali_Yun_Topic OK!\r\n");
}

