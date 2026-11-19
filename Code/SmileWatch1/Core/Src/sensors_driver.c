/**
  ******************************************************************************
  * @file           : sensors_driver.c
  * @brief          : Real I2C1 Sensor & Haptic Drivers for SmileWatch V2 PCB
  ******************************************************************************
  */

#include "sensors_driver.h"
#include "oled_ch1115.h"
#include <string.h>

/* Register definitions */
#define LSM6_REG_CTRL1_XL      0x10U
#define LSM6_REG_CTRL2_G       0x11U
#define LSM6_REG_CTRL3_C       0x12U
#define LSM6_REG_OUTX_L_G      0x22U
#define LSM6_REG_OUTX_L_A      0x28U

#define TMP117_REG_TEMP        0x00U
#define TMP117_REG_CONFIG      0x01U

#define VEML6030_REG_CONF      0x00U
#define VEML6030_REG_ALS       0x04U

#define MAX30101_REG_FIFO_DATA 0x07U
#define MAX30101_REG_MODE_CFG  0x09U
#define MAX30101_REG_SPO2_CFG  0x0AU
#define MAX30101_REG_LED1_PA   0x0CU
#define MAX30101_REG_LED2_PA   0x0DU

#define DRV2605_REG_MODE       0x01U
#define DRV2605_REG_LIB        0x03U
#define DRV2605_REG_WAVESEQ1   0x04U
#define DRV2605_REG_WAVESEQ2   0x05U
#define DRV2605_REG_GO         0x0CU

static SW_SensorPresence_t s_pres = {0};
static uint16_t s_imu_thresh_mg      = 1320U;
static uint32_t s_last_imu_poll_ms   = 0UL;
static uint32_t s_last_slow_poll_ms  = 0UL;
static uint32_t s_last_flick_ms      = 0UL;
static uint32_t s_last_step_ms       = 0UL;
static uint32_t s_freefall_start_ms  = 0UL;
static bool     s_freefall_armed     = false;
static uint32_t s_last_motion_ms     = 0UL;
static bool     s_imu_still_state    = false;
static uint16_t s_imu_energy_mg      = 0U;
static uint16_t s_imu_gyro_dps       = 0U;

static uint32_t s_hr_ir_avg          = 0UL;
static uint32_t s_hr_prev_ir         = 0UL;
static bool     s_hr_rising          = false;
static uint32_t s_hr_last_beat_ms    = 0UL;
static uint8_t  s_hr_bpm_est         = 72U;
static uint8_t  s_hr_spo2_est        = 98U;
static uint16_t s_ibi_buf[8]         = {0};
static uint8_t  s_ibi_idx            = 0U;
static uint8_t  s_ibi_count          = 0U;
static uint16_t s_hrv_rmssd_est      = 52U;

static uint32_t Sensors_ISqrt32(uint32_t val)
{
    uint32_t res = 0UL;
    uint32_t bit = 1UL << 30;
    while (bit > val) {
        bit >>= 2;
    }
    while (bit != 0UL) {
        if (val >= (res + bit)) {
            val -= (res + bit);
            res = (res >> 1) + bit;
        } else {
            res >>= 1;
        }
        bit >>= 2;
    }
    return res;
}

static HAL_StatusTypeDef Sensors_WriteReg(I2C_HandleTypeDef *hi2c, uint16_t dev_addr, uint8_t reg, uint8_t val)
{
    if (hi2c == NULL) return HAL_ERROR;
    return HAL_I2C_Mem_Write(hi2c, dev_addr, reg, I2C_MEMADD_SIZE_8BIT, &val, 1U, 15U);
}

static HAL_StatusTypeDef Sensors_ReadRegs(I2C_HandleTypeDef *hi2c, uint16_t dev_addr, uint8_t reg, uint8_t *buf, uint16_t len)
{
    if ((hi2c == NULL) || (buf == NULL)) return HAL_ERROR;
    return HAL_I2C_Mem_Read(hi2c, dev_addr, reg, I2C_MEMADD_SIZE_8BIT, buf, len, 15U);
}

