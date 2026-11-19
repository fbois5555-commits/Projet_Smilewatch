/**
  ******************************************************************************
  * @file           : smilewatch_ui.c
  * @brief          : SmileWatch V2 Application State Machine & 8-Screen UI (x3 Sub-views)
  *                   88x48 1-bit PMOLED Rendering (Topwin QG-8848TSWYG02 / CH1115)
  *                   Includes Automatic 6h Hand Split-Screen Avoidance
  ******************************************************************************
  */

#include "smilewatch_ui.h"
#include "sensors_driver.h"
#include "flash_storage.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define SW_OLED_TIMEOUT_MS          12000UL  /* Auto-sleep OLED after 12 s inactivity */
#define SW_BTN_LONG_MS              600UL    /* Long press threshold */
#define SW_BTN_DEBOUNCE_MS          30UL

static I2C_HandleTypeDef *s_hi2c = NULL;
static SW_SystemState_t   s_state;
static Miyota_Handle_t    s_miyota;

/* Internal button polling & task timers */
static bool     s_btn2h_prev        = false;
static bool     s_btn4h_prev        = false;
static uint32_t s_btn2h_down_ms     = 0U;
static uint32_t s_btn4h_down_ms     = 0U;
static bool     s_btn2h_long_fired  = false;
static bool     s_btn4h_long_fired  = false;
static uint32_t s_last_rtc_tick_ms  = 0U;
static uint32_t s_last_render_ms    = 0U;
static uint32_t s_last_bb_tick_ms   = 0U;
static uint32_t s_last_baseline_ms  = 0U;
static uint32_t s_last_coherence_ms = 0U;
static uint32_t s_win_steps         = 0UL;

/* Simulated PPG waveform lookup (22 samples, amplitude 0..16 px) */
static const uint8_t k_ppg_wave[22] = {
    4, 4, 5, 3, 2, 8, 15, 16, 11, 6, 4, 6, 8, 7, 5, 4, 4, 4, 4, 4, 4, 4
};

/* 24h Heart Rate History (44 points for 88 px width) */
static const uint8_t k_hr_24h_history[44] = {
    54, 52, 51, 50, 49, 48, 50, 52, 56, 62, 74, 82, 88, 95, 86, 78,
    72, 70, 75, 84, 92, 118, 126, 110, 96, 85, 80, 76, 74, 78, 85, 92,
    88, 82, 76, 72, 68, 65, 62, 58, 56, 55, 54, 52
};

/* 7-day Steps History (last index overwritten live by s_state.steps_count) */
static const uint16_t k_week_steps[7] = {
    7850, 10420, 6200, 9350, 11800, 8400, 6420
};

/* High-Definition 1-Bit Health & UI Bitmaps (row-major, MSB left) */
static const uint8_t k_garmin_heart_10x9[18] = {
    0x38, 0x00, 0x7C, 0x00, 0xFE, 0x00, 0xFF, 0x00, 0xFF, 0x00,
    0x7E, 0x00, 0x3C, 0x00, 0x18, 0x00, 0x00, 0x00
};

static const uint8_t k_garmin_bodybat_12x10[20] = {
    0x3F, 0xC0, 0x20, 0x40, 0x23, 0x40, 0x27, 0x40, 0x2E, 0x40,
    0x3C, 0x40, 0x28, 0x40, 0x20, 0x40, 0x3F, 0xC0, 0x0C, 0x00
};

static const uint8_t k_garmin_thermo_8x12[12] = {
    0x38, 0x44, 0x44, 0x54, 0x54, 0x54, 0x7C, 0xFE, 0xFE, 0xEE, 0x44, 0x38
};

static const uint8_t k_garmin_shoe_12x9[18] = {
    0x03, 0x00, 0x07, 0x80, 0x0F, 0xC0, 0x1F, 0xE0, 0x3F, 0xF0,
    0x7F, 0xF0, 0xFF, 0xF0, 0x7F, 0xE0, 0x00, 0x00
};

static const uint8_t k_garmin_bubble_11x9[18] = {
    0x3F, 0x80, 0x7F, 0xC0, 0xE9, 0xE0, 0xE0, 0xE0, 0xE9, 0xE0,
    0x7F, 0xC0, 0x3F, 0x80, 0x07, 0x00, 0x02, 0x00
};

static const uint8_t k_icon_gym_12x9[18] = {
    0x40, 0x20, 0xE0, 0x70, 0xE0, 0x70, 0xFF, 0xF0, 0xFF, 0xF0,
    0xE0, 0x70, 0xE0, 0x70, 0x40, 0x20, 0x00, 0x00
};

static const uint8_t k_icon_swim_12x9[18] = {
    0x00, 0xC0, 0x0C, 0xC0, 0x1F, 0x00, 0x7E, 0x00, 0x00, 0x00,
    0x66, 0x60, 0xFF, 0xF0, 0x00, 0x00, 0x66, 0x60
};

static const uint8_t k_icon_run_10x10[20] = {
    0x0C, 0x00, 0x0C, 0x00, 0x3E, 0x00, 0x5C, 0x80, 0x18, 0x00,
    0x3C, 0x00, 0x66, 0x00, 0xC3, 0x00, 0x81, 0x80, 0x00, 0x00
};

static const uint8_t k_icon_chrono_10x10[20] = {
    0x3C, 0x00, 0x18, 0x00, 0x3C, 0x40, 0x42, 0x00, 0x99, 0x00,
    0x9D, 0x00, 0x81, 0x00, 0x42, 0x00, 0x3C, 0x00, 0x00, 0x00
};

static const uint8_t k_icon_moon_10x10[20] = {
    0x38, 0x00, 0x70, 0x40, 0xE0, 0xE0, 0xE0, 0x40, 0xE0, 0x00,
    0xE0, 0x00, 0xF0, 0x00, 0x7C, 0x00, 0x38, 0x00, 0x00, 0x00
};

static const uint8_t k_icon_music_10x10[20] = {
    0x0F, 0x80, 0x0F, 0x80, 0x08, 0x80, 0x08, 0x80, 0x08, 0x80,
    0x38, 0xE0, 0x79, 0xE0, 0x79, 0xE0, 0x30, 0xC0, 0x00, 0x00
};

static const uint8_t k_icon_cam_12x9[18] = {
    0x0F, 0x00, 0x1F, 0x80, 0xFF, 0xF0, 0xF9, 0xF0, 0xF6, 0xF0,
    0xF6, 0xF0, 0xF9, 0xF0, 0xFF, 0xF0, 0x00, 0x00
};

static const uint8_t k_icon_alarm_10x10[20] = {
    0x18, 0x00, 0x3C, 0x00, 0x7E, 0x00, 0x7E, 0x00, 0x7E, 0x00,
    0xFF, 0x00, 0xFF, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00
};

static const uint8_t k_icon_weather_12x10[20] = {
    0x0C, 0x00, 0x1E, 0x00, 0x3F, 0x00, 0x1E, 0xC0, 0x07, 0xE0,
    0x0F, 0xF0, 0x1F, 0xF0, 0x1F, 0xF0, 0x00, 0x00, 0x00, 0x00
};

/* 16x16 GPS Turn-by-Turn Navigation Arrows: 0=Straight, 1=Right, 2=Left, 3=Roundabout, 4=Arrival */
static const uint8_t k_nav_arrows_16x16[5][32] = {
    /* 0: STRAIGHT */
    {
        0x01, 0x80, 0x03, 0xC0, 0x07, 0xE0, 0x0F, 0xF0, 0x1F, 0xF8, 0x3F, 0xFC, 0x03, 0xC0, 0x03, 0xC0,
        0x03, 0xC0, 0x03, 0xC0, 0x03, 0xC0, 0x03, 0xC0, 0x03, 0xC0, 0x03, 0xC0, 0x03, 0xC0, 0x00, 0x00
    },
    /* 1: RIGHT */
    {
        0x00, 0x00, 0x00, 0x60, 0x00, 0x70, 0x00, 0x78, 0x1F, 0xFC, 0x3F, 0xFE, 0x3F, 0xFC, 0x3C, 0x78,
        0x3C, 0x70, 0x3C, 0x60, 0x3C, 0x00, 0x3C, 0x00, 0x3C, 0x00, 0x3C, 0x00, 0x3C, 0x00, 0x00, 0x00
    },
    /* 2: LEFT */
    {
        0x00, 0x00, 0x06, 0x00, 0x0E, 0x00, 0x1E, 0x00, 0x3F, 0xF8, 0x7F, 0xFC, 0x3F, 0xFC, 0x1E, 0x3C,
        0x0E, 0x3C, 0x06, 0x3C, 0x00, 0x3C, 0x00, 0x3C, 0x00, 0x3C, 0x00, 0x3C, 0x00, 0x3C, 0x00, 0x00
    },
    /* 3: ROUNDABOUT */
    {
        0x01, 0x80, 0x03, 0xC0, 0x07, 0xE0, 0x01, 0x80, 0x0F, 0xF0, 0x1F, 0xF8, 0x38, 0x1C, 0x30, 0x0C,
        0x30, 0x0C, 0x30, 0x0C, 0x38, 0x1C, 0x1F, 0xF8, 0x0F, 0xF0, 0x01, 0x80, 0x01, 0x80, 0x00, 0x00
    },
    /* 4: ARRIVAL */
    {
        0x00, 0x00, 0x3C, 0xCC, 0x3C, 0xCC, 0x33, 0x33, 0x33, 0x33, 0x3C, 0xCC, 0x3C, 0xCC, 0x33, 0x33,
        0x33, 0x33, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x30, 0x00, 0x78, 0x00, 0x00, 0x00
    }
};

/* 19 Disciplines Sportives (Synchronisees avec le menu deroulant du simulateur) */
static const char * const k_sport_names[19] = {
    "MUSCU", "COURSE", "NAGE25M", "VELO", "HIIT",
    "RANDO", "TENNIS", "BOXE", "RAMEUR", "YOGA",
    "CORDE", "GRIMPE", "MARCHE", "TRAIL", "GAINAGE",
    "PADEL", "GOLF", "SKI", "COLLECT"
};

static const char * const k_sport_short[19] = {
    "MUSC", "COUR", "NAGE", "VELO", "HIIT",
    "RAND", "TENN", "BOXE", "RAME", "YOGA",
    "CORD", "GRIM", "MARC", "TRAI", "GAIN",
    "PADE", "GOLF", "SKI ", "COLL"
};

static void SmileWatch_TriggerHaptic(uint32_t now_ms, uint32_t duration_ms)
{
    HAL_GPIO_WritePin(DRV_EN_GPIO_Port, DRV_EN_Pin, GPIO_PIN_SET);
    s_state.haptic_until_ms = now_ms + duration_ms;
}

static void SmileWatch_UpdateSplit6h(void)
{
    if (s_state.split_6h_mode == 0U) {
        s_state.split_6h = false;
    } else if (s_state.split_6h_mode == 2U) {
        s_state.split_6h = true;
    } else {
        uint8_t h12 = (uint8_t)(s_state.hours % 12U);
        bool min_at_6h  = (s_state.minutes >= 26U) && (s_state.minutes <= 34U);
        bool hour_at_6h = ((h12 == 6U) && (s_state.minutes < 25U)) ||
                          ((h12 == 5U) && (s_state.minutes >= 35U));
        s_state.split_6h = (min_at_6h || hour_at_6h);
    }
    s_state.split_6h_force = (s_state.split_6h_mode == 2U);
}

static void SmileWatch_UpdateAdaptiveSport(void)
{
    uint32_t hr_max_est = (s_state.hr_max > 185U) ? (uint32_t)s_state.hr_max : 185UL;
    uint32_t rhr        = (s_state.rhr_base_bpm >= 42U) ? (uint32_t)s_state.rhr_base_bpm : 56UL;

    /* Uth VO2max estimate: 15.3 * (HRmax / RHR) */
    uint32_t vo2 = (153UL * hr_max_est) / (10UL * rhr);
    if (vo2 < 32UL) vo2 = 32UL;
    if (vo2 > 72UL) vo2 = 72UL;
    s_state.vo2_max = (uint8_t)vo2;

    /* Karvonen Heart Rate Reserve TRIMP & Recovery time */
    int32_t hrr_num = ((int32_t)s_state.heart_bpm - (int32_t)rhr) * 100L;
    int32_t hrr_den = (int32_t)(hr_max_est - rhr);
    if (hrr_den < 40L) hrr_den = 40L;
    int32_t hrr_pct = hrr_num / hrr_den;
    if (hrr_pct < 10L) hrr_pct = 10L;
    if (hrr_pct > 100L) hrr_pct = 100L;

    uint32_t dur_min = (s_state.sport_sec >= 60U) ? ((uint32_t)s_state.sport_sec / 60UL) : 1UL;
    uint32_t trimp   = (dur_min * (uint32_t)hrr_pct * (10UL + ((uint32_t)hrr_pct / 12UL))) / 600UL;
    uint32_t rec     = 4UL + (trimp / 2UL) +
                       ((100UL - (uint32_t)s_state.body_battery) / 6UL) +
                       ((s_state.readiness_state == 2U) ? 8UL : 0UL);
    if (rec < 4UL) rec = 4UL;
    if (rec > 72UL) rec = 72UL;
    s_state.recovery_hours = (uint8_t)rec;
}

void SmileWatch_UpdatePhysioEngine(bool imu_still, uint32_t now_ms)
{
    /* 1. Auto-learned personal baselines (EMA) every >= 60s during calm stillness */
    if ((now_ms != 0UL) && ((now_ms - s_last_baseline_ms) >= 60000UL) &&
        imu_still && !s_state.sport_active) {
        s_last_baseline_ms = now_ms;
        if ((s_state.heart_bpm >= 40U) && (s_state.heart_bpm <= 85U)) {
            uint32_t rhr_ema = ((uint32_t)s_state.rhr_base_bpm * 7UL + (uint32_t)s_state.heart_bpm + 4UL) / 8UL;
            if (rhr_ema < 42UL) rhr_ema = 42UL;
            if (rhr_ema > 80UL) rhr_ema = 80UL;
            s_state.rhr_base_bpm = (uint8_t)rhr_ema;
        }
        if ((s_state.hrv_rmssd_ms >= 15U) && (s_state.hrv_rmssd_ms <= 130U) &&
            (s_state.heart_bpm <= (uint8_t)(s_state.rhr_base_bpm + 12U))) {
            uint32_t hrv_ema = ((uint32_t)s_state.hrv_base_ms * 7UL + (uint32_t)s_state.hrv_rmssd_ms + 4UL) / 8UL;
            if (hrv_ema < 20UL) hrv_ema = 20UL;
            if (hrv_ema > 120UL) hrv_ema = 120UL;
            s_state.hrv_base_ms = (uint16_t)hrv_ema;
        }
        if ((s_state.temp_deci_c >= 355) && (s_state.temp_deci_c <= 374)) {
            int32_t t_ema = ((int32_t)s_state.temp_base_deci_c * 15L + (int32_t)s_state.temp_deci_c + 8L) / 16L;
            s_state.temp_base_deci_c = (int16_t)t_ema;
        }
    }

    /* 2. Real Stress Score (5..98) from HRV drop, HR elevation above RHR, and Temp drift */
    uint32_t hrv_base = (s_state.hrv_base_ms >= 15U) ? (uint32_t)s_state.hrv_base_ms : 15UL;
    uint32_t hrv_drop = (hrv_base > (uint32_t)s_state.hrv_rmssd_ms)
                        ? (((hrv_base - (uint32_t)s_state.hrv_rmssd_ms) * 100UL) / hrv_base)
                        : 0UL;
    uint32_t hr_elev  = (s_state.heart_bpm > s_state.rhr_base_bpm)
                        ? ((uint32_t)(s_state.heart_bpm - s_state.rhr_base_bpm) * 2UL)
                        : 0UL;
    int32_t  t_diff   = (int32_t)s_state.temp_deci_c - (int32_t)s_state.temp_base_deci_c;
    uint32_t temp_pen = (t_diff > 4L) ? ((uint32_t)(t_diff - 4L) * 4UL) : 0UL;

    uint32_t stress = 10UL + ((hrv_drop * 65UL) / 100UL) + ((hr_elev * 55UL) / 100UL) + temp_pen;
    if (stress < 5UL) stress = 5UL;
    if (stress > 98UL) stress = 98UL;
    s_state.stress_score = (uint8_t)stress;

    /* 3. Combined Anomaly & Readiness State: 0=FORME, 1=MODERE, 2=REPOS! */
    uint8_t flags = 0U;
    if (!s_state.sport_active && (s_state.heart_bpm >= (uint8_t)(s_state.rhr_base_bpm + 18U))) flags++;
    if (((uint32_t)s_state.hrv_rmssd_ms * 100UL) < (hrv_base * 75UL)) flags++;
    if (t_diff >= 5L) flags++;

    if ((flags >= 2U) || (s_state.temp_deci_c >= 378)) {
        s_state.readiness_state = 2U;
    } else if ((flags == 1U) || (s_state.body_battery < 40U)) {
        s_state.readiness_state = 1U;
    } else {
        s_state.readiness_state = 0U;
    }

    /* 4. Sleep Stage Classification: 0=EVEIL, 1=LEGER, 2=PROFOND, 3=REM */
    if (!imu_still || s_state.sport_active || (s_state.heart_bpm > (uint8_t)(s_state.rhr_base_bpm + 16U))) {
        s_state.sleep_stage = 0U;
    } else if ((s_state.heart_bpm <= (uint8_t)(s_state.rhr_base_bpm + 2U)) &&
               ((uint32_t)s_state.hrv_rmssd_ms >= ((hrv_base * 105UL) / 100UL))) {
        s_state.sleep_stage = 2U;
    } else if ((s_state.heart_bpm >= (uint8_t)(s_state.rhr_base_bpm + 5U)) &&
               ((uint32_t)s_state.hrv_rmssd_ms <= ((hrv_base * 90UL) / 100UL))) {
        s_state.sleep_stage = 3U;
    } else {
        s_state.sleep_stage = 1U;
    }

    /* 5. Dynamic Body Battery (5..100, rate-limited every 10 s) */
    if ((now_ms != 0UL) && ((now_ms - s_last_bb_tick_ms) >= 10000UL)) {
        s_last_bb_tick_ms = now_ms;
        int16_t bb = (int16_t)s_state.body_battery;
        if (s_state.sport_active && (s_state.heart_bpm > 115U)) {
            bb -= (s_state.heart_bpm > 155U) ? 2 : 1;
        } else if (s_state.stress_score >= 55U) {
            bb -= 1;
        } else if (imu_still && (s_state.stress_score <= 28U)) {
            bb += (s_state.sleep_stage == 2U) ? 2 : 1;
        }
        if (bb < 5) bb = 5;
        if (bb > 100) bb = 100;
        s_state.body_battery = (uint8_t)bb;
    }

    SmileWatch_UpdateAdaptiveSport();
}

