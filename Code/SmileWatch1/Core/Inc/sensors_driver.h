/**
  ******************************************************************************
  * @file           : sensors_driver.h
  * @brief          : Real I2C1 Sensor & Actuator Drivers for SmileWatch V2 PCB
  *                   LSM6DSOX (0x6A), MAX30101 (0x57), TMP117 (0x48),
  *                   VEML6030 (0x10), DRV2605L (0x5A) + BQ24040 ~CHG (PB2)
  ******************************************************************************
  */

#ifndef INC_SENSORS_DRIVER_H_
#define INC_SENSORS_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "main.h"

/* 7-bit I2C addresses shifted left for STM32 HAL I2C functions */
#define LSM6DSOX_ADDR    (0x6AU << 1U)
#define MAX30101_ADDR    (0x57U << 1U)
#define TMP117_ADDR      (0x48U << 1U)
#define VEML6030_ADDR    (0x10U << 1U)
#define DRV2605L_ADDR    (0x5AU << 1U)

typedef struct {
    bool imu_ok;
    bool hr_ok;
    bool temp_ok;
    bool als_ok;
    bool drv_ok;
} SW_SensorPresence_t;

typedef struct {
    bool     wrist_flick_evt;
    bool     fall_detected_evt;
    uint32_t steps_delta;
    int16_t  temp_deci_c;
    bool     temp_valid;
    uint16_t ambient_lux;
    bool     als_valid;
    uint8_t  heart_bpm;
    uint8_t  spo2_pct;
    bool     hr_valid;
    uint16_t hrv_rmssd_ms;
    bool     hrv_valid;
    bool     imu_still;
    uint16_t imu_energy_mg;
    uint16_t imu_gyro_dps;
    bool     qi_charging;
} SW_SensorSample_t;

void Sensors_Init(I2C_HandleTypeDef *hi2c, SW_SensorPresence_t *presence);
void Sensors_Poll(I2C_HandleTypeDef *hi2c,
                  uint32_t now_ms,
                  bool hr_enabled,
                  uint16_t mlc_thresh_mg,
                  SW_SensorSample_t *out);
void Sensors_PlayHaptic(I2C_HandleTypeDef *hi2c, uint8_t effect_id);
void Sensors_SetImuThreshold(I2C_HandleTypeDef *hi2c, uint16_t thresh_mg);

#ifdef __cplusplus
}
#endif

#endif /* INC_SENSORS_DRIVER_H_ */