void Sensors_Init(I2C_HandleTypeDef *hi2c, SW_SensorPresence_t *presence)
{
    memset(&s_pres, 0, sizeof(s_pres));
    HAL_GPIO_WritePin(TCA_EN_GPIO_Port, TCA_EN_Pin, GPIO_PIN_SET);

    if (hi2c != NULL) {
        if (HAL_I2C_IsDeviceReady(hi2c, LSM6DSOX_ADDR, 2U, 20U) == HAL_OK) {
            s_pres.imu_ok = true;
            /* BDU=1 + Auto-Increment, 52Hz +-4g Accel, 52Hz 500dps Gyro */
            (void)Sensors_WriteReg(hi2c, LSM6DSOX_ADDR, LSM6_REG_CTRL3_C, 0x44U);
            (void)Sensors_WriteReg(hi2c, LSM6DSOX_ADDR, LSM6_REG_CTRL1_XL, 0x38U);
            (void)Sensors_WriteReg(hi2c, LSM6DSOX_ADDR, LSM6_REG_CTRL2_G, 0x34U);
        }

        if (HAL_I2C_IsDeviceReady(hi2c, TMP117_ADDR, 2U, 20U) == HAL_OK) {
            s_pres.temp_ok = true;
            uint8_t cfg[2] = {0x02U, 0x20U};
            (void)HAL_I2C_Mem_Write(hi2c, TMP117_ADDR, TMP117_REG_CONFIG, I2C_MEMADD_SIZE_8BIT, cfg, 2U, 15U);
        }

        if (HAL_I2C_IsDeviceReady(hi2c, VEML6030_ADDR, 2U, 20U) == HAL_OK) {
            s_pres.als_ok = true;
            uint8_t conf[2] = {0x00U, 0x00U};
            (void)HAL_I2C_Mem_Write(hi2c, VEML6030_ADDR, VEML6030_REG_CONF, I2C_MEMADD_SIZE_8BIT, conf, 2U, 15U);
        }

        if (HAL_I2C_IsDeviceReady(hi2c, MAX30101_ADDR, 2U, 20U) == HAL_OK) {
            s_pres.hr_ok = true;
            (void)Sensors_WriteReg(hi2c, MAX30101_ADDR, MAX30101_REG_MODE_CFG, 0x03U);
            (void)Sensors_WriteReg(hi2c, MAX30101_ADDR, MAX30101_REG_SPO2_CFG, 0x27U);
            (void)Sensors_WriteReg(hi2c, MAX30101_ADDR, MAX30101_REG_LED1_PA, 0x24U);
            (void)Sensors_WriteReg(hi2c, MAX30101_ADDR, MAX30101_REG_LED2_PA, 0x24U);
        }

        if (HAL_I2C_IsDeviceReady(hi2c, DRV2605L_ADDR, 2U, 20U) == HAL_OK) {
            s_pres.drv_ok = true;
            (void)Sensors_WriteReg(hi2c, DRV2605L_ADDR, DRV2605_REG_MODE, 0x00U);
            (void)Sensors_WriteReg(hi2c, DRV2605L_ADDR, DRV2605_REG_LIB, 0x01U);
        }
    }

    if (presence != NULL) {
        *presence = s_pres;
    }
}

void Sensors_SetImuThreshold(I2C_HandleTypeDef *hi2c, uint16_t thresh_mg)
{
    (void)hi2c;
    if (thresh_mg < 950U) thresh_mg = 950U;
    if (thresh_mg > 2200U) thresh_mg = 2200U;
    s_imu_thresh_mg = thresh_mg;
}

void Sensors_PlayHaptic(I2C_HandleTypeDef *hi2c, uint8_t effect_id)
{
    HAL_GPIO_WritePin(DRV_EN_GPIO_Port, DRV_EN_Pin, GPIO_PIN_SET);
    if (s_pres.drv_ok && (hi2c != NULL)) {
        (void)Sensors_WriteReg(hi2c, DRV2605L_ADDR, DRV2605_REG_WAVESEQ1, effect_id);
        (void)Sensors_WriteReg(hi2c, DRV2605L_ADDR, DRV2605_REG_WAVESEQ2, 0x00U);
        (void)Sensors_WriteReg(hi2c, DRV2605L_ADDR, DRV2605_REG_GO, 0x01U);
    }
}

