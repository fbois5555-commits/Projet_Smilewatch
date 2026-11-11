/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "stm32wbxx_hal.h"

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

/* USER CODE BEGIN Private defines */
/* ============================================================================
 * SmileWatch V2 Hardware Pinout (STM32WB55VGYx WLCSP-100 on Carte_V2.kicad_pcb)
 * ==========================================================================*/
/* Push-Buttons: S2 (Top 2h = PC5, ball H5) & S1 (Bottom 4h = PC4, ball G4) */
#define BTN_TOP_2H_Pin          GPIO_PIN_5
#define BTN_TOP_2H_GPIO_Port    GPIOC
#define BTN_BOT_4H_Pin          GPIO_PIN_4
#define BTN_BOT_4H_GPIO_Port    GPIOC

/* Miyota 2035 Stepping Coil: TP6 H+ (PA7, ball H6) & TP7 H- (PB5, ball D6) */
#define MIYOTA_HP_Pin           GPIO_PIN_7
#define MIYOTA_HP_GPIO_Port     GPIOA
#define MIYOTA_HM_Pin           GPIO_PIN_5
#define MIYOTA_HM_GPIO_Port     GPIOB

/* OLED 0.5" QG-8848TSWYG02 (CH1115): /Ecran/GPIO_EC (PD7, ball A8) & /Ecran/RES (PB4, ball C7) */
#define OLED_PWR_EN_Pin         GPIO_PIN_7
#define OLED_PWR_EN_GPIO_Port   GPIOD
#define OLED_RES_Pin            GPIO_PIN_4
#define OLED_RES_GPIO_Port      GPIOB

/* Subsystems Enable: DRV2605L (PA1, ball G6), TCA9517D (PA4, ball K10), +5V_HR Boost (PB0, ball H2) */
#define DRV_EN_Pin              GPIO_PIN_1
#define DRV_EN_GPIO_Port        GPIOA
#define TCA_EN_Pin              GPIO_PIN_4
#define TCA_EN_GPIO_Port        GPIOA
#define HR_5V_EN_Pin            GPIO_PIN_0
#define HR_5V_EN_GPIO_Port      GPIOB

/* Sensor Interrupts & Charger Status */
#define HR_INT_Pin              GPIO_PIN_6
#define HR_INT_GPIO_Port        GPIOB
#define TEMP_ALERT_Pin          GPIO_PIN_9
#define TEMP_ALERT_GPIO_Port    GPIOA
#define IMU_INTSA_Pin           GPIO_PIN_10
#define IMU_INTSA_GPIO_Port     GPIOA
#define IMU_INTPA_Pin           GPIO_PIN_15
#define IMU_INTPA_GPIO_Port     GPIOA
#define ALS_INT_Pin             GPIO_PIN_12
#define ALS_INT_GPIO_Port       GPIOA
#define BQ_CHG_Pin              GPIO_PIN_2
#define BQ_CHG_GPIO_Port        GPIOB
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