void SmileWatch_ApplyBleCalibration(uint16_t phone_gps_dist_m,
                                    uint32_t phone_steps,
                                    uint32_t watch_steps_window,
                                    uint32_t now_ms)
{
    (void)now_ms;
    if (watch_steps_window == 0UL) {
        watch_steps_window = (s_state.steps_count > s_state.steps_at_last_calib)
                             ? (s_state.steps_count - s_state.steps_at_last_calib)
                             : s_state.steps_count;
    }

    if ((phone_gps_dist_m >= 100U) && (watch_steps_window >= 100UL)) {
        uint32_t meas_stride = ((uint32_t)phone_gps_dist_m * 100UL) / watch_steps_window;
        if (meas_stride < 55UL) meas_stride = 55UL;
        if (meas_stride > 115UL) meas_stride = 115UL;
        s_state.stride_cm = (uint8_t)(((uint32_t)s_state.stride_cm * 60UL + meas_stride * 40UL + 50UL) / 100UL);
        s_state.last_ble_gps_dist_m = phone_gps_dist_m;
    }

    if ((phone_steps >= 100UL) && (watch_steps_window >= 100UL)) {
        int32_t err_pct = (((int32_t)watch_steps_window - (int32_t)phone_steps) * 100L) / (int32_t)phone_steps;
        if (err_pct < -50L) err_pct = -50L;
        if (err_pct > 50L) err_pct = 50L;
        s_state.calib_error_pct = (int8_t)err_pct;

        if (err_pct > 5L) {
            int32_t adj = err_pct * 3L;
            if (adj < 15L) adj = 15L;
            if (adj > 60L) adj = 60L;
            uint16_t next_th = (uint16_t)(s_state.mlc_thresh_mg + (uint16_t)adj);
            s_state.mlc_thresh_mg = (next_th > 1850U) ? 1850U : next_th;
        } else if (err_pct < -5L) {
            int32_t adj = (-err_pct) * 3L;
            if (adj < 15L) adj = 15L;
            if (adj > 60L) adj = 60L;
            uint16_t next_th = (s_state.mlc_thresh_mg > (uint16_t)adj) ? (uint16_t)(s_state.mlc_thresh_mg - (uint16_t)adj) : 980U;
            s_state.mlc_thresh_mg = (next_th < 980U) ? 980U : next_th;
        }

        int32_t abs_err = (err_pct < 0L) ? -err_pct : err_pct;
        int32_t t_conf = 99L - abs_err;
        if (t_conf < 60L) t_conf = 60L;
        if (t_conf > 99L) t_conf = 99L;
        s_state.mlc_confidence_pct = (uint8_t)(((uint32_t)s_state.mlc_confidence_pct * 4UL + (uint32_t)t_conf * 6UL + 5UL) / 10UL);
    }

    s_state.steps_at_last_calib = s_state.steps_count;
    Sensors_SetImuThreshold(s_hi2c, s_state.mlc_thresh_mg);
    SmileWatch_UpdateAdaptiveSport();
    s_state.hw_flash_ok = FlashStorage_Save(&s_state, &s_miyota);
}

void SmileWatch_AutoDetectSport(uint16_t imu_energy_mg,
                                uint16_t imu_gyro_dps,
                                uint16_t spm,
                                uint32_t now_ms)
{
    (void)now_ms;
    uint8_t cls = 0U; /* 0=REPOS, 1=MARCHE, 2=COURSE, 3=MUSCU, 4=VELO, 5=NAGE */

    if ((imu_energy_mg < 160U) && (spm < 40U)) {
        cls = 0U;
    } else if ((imu_energy_mg < 260U) && (imu_gyro_dps < 60U) &&
               (s_state.heart_bpm >= 112U) && (spm < 50U)) {
        cls = 4U; /* VELO */
    } else if ((imu_gyro_dps >= 135U) && (imu_energy_mg >= 300U) && (spm < 75U)) {
        cls = 5U; /* NAGE */
    } else if ((imu_gyro_dps >= 85U) && (imu_energy_mg >= 240U) && (spm < 70U)) {
        cls = 3U; /* MUSCU */
    } else if ((imu_energy_mg >= 420U) || (spm >= 135U)) {
        cls = 2U; /* COURSE */
    } else if ((spm >= 50U) || (imu_energy_mg >= 160U)) {
        cls = 1U; /* MARCHE */
    }

    s_state.detected_activity = cls;
    if (!s_state.auto_sport_enabled) {
        return;
    }

    if (cls == 2U) {
        s_state.sport_type   = 1U; /* COURSE */
        s_state.sport_active = true;
    } else if (cls == 3U) {
        s_state.sport_type   = 0U; /* MUSCU */
        s_state.sport_active = true;
    } else if (cls == 4U) {
        s_state.sport_type   = 3U; /* VELO */
        s_state.sport_active = true;
    } else if (cls == 5U) {
        s_state.sport_type   = 2U; /* NAGE25M */
        s_state.sport_active = true;
    }
}

static bool SmileWatch_IsNightQuiet(void)
{
    return s_state.night_mode_enabled;
}

void SmileWatch_StartTriathlon(uint32_t now_ms)
{
    s_state.tri_active    = true;
    s_state.tri_stage     = 0U; /* NAGE */
    s_state.tri_total_sec = 0U;
    s_state.sport_type    = 2U; /* NAGE25M */
    s_state.sport_active  = true;
    s_state.sport_sec     = 0U;
    s_state.current_screen = SW_SCREEN_SPORT;
    s_state.sub_view       = 1U;
    s_state.display_awake  = true;
    s_state.last_activity_ms = now_ms;
    Sensors_PlayHaptic(s_hi2c, 14U);
    SmileWatch_TriggerHaptic(now_ms, 120U);
}

void SmileWatch_NextTriathlonStage(uint32_t now_ms)
{
    if (!s_state.tri_active) {
        SmileWatch_StartTriathlon(now_ms);
        return;
    }
    if (s_state.tri_stage < 4U) {
        s_state.tri_stage++;
        if (s_state.tri_stage == 2U) s_state.sport_type = 3U;      /* VELO */
        else if (s_state.tri_stage == 4U) s_state.sport_type = 1U; /* COURSE */
        Sensors_PlayHaptic(s_hi2c, 16U);
        SmileWatch_TriggerHaptic(now_ms, 90U);
    } else {
        s_state.tri_active   = false;
        s_state.sport_active = false;
        s_state.sport_sec    = s_state.tri_total_sec;
        SmileWatch_SaveWorkoutSession();
    }
}

void SmileWatch_TriggerSedentaryAlert(uint32_t now_ms)
{
    if (!s_state.sedentary_alert_enabled || SmileWatch_IsNightQuiet() || s_state.survival_mode) {
        return;
    }
    s_state.sedentary_alert_active = true;
    s_state.sedentary_min          = 0U;
    s_state.last_activity_ms       = now_ms;
    if (!s_state.display_awake) {
        s_state.display_awake = true;
        OLED_PowerOn();
        OLED_Init(s_hi2c);
    }
    Sensors_PlayHaptic(s_hi2c, 1U);
    SmileWatch_TriggerHaptic(now_ms, 20U);
}

void SmileWatch_ToggleTrackCalibration(uint16_t dist_m, uint32_t now_ms)
{
    (void)now_ms;
    if (!s_state.track_calib_active) {
        s_state.track_calib_active      = true;
        s_state.track_calib_dist_m      = (dist_m >= 100U) ? dist_m : 400U;
        s_state.track_calib_start_steps = s_state.steps_count;
    } else {
        s_state.track_calib_active = false;
        uint32_t delta = (s_state.steps_count > s_state.track_calib_start_steps)
                         ? (s_state.steps_count - s_state.track_calib_start_steps)
                         : 0UL;
        if (delta >= 50UL) {
            uint32_t meas = ((uint32_t)s_state.track_calib_dist_m * 100UL) / delta;
            if (meas < 55UL) meas = 55UL;
            if (meas > 115UL) meas = 115UL;
            s_state.stride_cm = (uint8_t)(((uint32_t)s_state.stride_cm * 60UL + meas * 40UL + 50UL) / 100UL);
            s_state.calib_error_pct = 0;
            if (s_state.mlc_confidence_pct <= 97U) {
                s_state.mlc_confidence_pct = (uint8_t)(s_state.mlc_confidence_pct + 2U);
            }
            s_state.hw_flash_ok = FlashStorage_Save(&s_state, &s_miyota);
        }
    }
}

void SmileWatch_CheckImuHrCoherence(uint16_t recent_steps_per_min, uint32_t now_ms)
{
    (void)now_ms;
    if ((recent_steps_per_min >= 70U) && !s_state.sport_active &&
        (s_state.heart_bpm <= (uint8_t)(s_state.rhr_base_bpm + 10U))) {
        uint16_t next_th = (uint16_t)(s_state.mlc_thresh_mg + 25U);
        s_state.mlc_thresh_mg = (next_th > 1850U) ? 1850U : next_th;
        if (s_state.imu_fp_rejects < 255U) {
            s_state.imu_fp_rejects++;
        }
        Sensors_SetImuThreshold(s_hi2c, s_state.mlc_thresh_mg);
        s_state.hw_flash_ok = FlashStorage_Save(&s_state, &s_miyota);
    }
}

void SmileWatch_SubmitSportFeedback(int8_t rep_delta, int8_t corrected_sport, uint32_t now_ms)
{
    (void)now_ms;
    if ((rep_delta == 0) && (corrected_sport < 0)) {
        if (s_state.mlc_confidence_pct <= 96U) {
            s_state.mlc_confidence_pct = (uint8_t)(s_state.mlc_confidence_pct + 3U);
        } else {
            s_state.mlc_confidence_pct = 99U;
        }
        s_state.calib_error_pct = 0;
    } else {
        if (rep_delta < 0) {
            uint16_t next_th = (uint16_t)(s_state.mlc_thresh_mg + 30U);
            s_state.mlc_thresh_mg = (next_th > 1850U) ? 1850U : next_th;
            if ((s_state.sport_type == 0U) && (s_state.gym_reps > (uint8_t)(-rep_delta))) {
                s_state.gym_reps = (uint8_t)(s_state.gym_reps - (uint8_t)(-rep_delta));
            }
            s_state.calib_error_pct = 10;
            if (s_state.mlc_confidence_pct > 67U) s_state.mlc_confidence_pct = (uint8_t)(s_state.mlc_confidence_pct - 2U);
        } else if (rep_delta > 0) {
            uint16_t next_th = (s_state.mlc_thresh_mg > 30U) ? (uint16_t)(s_state.mlc_thresh_mg - 30U) : 980U;
            s_state.mlc_thresh_mg = (next_th < 980U) ? 980U : next_th;
            if (s_state.sport_type == 0U) {
                s_state.gym_reps = (uint8_t)(s_state.gym_reps + (uint8_t)rep_delta);
            }
            s_state.calib_error_pct = -10;
            if (s_state.mlc_confidence_pct > 67U) s_state.mlc_confidence_pct = (uint8_t)(s_state.mlc_confidence_pct - 2U);
        }
        if ((corrected_sport >= 0) && (corrected_sport < (int8_t)SW_SPORT_COUNT)) {
            s_state.sport_type = (uint8_t)corrected_sport;
            if (s_state.history_count > 0U) {
                uint8_t latest = (uint8_t)((s_state.history_head + SW_HISTORY_MAX - 1U) % SW_HISTORY_MAX);
                s_state.workout_history[latest].sport_type = s_state.sport_type;
            }
            if (s_state.mlc_trees_used < 8U) {
                s_state.mlc_trees_used++;
            }
        }
    }

    Sensors_SetImuThreshold(s_hi2c, s_state.mlc_thresh_mg);
    SmileWatch_UpdateAdaptiveSport();
    s_state.hw_flash_ok = FlashStorage_Save(&s_state, &s_miyota);
}

void SmileWatch_AutoDetectSport(uint16_t imu_energy_mg,
                                uint16_t imu_gyro_dps,
                                uint16_t spm,
                                uint32_t now_ms)
{
    (void)now_ms;
    uint8_t cls = 0U; /* 0=REPOS, 1=MARCHE, 2=COURSE, 3=MUSCU, 4=VELO, 5=NAGE */

    if ((imu_energy_mg < 160U) && (spm < 40U)) {
        cls = 0U;
    } else if ((imu_energy_mg < 260U) && (imu_gyro_dps < 60U) &&
               (s_state.heart_bpm >= 112U) && (spm < 50U)) {
        cls = 4U; /* VELO */
    } else if ((imu_gyro_dps >= 135U) && (imu_energy_mg >= 300U) && (spm < 75U)) {
        cls = 5U; /* NAGE */
    } else if ((imu_gyro_dps >= 85U) && (imu_energy_mg >= 240U) && (spm < 70U)) {
        cls = 3U; /* MUSCU */
    } else if ((imu_energy_mg >= 420U) || (spm >= 135U)) {
        cls = 2U; /* COURSE */
    } else if ((spm >= 50U) || (imu_energy_mg >= 160U)) {
        cls = 1U; /* MARCHE */
    }

    s_state.detected_activity = cls;
    if (!s_state.auto_sport_enabled) {
        return;
    }

    if (cls == 2U) {
        s_state.sport_type   = 1U; /* COURSE */
        s_state.sport_active = true;
    } else if (cls == 3U) {
        s_state.sport_type   = 0U; /* MUSCU */
        s_state.sport_active = true;
    } else if (cls == 4U) {
        s_state.sport_type   = 3U; /* VELO */
        s_state.sport_active = true;
    } else if (cls == 5U) {
        s_state.sport_type   = 2U; /* NAGE25M */
        s_state.sport_active = true;
    }
}

static bool SmileWatch_IsNightQuiet(void)
{
    return s_state.night_mode_enabled;
}

void SmileWatch_StartTriathlon(uint32_t now_ms)
{
    s_state.tri_active    = true;
    s_state.tri_stage     = 0U; /* NAGE */
    s_state.tri_total_sec = 0U;
    s_state.sport_type    = 2U; /* NAGE25M */
    s_state.sport_active  = true;
    s_state.sport_sec     = 0U;
    s_state.current_screen = SW_SCREEN_SPORT;
    s_state.sub_view       = 1U;
    s_state.display_awake  = true;
    s_state.last_activity_ms = now_ms;
    Sensors_PlayHaptic(s_hi2c, 14U);
    SmileWatch_TriggerHaptic(now_ms, 120U);
}

void SmileWatch_NextTriathlonStage(uint32_t now_ms)
{
    if (!s_state.tri_active) {
        SmileWatch_StartTriathlon(now_ms);
        return;
    }
    if (s_state.tri_stage < 4U) {
        s_state.tri_stage++;
        if (s_state.tri_stage == 2U) s_state.sport_type = 3U;      /* VELO */
        else if (s_state.tri_stage == 4U) s_state.sport_type = 1U; /* COURSE */
        Sensors_PlayHaptic(s_hi2c, 16U);
        SmileWatch_TriggerHaptic(now_ms, 90U);
    } else {
        s_state.tri_active   = false;
        s_state.sport_active = false;
        s_state.sport_sec    = s_state.tri_total_sec;
        SmileWatch_SaveWorkoutSession();
    }
}

void SmileWatch_TriggerSedentaryAlert(uint32_t now_ms)
{
    if (!s_state.sedentary_alert_enabled || SmileWatch_IsNightQuiet() || s_state.survival_mode) {
        return;
    }
    s_state.sedentary_alert_active = true;
    s_state.sedentary_min          = 0U;
    s_state.last_activity_ms       = now_ms;
    if (!s_state.display_awake) {
        s_state.display_awake = true;
        OLED_PowerOn();
        OLED_Init(s_hi2c);
    }
    Sensors_PlayHaptic(s_hi2c, 1U);
    SmileWatch_TriggerHaptic(now_ms, 20U);
}

void SmileWatch_ToggleTrackCalibration(uint16_t dist_m, uint32_t now_ms)
{
    (void)now_ms;
    if (!s_state.track_calib_active) {
        s_state.track_calib_active      = true;
        s_state.track_calib_dist_m      = (dist_m >= 100U) ? dist_m : 400U;
        s_state.track_calib_start_steps = s_state.steps_count;
    } else {
        s_state.track_calib_active = false;
        uint32_t delta = (s_state.steps_count > s_state.track_calib_start_steps)
                         ? (s_state.steps_count - s_state.track_calib_start_steps)
                         : 0UL;
        if (delta >= 50UL) {
            uint32_t meas = ((uint32_t)s_state.track_calib_dist_m * 100UL) / delta;
            if (meas < 55UL) meas = 55UL;
            if (meas > 115UL) meas = 115UL;
            s_state.stride_cm = (uint8_t)(((uint32_t)s_state.stride_cm * 60UL + meas * 40UL + 50UL) / 100UL);
            s_state.calib_error_pct = 0;
            if (s_state.mlc_confidence_pct <= 97U) {
                s_state.mlc_confidence_pct = (uint8_t)(s_state.mlc_confidence_pct + 2U);
            }
            s_state.hw_flash_ok = FlashStorage_Save(&s_state, &s_miyota);
        }
    }
}

void SmileWatch_UpdatePowerEstimate(uint32_t now_ms)
{
    (void)now_ms;
    s_state.oled_lit_pixels = OLED_CountLitPixels();

    if (s_state.survival_mode || (s_state.power_profile == 0U)) {
        s_state.current_inst_ua = 16UL;
        s_state.current_avg_ua  = 16U;
        uint32_t days = (s_state.battery_pct == 0U) ? 0UL :
                        ((2520000UL * (uint32_t)s_state.battery_pct) / (100UL * 155UL * 24UL));
        if ((s_state.battery_pct > 0U) && (days < 1UL)) days = 1UL;
        s_state.autonomy_days = (uint16_t)days;
        return;
    }

    uint32_t base_ua = 46UL; /* Miyota 7uA + STM32 Stop2 3.5uA + LDOs/TMP/ALS 35.5uA */
    uint32_t ai_ua   = 15UL + 13UL + 3UL +
                       (s_state.wrist_nav_enabled ? 12UL : 0UL) +
                       (s_state.sport_active ? 480UL : 0UL);
    uint32_t ble_avg_ua  = s_state.ble_connected ? ((s_state.power_profile == 1U) ? 20UL : 35UL) : 0UL;
    uint32_t ble_inst_ua = ble_avg_ua + (s_state.ota_active ? 4500UL : 0UL);

    uint32_t contrast_pct = (s_state.ambient_lux < 15U) ? 45UL :
                            (s_state.ambient_lux < 200U) ? 75UL : 100UL;
    uint32_t oled_lit     = s_state.display_awake ? (uint32_t)s_state.oled_lit_pixels : 800UL;
    uint32_t oled_ref_ua  = 600UL + (11400UL * oled_lit * contrast_pct) / (4224UL * 100UL);
    uint32_t oled_inst_ua = s_state.display_awake ? oled_ref_ua : 0UL;
    uint32_t oled_min_day = (s_state.power_profile == 1U) ? 2UL :
                            (s_state.power_profile == 3U) ? 45UL : 10UL;
    uint32_t oled_avg_ua  = (oled_ref_ua * oled_min_day) / 1440UL;

    uint32_t hr_inst_ua = (s_state.hr_measuring &&
                           (s_state.sport_active || (s_state.current_screen == SW_SCREEN_HEART)))
                          ? 15000UL : 0UL;
    uint32_t hr_avg_ua  = !s_state.hr_measuring ? 0UL :
                          (s_state.power_profile == 1U) ? 125UL :
                          (s_state.power_profile == 3U) ? 1125UL : 500UL;

    uint32_t hap_inst_ua     = (s_state.haptic_until_ms != 0U) ? 80000UL : 0UL;
    uint32_t hap_avg_ua      = 5UL;
    uint32_t miyota_burst_ua = (s_miyota.analog_pos_sec != s_miyota.target_pos_sec) ? 220UL : 0UL;

    s_state.current_inst_ua = base_ua + ai_ua + ble_inst_ua + oled_inst_ua + hr_inst_ua + hap_inst_ua + miyota_burst_ua;
    uint32_t avg_ua = base_ua + ai_ua + ble_avg_ua + oled_avg_ua + hr_avg_ua + hap_avg_ua;
    if (avg_ua < 16UL) avg_ua = 16UL;
    if (avg_ua > 65000UL) avg_ua = 65000UL;
    s_state.current_avg_ua = (uint16_t)avg_ua;

    uint32_t days = (s_state.battery_pct == 0U) ? 0UL :
                    ((252000UL * (uint32_t)s_state.battery_pct) / (100UL * avg_ua * 24UL));
    if ((s_state.battery_pct > 0U) && (days < 1UL)) days = 1UL;
    s_state.autonomy_days = (uint16_t)days;
}

