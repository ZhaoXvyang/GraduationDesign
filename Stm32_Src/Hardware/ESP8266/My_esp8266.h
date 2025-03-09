#ifndef __MY_ESP8266_H
#define __MY_ESP8266_H

#include "stm32f1xx_hal.h"  // ������ľ���MCU�ͺŵ���
#include <string.h>



#define		 ALI_USERNAME		"ESP8266_dev&k282yy0B25v"       // �û���
#define		 ALICLIENTLD		"k282yy0B25v.ESP8266_dev|securemode=2\\,signmethod=hmacsha256\\,timestamp=1741508275851|"				// �ͻ�id
#define		 ALI_PASSWD			"9f0ec45119ad01411e2fd87fa9dd0403a610529d00b7446f9e514d6c77aa8efd"           // MQTT ����
#define		 ALI_MQTT_HOSTURL	"iot-06z00fmnyhez6wa.mqtt.iothub.aliyuncs.com"			// mqtt���ӵ���ַ
#define		 ALI_PORT			"1883"				// �˿�

#define      ALI_TOPIC_SET      "/sys/k282yy0B25v/ESP8266_dev/thing/service/property/set"
#define      ALI_TOPIC_POST     "/sys/k282yy0B25v/ESP8266_dev/thing/event/property/post"

#define ESPBUFF_MAX_SIZE  1024  // ����ʵ����Ҫ����
extern uint8_t esp_buff[ESPBUFF_MAX_SIZE];  // �洢���յ���ESP8266����
extern uint16_t esp_cnt;  // �������ٽ��յ����ݳ���

// ��ʼ��ESP8266
void ESP8266_Init(void);

// ����AT����ȴ���Ӧ
uint8_t ESP8266_SendCmd(uint8_t *cmd, uint8_t *res);

// ������ջ�����
void ESP8266_Clear(void);

// ͨ�����ڷ����ַ���
void ESP8266_SendString(uint8_t *str, uint8_t len);

// ���Ӱ�����
void Ali_Yun_Init(void);

// ����Topic
void Ali_Yun_Topic(void);


#endif // __MY_ESP8266_H
