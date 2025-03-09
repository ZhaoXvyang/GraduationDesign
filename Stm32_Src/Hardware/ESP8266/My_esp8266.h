#ifndef __MY_ESP8266_H
#define __MY_ESP8266_H

#include "stm32f1xx_hal.h"  // 根据你的具体MCU型号调整
#include <string.h>



#define		 ALI_USERNAME		"ESP8266_dev&k282yy0B25v"       // 用户名
#define		 ALICLIENTLD		"k282yy0B25v.ESP8266_dev|securemode=2\\,signmethod=hmacsha256\\,timestamp=1741508275851|"				// 客户id
#define		 ALI_PASSWD			"9f0ec45119ad01411e2fd87fa9dd0403a610529d00b7446f9e514d6c77aa8efd"           // MQTT 密码
#define		 ALI_MQTT_HOSTURL	"iot-06z00fmnyhez6wa.mqtt.iothub.aliyuncs.com"			// mqtt连接的网址
#define		 ALI_PORT			"1883"				// 端口

#define      ALI_TOPIC_SET      "/sys/k282yy0B25v/ESP8266_dev/thing/service/property/set"
#define      ALI_TOPIC_POST     "/sys/k282yy0B25v/ESP8266_dev/thing/event/property/post"

#define ESPBUFF_MAX_SIZE  1024  // 根据实际需要调整
extern uint8_t esp_buff[ESPBUFF_MAX_SIZE];  // 存储接收到的ESP8266数据
extern uint16_t esp_cnt;  // 用来跟踪接收的数据长度

// 初始化ESP8266
void ESP8266_Init(void);

// 发送AT命令并等待响应
uint8_t ESP8266_SendCmd(uint8_t *cmd, uint8_t *res);

// 清除接收缓冲区
void ESP8266_Clear(void);

// 通过串口发送字符串
void ESP8266_SendString(uint8_t *str, uint8_t len);

// 连接阿里云
void Ali_Yun_Init(void);

// 订阅Topic
void Ali_Yun_Topic(void);


#endif // __MY_ESP8266_H