static void SmileWatch_SyncMiyotaWithPillPause(void)
{
    uint32_t h12      = (uint32_t)(s_state.hours % 12U);
    uint32_t real_sec = (h12 * 3600UL) + ((uint32_t)s_state.minutes * 60UL) + (uint32_t)s_state.seconds;
    bool in_pill_zone = (s_state.minutes >= 24U) && (s_state.minutes <= 35U);

    if (s_state.pill_hand_pause_enabled && s_state.display_awake && !s_state.survival_mode && in_pill_zone) {
        uint32_t park_sec = (h12 * 3600UL) + (24UL * 60UL);
        uint32_t dist_past_park = (s_miyota.analog_pos_sec + MIYOTA_SEC_PER_12H - park_sec) % MIYOTA_SEC_PER_12H;
        if ((dist_past_park > 0UL) && (dist_past_park < 780UL)) {
            s_miyota.target_pos_sec = s_miyota.analog_pos_sec;
        } else {
            s_miyota.target_pos_sec = park_sec;
        }
        s_state.pill_hand_paused = true;
    } else {
        s_state.pill_hand_paused = false;
        s_miyota.target_pos_sec  = real_sec % MIYOTA_SEC_PER_12H;
    }
}

/* ============================================================================
 * HEADER HELPER: Pill-safe Title + Time Fallback (Hand Pause) + Annex Split
 * ==========================================================================*/
void SmileWatch_DrawHeader(const char *title, uint8_t sub_idx)
{
    if (s_state.split_6h) {
        char short_t[5];
        if (s_state.pill_hand_paused) {
            (void)snprintf(short_t, sizeof(short_t), "%02u%02u", s_state.hours, s_state.minutes);
        } else {
            strncpy(short_t, title, 4U);
            short_t[4] = '\0';
        }
        OLED_DrawStr(12, 3, short_t, 1, 1);

        for (int16_t i = 0; i < 3; i++) {
            int16_t ix = (int16_t)(51 + i * 5);
            if (i == (int16_t)sub_idx) {
                OLED_FillRect(ix, 4, 4, 4, 1);
            } else {
                OLED_DrawRect(ix, 4, 4, 4, 1);
            }
        }
        OLED_DrawStr(65, 3, "6H", 1, 1);
        OLED_DrawLine(8, 11, 35, 11, 1);
        OLED_DrawLine(52, 11, 79, 11, 1);
        for (int16_t y = 2; y < 46; y += 3) {
            OLED_DrawPixel(37, y, 1);
            OLED_DrawPixel(50, y, 1);
        }
    } else {
        if (s_state.pill_hand_paused) {
            char hbuf[8];
            (void)snprintf(hbuf, sizeof(hbuf), "%02u:%02u", s_state.hours, s_state.minutes);
            OLED_DrawStr(12, 3, hbuf, 1, 1);
            OLED_FillRect(45, 4, 2, 5, 1);
            OLED_FillRect(48, 4, 2, 5, 1);
        } else {
            OLED_DrawStr(12, 3, title, 1, 1);
        }
        if (s_state.ble_connected) {
            OLED_DrawIcon(54, 3, OLED_ICON_BLE, 1);
        }
        for (int16_t i = 0; i < 3; i++) {
            int16_t ix = (int16_t)(62 + i * 5);
            if (i == (int16_t)sub_idx) {
                OLED_FillRect(ix, 4, 4, 4, 1);
            } else {
                OLED_DrawRect(ix, 4, 4, 4, 1);
            }
        }
        OLED_DrawLine(8, 11, 79, 11, 1);
    }
}

/* ============================================================================
 * SCREEN 1: ACCUEIL (A=Minimaliste, B=Dashboard, C=Energie/Autonomie)
 * ==========================================================================*/
static void Render_ScreenHome(void)
{
    char buf[24];

    if (s_state.split_6h) {
        char wd2[3];
        SmileWatch_DrawHeader("HOME", s_state.sub_view);
        (void)snprintf(buf, sizeof(buf), "%02u:%02u", s_state.hours, s_state.minutes);
        OLED_DrawStr(4, 14, buf, 1, 1);
        if (s_state.steps_count >= 10000UL) {
            (void)snprintf(buf, sizeof(buf), "%lu.%luK",
                           (unsigned long)(s_state.steps_count / 1000UL),
                           (unsigned long)((s_state.steps_count % 1000UL) / 100UL));
        } else {
            (void)snprintf(buf, sizeof(buf), "%luP", (unsigned long)s_state.steps_count);
        }
        OLED_DrawStr(4, 25, buf, 1, 1);
        strncpy(wd2, s_state.weekday_str, 2U);
        wd2[2] = '\0';
        (void)snprintf(buf, sizeof(buf), "%s%02u", wd2, s_state.day);
        OLED_DrawStr(10, 37, buf, 1, 1);

        OLED_DrawBitmap(52, 13, k_garmin_heart_10x9, 10, 9, 1);
        (void)snprintf(buf, sizeof(buf), "%u", s_state.heart_bpm);
        OLED_DrawStr(64, 14, buf, 1, 1);
        (void)snprintf(buf, sizeof(buf), "%d.%dC", s_state.temp_deci_c / 10, s_state.temp_deci_c % 10);
        OLED_DrawStr(52, 25, buf, 1, 1);
        (void)snprintf(buf, sizeof(buf), "%u%%", s_state.battery_pct);
        OLED_DrawStr(52, 37, buf, 1, 1);
        return;
    }

    if (s_state.sub_view == 0U) {
        if (s_state.survival_mode) {
            SmileWatch_DrawHeader("SURVIE", 0U);
            (void)snprintf(buf, sizeof(buf), "%u%%", s_state.battery_pct);
            OLED_DrawStr(18, 15, buf, 2, 1);
            OLED_DrawStr(11, 37, "MIYOTA ONLY", 1, 1);
            return;
        }
        if (s_state.wake_summary_active) {
            int16_t dt = (int16_t)(s_state.temp_deci_c - s_state.temp_base_deci_c);
            const char *rec = (s_state.coach_rec_code == 0U) ? "Z2 REC" :
                              (s_state.coach_rec_code == 1U) ? "Z3 END" :
                              (s_state.coach_rec_code == 2U) ? "Z4 SEU" :
                              (s_state.coach_rec_code == 3U) ? "HIIT" : "REPOS!";
            SmileWatch_DrawHeader("REVEIL", 0U);
            (void)snprintf(buf, sizeof(buf), "R:%u H:%u", s_state.rhr_base_bpm, s_state.hrv_rmssd_ms);
            OLED_DrawStr(4, 12, buf, 1, 1);
            (void)snprintf(buf, sizeof(buf), "T:%s%d.%d %s", (dt >= 0) ? "+" : "-",
                           (dt >= 0 ? dt : -dt) / 10, (dt >= 0 ? dt : -dt) % 10,
                           s_state.pre_illness_alert ? "AL!" : "OK");
            OLED_DrawStr(4, 21, buf, 1, 1);
            (void)snprintf(buf, sizeof(buf), "COACH:%s", rec);
            OLED_DrawStr(4, 29, buf, 1, 1);
            OLED_DrawStr(11, 37, "4H:FERMER", 1, 1);
            return;
        }
        char wd2[3];
        if (s_state.ble_connected) {
            OLED_DrawIcon(12, 3, OLED_ICON_BLE, 1);
        }
        if (s_state.pill_hand_paused) {
            (void)snprintf(buf, sizeof(buf), "%02u:%02u", s_state.hours, s_state.minutes);
            OLED_DrawStr(21, 3, buf, 1, 1);
            OLED_FillRect(54, 4, 2, 5, 1);
            OLED_FillRect(57, 4, 2, 5, 1);
        } else {
            strncpy(wd2, s_state.weekday_str, 2U);
            wd2[2] = '\0';
            (void)snprintf(buf, sizeof(buf), "%s %02u/%02u", wd2, s_state.day, s_state.month);
            OLED_DrawStr(21, 3, buf, 1, 1);
        }
        OLED_DrawRect(67, 4, 8, 5, 1);
        OLED_FillRect(75, 5, 2, 3, 1);
        OLED_FillRect(68, 5, (int16_t)((s_state.battery_pct * 6U) / 100U), 3, 1);
        OLED_DrawLine(8, 11, 79, 11, 1);

        if (s_state.steps_count >= 100000UL) {
            (void)snprintf(buf, sizeof(buf), "%luK", (unsigned long)(s_state.steps_count / 1000UL));
        } else {
            (void)snprintf(buf, sizeof(buf), "%lu", (unsigned long)s_state.steps_count);
        }
        OLED_DrawStr(3, 14, buf, 2, 1);
        OLED_DrawStr(65, 19, "PAS", 1, 1);

        OLED_DrawRoundRect(6, 31, 76, 5, 2, 1);
        for (int16_t gx = 8; gx < 80; gx += 2) {
            OLED_DrawPixel(gx, 33, 1);
        }
        uint32_t pct = (s_state.steps_goal > 0U) ? ((s_state.steps_count * 100UL) / s_state.steps_goal) : 0UL;
        if (pct > 100UL) pct = 100UL;
        if (pct > 0U) {
            OLED_FillRoundRect(7, 32, (int16_t)((pct * 74UL) / 100UL), 3, 1, 1);
        }

        (void)snprintf(buf, sizeof(buf), "%u%% %u.%02uV",
                       s_state.battery_pct, s_state.battery_mv / 1000U, (s_state.battery_mv % 1000U) / 10U);
        OLED_DrawStr(10, 37, buf, 1, 1);
        if (s_state.qi_charging) {
            OLED_DrawIcon(69, 36, OLED_ICON_BOLT, 1);
        }
    } else if (s_state.sub_view == 1U) {
        SmileWatch_DrawHeader("HOME", 1U);
        (void)snprintf(buf, sizeof(buf), "%02u:%02u", s_state.hours, s_state.minutes);
        OLED_DrawStr(4, 13, buf, 2, 1);
        (void)snprintf(buf, sizeof(buf), ":%02u", s_state.seconds);
        OLED_DrawStr(65, 20, buf, 1, 1);
        OLED_DrawLine(3, 28, 84, 28, 1);

        OLED_DrawBitmap(5, 30, k_garmin_heart_10x9, 10, 9, 1);
        (void)snprintf(buf, sizeof(buf), "%u", s_state.heart_bpm);
        OLED_DrawStr(16, 31, buf, 1, 1);
        (void)snprintf(buf, sizeof(buf), "%d.%dC", s_state.temp_deci_c / 10, s_state.temp_deci_c % 10);
        OLED_DrawStr(34, 31, buf, 1, 1);
        if (s_state.battery_pct >= 100U) {
            (void)snprintf(buf, sizeof(buf), "100");
        } else {
            (void)snprintf(buf, sizeof(buf), "%u%%", s_state.battery_pct);
        }
        OLED_DrawStr(64, 31, buf, 1, 1);
    } else {
        SmileWatch_DrawHeader("ENERGIE", 2U);
        (void)snprintf(buf, sizeof(buf), "BAT %u%%", s_state.battery_pct);
        OLED_DrawStr(4, 12, buf, 1, 1);
        (void)snprintf(buf, sizeof(buf), "%u.%02uV", s_state.battery_mv / 1000U, (s_state.battery_mv % 1000U) / 10U);
        OLED_DrawStr(52, 12, buf, 1, 1);

        OLED_DrawRoundRect(2, 21, 84, 5, 2, 1);
        for (int16_t gx = 4; gx < 84; gx += 2) {
            OLED_DrawPixel(gx, 23, 1);
        }
        if (s_state.battery_pct > 0U) {
            OLED_FillRoundRect(3, 22, (int16_t)((s_state.battery_pct * 82U) / 100U), 3, 1, 1);
        }

        if (s_state.current_avg_ua >= 1000U) {
            (void)snprintf(buf, sizeof(buf), "CONSO:%u.%uMA",
                           (unsigned int)(s_state.current_avg_ua / 1000U),
                           (unsigned int)((s_state.current_avg_ua % 1000U) / 100U));
        } else {
            (void)snprintf(buf, sizeof(buf), "CONSO:%uUA", (unsigned int)s_state.current_avg_ua);
        }
        OLED_DrawStr(5, 29, buf, 1, 1);
        uint16_t days = s_state.autonomy_days;
        (void)snprintf(buf, sizeof(buf), "AUT:%uJ %s", days, s_state.qi_charging ? "QI" : "OK");
        OLED_DrawStr(11, 37, buf, 1, 1);
    }
}

/* ============================================================================
 * SCREEN 2: CARDIO & SPO2 (MAX30101)
 * ==========================================================================*/
static void Render_ScreenHeart(void)
{
    char buf[24];
    SmileWatch_DrawHeader("CARDIO", s_state.sub_view);

    if (s_state.split_6h) {
        OLED_DrawStr(4, 13, "BPM", 1, 1);
        (void)snprintf(buf, sizeof(buf), "%u", s_state.heart_bpm);
        OLED_DrawStr(3, 21, buf, 2, 1);
        OLED_DrawStr(10, 37, s_state.hr_measuring ? "LIVE" : "STOP", 1, 1);

        OLED_DrawStr(52, 13, "SPO2", 1, 1);
        (void)snprintf(buf, sizeof(buf), "%u%%", s_state.spo2_pct);
        OLED_DrawStr(52, 24, buf, 1, 1);
        uint8_t zone = (s_state.heart_bpm < 100U) ? 1U :
                       (s_state.heart_bpm < 120U) ? 2U :
                       (s_state.heart_bpm < 140U) ? 3U :
                       (s_state.heart_bpm < 160U) ? 4U : 5U;
        (void)snprintf(buf, sizeof(buf), "ZN:%u", zone);
        OLED_DrawStr(52, 37, buf, 1, 1);
        return;
    }

    if (s_state.sub_view == 0U) {
        int16_t hy = ((s_state.ppg_phase % 6U) < 2U) ? 14 : 15;
        OLED_DrawBitmap(5, hy, k_garmin_heart_10x9, 10, 9, 1);
        (void)snprintf(buf, sizeof(buf), "%u", s_state.heart_bpm);
        OLED_DrawStr(18, 13, buf, 2, 1);
        OLED_DrawStr(55, 20, "BPM", 1, 1);
        OLED_DrawStr(67, 13, s_state.hr_measuring ? "ON" : "OFF", 1, 1);

        int16_t prev_y = 38;
        for (int16_t x = 12; x <= 75; x++) {
            uint8_t idx = (uint8_t)((x + s_state.ppg_phase) % 22U);
            int16_t y = (int16_t)(43 - (k_ppg_wave[idx] * 10) / 16);
            if (x > 12) OLED_DrawLine((int16_t)(x - 1), prev_y, x, y, 1);
            prev_y = y;
        }
    } else if (s_state.sub_view == 1U) {
        OLED_DrawStr(4, 13, "SPO2", 1, 1);
        (void)snprintf(buf, sizeof(buf), "%u", s_state.spo2_pct);
        OLED_DrawStr(3, 22, buf, 2, 1);
        OLED_DrawStr(38, 28, "%", 1, 1);

        OLED_DrawLine(45, 13, 45, 43, 1);
        (void)snprintf(buf, sizeof(buf), "MIN%u", s_state.hr_min);
        OLED_DrawStr(48, 14, buf, 1, 1);
        (void)snprintf(buf, sizeof(buf), "MAX%u", s_state.hr_max);
        OLED_DrawStr(48, 25, buf, 1, 1);
        uint8_t zone = (s_state.heart_bpm < 100U) ? 1U :
                       (s_state.heart_bpm < 120U) ? 2U :
                       (s_state.heart_bpm < 140U) ? 3U :
                       (s_state.heart_bpm < 160U) ? 4U : 5U;
        (void)snprintf(buf, sizeof(buf), "ZONE%u", zone);
        OLED_DrawStr(47, 36, buf, 1, 1);
    } else {
        for (int16_t ax = 11; ax <= 75; ax += 3) {
            OLED_DrawPixel(ax, 35, 1);
        }
        int16_t prev_y = 34;
        for (int16_t i = 0; i < 44; i++) {
            int16_t bpm = (int16_t)k_hr_24h_history[i];
            int16_t h = ((bpm - 40) * 21) / 100;
            if (h < 1) h = 1;
            if (h > 21) h = 21;
            int16_t y = (int16_t)(35 - h);
            int16_t x = (int16_t)(11 + (i * 64) / 43);
            int16_t px = (int16_t)(11 + ((i - 1) * 64) / 43);
            if (i > 0) OLED_DrawLine(px, prev_y, x, y, 1);
            prev_y = y;
        }
        OLED_DrawStr(11, 37, "0H  12H 24H", 1, 1);
    }
}

/* ============================================================================
 * SCREEN 3: SANTE (TMP117 +-0.1C, Body Battery & Stress, Sommeil)
 * ==========================================================================*/
