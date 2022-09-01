/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "stm32l4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define JOY_V1_Pin GPIO_PIN_0
#define JOY_V1_GPIO_Port GPIOC
#define JOY_V2_Pin GPIO_PIN_1
#define JOY_V2_GPIO_Port GPIOC
#define JOY_H1_Pin GPIO_PIN_2
#define JOY_H1_GPIO_Port GPIOC
#define JOY_H2_Pin GPIO_PIN_3
#define JOY_H2_GPIO_Port GPIOC
#define HALL_1_Pin GPIO_PIN_0
#define HALL_1_GPIO_Port GPIOA
#define HALL_2_Pin GPIO_PIN_1
#define HALL_2_GPIO_Port GPIOA
#define VBAT_Pin GPIO_PIN_2
#define VBAT_GPIO_Port GPIOA
#define R1_Pin GPIO_PIN_4
#define R1_GPIO_Port GPIOA
#define R2_Pin GPIO_PIN_5
#define R2_GPIO_Port GPIOA
#define R3_Pin GPIO_PIN_6
#define R3_GPIO_Port GPIOA
#define R4_Pin GPIO_PIN_7
#define R4_GPIO_Port GPIOA
#define R5_Pin GPIO_PIN_4
#define R5_GPIO_Port GPIOC
#define R6_Pin GPIO_PIN_5
#define R6_GPIO_Port GPIOC
#define R7_Pin GPIO_PIN_0
#define R7_GPIO_Port GPIOB
#define C1_Pin GPIO_PIN_1
#define C1_GPIO_Port GPIOB
#define C2_Pin GPIO_PIN_2
#define C2_GPIO_Port GPIOB
#define JOY_B2_Pin GPIO_PIN_10
#define JOY_B2_GPIO_Port GPIOB
#define JOY_B1_Pin GPIO_PIN_11
#define JOY_B1_GPIO_Port GPIOB
#define VIBE_Pin GPIO_PIN_6
#define VIBE_GPIO_Port GPIOC
#define BUZZER_Pin GPIO_PIN_7
#define BUZZER_GPIO_Port GPIOC
#define CHG_CTRL_Pin GPIO_PIN_8
#define CHG_CTRL_GPIO_Port GPIOA
#define MCU_TX_Pin GPIO_PIN_9
#define MCU_TX_GPIO_Port GPIOA
#define MCU_RX_Pin GPIO_PIN_10
#define MCU_RX_GPIO_Port GPIOA
#define LED1_Pin GPIO_PIN_11
#define LED1_GPIO_Port GPIOA
#define LED2_Pin GPIO_PIN_12
#define LED2_GPIO_Port GPIOA
#define PWM_CH1_Pin GPIO_PIN_15
#define PWM_CH1_GPIO_Port GPIOA
#define UART_TX_Pin GPIO_PIN_10
#define UART_TX_GPIO_Port GPIOC
#define UART_RX_Pin GPIO_PIN_11
#define UART_RX_GPIO_Port GPIOC
#define PWM_CH2_Pin GPIO_PIN_3
#define PWM_CH2_GPIO_Port GPIOB
#define LED3_Pin GPIO_PIN_4
#define LED3_GPIO_Port GPIOB
#define LED4_Pin GPIO_PIN_5
#define LED4_GPIO_Port GPIOB
#define LED5_Pin GPIO_PIN_6
#define LED5_GPIO_Port GPIOB
#define LED6_Pin GPIO_PIN_7
#define LED6_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