void Sensors_Poll(I2C_HandleTypeDef *hi2c,
                  uint32_t now_ms,
                  bool hr_enabled,
                  uint16_t mlc_thresh_mg,
                  SW_SensorSample_t *out)
{
    if (out == NULL) return;
    memset(out, 0, sizeof(*out));
    s_imu_thresh_mg = (mlc_thresh_mg > 0U) ? mlc_thresh_mg : s_imu_thresh_mg;

    /* Hardware charger status (~CHG is active low open-drain) & cached stillness */
    out->qi_charging   = (HAL_GPIO_ReadPin(BQ_CHG_GPIO_Port, BQ_CHG_Pin) == GPIO_PIN_RESET);
    out->imu_still     = s_pres.imu_ok ? s_imu_still_state : true;
    out->imu_energy_mg = s_imu_energy_mg;
    out->imu_gyro_dps  = s_imu_gyro_dps;

    if (hi2c == NULL) return;

    /* 1. Fast ~10 Hz IMU & PPG polling (100 ms) */
    if ((now_ms - s_last_imu_poll_ms) >= 100UL) {
        s_last_imu_poll_ms = now_ms;

        if (s_pres.imu_ok) {
            uint8_t raw[12];
            if (Sensors_ReadRegs(hi2c, LSM6DSOX_ADDR, LSM6_REG_OUTX_L_G, raw, 12U) == HAL_OK) {
                int16_t gz_raw = (int16_t)((uint16_t)raw[4] | ((uint16_t)raw[5] << 8));
                int16_t ax_raw = (int16_t)((uint16_t)raw[6] | ((uint16_t)raw[7] << 8));
                int16_t ay_raw = (int16_t)((uint16_t)raw[8] | ((uint16_t)raw[9] << 8));
                int16_t az_raw = (int16_t)((uint16_t)raw[10] | ((uint16_t)raw[11] << 8));

                int32_t gz_dps = ((int32_t)gz_raw * 175L) / 10000L;
                if (gz_dps < 0) gz_dps = -gz_dps;
                if ((gz_dps > 240L) && ((now_ms - s_last_flick_ms) >= 700UL)) {
                    s_last_flick_ms = now_ms;
                    out->wrist_flick_evt = true;
                }

                int32_t ax_mg = ((int32_t)ax_raw * 122L) / 1000L;
                int32_t ay_mg = ((int32_t)ay_raw * 122L) / 1000L;
                int32_t az_mg = ((int32_t)az_raw * 122L) / 1000L;
                uint64_t mag2 = (uint64_t)(ax_mg * ax_mg) +
                                (uint64_t)(ay_mg * ay_mg) +
                                (uint64_t)(az_mg * az_mg);

                /* Motion vs stillness detection & live IMU energy/gyro EMA features */
                uint32_t mag_mg = Sensors_ISqrt32((uint32_t)mag2);
                int32_t dyn_mg = (int32_t)mag_mg - 1000L;
                if (dyn_mg < 0L) dyn_mg = -dyn_mg;
                s_imu_energy_mg = (uint16_t)(((uint32_t)s_imu_energy_mg * 3UL + (uint32_t)dyn_mg + 2UL) / 4UL);
                s_imu_gyro_dps  = (uint16_t)(((uint32_t)s_imu_gyro_dps * 3UL + (uint32_t)gz_dps + 2UL) / 4UL);
                out->imu_energy_mg = s_imu_energy_mg;
                out->imu_gyro_dps  = s_imu_gyro_dps;

                if ((gz_dps > 35L) || (mag2 < (820ULL * 820ULL)) || (mag2 > (1180ULL * 1180ULL))) {
                    s_last_motion_ms = now_ms;
                }
                s_imu_still_state = ((now_ms - s_last_motion_ms) >= 5000UL);
                out->imu_still = s_imu_still_state;

                /* Step peak detector driven by adaptive mlc_thresh_mg */
                uint64_t step_th2 = (uint64_t)s_imu_thresh_mg * (uint64_t)s_imu_thresh_mg;
                if ((mag2 > step_th2) && ((now_ms - s_last_step_ms) >= 320UL)) {
                    s_last_step_ms = now_ms;
                    out->steps_delta = 1UL;
                }

                /* Fall detection: free-fall (< 350 mg) followed by impact (> 2600 mg) */
                if (mag2 < (350ULL * 350ULL)) {
                    s_freefall_armed = true;
                    s_freefall_start_ms = now_ms;
                } else if (s_freefall_armed) {
                    if ((now_ms - s_freefall_start_ms) > 1200UL) {
                        s_freefall_armed = false;
                    } else if (mag2 > (2600ULL * 2600ULL)) {
                        s_freefall_armed = false;
                        out->fall_detected_evt = true;
                    }
                }
            }
        }

        if (hr_enabled && s_pres.hr_ok) {
            uint8_t fifo[6];
            if (Sensors_ReadRegs(hi2c, MAX30101_ADDR, MAX30101_REG_FIFO_DATA, fifo, 6U) == HAL_OK) {
                uint32_t red = (((uint32_t)fifo[0] << 16) | ((uint32_t)fifo[1] << 8) | fifo[2]) & 0x3FFFFUL;
                uint32_t ir  = (((uint32_t)fifo[3] << 16) | ((uint32_t)fifo[4] << 8) | fifo[5]) & 0x3FFFFUL;

                if (ir > 10000UL) {
                    if (s_hr_ir_avg == 0UL) s_hr_ir_avg = ir;
                    s_hr_ir_avg = (s_hr_ir_avg * 7UL + ir) / 8UL;

                    if (ir > s_hr_prev_ir && ir > s_hr_ir_avg) {
                        s_hr_rising = true;
                    } else if (s_hr_rising && ir < s_hr_prev_ir) {
                        s_hr_rising = false;
                        uint32_t ibi = now_ms - s_hr_last_beat_ms;
                        if ((ibi >= 320UL) && (ibi <= 1500UL)) {
                            uint32_t bpm = 60000UL / ibi;
                            s_hr_bpm_est = (uint8_t)((s_hr_bpm_est * 3U + (uint8_t)bpm) / 4U);
                            if (red > 0UL && ir > 0UL) {
                                uint32_t ratio = (red * 100UL) / ir;
                                uint32_t spo2 = (ratio > 110UL) ? 95UL : (110UL - (ratio / 8UL));
                                if (spo2 > 100UL) spo2 = 100UL;
                                if (spo2 < 90UL) spo2 = 90UL;
                                s_hr_spo2_est = (uint8_t)spo2;
                            }

                            /* Store IBI and compute real RMSSD HRV over chronological window */
                            s_ibi_buf[s_ibi_idx] = (uint16_t)ibi;
                            s_ibi_idx = (uint8_t)((s_ibi_idx + 1U) % 8U);
                            if (s_ibi_count < 8U) {
                                s_ibi_count++;
                            }
                            if (s_ibi_count >= 4U) {
                                uint8_t start_idx = (s_ibi_count < 8U) ? 0U : s_ibi_idx;
                                uint64_t sum_sq = 0ULL;
                                uint8_t valid_pairs = 0U;
                                for (uint8_t k = 1U; k < s_ibi_count; k++) {
                                    uint16_t prev_ibi = s_ibi_buf[(uint8_t)((start_idx + k - 1U) % 8U)];
                                    uint16_t curr_ibi = s_ibi_buf[(uint8_t)((start_idx + k) % 8U)];
                                    int32_t diff = (int32_t)curr_ibi - (int32_t)prev_ibi;
                                    if (diff < 0L) diff = -diff;
                                    if (diff <= 250L) {
                                        sum_sq += (uint64_t)diff * (uint64_t)diff;
                                        valid_pairs++;
                                    }
                                }
                                if (valid_pairs >= 2U) {
                                    uint32_t rmssd = Sensors_ISqrt32((uint32_t)(sum_sq / (uint64_t)valid_pairs));
                                    if (rmssd < 12UL) rmssd = 12UL;
                                    if (rmssd > 140UL) rmssd = 140UL;
                                    s_hrv_rmssd_est = (uint16_t)(((uint32_t)s_hrv_rmssd_est * 3UL + rmssd + 2UL) / 4UL);
                                    out->hrv_rmssd_ms = s_hrv_rmssd_est;
                                    out->hrv_valid    = true;
                                }
                            }
                        }
                        s_hr_last_beat_ms = now_ms;
                    }
                    s_hr_prev_ir = ir;
                    out->heart_bpm = s_hr_bpm_est;
                    out->spo2_pct  = s_hr_spo2_est;
                    out->hr_valid  = true;
                }
            }
        }
    }

    /* 2. Slow 1 Hz environmental polling: TMP117 & VEML6030 -> CH1115 auto-contrast */
    if ((now_ms - s_last_slow_poll_ms) >= 1000UL) {
        s_last_slow_poll_ms = now_ms;

        if (s_pres.temp_ok) {
            uint8_t tb[2];
            if (Sensors_ReadRegs(hi2c, TMP117_ADDR, TMP117_REG_TEMP, tb, 2U) == HAL_OK) {
                int16_t raw = (int16_t)(((uint16_t)tb[0] << 8) | tb[1]);
                out->temp_deci_c = (int16_t)(((int64_t)raw * 78125LL) / 1000000LL);
                out->temp_valid = true;
            }
        }

        if (s_pres.als_ok) {
            uint8_t ab[2];
            if (Sensors_ReadRegs(hi2c, VEML6030_ADDR, VEML6030_REG_ALS, ab, 2U) == HAL_OK) {
                uint16_t raw = (uint16_t)ab[0] | ((uint16_t)ab[1] << 8);
                uint32_t lux = ((uint32_t)raw * 576UL) / 10000UL;
                if (lux > 65000UL) lux = 65000UL;
                out->ambient_lux = (uint16_t)lux;
                out->als_valid = true;

                uint8_t contrast = (lux < 15UL) ? 0x35U : (lux < 200UL) ? 0x80U : 0xCFU;
                OLED_SetContrast(hi2c, contrast);
            }
        }
    }
}