static void Render_ScreenHealth(void)
{
    char buf[24];
    SmileWatch_DrawHeader("SANTE", s_state.sub_view);

    if (s_state.split_6h) {
        OLED_DrawBitmap(4, 13, k_garmin_thermo_8x12, 8, 12, 1);
        OLED_DrawStr(15, 15, "TMP", 1, 1);
        (void)snprintf(buf, sizeof(buf), "%d.%dC", s_state.temp_deci_c / 10, s_state.temp_deci_c % 10);
        OLED_DrawStr(4, 27, buf, 1, 1);
        OLED_DrawStr(10, 37, (s_state.temp_deci_c >= 378) ? "FIEV" : "NORM", 1, 1);

        (void)snprintf(buf, sizeof(buf), "BB%u", s_state.body_battery);
        OLED_DrawStr(52, 14, buf, 1, 1);
        (void)snprintf(buf, sizeof(buf), "ST%u", s_state.stress_score);
        OLED_DrawStr(52, 25, buf, 1, 1);
        uint16_t sh = s_state.sleep_min / 60U;
        if (sh > 9U) {
            (void)snprintf(buf, sizeof(buf), "%uH", sh);
        } else {
            (void)snprintf(buf, sizeof(buf), "%uH%02u", sh, s_state.sleep_min % 60U);
        }
        OLED_DrawStr(52, 37, buf, 1, 1);
        return;
    }

    if (s_state.sub_view == 0U) {
        OLED_DrawBitmap(8, 14, k_garmin_thermo_8x12, 8, 12, 1);
        (void)snprintf(buf, sizeof(buf), "%d.%d", s_state.temp_deci_c / 10, s_state.temp_deci_c % 10);
        OLED_DrawStr(21, 14, buf, 2, 1);
        OLED_DrawStr(69, 14, "C", 2, 1);
        OLED_DrawLine(4, 31, 83, 31, 1);

        int16_t diff = s_state.temp_deci_c - s_state.temp_base_deci_c;
        int16_t adiff = (diff < 0) ? -diff : diff;
        (void)snprintf(buf, sizeof(buf), "%s%d.%dC %s",
                       (diff >= 0) ? "+" : "-", adiff / 10, adiff % 10,
                       (s_state.temp_deci_c >= 378) ? "FIEV!" : "NORM");
        OLED_DrawStr(11, 37, buf, 1, 1);
    } else if (s_state.sub_view == 1U) {
        OLED_DrawBitmap(4, 13, k_garmin_bodybat_12x10, 12, 10, 1);
        (void)snprintf(buf, sizeof(buf), "BODY:%u%%", s_state.body_battery);
        OLED_DrawStr(18, 14, buf, 1, 1);

        OLED_DrawRoundRect(4, 22, 80, 5, 2, 1);
        for (int16_t gx = 6; gx < 82; gx += 2) {
            OLED_DrawPixel(gx, 24, 1);
        }
        if (s_state.body_battery > 0U) {
            OLED_FillRoundRect(5, 23, (int16_t)((s_state.body_battery * 78U) / 100U), 3, 1, 1);
        }

        (void)snprintf(buf, sizeof(buf), "HRV:%u ST:%u", s_state.hrv_rmssd_ms, s_state.stress_score);
        OLED_DrawStr(4, 29, buf, 1, 1);
        OLED_DrawStr(11, 37, (s_state.readiness_state == 0U) ? "ETAT: FORME" :
                             (s_state.readiness_state == 1U) ? "ETAT:MODERE" : "ETAT:REPOS!", 1, 1);
    } else {
        OLED_DrawBitmap(4, 12, k_icon_moon_10x10, 10, 10, 1);
        OLED_DrawStr(15, 13, "SOMMEIL", 1, 1);
        (void)snprintf(buf, sizeof(buf), "%uH%02u", s_state.sleep_min / 60U, s_state.sleep_min % 60U);
        OLED_DrawStr(57, 13, buf, 1, 1);

        OLED_DrawRect(4, 22, 80, 6, 1);
        int16_t deep_w = (int16_t)((s_state.sleep_deep_pct * 78U) / 100U);
        int16_t rem_w  = (int16_t)((s_state.sleep_rem_pct * 78U) / 100U);
        if (deep_w > 78) deep_w = 78;
        if (deep_w + rem_w > 78) rem_w = (int16_t)(78 - deep_w);
        if (deep_w > 0) {
            OLED_FillRect(5, 23, deep_w, 4, 1);
        }
        OLED_DrawLine((int16_t)(5 + deep_w + rem_w), 22, (int16_t)(5 + deep_w + rem_w), 27, 1);

        (void)snprintf(buf, sizeof(buf), "PR%u%% REM%u%%", s_state.sleep_deep_pct, s_state.sleep_rem_pct);
        OLED_DrawStr(8, 30, buf, 1, 1);
        (void)snprintf(buf, sizeof(buf), "Q:%u %s", s_state.sleep_score,
                       (s_state.sleep_stage == 2U) ? "PROF" :
                       (s_state.sleep_stage == 1U) ? "LEGR" :
                       (s_state.sleep_stage == 3U) ? "REM" : "EVEI");
        OLED_DrawStr(12, 37, buf, 1, 1);
    }
}

/* ============================================================================
 * SCREEN 4: ACTIVITE QUOTIDIENNE (LSM6DSOX Podometre + 7j)
 * ==========================================================================*/
static void Render_ScreenActivity(void)
{
    char buf[24];
    SmileWatch_DrawHeader("ACTIV", s_state.sub_view);

    uint32_t pct = (s_state.steps_goal > 0U) ? ((s_state.steps_count * 100UL) / s_state.steps_goal) : 0UL;
    if (pct > 100UL) pct = 100UL;
    uint32_t dist_hm = (s_state.steps_count * (uint32_t)((s_state.stride_cm > 0U) ? s_state.stride_cm : 74U)) / 10000UL;
    uint32_t kcal    = (s_state.steps_count * 4UL) / 100UL;

    if (s_state.split_6h) {
        OLED_DrawBitmap(3, 13, k_garmin_shoe_12x9, 12, 9, 1);
        if (pct >= 100UL) {
            (void)snprintf(buf, sizeof(buf), "100");
        } else {
            (void)snprintf(buf, sizeof(buf), "%u%%", (unsigned int)pct);
        }
        OLED_DrawStr(17, 13, buf, 1, 1);
        if (s_state.steps_count >= 100000UL) {
            (void)snprintf(buf, sizeof(buf), "%luK", (unsigned long)(s_state.steps_count / 1000UL));
        } else {
            (void)snprintf(buf, sizeof(buf), "%lu", (unsigned long)s_state.steps_count);
        }
        OLED_DrawStr(4, 24, buf, 1, 1);
        OLED_DrawRect(5, 34, 30, 5, 1);
        if (pct > 0U) {
            OLED_FillRect(6, 35, (int16_t)((pct * 28UL) / 100UL), 3, 1);
        }

        if (dist_hm >= 100UL) {
            (void)snprintf(buf, sizeof(buf), "%lu.%uK", (unsigned long)(dist_hm / 10UL), (unsigned long)(dist_hm % 10UL));
        } else {
            (void)snprintf(buf, sizeof(buf), "%lu.%uKM", (unsigned long)(dist_hm / 10UL), (unsigned long)(dist_hm % 10UL));
        }
        OLED_DrawStr(52, 13, buf, 1, 1);
        if (kcal >= 1000UL) {
            (void)snprintf(buf, sizeof(buf), "%luK", (unsigned long)kcal);
        } else {
            (void)snprintf(buf, sizeof(buf), "%luKC", (unsigned long)kcal);
        }
        OLED_DrawStr(52, 25, buf, 1, 1);
        OLED_DrawStr(52, 37, "10KP", 1, 1);
        return;
    }

    if (s_state.sub_view == 0U) {
        if (s_state.steps_count >= 100000UL) {
            (void)snprintf(buf, sizeof(buf), "%luK", (unsigned long)(s_state.steps_count / 1000UL));
        } else {
            (void)snprintf(buf, sizeof(buf), "%lu", (unsigned long)s_state.steps_count);
        }
        OLED_DrawStr(3, 14, buf, 2, 1);
        (void)snprintf(buf, sizeof(buf), "%u%%", (unsigned int)pct);
        OLED_DrawStr(64, 17, buf, 1, 1);

        OLED_DrawRoundRect(5, 31, 78, 5, 2, 1);
        for (int16_t gx = 7; gx < 81; gx += 2) {
            OLED_DrawPixel(gx, 33, 1);
        }
        if (pct > 0U) {
            OLED_FillRoundRect(6, 32, (int16_t)((pct * 76UL) / 100UL), 3, 1, 1);
        }

        if (dist_hm >= 100UL) {
            (void)snprintf(buf, sizeof(buf), "%luKM %luKC", (unsigned long)(dist_hm / 10UL), (unsigned long)kcal);
        } else {
            (void)snprintf(buf, sizeof(buf), "%lu.%uKM %luKC",
                           (unsigned long)(dist_hm / 10UL),
                           (unsigned long)(dist_hm % 10UL),
                           (unsigned long)kcal);
        }
        OLED_DrawStr(11, 37, buf, 1, 1);
    } else if (s_state.sub_view == 1U) {
        (void)snprintf(buf, sizeof(buf), "DIST: %lu.%uKM", (unsigned long)(dist_hm / 10UL), (unsigned long)(dist_hm % 10UL));
        OLED_DrawStr(5, 12, buf, 1, 1);
        (void)snprintf(buf, sizeof(buf), "ENERG:%lu KC", (unsigned long)kcal);
        OLED_DrawStr(4, 21, buf, 1, 1);
        (void)snprintf(buf, sizeof(buf), "ACTIF:%lu MIN", (unsigned long)(s_state.steps_count / 110UL));
        OLED_DrawStr(5, 29, buf, 1, 1);
        (void)snprintf(buf, sizeof(buf), "OBJ: %lu", (unsigned long)s_state.steps_goal);
        OLED_DrawStr(14, 37, buf, 1, 1);
    } else {
        for (int16_t gx = 10; gx <= 77; gx += 3) {
            OLED_DrawPixel(gx, 18, 1);
        }
        const char *days = "LMMJVSD";
        for (int16_t d = 0; d < 7; d++) {
            uint32_t st = (d == 6) ? s_state.steps_count : (uint32_t)k_week_steps[d];
            int16_t h = (int16_t)((st * 20UL) / 12000UL);
            if (h < 2) h = 2;
            if (h > 20) h = 20;
            int16_t bx = (int16_t)(11 + d * 10);
            OLED_FillRect(bx, (int16_t)(35 - h), 6, h, 1);
            OLED_DrawChar((int16_t)(bx + 1), 37, days[d], 1, 1);
        }
    }
}

/* ============================================================================
 * SCREEN 5: SPORT AUTONOME (Menu Deroulant 10 Sports, Live & IA MLC/Recup)
 * ==========================================================================*/
static void Render_ScreenSport(void)
{
    char buf[24];
    SmileWatch_DrawHeader("SPORT", s_state.sub_view);
    uint8_t curr = (uint8_t)(s_state.sport_type % 19U);
    uint8_t prev = (uint8_t)((curr + 18U) % 19U);
    uint8_t next = (uint8_t)((curr + 1U) % 19U);

    if (s_state.split_6h) {
        if (s_state.sub_view == 0U) {
            OLED_DrawStr(4, 13, "MENU", 1, 1);
            OLED_DrawStr(4, 25, k_sport_short[curr], 1, 1);
            (void)snprintf(buf, sizeof(buf), "#%u", (unsigned int)(curr + 1U));
            OLED_DrawStr(10, 37, buf, 1, 1);
        } else if (s_state.sub_view == 1U) {
            OLED_DrawStr(4, 13, k_sport_short[curr], 1, 1);
            if (curr == 0U) {
                (void)snprintf(buf, sizeof(buf), "%ux%u", s_state.gym_sets, s_state.gym_reps);
                OLED_DrawStr(4, 25, buf, 1, 1);
                (void)snprintf(buf, sizeof(buf), "%us", s_state.gym_rest_sec);
                OLED_DrawStr(10, 37, buf, 1, 1);
            } else if (curr == 2U) {
                (void)snprintf(buf, sizeof(buf), "%uL", s_state.swim_laps);
                OLED_DrawStr(4, 25, buf, 1, 1);
                uint16_t sm = (uint16_t)(s_state.swim_laps * 25U);
                if (sm >= 1000U) {
                    (void)snprintf(buf, sizeof(buf), "%u.%uK", sm / 1000U, (sm % 1000U) / 100U);
                } else {
                    (void)snprintf(buf, sizeof(buf), "%uM", sm);
                }
                OLED_DrawStr(10, 37, buf, 1, 1);
            } else {
                (void)snprintf(buf, sizeof(buf), "%u'%02u", (s_state.run_pace_sec / 60U) % 10U, s_state.run_pace_sec % 60U);
                OLED_DrawStr(4, 25, buf, 1, 1);
                OLED_DrawStr(10, 37, "/KM", 1, 1);
            }
        } else {
            OLED_DrawStr(4, 13, "MLC", 1, 1);
            (void)snprintf(buf, sizeof(buf), "IA%u%%", (unsigned int)(s_state.mlc_confidence_pct % 100U));
            OLED_DrawStr(4, 25, buf, 1, 1);
            (void)snprintf(buf, sizeof(buf), "R%uH", s_state.recovery_hours);
            OLED_DrawStr(10, 37, buf, 1, 1);
        }

        OLED_DrawStr(52, 13, s_state.sport_active ? "ACTIF" : "PAUSE", 1, 1);
        (void)snprintf(buf, sizeof(buf), "%02u:%02u", (s_state.sport_sec / 60U) % 100U, s_state.sport_sec % 60U);
        OLED_DrawStr(52, 25, buf, 1, 1);
        (void)snprintf(buf, sizeof(buf), "%uB", s_state.heart_bpm);
        OLED_DrawStr(52, 37, buf, 1, 1);
        return;
    }

    if (s_state.sub_view == 0U) {
        OLED_DrawStr(16, 13, k_sport_names[prev], 1, 1);
        OLED_DrawRoundRect(6, 21, 66, 11, 3, 1);
        (void)snprintf(buf, sizeof(buf), ">%s", k_sport_names[curr]);
        OLED_DrawStr(10, 23, buf, 1, 1);

        OLED_DrawLine(77, 14, 77, 33, 1);
        int16_t ty = (int16_t)(14 + (curr * 16) / 18);
        OLED_FillRect(76, ty, 3, 4, 1);

        (void)snprintf(buf, sizeof(buf), "4HL:%s", k_sport_names[next]);
        OLED_DrawStr(11, 37, buf, 1, 1);
    } else if (s_state.sub_view == 1U) {
        if (s_state.tri_active) {
            const char *tri_lbl = (s_state.tri_stage == 0U) ? "NAGE" :
                                  (s_state.tri_stage == 1U) ? "TR-1" :
                                  (s_state.tri_stage == 2U) ? "VELO" :
                                  (s_state.tri_stage == 3U) ? "TR-2" : "CAP ";
            OLED_DrawStr(4, 12, "TRIATHLON", 1, 1);
            OLED_DrawStr(62, 12, tri_lbl, 1, 1);
            (void)snprintf(buf, sizeof(buf), "%02u:%02u", (s_state.tri_total_sec / 60U) % 100U, s_state.tri_total_sec % 60U);
            OLED_DrawStr(4, 21, buf, 2, 1);
            (void)snprintf(buf, sizeof(buf), "%uB", s_state.heart_bpm);
            OLED_DrawStr(66, 24, buf, 1, 1);
            OLED_DrawStr(11, 37, "4HL:ETAPE+", 1, 1);
        } else {
            if (curr == 2U) {
                OLED_DrawBitmap(4, 12, k_icon_swim_12x9, 12, 9, 1);
            } else if ((curr == 0U) || (curr == 4U) || (curr == 7U) || (curr == 8U)) {
                OLED_DrawBitmap(4, 12, k_icon_gym_12x9, 12, 9, 1);
            } else {
                OLED_DrawBitmap(4, 12, k_icon_run_10x10, 10, 10, 1);
            }
            OLED_DrawStr(18, 12, k_sport_names[curr], 1, 1);
            OLED_DrawStr(64, 12, s_state.sport_active ? "ACT" : "PAU", 1, 1);

            if (curr == 0U) {
                (void)snprintf(buf, sizeof(buf), "%u", s_state.gym_reps);
                OLED_DrawStr(4, 22, buf, 2, 1);
                OLED_DrawStr(28, 27, "REPS", 1, 1);
                (void)snprintf(buf, sizeof(buf), "S:%u", s_state.gym_sets);
                OLED_DrawStr(56, 23, buf, 1, 1);
                (void)snprintf(buf, sizeof(buf), "R:%us %uB", s_state.gym_rest_sec, s_state.heart_bpm);
                OLED_DrawStr(11, 37, buf, 1, 1);
            } else if (curr == 2U) {
                (void)snprintf(buf, sizeof(buf), "%u", s_state.swim_laps);
                OLED_DrawStr(4, 22, buf, 2, 1);
                OLED_DrawStr(34, 22, "LONG", 1, 1);
                (void)snprintf(buf, sizeof(buf), "%uM", s_state.swim_laps * 25U);
                OLED_DrawStr(34, 30, buf, 1, 1);
                OLED_DrawStr(11, 37, (s_state.swim_stroke == 0U) ? "CRAWL" : "BRASSE", 1, 1);
                (void)snprintf(buf, sizeof(buf), "%02u:%02u", (s_state.sport_sec / 60U) % 100U, s_state.sport_sec % 60U);
                OLED_DrawStr(48, 37, buf, 1, 1);
            } else if (curr == 3U) {
                uint16_t spd = (uint16_t)(180U + (s_state.sport_metric % 220U));
                (void)snprintf(buf, sizeof(buf), "%u.%u", spd / 10U, spd % 10U);
                OLED_DrawStr(4, 22, buf, 2, 1);
                OLED_DrawStr(54, 22, "KMH", 1, 1);
                (void)snprintf(buf, sizeof(buf), "%uB", s_state.heart_bpm);
                OLED_DrawStr(54, 30, buf, 1, 1);
                (void)snprintf(buf, sizeof(buf), "CAD:%uRPM", (unsigned int)(75U + (s_state.sport_metric % 25U)));
                OLED_DrawStr(11, 37, buf, 1, 1);
            } else if (curr == 4U) {
                /* Tabata 20s Work / 10s Rest x 8 */
                (void)snprintf(buf, sizeof(buf), "%02u", s_state.tabata_sec_left);
                OLED_DrawStr(4, 22, buf, 2, 1);
                OLED_DrawStr(32, 22, s_state.tabata_work ? "EFFORT" : "REPOS", 1, 1);
                (void)snprintf(buf, sizeof(buf), "R:%u/8", s_state.tabata_round);
                OLED_DrawStr(32, 30, buf, 1, 1);
                (void)snprintf(buf, sizeof(buf), "TABATA %uB", s_state.heart_bpm);
                OLED_DrawStr(11, 37, buf, 1, 1);
            } else if (curr == 9U) {
                /* Guided Coherence 5s Inhale / 5s Exhale */
                uint8_t bp = s_state.breath_phase_sec % 10U;
                bool inh = (bp < 5U);
                int16_t fill = inh ? (int16_t)(((bp + 1U) * 76U) / 5U) : (int16_t)(((10U - bp) * 76U) / 5U);
                OLED_DrawRoundRect(5, 22, 78, 7, 3, 1);
                if (fill > 2) {
                    OLED_FillRoundRect(6, 23, fill, 5, 2, 1);
                }
                OLED_DrawStr(11, 37, inh ? "INSPIR 5S" : "EXPIR  5S", 1, 1);
            } else if ((curr == 6U) || (curr == 7U) || (curr == 8U) || (curr == 10U) || (curr == 11U) || (curr == 15U) || (curr == 16U) || (curr == 17U) || (curr == 18U)) {
                (void)snprintf(buf, sizeof(buf), "%u", (unsigned int)(s_state.sport_metric % 1000U));
                OLED_DrawStr(4, 22, buf, 2, 1);
                OLED_DrawStr(44, 22, ((curr == 6U) || (curr == 15U)) ? "COUPS" :
                                     (curr == 8U) ? "RAMES" :
                                     (curr == 10U) ? "SAUTS" :
                                     (curr == 16U) ? "SWING" :
                                     (curr == 17U) ? "DESC" :
                                     (curr == 18U) ? "SPRIN" : "MVTS", 1, 1);
                (void)snprintf(buf, sizeof(buf), "%uB", s_state.heart_bpm);
                OLED_DrawStr(44, 30, buf, 1, 1);
                (void)snprintf(buf, sizeof(buf), "T:%02u:%02u", (s_state.sport_sec / 60U) % 100U, s_state.sport_sec % 60U);
                OLED_DrawStr(11, 37, buf, 1, 1);
            } else if (curr == 14U) {
                /* Gainage / Mobilite: cycle posture 30s */
                uint8_t rem30 = (uint8_t)(30U - (s_state.sport_sec % 30U));
                (void)snprintf(buf, sizeof(buf), "%02u", rem30);
                OLED_DrawStr(4, 22, buf, 2, 1);
                OLED_DrawStr(32, 22, "POSTURE", 1, 1);
                (void)snprintf(buf, sizeof(buf), "%uB", s_state.heart_bpm);
                OLED_DrawStr(32, 30, buf, 1, 1);
                (void)snprintf(buf, sizeof(buf), "T:%02u:%02u", (s_state.sport_sec / 60U) % 100U, s_state.sport_sec % 60U);
                OLED_DrawStr(11, 37, buf, 1, 1);
            } else {
                (void)snprintf(buf, sizeof(buf), "%02u:%02u", (s_state.sport_sec / 60U) % 100U, s_state.sport_sec % 60U);
                OLED_DrawStr(4, 22, buf, 2, 1);
                (void)snprintf(buf, sizeof(buf), "%u", s_state.heart_bpm);
                OLED_DrawStr(65, 21, buf, 1, 1);
                OLED_DrawStr(65, 29, "BPM", 1, 1);
                if ((curr == 5U) || (curr == 13U)) {
                    (void)snprintf(buf, sizeof(buf), "D+%uM %uB", (unsigned int)(s_state.sport_metric % 1000U), s_state.heart_bpm);
                } else if (s_state.coach_enabled && s_state.sport_active) {
                    (void)snprintf(buf, sizeof(buf), "Z%u/Z%u %s",
                                   s_state.current_hr_zone, s_state.target_hr_zone,
                                   (s_state.coach_status == 2U) ? "RAL" :
                                   (s_state.coach_status == 1U) ? "ACC" : "OK");
                } else {
                    (void)snprintf(buf, sizeof(buf), "AL %u'%02u/KM", s_state.run_pace_sec / 60U, s_state.run_pace_sec % 60U);
                }
                OLED_DrawStr(11, 37, buf, 1, 1);
            }
        }
    } else {
        (void)snprintf(buf, sizeof(buf), "IA MLC:%u%%", s_state.mlc_confidence_pct);
        OLED_DrawStr(4, 12, buf, 1, 1);
        if (s_state.calib_error_pct == 0) {
            OLED_DrawStr(68, 12, "OK", 1, 1);
        } else {
            int16_t aerr = (s_state.calib_error_pct < 0)
                           ? (int16_t)(-s_state.calib_error_pct)
                           : (int16_t)s_state.calib_error_pct;
            (void)snprintf(buf, sizeof(buf), "%s%d", (s_state.calib_error_pct > 0) ? "+" : "-", (int)aerr);
            OLED_DrawStr(66, 12, buf, 1, 1);
        }

        OLED_DrawRoundRect(4, 21, 80, 5, 2, 1);
        for (int16_t gx = 6; gx < 82; gx += 2) {
            OLED_DrawPixel(gx, 23, 1);
        }
        if (s_state.mlc_confidence_pct > 0U) {
            OLED_FillRoundRect(5, 22, (int16_t)((s_state.mlc_confidence_pct * 78U) / 100U), 3, 1, 1);
        }

        if ((curr == 1U) || (curr == 5U)) {
            (void)snprintf(buf, sizeof(buf), "FL:%uCM V%u", s_state.stride_cm, s_state.vo2_max);
        } else {
            (void)snprintf(buf, sizeof(buf), "S:%uMG V%u", s_state.mlc_thresh_mg, s_state.vo2_max);
        }
        OLED_DrawStr(5, 29, buf, 1, 1);
        if (s_state.hrr_active) {
            (void)snprintf(buf, sizeof(buf), "HRR60: %uS", s_state.hrr_sec_left);
        } else if (s_state.hrr_drop_bpm > 0U) {
            (void)snprintf(buf, sizeof(buf), "R:%uH -%uB", s_state.recovery_hours, s_state.hrr_drop_bpm);
        } else if (s_state.coach_enabled) {
            const char *rec = (s_state.coach_rec_code == 0U) ? "Z2 REC" :
                              (s_state.coach_rec_code == 1U) ? "Z3 END" :
                              (s_state.coach_rec_code == 2U) ? "Z4 SEU" :
                              (s_state.coach_rec_code == 3U) ? "HIIT" : "REPOS!";
            (void)snprintf(buf, sizeof(buf), "C:%s", rec);
        } else {
            (void)snprintf(buf, sizeof(buf), "R:%uH %s", s_state.recovery_hours,
                           (s_state.readiness_state == 0U) ? "FORME" :
                           (s_state.readiness_state == 1U) ? "MOD" : "REP!");
        }
        OLED_DrawStr(11, 37, buf, 1, 1);
    }
}

