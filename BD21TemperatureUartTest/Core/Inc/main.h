/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32f4xx_hal.h"

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

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define KEY_READ_Pin GPIO_PIN_13
#define KEY_READ_GPIO_Port GPIOC
#define KEY_CLEAR_Pin GPIO_PIN_14
#define KEY_CLEAR_GPIO_Port GPIOC
#define KEY_TEST_Pin GPIO_PIN_15
#define KEY_TEST_GPIO_Port GPIOC
#define LED_19_Pin GPIO_PIN_2
#define LED_19_GPIO_Port GPIOC
#define LED_20_Pin GPIO_PIN_3
#define LED_20_GPIO_Port GPIOC
#define LED_1_Pin GPIO_PIN_0
#define LED_1_GPIO_Port GPIOB
#define LED_2_Pin GPIO_PIN_1
#define LED_2_GPIO_Port GPIOB
#define LED_3_Pin GPIO_PIN_12
#define LED_3_GPIO_Port GPIOB
#define LED_4_Pin GPIO_PIN_13
#define LED_4_GPIO_Port GPIOB
#define LED_5_Pin GPIO_PIN_14
#define LED_5_GPIO_Port GPIOB
#define LED_6_Pin GPIO_PIN_15
#define LED_6_GPIO_Port GPIOB
#define LED_7_Pin GPIO_PIN_8
#define LED_7_GPIO_Port GPIOC
#define LED_8_Pin GPIO_PIN_9
#define LED_8_GPIO_Port GPIOC
#define LED_9_Pin GPIO_PIN_8
#define LED_9_GPIO_Port GPIOA
#define LED_10_Pin GPIO_PIN_11
#define LED_10_GPIO_Port GPIOA
#define LED_11_Pin GPIO_PIN_12
#define LED_11_GPIO_Port GPIOA
#define LED_12_Pin GPIO_PIN_10
#define LED_12_GPIO_Port GPIOC
#define LED_13_Pin GPIO_PIN_11
#define LED_13_GPIO_Port GPIOC
#define LED_STATUS_Pin GPIO_PIN_3
#define LED_STATUS_GPIO_Port GPIOB
#define LED_14_Pin GPIO_PIN_5
#define LED_14_GPIO_Port GPIOB
#define LED_15_Pin GPIO_PIN_6
#define LED_15_GPIO_Port GPIOB
#define LED_16_Pin GPIO_PIN_7
#define LED_16_GPIO_Port GPIOB
#define LED_17_Pin GPIO_PIN_8
#define LED_17_GPIO_Port GPIOB
#define LED_18_Pin GPIO_PIN_9
#define LED_18_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* Base address of the Flash sectors */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbytes */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbytes */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
