/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
extern  void showData(void);
extern  void readData(void);
void Update_Display(void);
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
// 共享的环境数据
extern float temperature;
extern float density;
extern uint8_t humidity;
extern uint16_t airQuality;

// 共享的报警阈值
extern uint8_t selectedThresholdIndex;
extern float tempThreshold;
extern uint8_t humiThreshold;
extern uint16_t airQThreshold;
extern int pressThreshold;
extern float pm25Threshold;
extern uint8_t ThresholdsChanged;
extern volatile uint8_t newDataFlag;  // 标志位，指示新数据可供处理
// 发送阈值

// ESP8266接收缓冲区


/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
void Send_Thresholds(void);
void Ali_Yun_GetRCV(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_Pin GPIO_PIN_13
#define LED_GPIO_Port GPIOC
#define ILED_Pin GPIO_PIN_5
#define ILED_GPIO_Port GPIOA
#define LED_STATE_Pin GPIO_PIN_6
#define LED_STATE_GPIO_Port GPIOA
#define DHT11_Pin GPIO_PIN_7
#define DHT11_GPIO_Port GPIOA
#define KEY3_Pin GPIO_PIN_14
#define KEY3_GPIO_Port GPIOB
#define KEY3_EXTI_IRQn EXTI15_10_IRQn
#define KEY2_Pin GPIO_PIN_15
#define KEY2_GPIO_Port GPIOB
#define KEY2_EXTI_IRQn EXTI15_10_IRQn
#define KEY1_Pin GPIO_PIN_8
#define KEY1_GPIO_Port GPIOA
#define KEY1_EXTI_IRQn EXTI9_5_IRQn
#define OLED_SCL_Pin GPIO_PIN_6
#define OLED_SCL_GPIO_Port GPIOB
#define OLED_SDA_Pin GPIO_PIN_7
#define OLED_SDA_GPIO_Port GPIOB
#define LED_POWER_Pin GPIO_PIN_8
#define LED_POWER_GPIO_Port GPIOB
#define BEEP_Pin GPIO_PIN_9
#define BEEP_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