/* ============================================================================
 * SCREEN 6: CHRONOMETRE & MINUTEUR
 * ==========================================================================*/
static void Render_ScreenChrono(void)
{
    char buf[24];
    SmileWatch_DrawHeader("CHRONO", s_state.sub_view);

    uint32_t total_s = s_state.chrono_tenths / 10UL;
    uint32_t cm = (total_s / 60UL) % 100UL;
    uint32_t cs = total_s % 60UL;
    uint32_t ct = s_state.chrono_tenths % 10UL;

    uint32_t lap_s = s_state.chrono_lap_tenths / 10UL;
    uint32_t lm = (lap_s / 60UL) % 100UL;
    uint32_t ls = lap_s % 60UL;
    uint32_t lt = s_state.chrono_lap_tenths % 10UL;

    if (s_state.split_6h) {
        if (s_state.sub_view == 1U) {
            OLED_DrawStr(4, 13, "TIMER", 1, 1);
            (void)snprintf(buf, sizeof(buf), "%02u:%02u", s_state.timer_rem_sec / 60U, s_state.timer_rem_sec % 60U);
            OLED_DrawStr(4, 25, buf, 1, 1);
            OLED_DrawStr(10, 37, s_state.timer_running ? "RUN" : "STOP", 1, 1);
        } else {
            OLED_DrawStr(4, 13, "CHRON", 1, 1);
            (void)snprintf(buf, sizeof(buf), "%02lu:%02lu", (unsigned long)cm, (unsigned long)cs);
            OLED_DrawStr(4, 25, buf, 1, 1);
            (void)snprintf(buf, sizeof(buf), ".%lu%s", (unsigned long)ct, s_state.chrono_running ? "GO" : "--");
            OLED_DrawStr(10, 37, buf, 1, 1);
        }

        OLED_DrawStr(52, 13, "LAP", 1, 1);
        (void)snprintf(buf, sizeof(buf), "%02lu:%02lu", (unsigned long)lm, (unsigned long)ls);
        OLED_DrawStr(52, 25, buf, 1, 1);
        (void)snprintf(buf, sizeof(buf), "P%uM", s_state.timer_preset_sec / 60U);
        OLED_DrawStr(52, 37, buf, 1, 1);
        return;
    }

    if (s_state.sub_view == 0U) {
        OLED_DrawBitmap(4, 12, k_icon_chrono_10x10, 10, 10, 1);
        OLED_DrawStr(58, 12, s_state.chrono_running ? "RUN" : "STOP", 1, 1);
        (void)snprintf(buf, sizeof(buf), "%02lu:%02lu", (unsigned long)cm, (unsigned long)cs);
        OLED_DrawStr(4, 22, buf, 2, 1);
        (void)snprintf(buf, sizeof(buf), ".%lu", (unsigned long)ct);
        OLED_DrawStr(64, 28, buf, 1, 1);

        OLED_DrawStr(11, 37, s_state.chrono_running ? "4HL: LAP" :
                             (s_state.chrono_tenths > 0UL) ? "4HL: RESET" : "4HL: START", 1, 1);
    } else if (s_state.sub_view == 1U) {
        (void)snprintf(buf, sizeof(buf), "TIMER %uM", s_state.timer_preset_sec / 60U);
        OLED_DrawStr(4, 12, buf, 1, 1);
        OLED_DrawStr(64, 12, s_state.timer_running ? "ON" : "OFF", 1, 1);
        (void)snprintf(buf, sizeof(buf), "%02u:%02u", s_state.timer_rem_sec / 60U, s_state.timer_rem_sec % 60U);
        OLED_DrawStr(14, 21, buf, 2, 1);

        OLED_DrawRoundRect(10, 37, 68, 5, 2, 1);
        for (int16_t gx = 12; gx < 76; gx += 2) {
            OLED_DrawPixel(gx, 39, 1);
        }
        if (s_state.timer_preset_sec > 0U) {
            uint32_t w = ((uint32_t)s_state.timer_rem_sec * 66UL) / (uint32_t)s_state.timer_preset_sec;
            if (w > 0UL) OLED_FillRoundRect(11, 38, (int16_t)w, 3, 1, 1);
        }
    } else {
        OLED_DrawStr(4, 13, "DERNIER LAP:", 1, 1);
        (void)snprintf(buf, sizeof(buf), "%02lu:%02lu.%lu", (unsigned long)lm, (unsigned long)ls, (unsigned long)lt);
        OLED_DrawStr(4, 22, buf, 1, 1);
        (void)snprintf(buf, sizeof(buf), "PRESET:%u MIN", s_state.timer_preset_sec / 60U);
        OLED_DrawStr(6, 30, buf, 1, 1);
        OLED_DrawStr(11, 37, "4HL:5/10/25", 1, 1);
    }
}

/* ============================================================================
 * SCREEN 7: NOTIFICATIONS ANCS, GPS TURN-BY-TURN & METEO/AGENDA
 * ==========================================================================*/
static void Render_ScreenNotif(void)
{
    char buf[24];
    SmileWatch_DrawHeader("NOTIFS", s_state.sub_view);
    uint8_t ndir = (s_state.nav_dir < 5U) ? s_state.nav_dir : 0U;

    if (s_state.split_6h) {
        if (s_state.sub_view == 0U) {
            OLED_DrawStr(4, 13, (s_state.notif_unread > 0U) ? "MSG!" : "0MSG", 1, 1);
            char st[6];
            strncpy(st, s_state.notif_title, 5U); st[5] = '\0';
            OLED_DrawStr(4, 25, st, 1, 1);
            char sb[5];
            strncpy(sb, s_state.notif_body, 4U); sb[4] = '\0';
            OLED_DrawStr(10, 37, sb, 1, 1);

            OLED_DrawStr(52, 13, (s_state.notif_unread > 0U) ? "NONLU" : "LU OK", 1, 1);
            OLED_DrawStr(52, 25, s_state.calendar_time, 1, 1);
            (void)snprintf(buf, sizeof(buf), "%dC", (int)s_state.weather_temp_c);
            OLED_DrawStr(52, 37, buf, 1, 1);
        } else if (s_state.sub_view == 1U) {
            OLED_DrawBitmap(8, 12, k_nav_arrows_16x16[ndir], 16, 16, 1);
            OLED_DrawStr(4, 29, s_state.nav_eta, 1, 1);
            if (s_state.nav_dist_m >= 10000U) {
                (void)snprintf(buf, sizeof(buf), "%uK", s_state.nav_dist_m / 1000U);
            } else if (s_state.nav_dist_m >= 1000U) {
                (void)snprintf(buf, sizeof(buf), "%u.%uK",
                               s_state.nav_dist_m / 1000U, (s_state.nav_dist_m % 1000U) / 100U);
            } else {
                (void)snprintf(buf, sizeof(buf), "%uM", s_state.nav_dist_m);
            }
            OLED_DrawStr(10, 37, buf, 1, 1);

            OLED_DrawStr(52, 13, (ndir == 1U) ? "DROIT" :
                                 (ndir == 2U) ? "GAUCH" :
                                 (ndir == 3U) ? "RONDP" :
                                 (ndir == 4U) ? "ARRIV" : "TOUTD", 1, 1);
            char ns[6];
            strncpy(ns, s_state.nav_street, 5U); ns[5] = '\0';
            OLED_DrawStr(52, 25, ns, 1, 1);
            OLED_DrawStr(52, 37, "GPS", 1, 1);
        } else {
            OLED_DrawStr(4, 13, "METEO", 1, 1);
            (void)snprintf(buf, sizeof(buf), "%dC", (int)s_state.weather_temp_c);
            if (strlen(buf) > 3U) {
                OLED_DrawStr(4, 25, buf, 1, 1);
            } else {
                OLED_DrawStr(3, 21, buf, 2, 1);
            }
            if (s_state.weather_rain_pct >= 100U) {
                (void)snprintf(buf, sizeof(buf), "P100");
            } else {
                (void)snprintf(buf, sizeof(buf), "P%u%%", s_state.weather_rain_pct);
            }
            OLED_DrawStr(10, 37, buf, 1, 1);

            OLED_DrawStr(52, 13, "AGEND", 1, 1);
            OLED_DrawStr(52, 25, s_state.calendar_time, 1, 1);
            char sc[5];
            strncpy(sc, s_state.calendar_title, 4U); sc[4] = '\0';
            OLED_DrawStr(52, 37, sc, 1, 1);
        }
        return;
    }

    if (s_state.sub_view == 0U) {
        if (s_state.notif_unread == 0U) {
            OLED_DrawStr(17, 15, "AUCUN", 2, 1);
            OLED_DrawStr(11, 37, "0 MSG NONLU", 1, 1);
        } else {
            char nt[12];
            OLED_DrawBitmap(4, 13, k_garmin_bubble_11x9, 11, 9, 1);
            strncpy(nt, s_state.notif_title, 11U); nt[11] = '\0';
            OLED_DrawStr(17, 14, nt, 1, 1);
            OLED_DrawLine(4, 23, 83, 23, 1);

            char l1[14];
            char l2[12];
            strncpy(l1, s_state.notif_body, 13U); l1[13] = '\0';
            OLED_DrawStr(5, 26, l1, 1, 1);
            if (strlen(s_state.notif_body) > 13U) {
                strncpy(l2, &s_state.notif_body[13], 11U); l2[11] = '\0';
                OLED_DrawStr(11, 37, l2, 1, 1);
            }
        }
    } else if (s_state.sub_view == 1U) {
        OLED_DrawBitmap(4, 12, k_nav_arrows_16x16[ndir], 16, 16, 1);
        if (ndir == 4U) {
            (void)snprintf(buf, sizeof(buf), "ARR");
        } else if (s_state.nav_dist_m >= 10000U) {
            (void)snprintf(buf, sizeof(buf), "%uK", s_state.nav_dist_m / 1000U);
        } else if (s_state.nav_dist_m >= 1000U) {
            (void)snprintf(buf, sizeof(buf), "%u.%uK",
                           s_state.nav_dist_m / 1000U, (s_state.nav_dist_m % 1000U) / 100U);
        } else {
            (void)snprintf(buf, sizeof(buf), "%uM", s_state.nav_dist_m);
        }
        OLED_DrawStr(23, 12, buf, 2, 1);

        OLED_DrawRoundRect(23, 29, 60, 5, 2, 1);
        for (int16_t gx = 25; gx < 81; gx += 2) {
            OLED_DrawPixel(gx, 31, 1);
        }
        uint32_t prog = (s_state.nav_dist_m >= 300U) ? 0UL :
                        (((300UL - (uint32_t)s_state.nav_dist_m) * 58UL) / 300UL);
        if (prog > 0UL) {
            OLED_FillRoundRect(24, 30, (int16_t)prog, 3, 1, 1);
        }

        char st11[12];
        strncpy(st11, s_state.nav_street, 11U); st11[11] = '\0';
        OLED_DrawStr(11, 37, st11, 1, 1);
    } else {
        OLED_DrawBitmap(4, 12, k_icon_weather_12x10, 12, 10, 1);
        (void)snprintf(buf, sizeof(buf), "%dC %s", (int)s_state.weather_temp_c, s_state.weather_desc);
        char w11[12];
        strncpy(w11, buf, 11U); w11[11] = '\0';
        OLED_DrawStr(18, 13, w11, 1, 1);
        OLED_DrawLine(2, 24, 85, 24, 1);

        (void)snprintf(buf, sizeof(buf), "RDV %s", s_state.calendar_time);
        OLED_DrawStr(5, 28, buf, 1, 1);
        char c11[12];
        strncpy(c11, s_state.calendar_title, 11U); c11[11] = '\0';
        OLED_DrawStr(11, 37, c11, 1, 1);
    }
}

/* ============================================================================
 * SCREEN 8: SMART BLE HID, ALARME HAPTIQUE & DIAGNOSTIC SYSTEME
 * ==========================================================================*/
static void Render_ScreenSmart(void)
{
    char buf[24];
    SmileWatch_DrawHeader("SMART", s_state.sub_view);

    if (s_state.split_6h) {
        if (s_state.sub_view == 0U) {
            OLED_DrawStr(4, 13, "HID", 1, 1);
            OLED_DrawStr(4, 25, (s_state.hid_mode == 0U) ?
                                (s_state.media_playing ? "PLAY" : "PAUSE") : "PHOTO", 1, 1);
            (void)snprintf(buf, sizeof(buf), "P:%u", s_state.hid_shots);
            OLED_DrawStr(10, 37, (s_state.hid_mode == 0U) ? "SON" : buf, 1, 1);
        } else if (s_state.sub_view == 1U) {
            OLED_DrawStr(4, 13, "ALARM", 1, 1);
            (void)snprintf(buf, sizeof(buf), "%02u:%02u", s_state.alarm_hour, s_state.alarm_min);
            OLED_DrawStr(4, 25, buf, 1, 1);
            OLED_DrawStr(10, 37, s_state.alarm_enabled ? "ON" : "OFF", 1, 1);
        } else {
            if (s_state.ota_active) {
                OLED_DrawStr(4, 13, "FOTA", 1, 1);
                (void)snprintf(buf, sizeof(buf), "%u%%", s_state.ota_progress_pct);
                OLED_DrawStr(4, 25, buf, 1, 1);
                OLED_DrawStr(10, 37, "BLE", 1, 1);
            } else {
                OLED_DrawStr(4, 13, "SYS", 1, 1);
                (void)snprintf(buf, sizeof(buf), "%u.%02uV", s_state.battery_mv / 1000U, (s_state.battery_mv % 1000U) / 10U);
                OLED_DrawStr(4, 25, buf, 1, 1);
                if (s_state.ambient_lux >= 1000U) {
                    (void)snprintf(buf, sizeof(buf), "%uK", s_state.ambient_lux / 1000U);
                } else {
                    (void)snprintf(buf, sizeof(buf), "%uL", s_state.ambient_lux);
                }
                OLED_DrawStr(10, 37, buf, 1, 1);
            }
        }

        OLED_DrawStr(52, 13, s_state.ble_connected ? "BLEOK" : "NOBLE", 1, 1);
        (void)snprintf(buf, sizeof(buf), "%u%%", s_state.battery_pct);
        OLED_DrawStr(52, 25, buf, 1, 1);
        OLED_DrawStr(52, 37, "[4H]", 1, 1);
        return;
    }

    if (s_state.sub_view == 0U) {
        if (s_state.hid_mode == 0U) {
            char mt[14];
            OLED_DrawBitmap(4, 12, k_icon_music_10x10, 10, 10, 1);
            OLED_DrawStr(15, 13, "BLE HID", 1, 1);
            OLED_DrawStr(60, 13, "MUSQ", 1, 1);
            strncpy(mt, s_state.media_track, 13U); mt[13] = '\0';
            OLED_DrawStr(4, 24, mt, 1, 1);
            OLED_DrawStr(11, 37, s_state.media_playing ? "> PLAY" : "|| PAUS", 1, 1);
            if (s_state.media_playing) {
                for (int16_t b = 0; b < 4; b++) {
                    int16_t eh = (int16_t)(2 + ((s_state.ppg_phase + (uint8_t)(b * 3)) % 6U));
                    OLED_FillRect((int16_t)(58 + b * 4), (int16_t)(42 - eh), 3, eh, 1);
                }
            }
        } else {
            OLED_DrawBitmap(4, 12, k_icon_cam_12x9, 12, 9, 1);
            OLED_DrawStr(17, 13, "BLE HID", 1, 1);
            OLED_DrawStr(64, 13, "CAM", 1, 1);
            OLED_DrawStr(4, 24, "DECLENCHEUR", 1, 1);
            (void)snprintf(buf, sizeof(buf), "PHOTOS: %u", s_state.hid_shots);
            OLED_DrawStr(11, 37, buf, 1, 1);
        }
    } else if (s_state.sub_view == 1U) {
        OLED_DrawBitmap(4, 12, k_icon_alarm_10x10, 10, 10, 1);
        OLED_DrawStr(16, 13, "ALARME", 1, 1);
        OLED_DrawStr(55, 13, "DRV26", 1, 1);
        (void)snprintf(buf, sizeof(buf), "%02u:%02u", s_state.alarm_hour, s_state.alarm_min);
        OLED_DrawStr(3, 22, buf, 2, 1);
        OLED_DrawStr(63, 26, (!s_state.alarm_enabled) ? "OFF" :
                                 (s_state.smart_wake_enabled ? "DOUX" : "STD"), 1, 1);
        OLED_DrawStr(11, 37, s_state.smart_wake_triggered ? "REVEIL DOUX" :
                             (!s_state.alarm_enabled ? "4HL:ACTIVER" :
                             (s_state.smart_wake_enabled ? "DOUX -20MIN" : "4HL:  DOUX")), 1, 1);
    } else {
        if (s_state.ota_active) {
            (void)snprintf(buf, sizeof(buf), "FOTA BLE V%u.%u", s_state.fw_major, (unsigned int)(s_state.fw_minor + 1U));
            OLED_DrawStr(5, 13, buf, 1, 1);
            OLED_DrawRoundRect(4, 23, 80, 5, 2, 1);
            for (int16_t gx = 6; gx < 82; gx += 2) {
                OLED_DrawPixel(gx, 25, 1);
            }
            if (s_state.ota_progress_pct > 0U) {
                OLED_FillRoundRect(5, 24, (int16_t)((s_state.ota_progress_pct * 78U) / 100U), 3, 1, 1);
            }
            (void)snprintf(buf, sizeof(buf), "DL:%u%% CRC", s_state.ota_progress_pct);
            OLED_DrawStr(11, 37, buf, 1, 1);
        } else {
            (void)snprintf(buf, sizeof(buf), "FW%u.%u B:%u%%", s_state.fw_major, s_state.fw_minor, s_state.battery_pct);
            OLED_DrawStr(4, 12, buf, 1, 1);
            (void)snprintf(buf, sizeof(buf), "FL:%u%% RAM:%u%%", s_state.flash_used_pct, s_state.ram_used_pct);
            OLED_DrawStr(4, 21, buf, 1, 1);
            (void)snprintf(buf, sizeof(buf), "MLC%u/8 OTA:OK", s_state.mlc_trees_used);
            OLED_DrawStr(4, 29, buf, 1, 1);
            OLED_DrawStr(11, 37, "CAL:2+4 +1M", 1, 1);
        }
    }
}

/* ============================================================================
 * SCREEN 9: PARAMETRES (NAV/SOS/REVEIL DOUX), HISTORIQUE & STATUT I2C/FLASH
 * ==========================================================================*/
static void Render_ScreenConfig(void)
{
    char buf[24];
    SmileWatch_DrawHeader("CONFIG", s_state.sub_view);

    if (s_state.split_6h) {
        if (s_state.sub_view == 0U) {
            OLED_DrawStr(4, 13, "PARAM", 1, 1);
            (void)snprintf(buf, sizeof(buf), "NV:%s", s_state.wrist_nav_enabled ? "ON" : "OF");
            OLED_DrawStr(4, 25, buf, 1, 1);
            (void)snprintf(buf, sizeof(buf), "SO:%s", s_state.fall_sos_enabled ? "1" : "0");
            OLED_DrawStr(10, 37, buf, 1, 1);

            (void)snprintf(buf, sizeof(buf), "AG:%s", s_state.pill_hand_pause_enabled ? "ON" : "OF");
            OLED_DrawStr(52, 13, buf, 1, 1);
            OLED_DrawStr(52, 25, s_state.smart_wake_enabled ? "DOUX" : "FIXE", 1, 1);
            OLED_DrawStr(52, 37, "[4H]", 1, 1);
        } else if (s_state.sub_view == 1U) {
            OLED_DrawStr(4, 13, "HISTO", 1, 1);
            if (s_state.history_count > 0U) {
                uint8_t hi = (uint8_t)(s_state.history_view_idx % s_state.history_count);
                uint8_t sp = (uint8_t)(s_state.workout_history[hi].sport_type % 19U);
                OLED_DrawStr(4, 25, k_sport_short[sp], 1, 1);
                (void)snprintf(buf, sizeof(buf), "#%u/%u", (unsigned int)(hi + 1U), (unsigned int)s_state.history_count);
                OLED_DrawStr(10, 37, buf, 1, 1);

                (void)snprintf(buf, sizeof(buf), "%uM", (unsigned int)(s_state.workout_history[hi].duration_sec / 60U));
                OLED_DrawStr(52, 13, buf, 1, 1);
                (void)snprintf(buf, sizeof(buf), "%uB", (unsigned int)s_state.workout_history[hi].avg_bpm);
                OLED_DrawStr(52, 25, buf, 1, 1);
                (void)snprintf(buf, sizeof(buf), "R%uH", (unsigned int)s_state.workout_history[hi].recovery_h);
                OLED_DrawStr(52, 37, buf, 1, 1);
            } else {
                OLED_DrawStr(4, 25, "VIDE", 1, 1);
                OLED_DrawStr(52, 25, "0 SEA", 1, 1);
            }
        } else {
            OLED_DrawStr(4, 13, "I2C1", 1, 1);
            (void)snprintf(buf, sizeof(buf), "IM:%s", s_state.hw_imu_ok ? "OK" : "--");
            OLED_DrawStr(4, 25, buf, 1, 1);
            (void)snprintf(buf, sizeof(buf), "FL:%s", s_state.hw_flash_ok ? "1" : "0");
            OLED_DrawStr(10, 37, buf, 1, 1);

            (void)snprintf(buf, sizeof(buf), "HR:%s", s_state.hw_hr_ok ? "OK" : "--");
            OLED_DrawStr(52, 13, buf, 1, 1);
            (void)snprintf(buf, sizeof(buf), "TP:%s", s_state.hw_temp_ok ? "OK" : "--");
            OLED_DrawStr(52, 25, buf, 1, 1);
            OLED_DrawStr(52, 37, "SAVE", 1, 1);
        }
        return;
    }

    if (s_state.sub_view == 0U) {
        (void)snprintf(buf, sizeof(buf), "NV:%s SO:%s",
                       s_state.wrist_nav_enabled ? "ON" : "OF",
                       s_state.fall_sos_enabled ? "ON" : "OF");
        OLED_DrawStr(4, 12, buf, 1, 1);
        (void)snprintf(buf, sizeof(buf), "DX:%s NU:%s",
                       s_state.smart_wake_enabled ? "ON" : "OF",
                       s_state.night_mode_enabled ? "ON" : "OF");
        OLED_DrawStr(4, 21, buf, 1, 1);
        (void)snprintf(buf, sizeof(buf), "AG:%s SD:%s",
                       s_state.pill_hand_pause_enabled ? "ON" : "OF",
                       s_state.sedentary_alert_enabled ? "ON" : "OF");
        OLED_DrawStr(4, 29, buf, 1, 1);
        OLED_DrawStr(11, 37, "4HL:NUIT ON", 1, 1);
    } else if (s_state.sub_view == 1U) {
        if (s_state.history_count == 0U) {
            OLED_DrawStr(14, 18, "AUCUNE", 2, 1);
            OLED_DrawStr(11, 37, "SEANCE ENR.", 1, 1);
        } else {
            uint8_t hi = (uint8_t)(s_state.history_view_idx % s_state.history_count);
            uint8_t sp = (uint8_t)(s_state.workout_history[hi].sport_type % 19U);
            (void)snprintf(buf, sizeof(buf), "#%u/%u %s", (unsigned int)(hi + 1U), (unsigned int)s_state.history_count, k_sport_names[sp]);
            OLED_DrawStr(4, 12, buf, 1, 1);
            (void)snprintf(buf, sizeof(buf), "%uMIN %uBPM",
                           (unsigned int)(s_state.workout_history[hi].duration_sec / 60U),
                           (unsigned int)s_state.workout_history[hi].avg_bpm);
            OLED_DrawStr(4, 21, buf, 1, 1);
            if (s_state.workout_history[hi].kcal >= 1000U) {
                (void)snprintf(buf, sizeof(buf), "%uK REC:%uH",
                               (unsigned int)(s_state.workout_history[hi].kcal / 1000U),
                               (unsigned int)s_state.workout_history[hi].recovery_h);
            } else {
                (void)snprintf(buf, sizeof(buf), "%uKC REC:%uH",
                               (unsigned int)s_state.workout_history[hi].kcal,
                               (unsigned int)s_state.workout_history[hi].recovery_h);
            }
            OLED_DrawStr(4, 29, buf, 1, 1);
            OLED_DrawStr(11, 37, "4HL:SUIVANT", 1, 1);
        }
    } else {
        (void)snprintf(buf, sizeof(buf), "IMU:%s HR:%s", s_state.hw_imu_ok ? "OK" : "--", s_state.hw_hr_ok ? "OK" : "--");
        OLED_DrawStr(4, 12, buf, 1, 1);
        (void)snprintf(buf, sizeof(buf), "TMP:%s AL:%s", s_state.hw_temp_ok ? "OK" : "--", s_state.hw_als_ok ? "OK" : "--");
        OLED_DrawStr(4, 21, buf, 1, 1);
        (void)snprintf(buf, sizeof(buf), "DRV:%s FL:%s", s_state.hw_drv_ok ? "OK" : "--", s_state.hw_flash_ok ? "OK" : "--");
        OLED_DrawStr(4, 29, buf, 1, 1);
        OLED_DrawStr(11, 37, "4HL:SAUVER", 1, 1);
    }
}

static void Render_SosOverlay(void)
{
    char buf[24];
    if (s_state.sos_sent) {
        if ((s_state.seconds & 1U) == 0U) {
            OLED_FillRect(8, 2, 72, 10, 1);
            OLED_DrawStr(17, 4, "! SOS !", 1, 0);
        } else {
            OLED_DrawStr(17, 4, "! SOS !", 1, 1);
        }
        OLED_DrawLine(8, 12, 79, 12, 1);
        (void)snprintf(buf, sizeof(buf), "ICE:%s", s_state.ice_name);
        buf[13] = '\0';
        OLED_DrawStr(4, 15, buf, 1, 1);
        (void)snprintf(buf, sizeof(buf), "%s", s_state.ice_phone);
        buf[13] = '\0';
        OLED_DrawStr(4, 25, buf, 1, 1);
        OLED_DrawStr(11, 37, "ENVOYE! 4H", 1, 1);
    } else {
        OLED_DrawStr(17, 4, "! CHUTE !", 1, 1);
        OLED_DrawLine(8, 12, 79, 12, 1);
        (void)snprintf(buf, sizeof(buf), "%02uS", s_state.sos_countdown_sec);
        OLED_DrawStr(26, 17, buf, 2, 1);
        OLED_DrawStr(11, 37, "4H: ANNULER", 1, 1);
    }
}

void SmileWatch_RenderCurrentScreen(void)
{
    OLED_Clear();

    if (!s_state.display_awake) {
        OLED_Flush(s_hi2c);
        return;
    }

    if (s_state.sos_active) {
        Render_SosOverlay();
        OLED_Flush(s_hi2c);
        return;
    }

    switch (s_state.current_screen) {
        case SW_SCREEN_HOME:     Render_ScreenHome();     break;
        case SW_SCREEN_HEART:    Render_ScreenHeart();    break;
        case SW_SCREEN_HEALTH:   Render_ScreenHealth();   break;
        case SW_SCREEN_ACTIVITY: Render_ScreenActivity(); break;
        case SW_SCREEN_SPORT:    Render_ScreenSport();    break;
        case SW_SCREEN_CHRONO:   Render_ScreenChrono();   break;
        case SW_SCREEN_NOTIF:    Render_ScreenNotif();    break;
        case SW_SCREEN_SMART:    Render_ScreenSmart();    break;
        case SW_SCREEN_CONFIG:   Render_ScreenConfig();   break;
        default:                 Render_ScreenHome();     break;
    }

    OLED_Flush(s_hi2c);
    s_state.oled_lit_pixels = OLED_CountLitPixels();
}

void SmileWatch_Init(I2C_HandleTypeDef *hi2c)
{
    s_hi2c = hi2c;
    memset(&s_state, 0, sizeof(s_state));

    s_state.hours            = 10U;
    s_state.minutes          = 10U;
    s_state.seconds          = 30U;
    s_state.day              = 18U;
    s_state.month            = 11U;
    strncpy(s_state.weekday_str, "MER", sizeof(s_state.weekday_str) - 1U);

    s_state.battery_pct      = 84U;
    s_state.battery_mv       = 4020U;
    s_state.qi_charging      = false;
    s_state.ble_connected    = true;
    s_state.survival_mode    = false;
    s_state.auto_brightness_enabled = true;
    s_state.oled_contrast    = 185U;
    s_state.look_to_wake_enabled = true;
    s_state.stop2_active     = false;
    s_state.coach_enabled    = true;
    s_state.target_hr_zone   = 3U;
    s_state.current_hr_zone  = 1U;
    s_state.coach_status     = 0U;
    s_state.coach_rec_code   = 1U;
    s_state.training_load_acute   = 42U;
    s_state.training_load_chronic = 40U;
    s_state.pre_illness_alert     = false;
    s_state.wake_summary_active   = false;
    s_state.wake_summary_shown_today = false;
    s_state.last_sleep_min   = 46U;

    s_state.heart_bpm        = 72U;
    s_state.spo2_pct         = 98U;
    s_state.hr_measuring     = true;
    s_state.body_battery     = 78U;
    s_state.stress_score     = 24U;
    s_state.hr_min           = 48U;
    s_state.hr_max           = 126U;
    s_state.temp_deci_c      = 366;
    s_state.temp_base_deci_c = 365;
    s_state.steps_count      = 6420UL;
    s_state.steps_goal       = 10000UL;
    s_state.ambient_lux      = 250U;

    s_state.sleep_min        = 462U;
    s_state.sleep_score      = 86U;
    s_state.sleep_deep_pct   = 28U;
    s_state.sleep_rem_pct    = 22U;

    s_state.sport_type          = 0U;
    s_state.sport_metric        = 142U;
    s_state.mlc_confidence_pct  = 78U;
    s_state.mlc_thresh_mg       = 1320U;
    s_state.stride_cm           = 74U;
    s_state.recovery_hours      = 14U;
    s_state.vo2_max             = 51U;
    s_state.sport_sessions_done = 12U;
    s_state.sport_active        = false;
    s_state.sport_sec           = 754U;
    s_state.gym_sets            = 3U;
    s_state.gym_reps            = 12U;
    s_state.gym_rest_sec        = 45U;
    s_state.swim_laps           = 16U;
    s_state.swim_stroke         = 0U;
    s_state.run_pace_sec        = 315U;

    s_state.chrono_running     = false;
    s_state.chrono_tenths      = 0UL;
    s_state.chrono_lap_tenths  = 428UL;
    s_state.timer_running      = false;
    s_state.timer_rem_sec      = 300U;
    s_state.timer_preset_sec   = 300U;

    s_state.notif_unread     = 1U;
    s_state.notif_index      = 0U;
    strncpy(s_state.notif_title, "SMS - ALEXIS", sizeof(s_state.notif_title) - 1U);
    strncpy(s_state.notif_body,  "RDV FABLAB MARDI 13H30 OK", sizeof(s_state.notif_body) - 1U);
    s_state.nav_active       = false;
    s_state.nav_dir          = 1U;
    s_state.nav_dist_m       = 120U;
    strncpy(s_state.nav_street, "RUE PONTOISE", sizeof(s_state.nav_street) - 1U);
    strncpy(s_state.nav_eta, "13:28", sizeof(s_state.nav_eta) - 1U);
    s_state.weather_temp_c   = 19;
    s_state.weather_rain_pct = 15U;
    strncpy(s_state.weather_desc, "ECLAIRCIES", sizeof(s_state.weather_desc) - 1U);
    strncpy(s_state.calendar_time, "13:30", sizeof(s_state.calendar_time) - 1U);
    strncpy(s_state.calendar_title, "FABLAB B204", sizeof(s_state.calendar_title) - 1U);

    s_state.media_playing    = false;
    s_state.hid_mode         = 0U;
    strncpy(s_state.media_track, "DAFT PUNK-VER", sizeof(s_state.media_track) - 1U);
    s_state.hid_shots        = 0U;
    s_state.alarm_enabled    = true;
    s_state.alarm_hour       = 7U;
    s_state.alarm_min        = 30U;
    s_state.ota_active       = false;
    s_state.ota_progress_pct = 0U;
    s_state.fw_major         = 2U;
    s_state.fw_minor         = 1U;
    s_state.flash_used_pct   = 6U;
    s_state.ram_used_pct     = 2U;
    s_state.mlc_trees_used   = 4U;

    s_state.current_screen   = SW_SCREEN_HOME;
    s_state.sub_view         = 0U;
    s_state.split_6h_mode    = 0U;
    s_state.split_6h_force   = false;
    SmileWatch_UpdateSplit6h();
    s_state.display_awake    = true;

    s_state.wrist_nav_enabled       = true;
    s_state.fall_sos_enabled        = true;
    s_state.smart_wake_enabled      = true;
    s_state.pill_hand_pause_enabled = true;
    s_state.pill_hand_paused        = false;
    s_state.smart_wake_triggered    = false;
    s_state.sos_active           = false;
    s_state.sos_countdown_sec    = 0U;
    s_state.sos_sent             = false;

    /* Default seed workout history (overridden if Flash Page 127 has saved records) */
    s_state.workout_history[0].sport_type   = 0U;
    s_state.workout_history[0].duration_sec = 1080U;
    s_state.workout_history[0].avg_bpm      = 132U;
    s_state.workout_history[0].kcal         = 145U;
    s_state.workout_history[0].recovery_h   = 14U;

    s_state.workout_history[1].sport_type   = 1U;
    s_state.workout_history[1].duration_sec = 1920U;
    s_state.workout_history[1].avg_bpm      = 158U;
    s_state.workout_history[1].kcal         = 380U;
    s_state.workout_history[1].recovery_h   = 24U;

    s_state.workout_history[2].sport_type   = 2U;
    s_state.workout_history[2].duration_sec = 1500U;
    s_state.workout_history[2].avg_bpm      = 144U;
    s_state.workout_history[2].kcal         = 290U;
    s_state.workout_history[2].recovery_h   = 18U;

    s_state.history_count    = 3U;
    s_state.history_head     = 3U;
    s_state.history_view_idx = 2U;

    s_state.hrv_rmssd_ms        = 54U;
    s_state.hrv_base_ms         = 52U;
    s_state.rhr_base_bpm        = 56U;
    s_state.sleep_stage         = 0U;
    s_state.readiness_state     = 0U;
    s_state.baseline_nights     = 5U;
    s_state.steps_at_last_calib = 0UL;
    s_state.calib_error_pct     = 0;
    s_state.last_ble_gps_dist_m = 0U;
    s_state.imu_fp_rejects      = 0U;
    s_state.power_profile       = 2U;
    s_state.auto_sport_enabled  = true;
    s_state.detected_activity   = 0U;
    s_state.track_calib_active  = false;
    s_state.track_calib_dist_m  = 400U;
    s_state.track_calib_start_steps = 0UL;
    strncpy(s_state.ice_name,  "MAMAN", sizeof(s_state.ice_name) - 1U);
    strncpy(s_state.ice_phone, "0612345678", sizeof(s_state.ice_phone) - 1U);

    s_state.night_mode_enabled      = false;
    s_state.sedentary_alert_enabled = true;
    s_state.sedentary_min           = 0U;
    s_state.sedentary_alert_active  = false;
    s_state.tri_active              = false;
    s_state.tri_stage               = 0U;
    s_state.tri_total_sec           = 0U;
    s_state.tabata_work             = true;
    s_state.tabata_round            = 1U;
    s_state.tabata_sec_left         = 20U;
    s_state.breath_phase_sec        = 0U;
    s_state.hrr_active              = false;
    s_state.hrr_sec_left            = 0U;
    s_state.hrr_start_bpm           = 0U;
    s_state.hrr_drop_bpm            = 0U;

    HAL_GPIO_WritePin(TCA_EN_GPIO_Port, TCA_EN_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(HR_5V_EN_GPIO_Port, HR_5V_EN_Pin, GPIO_PIN_SET);

    SW_SensorPresence_t pres;
    Sensors_Init(s_hi2c, &pres);
    s_state.hw_imu_ok  = pres.imu_ok;
    s_state.hw_hr_ok   = pres.hr_ok;
    s_state.hw_temp_ok = pres.temp_ok;
    s_state.hw_als_ok  = pres.als_ok;
    s_state.hw_drv_ok  = pres.drv_ok;

    Miyota_Init(&s_miyota);
    if (FlashStorage_Load(&s_state, &s_miyota)) {
        s_state.hw_flash_ok = true;
        SmileWatch_UpdateSplit6h();
        SmileWatch_SyncMiyotaWithPillPause();
    } else {
        SmileWatch_SyncMiyotaWithPillPause();
        s_miyota.analog_pos_sec = s_miyota.target_pos_sec;
        s_state.hw_flash_ok = FlashStorage_Save(&s_state, &s_miyota);
    }
    Sensors_SetImuThreshold(s_hi2c, s_state.mlc_thresh_mg);

    OLED_Init(s_hi2c);
    SmileWatch_UpdatePhysioEngine(true, 0UL);
    SmileWatch_UpdatePowerEstimate(0UL);
    SmileWatch_RenderCurrentScreen();
    SmileWatch_UpdatePowerEstimate(0UL);
}

void SmileWatch_SaveWorkoutSession(void)
{
    uint8_t idx = (uint8_t)(s_state.history_head % SW_HISTORY_MAX);
    uint16_t dur = (s_state.sport_sec > 0U) ? s_state.sport_sec : 60U;
    uint16_t est_kcal = (uint16_t)((((uint32_t)dur * (uint32_t)((s_state.heart_bpm > 70U) ? s_state.heart_bpm : 110U)) / 900UL));
    if (est_kcal < 10U) est_kcal = 10U;

    s_state.workout_history[idx].sport_type   = (uint8_t)(s_state.sport_type % SW_SPORT_COUNT);
    s_state.workout_history[idx].duration_sec = dur;
    s_state.workout_history[idx].avg_bpm      = (s_state.heart_bpm > 45U) ? s_state.heart_bpm : 120U;
    s_state.workout_history[idx].kcal         = est_kcal;
    s_state.workout_history[idx].recovery_h   = s_state.recovery_hours;

    s_state.history_head = (uint8_t)((idx + 1U) % SW_HISTORY_MAX);
    if (s_state.history_count < SW_HISTORY_MAX) {
        s_state.history_count++;
    }
    s_state.history_view_idx = idx;
    s_state.sport_sessions_done++;
    s_state.hrr_active    = true;
    s_state.hrr_sec_left  = 60U;
    s_state.hrr_start_bpm = s_state.heart_bpm;
    /* Training load analysis & next-session optimization */
    uint16_t sess_trimp = (uint16_t)(((uint32_t)dur / 60UL) * (uint32_t)((s_state.current_hr_zone > 0U) ? s_state.current_hr_zone : 2U));
    s_state.training_load_acute   = (uint16_t)((s_state.training_load_acute * 6U + sess_trimp) / 7U);
    s_state.training_load_chronic = (uint16_t)((s_state.training_load_chronic * 27U + sess_trimp) / 28U);
    if (s_state.pre_illness_alert || (s_state.readiness_state == 2U)) {
        s_state.coach_rec_code = 4U; /* REPOS! */
        s_state.target_hr_zone = 1U;
    } else if (s_state.training_load_acute > (uint16_t)((s_state.training_load_chronic * 13U) / 10U)) {
        s_state.coach_rec_code = 0U; /* Z2 REC */
        s_state.target_hr_zone = 2U;
    } else if (s_state.readiness_state == 0U) {
        s_state.coach_rec_code = 2U; /* Z4 SEU */
        s_state.target_hr_zone = 4U;
    } else {
        s_state.coach_rec_code = 1U; /* Z3 END */
        s_state.target_hr_zone = 3U;
    }
    SmileWatch_UpdateAdaptiveSport();
    Sensors_SetImuThreshold(s_hi2c, s_state.mlc_thresh_mg);
    s_state.hw_flash_ok = FlashStorage_Save(&s_state, &s_miyota);
}

bool SmileWatch_NeedsFastTick(void)
{
    return s_state.display_awake || s_state.sport_active || s_state.hrr_active || s_state.sos_active;
}

void SmileWatch_ShowWakeSummary(uint32_t now_ms)
{
    int16_t dt = (int16_t)(s_state.temp_deci_c - s_state.temp_base_deci_c);
    bool hr_up = (s_state.heart_bpm >= (uint8_t)(s_state.rhr_base_bpm + 6U));
    bool hrv_dn = (s_state.hrv_rmssd_ms < (uint16_t)((s_state.hrv_base_ms * 80U) / 100U));
    s_state.pre_illness_alert = (dt >= 8) && hr_up && hrv_dn;
    if (s_state.pre_illness_alert || (s_state.readiness_state == 2U)) {
        s_state.coach_rec_code = 4U;
        s_state.target_hr_zone = 1U;
    } else if (s_state.training_load_acute > (uint16_t)((s_state.training_load_chronic * 13U) / 10U)) {
        s_state.coach_rec_code = 0U;
        s_state.target_hr_zone = 2U;
    } else if (s_state.readiness_state == 0U) {
        s_state.coach_rec_code = 2U;
        s_state.target_hr_zone = 4U;
    } else {
        s_state.coach_rec_code = 1U;
        s_state.target_hr_zone = 3U;
    }
    s_state.wake_summary_active = true;
    s_state.wake_summary_shown_today = true;
    s_state.current_screen = SW_SCREEN_HOME;
    s_state.sub_view = 0U;
    s_state.last_activity_ms = now_ms;
    if (!s_state.display_awake && !s_state.survival_mode) {
        s_state.display_awake = true;
        OLED_PowerOn();
        OLED_Init(s_hi2c);
        SmileWatch_SyncMiyotaWithPillPause();
    }
    SmileWatch_RenderCurrentScreen();
}

void SmileWatch_HandleLookToWake(bool looking_at_watch, uint32_t now_ms)
{
    if (!looking_at_watch || SmileWatch_IsNightQuiet() || s_state.survival_mode || s_state.sos_active) {
        return;
    }
    s_state.last_activity_ms = now_ms;
    if (!s_state.display_awake) {
        s_state.display_awake = true;
        OLED_PowerOn();
        OLED_Init(s_hi2c);
        SmileWatch_SyncMiyotaWithPillPause();
        SmileWatch_TriggerHaptic(now_ms, 15U);
        SmileWatch_RenderCurrentScreen();
    }
}

void SmileWatch_HandleWristFlick(uint32_t now_ms)
{
    if (!s_state.wrist_nav_enabled || SmileWatch_IsNightQuiet() || s_state.sos_active || s_state.survival_mode) return;
    if (!s_state.display_awake && s_state.look_to_wake_enabled) return;
    s_state.last_activity_ms = now_ms;
    if (!s_state.display_awake) {
        s_state.display_awake = true;
        SmileWatch_SyncMiyotaWithPillPause();
        OLED_PowerOn();
        OLED_Init(s_hi2c);
    } else if (s_state.current_screen == SW_SCREEN_SPORT && s_state.sub_view == 0U) {
        s_state.sport_type = (uint8_t)((s_state.sport_type + 1U) % SW_SPORT_COUNT);
    } else {
        s_state.sub_view = (uint8_t)((s_state.sub_view + 1U) % 3U);
    }
    SmileWatch_TriggerHaptic(now_ms, 18U);
    SmileWatch_RenderCurrentScreen();
}

void SmileWatch_TriggerFallSos(uint32_t now_ms)
{
    if (!s_state.fall_sos_enabled || s_state.survival_mode) return;
    s_state.last_activity_ms = now_ms;
    s_state.sos_active        = true;
    s_state.sos_countdown_sec = 10U;
    s_state.sos_sent          = false;
    if (!s_state.display_awake) {
        s_state.display_awake = true;
        SmileWatch_SyncMiyotaWithPillPause();
        OLED_PowerOn();
        OLED_Init(s_hi2c);
    }
    Sensors_PlayHaptic(s_hi2c, 14U);
    SmileWatch_TriggerHaptic(now_ms, 220U);
    SmileWatch_RenderCurrentScreen();
}

void SmileWatch_HandleButtonEvent(SW_ButtonEvent_t evt, uint32_t now_ms)
{
    if (evt == SW_BTN_EVT_NONE) return;

    s_state.last_activity_ms = now_ms;

    if (s_state.sos_active) {
        if ((evt == SW_BTN_EVT_4H_SHORT) || (evt == SW_BTN_EVT_4H_LONG)) {
            s_state.sos_active        = false;
            s_state.sos_sent          = false;
            s_state.sos_countdown_sec = 0U;
            SmileWatch_TriggerHaptic(now_ms, 50U);
            SmileWatch_RenderCurrentScreen();
            return;
        }
    }

    if (s_state.wake_summary_active && (s_state.current_screen == SW_SCREEN_HOME) &&
        ((evt == SW_BTN_EVT_4H_SHORT) || (evt == SW_BTN_EVT_4H_LONG))) {
        s_state.wake_summary_active = false;
        SmileWatch_TriggerHaptic(now_ms, 20U);
        SmileWatch_RenderCurrentScreen();
        return;
    }

    if (!s_state.display_awake && (!s_state.survival_mode || (evt == SW_BTN_EVT_2H_SHORT))) {
        s_state.display_awake = true;
        if (s_state.survival_mode) {
            s_state.current_screen = SW_SCREEN_HOME;
            s_state.sub_view = 0U;
        } else if (s_state.hr_measuring) {
            HAL_GPIO_WritePin(HR_5V_EN_GPIO_Port, HR_5V_EN_Pin, GPIO_PIN_SET);
        }
        SmileWatch_SyncMiyotaWithPillPause();
        OLED_PowerOn();
        OLED_Init(s_hi2c);
        SmileWatch_TriggerHaptic(now_ms, 35U);
        SmileWatch_RenderCurrentScreen();
        return;
    }

    switch (evt) {
        case SW_BTN_EVT_2H_SHORT:
            s_state.current_screen = (SW_ScreenId_t)((s_state.current_screen + 1U) % SW_SCREEN_COUNT);
            s_state.sub_view = 0U;
            SmileWatch_TriggerHaptic(now_ms, 25U);
            break;

        case SW_BTN_EVT_2H_LONG:
            if (s_state.current_screen != SW_SCREEN_HOME) {
                s_state.current_screen = SW_SCREEN_HOME;
                s_state.sub_view = 0U;
            } else {
                s_state.display_awake = false;
                OLED_PowerOff();
                SmileWatch_SyncMiyotaWithPillPause();
            }
            SmileWatch_TriggerHaptic(now_ms, 60U);
            break;

        case SW_BTN_EVT_4H_SHORT:
            s_state.sub_view = (uint8_t)((s_state.sub_view + 1U) % 3U);
            SmileWatch_TriggerHaptic(now_ms, 20U);
            break;

        case SW_BTN_EVT_4H_LONG:
            SmileWatch_TriggerHaptic(now_ms, 80U);
            switch (s_state.current_screen) {
                case SW_SCREEN_HOME:
                    s_state.split_6h_mode = (uint8_t)((s_state.split_6h_mode + 1U) % 3U);
                    SmileWatch_UpdateSplit6h();
                    break;
                case SW_SCREEN_HEART:
                    s_state.hr_measuring = !s_state.hr_measuring;
                    HAL_GPIO_WritePin(HR_5V_EN_GPIO_Port, HR_5V_EN_Pin,
                                      s_state.hr_measuring ? GPIO_PIN_SET : GPIO_PIN_RESET);
                    SmileWatch_UpdatePowerEstimate(now_ms);
                    break;
                case SW_SCREEN_HEALTH:
                    s_state.temp_base_deci_c = s_state.temp_deci_c;
                    SmileWatch_UpdatePhysioEngine(true, now_ms);
                    break;
                case SW_SCREEN_ACTIVITY:
                    s_state.steps_count = 0UL;
                    s_state.steps_at_last_calib = 0UL;
                    break;
                case SW_SCREEN_SPORT:
                    if (s_state.sub_view == 0U) {
                        s_state.sport_type = (uint8_t)((s_state.sport_type + 1U) % SW_SPORT_COUNT);
                    } else if (s_state.sub_view == 1U) {
                        if (s_state.tri_active) {
                            SmileWatch_NextTriathlonStage(now_ms);
                        } else if (s_state.sport_type == 0U) {
                            s_state.gym_sets++;
                            s_state.gym_reps = 0U;
                            s_state.gym_rest_sec = 60U;
                            s_state.sport_active = true;
                            s_state.sport_sessions_done++;
                            SmileWatch_UpdateAdaptiveSport();
                        } else {
                            if (s_state.sport_active && (s_state.sport_sec >= 10U)) {
                                s_state.sport_active = false;
                                SmileWatch_SaveWorkoutSession();
                            } else {
                                s_state.sport_active = !s_state.sport_active;
                                s_state.sport_sessions_done++;
                                SmileWatch_UpdateAdaptiveSport();
                            }
                        }
                    } else {
                        SmileWatch_SubmitSportFeedback(0, -1, now_ms);
                    }
                    break;
                case SW_SCREEN_CHRONO:
                    if (s_state.sub_view == 0U) {
                        if (s_state.chrono_running) {
                            s_state.chrono_running = false;
                            s_state.chrono_lap_tenths = s_state.chrono_tenths;
                        } else if (s_state.chrono_tenths > 0UL) {
                            s_state.chrono_tenths = 0UL;
                            s_state.chrono_lap_tenths = 0UL;
                        } else {
                            s_state.chrono_running = true;
                        }
                    } else if (s_state.sub_view == 1U) {
                        if (s_state.timer_rem_sec == 0U) {
                            s_state.timer_rem_sec = s_state.timer_preset_sec;
                        }
                        s_state.timer_running = !s_state.timer_running;
                    } else {
                        s_state.timer_preset_sec = (s_state.timer_preset_sec == 300U) ? 600U :
                                                   (s_state.timer_preset_sec == 600U) ? 1500U : 300U;
                        s_state.timer_rem_sec = s_state.timer_preset_sec;
                        s_state.timer_running = false;
                    }
                    break;
                case SW_SCREEN_NOTIF:
                    if (s_state.sub_view == 1U) {
                        s_state.nav_active = !s_state.nav_active;
                    } else {
                        s_state.notif_unread = 0U;
                    }
                    break;
                case SW_SCREEN_SMART:
                    if (s_state.sub_view == 0U) {
                        if (s_state.hid_mode == 0U) {
                            s_state.media_playing = !s_state.media_playing;
                        } else {
                            s_state.hid_shots++;
                        }
                    } else if (s_state.sub_view == 1U) {
                        if (!s_state.alarm_enabled) {
                            s_state.alarm_enabled = true;
                            s_state.smart_wake_enabled = false;
                        } else if (!s_state.smart_wake_enabled) {
                            s_state.smart_wake_enabled = true;
                        } else {
                            s_state.alarm_enabled = false;
                            s_state.smart_wake_enabled = false;
                        }
                        s_state.smart_wake_triggered = false;
                        s_state.hw_flash_ok = FlashStorage_Save(&s_state, &s_miyota);
                    } else {
                        s_miyota.target_pos_sec = (s_miyota.target_pos_sec + 60UL) % MIYOTA_SEC_PER_12H;
                    }
                    break;
                case SW_SCREEN_CONFIG:
                    if (s_state.sub_view == 0U) {
                        s_state.night_mode_enabled = !s_state.night_mode_enabled;
                        s_state.hw_flash_ok = FlashStorage_Save(&s_state, &s_miyota);
                    } else if (s_state.sub_view == 1U) {
                        if (s_state.history_count > 0U) {
                            s_state.history_view_idx = (uint8_t)((s_state.history_view_idx + 1U) % s_state.history_count);
                        }
                    } else {
                        s_state.hw_flash_ok = FlashStorage_Save(&s_state, &s_miyota);
                    }
                    break;
                default:
                    break;
            }
            break;

        case SW_BTN_EVT_BOTH_LONG: {
            SmileWatch_TriggerHaptic(now_ms, 150U);
            uint32_t rtc12 = ((uint32_t)(s_state.hours % 12U) * 3600UL) +
                             ((uint32_t)s_state.minutes * 60UL) +
                             (uint32_t)s_state.seconds;
            Miyota_SetCalibrated12h00(&s_miyota, rtc12);
            break;
        }

        default:
            break;
    }

    SmileWatch_RenderCurrentScreen();
}

static bool SmileWatch_IsGpsAppTitle(const char *title)
{
    return (strncmp(title, "MAPS", 4U) == 0) ||
           (strncmp(title, "WAZE", 4U) == 0) ||
           (strncmp(title, "PLANS", 5U) == 0) ||
           (strncmp(title, "GPS", 3U) == 0);
}

static uint32_t SmileWatch_ParseGpsNotification(const char *body)
{
    s_state.nav_active = true;

    if (strstr(body, "DROITE") != NULL) {
        s_state.nav_dir = 1U;
    } else if (strstr(body, "GAUCHE") != NULL) {
        s_state.nav_dir = 2U;
    } else if (strstr(body, "ROND") != NULL) {
        s_state.nav_dir = 3U;
    } else if (strstr(body, "ARRIV") != NULL) {
        s_state.nav_dir = 4U;
    } else {
        s_state.nav_dir = 0U;
    }

    uint32_t dist = 0UL;
    bool found_digit = false;
    const char *p = body;
    while (*p != '\0') {
        if ((*p >= '0') && (*p <= '9')) {
            found_digit = true;
            dist = (dist * 10UL) + (uint32_t)(*p - '0');
        } else if (found_digit) {
            break;
        }
        p++;
    }
    if (dist > 999UL) dist = 999UL;
    s_state.nav_dist_m = (s_state.nav_dir == 4U) ? 0U : (uint16_t)dist;

    const char *street_ptr = strstr(body, "->");
    if (street_ptr != NULL) {
        street_ptr += 2;
    } else if ((*p == 'M') || (*p == 'm')) {
        street_ptr = p + 1;
    } else {
        street_ptr = body;
    }
    while (*street_ptr == ' ') {
        street_ptr++;
    }
    if (*street_ptr != '\0') {
        strncpy(s_state.nav_street, street_ptr, sizeof(s_state.nav_street) - 1U);
        s_state.nav_street[sizeof(s_state.nav_street) - 1U] = '\0';
    }

    /* Differentiated DRV2605L haptic cue per turn direction */
    if (s_state.nav_dir == 1U) return 110U;
    if (s_state.nav_dir == 2U) return 220U;
    if (s_state.nav_dir == 3U) return 160U;
    if (s_state.nav_dir == 4U) return 320U;
    return 80U;
}

void SmileWatch_PushNotification(const char *title, const char *body, uint32_t now_ms)
{
    if ((title == NULL) || (body == NULL)) return;
    strncpy(s_state.notif_title, title, sizeof(s_state.notif_title) - 1U);
    s_state.notif_title[sizeof(s_state.notif_title) - 1U] = '\0';
    strncpy(s_state.notif_body, body, sizeof(s_state.notif_body) - 1U);
    s_state.notif_body[sizeof(s_state.notif_body) - 1U] = '\0';

    s_state.notif_unread     = 1U;
    s_state.current_screen   = SW_SCREEN_NOTIF;
    s_state.last_activity_ms = now_ms;

    uint32_t haptic_ms = 180U;
    if (SmileWatch_IsGpsAppTitle(title)) {
        s_state.sub_view = 1U;
        haptic_ms = SmileWatch_ParseGpsNotification(body);
    } else {
        s_state.sub_view = 0U;
    }

    if (!s_state.display_awake && !s_state.survival_mode && !SmileWatch_IsNightQuiet()) {
        s_state.display_awake = true;
        OLED_PowerOn();
        OLED_Init(s_hi2c);
    }
    if (!SmileWatch_IsNightQuiet()) {
        SmileWatch_TriggerHaptic(now_ms, haptic_ms);
    }
    SmileWatch_RenderCurrentScreen();
}

void SmileWatch_LoopTask(uint32_t now_ms)
{
    bool b2 = (HAL_GPIO_ReadPin(BTN_TOP_2H_GPIO_Port, BTN_TOP_2H_Pin) == GPIO_PIN_RESET);
    bool b4 = (HAL_GPIO_ReadPin(BTN_BOT_4H_GPIO_Port, BTN_BOT_4H_Pin) == GPIO_PIN_RESET);

    if (b2 && !s_btn2h_prev) { s_btn2h_down_ms = now_ms; s_btn2h_long_fired = false; }
    if (b4 && !s_btn4h_prev) { s_btn4h_down_ms = now_ms; s_btn4h_long_fired = false; }

    if (b2 && b4 && !s_btn2h_long_fired && !s_btn4h_long_fired) {
        if (((now_ms - s_btn2h_down_ms) >= 1000UL) && ((now_ms - s_btn4h_down_ms) >= 1000UL)) {
            s_btn2h_long_fired = true;
            s_btn4h_long_fired = true;
            SmileWatch_HandleButtonEvent(SW_BTN_EVT_BOTH_LONG, now_ms);
        }
    } else {
        if (b2 && !s_btn2h_long_fired && ((now_ms - s_btn2h_down_ms) >= SW_BTN_LONG_MS)) {
            s_btn2h_long_fired = true;
            SmileWatch_HandleButtonEvent(SW_BTN_EVT_2H_LONG, now_ms);
        }
        if (b4 && !s_btn4h_long_fired && ((now_ms - s_btn4h_down_ms) >= SW_BTN_LONG_MS)) {
            s_btn4h_long_fired = true;
            SmileWatch_HandleButtonEvent(SW_BTN_EVT_4H_LONG, now_ms);
        }
    }

    if (!b2 && s_btn2h_prev && !s_btn2h_long_fired && ((now_ms - s_btn2h_down_ms) >= SW_BTN_DEBOUNCE_MS)) {
        SmileWatch_HandleButtonEvent(SW_BTN_EVT_2H_SHORT, now_ms);
    }
    if (!b4 && s_btn4h_prev && !s_btn4h_long_fired && ((now_ms - s_btn4h_down_ms) >= SW_BTN_DEBOUNCE_MS)) {
        SmileWatch_HandleButtonEvent(SW_BTN_EVT_4H_SHORT, now_ms);
    }
    s_btn2h_prev = b2;
    s_btn4h_prev = b4;

    if ((s_state.haptic_until_ms != 0U) && ((int32_t)(now_ms - s_state.haptic_until_ms) >= 0)) {
        HAL_GPIO_WritePin(DRV_EN_GPIO_Port, DRV_EN_Pin, GPIO_PIN_RESET);
        s_state.haptic_until_ms = 0U;
    }

    SW_SensorSample_t sample;
    memset(&sample, 0, sizeof(sample));
    if (s_state.survival_mode) {
        /* Extreme survival mode: no pedometer, no IMU, no HR, no I2C polling — Miyota + Qi check only */
        s_state.qi_charging = (HAL_GPIO_ReadPin(BQ_CHG_GPIO_Port, BQ_CHG_Pin) == GPIO_PIN_RESET);
    } else {
        Sensors_Poll(s_hi2c, now_ms, s_state.hr_measuring, s_state.mlc_thresh_mg, &sample);
        if (s_state.hw_imu_ok) {
            if (sample.steps_delta > 0UL) {
                s_state.steps_count += sample.steps_delta;
                s_win_steps         += sample.steps_delta;
            }
            if (sample.wrist_flick_evt) {
                SmileWatch_HandleWristFlick(now_ms);
            }
            if (sample.fall_detected_evt) {
                SmileWatch_TriggerFallSos(now_ms);
            }
        }
    }
    if (sample.temp_valid) {
        s_state.temp_deci_c = sample.temp_deci_c;
    }
    if (sample.als_valid) {
        s_state.ambient_lux = sample.ambient_lux;
        if (s_state.display_awake && s_state.auto_brightness_enabled) {
            uint8_t target_c = (s_state.ambient_lux < 20U) ? 40U :
                               (s_state.ambient_lux < 250U) ? 110U :
                               (s_state.ambient_lux < 1500U) ? 185U : 255U;
            if (target_c != s_state.oled_contrast) {
                s_state.oled_contrast = target_c;
                OLED_SetContrast(s_hi2c, target_c);
            }
        }
    }
    if (sample.hr_valid) {
        s_state.heart_bpm = sample.heart_bpm;
        s_state.spo2_pct  = sample.spo2_pct;
        if (sample.heart_bpm < s_state.hr_min) s_state.hr_min = sample.heart_bpm;
        if (sample.heart_bpm > s_state.hr_max) s_state.hr_max = sample.heart_bpm;
    }
    if (sample.hrv_valid) {
        s_state.hrv_rmssd_ms = sample.hrv_rmssd_ms;
    }
    if (s_state.hw_imu_ok || s_state.hw_temp_ok || s_state.hw_als_ok) {
        s_state.qi_charging = sample.qi_charging;
    }
    if ((now_ms - s_last_coherence_ms) >= 15000UL) {
        s_last_coherence_ms = now_ms;
        uint16_t spm = (uint16_t)(s_win_steps * 4UL);
        if (s_state.hw_imu_ok && s_state.hw_hr_ok) {
            SmileWatch_CheckImuHrCoherence(spm, now_ms);
        }
        if (s_state.hw_imu_ok) {
            SmileWatch_AutoDetectSport(sample.imu_energy_mg, sample.imu_gyro_dps, spm, now_ms);
        }
        s_win_steps = 0UL;
    }

    if ((now_ms - s_last_rtc_tick_ms) >= 1000UL) {
        s_last_rtc_tick_ms += 1000UL;
        s_state.seconds++;
        if (s_state.seconds >= 60U) {
            s_state.seconds = 0U;
            s_state.minutes++;
            if (s_state.minutes >= 60U) {
                s_state.minutes = 0U;
                s_state.hours = (uint8_t)((s_state.hours + 1U) % 24U);
                if (s_state.hours == 0U) {
                    s_state.smart_wake_triggered = false;
                    s_state.wake_summary_shown_today = false;
                }
            }
        }
        /* Ultimate Reserve Mode (< 10% battery): cut BLE, HR, and auto-wake */
        if ((s_state.battery_pct <= 10U) && !s_state.qi_charging) {
            s_state.survival_mode = true;
            s_state.ble_connected = false;
            s_state.hr_measuring  = false;
            s_state.power_profile = 0U;
            HAL_GPIO_WritePin(HR_5V_EN_GPIO_Port, HR_5V_EN_Pin, GPIO_PIN_RESET);
        } else if ((s_state.battery_pct > 15U) && s_state.survival_mode) {
            s_state.survival_mode = false;
        }
        /* Live Sport Coach HR Zone + Haptic Coaching (every 20s) */
        if (s_state.sport_active && s_state.coach_enabled) {
            uint16_t hr_pct = ((uint16_t)s_state.heart_bpm * 100U) / 190U;
            s_state.current_hr_zone = (hr_pct < 60U) ? 1U : (hr_pct < 70U) ? 2U : (hr_pct < 80U) ? 3U : (hr_pct < 90U) ? 4U : 5U;
            if (s_state.current_hr_zone > s_state.target_hr_zone) {
                s_state.coach_status = 2U; /* RALENTIR */
                if (((s_state.sport_sec % 20U) == 0U) && !SmileWatch_IsNightQuiet()) {
                    Sensors_PlayHaptic(s_hi2c, 14U);
                    SmileWatch_TriggerHaptic(now_ms, 120U);
                }
            } else if ((s_state.current_hr_zone < s_state.target_hr_zone) && (s_state.sport_sec >= 30U)) {
                s_state.coach_status = 1U; /* ACCELERER */
                if (((s_state.sport_sec % 20U) == 0U) && !SmileWatch_IsNightQuiet()) {
                    Sensors_PlayHaptic(s_hi2c, 1U);
                    SmileWatch_TriggerHaptic(now_ms, 50U);
                }
            } else {
                s_state.coach_status = 0U; /* ZONE OK */
            }
        }
        /* Morning Wake Summary trigger (06h..11h once awake) */
        if (!s_state.wake_summary_shown_today && (s_state.hours >= 6U) && (s_state.hours <= 11U) && (s_state.sleep_stage == 0U)) {
            SmileWatch_ShowWakeSummary(now_ms);
        }
        s_state.stop2_active = !SmileWatch_NeedsFastTick();
        SmileWatch_UpdatePhysioEngine(sample.imu_still, now_ms);
        SmileWatch_UpdatePowerEstimate(now_ms);
        SmileWatch_UpdateSplit6h();
        SmileWatch_SyncMiyotaWithPillPause();

        if (s_state.sos_active && !s_state.sos_sent) {
            if (s_state.sos_countdown_sec > 0U) {
                s_state.sos_countdown_sec--;
                Sensors_PlayHaptic(s_hi2c, 14U);
                SmileWatch_TriggerHaptic(now_ms, 150U);
            }
            if (s_state.sos_countdown_sec == 0U) {
                s_state.sos_sent = true;
                Sensors_PlayHaptic(s_hi2c, 47U);
                SmileWatch_TriggerHaptic(now_ms, 400U);
            }
        } else if (s_state.sos_active && s_state.sos_sent && ((s_state.seconds & 1U) == 0U)) {
            Sensors_PlayHaptic(s_hi2c, 47U);
            SmileWatch_TriggerHaptic(now_ms, 220U);
        }

        if (s_state.sport_active) {
            s_state.sport_sec++;
            if (s_state.tri_active) {
                s_state.tri_total_sec++;
            }
            /* Tabata 20s Work / 10s Rest x 8 when HIIT (4) is active */
            if (s_state.sport_type == 4U) {
                if (s_state.tabata_sec_left > 1U) {
                    s_state.tabata_sec_left--;
                } else {
                    if (s_state.tabata_work) {
                        s_state.tabata_work = false;
                        s_state.tabata_sec_left = 10U;
                        Sensors_PlayHaptic(s_hi2c, 16U);
                        SmileWatch_TriggerHaptic(now_ms, 80U);
                    } else {
                        s_state.tabata_work = true;
                        s_state.tabata_sec_left = 20U;
                        s_state.tabata_round = (s_state.tabata_round < 8U) ? (uint8_t)(s_state.tabata_round + 1U) : 1U;
                        Sensors_PlayHaptic(s_hi2c, 14U);
                        SmileWatch_TriggerHaptic(now_ms, 140U);
                    }
                }
            } else if ((s_state.sport_type == 14U) && ((s_state.sport_sec % 30U) == 0U)) {
                /* Gainage / Mobilite: micro-vibration toutes les 30s */
                if (!SmileWatch_IsNightQuiet()) {
                    Sensors_PlayHaptic(s_hi2c, 1U);
                    SmileWatch_TriggerHaptic(now_ms, 30U);
                }
            } else if ((s_state.sport_type == 13U) && ((s_state.sport_sec % 1200U) == 0U)) {
                /* Trail: rappel hydratation toutes les 20 min */
                if (!SmileWatch_IsNightQuiet()) {
                    Sensors_PlayHaptic(s_hi2c, 14U);
                    SmileWatch_TriggerHaptic(now_ms, 100U);
                }
            }
        }
        /* Guided Coherence Breathing 5s Inhale / 5s Exhale on YOGA (9) */
        if ((s_state.current_screen == SW_SCREEN_SPORT) && (s_state.sub_view == 1U) && (s_state.sport_type == 9U)) {
            s_state.breath_phase_sec = (uint8_t)((s_state.breath_phase_sec + 1U) % 10U);
            if ((s_state.breath_phase_sec == 0U) || (s_state.breath_phase_sec == 5U)) {
                Sensors_PlayHaptic(s_hi2c, 1U);
                SmileWatch_TriggerHaptic(now_ms, 20U);
            }
        }
        /* Post-workout 60s Heart Rate Recovery (HRR60) */
        if (s_state.hrr_active) {
            if (s_state.hrr_sec_left > 0U) {
                s_state.hrr_sec_left--;
            }
            if (s_state.hrr_sec_left == 0U) {
                s_state.hrr_active = false;
                s_state.hrr_drop_bpm = (s_state.hrr_start_bpm > s_state.heart_bpm)
                                       ? (uint8_t)(s_state.hrr_start_bpm - s_state.heart_bpm)
                                       : 0U;
            }
        }
        /* Sedentary 50-minute stillness counter */
        if ((s_state.seconds == 0U) && s_state.sedentary_alert_enabled && !s_state.sport_active) {
            if (sample.imu_still) {
                if (s_state.sedentary_min < 255U) s_state.sedentary_min++;
                if (s_state.sedentary_min >= 50U) {
                    SmileWatch_TriggerSedentaryAlert(now_ms);
                }
            } else {
                s_state.sedentary_min = 0U;
                s_state.sedentary_alert_active = false;
            }
        }
        if (s_state.nav_active && (s_state.nav_dist_m > 0U)) {
            s_state.nav_dist_m = (s_state.nav_dist_m > 5U) ? (uint16_t)(s_state.nav_dist_m - 5U) : 0U;
        }
        if (s_state.gym_rest_sec > 0U) {
            s_state.gym_rest_sec--;
        }
        if (s_state.timer_running) {
            if (s_state.timer_rem_sec > 0U) {
                s_state.timer_rem_sec--;
            }
            if (s_state.timer_rem_sec == 0U) {
                s_state.timer_running = false;
                SmileWatch_TriggerHaptic(now_ms, 250U);
            }
        }

        /* Smart Wake-Up (-20 min window, triggers outside Deep Sleep) */
        if (s_state.alarm_enabled && s_state.smart_wake_enabled &&
            !s_state.smart_wake_triggered && (s_state.seconds == 0U)) {
            uint16_t now_min   = (uint16_t)((uint16_t)s_state.hours * 60U + (uint16_t)s_state.minutes);
            uint16_t alarm_min = (uint16_t)((uint16_t)s_state.alarm_hour * 60U + (uint16_t)s_state.alarm_min);
            uint16_t delta_min = (uint16_t)((alarm_min - now_min + 1440U) % 1440U);
            if ((delta_min >= 1U) && (delta_min <= 20U) && (s_state.sleep_stage != 2U)) {
                s_state.smart_wake_triggered = true;
                s_state.current_screen       = SW_SCREEN_SMART;
                s_state.sub_view             = 1U;
                s_state.last_activity_ms     = now_ms;
                if (!s_state.display_awake && !s_state.survival_mode) {
                    s_state.display_awake = true;
                    OLED_PowerOn();
                    OLED_Init(s_hi2c);
                }
                Sensors_PlayHaptic(s_hi2c, 52U);
                SmileWatch_TriggerHaptic(now_ms, 180U);
            }
        }

        if (s_state.alarm_enabled &&
            (s_state.hours == s_state.alarm_hour) &&
            (s_state.minutes == s_state.alarm_min) &&
            (s_state.seconds == 0U)) {
            s_state.current_screen   = SW_SCREEN_SMART;
            s_state.sub_view         = 1U;
            s_state.last_activity_ms = now_ms;
            if (!s_state.display_awake && !s_state.survival_mode) {
                s_state.display_awake = true;
                OLED_PowerOn();
                OLED_Init(s_hi2c);
            }
            SmileWatch_TriggerHaptic(now_ms, 300U);
        }
    }

    Miyota_Task(&s_miyota, now_ms);

    if (s_state.display_awake && ((now_ms - s_state.last_activity_ms) >= SW_OLED_TIMEOUT_MS)) {
        s_state.display_awake = false;
        OLED_PowerOff();
        SmileWatch_SyncMiyotaWithPillPause();
        /* Power-saving: cut +5V_HR boost when screen sleeps outside active sport/HRR60 */
        if (!s_state.sport_active && !s_state.hrr_active) {
            HAL_GPIO_WritePin(HR_5V_EN_GPIO_Port, HR_5V_EN_Pin, GPIO_PIN_RESET);
        }
    }

    if ((now_ms - s_last_render_ms) >= 100UL) {
        s_last_render_ms = now_ms;
        if (s_state.chrono_running) {
            s_state.chrono_tenths++;
        }
        if (s_state.display_awake) {
            s_state.ppg_phase = (uint8_t)((s_state.ppg_phase + 1U) % 22U);
            SmileWatch_RenderCurrentScreen();
        }
    }
}

SW_SystemState_t* SmileWatch_GetState(void)
{
    return &s_state;
}

Miyota_Handle_t* SmileWatch_GetMiyota(void)
{
    return &s_miyota;
}
