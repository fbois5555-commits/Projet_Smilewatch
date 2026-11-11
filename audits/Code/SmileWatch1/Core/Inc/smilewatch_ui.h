/**
  ******************************************************************************
  * @file           : smilewatch_ui.h
  * @brief          : SmileWatch V2 Application State Machine & 6-Screen UI
  *                   Buttons: 2h Top (S2 / PC5) & 4h Bottom (S1 / PC4)
  *                   Display: 88x48 PMOLED (CH1115 at 6 o'clock)
  ******************************************************************************
  */

#ifndef INC_SMILEWATCH_UI_H_
#define INC_SMILEWATCH_UI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "main.h"
#include "oled_ch1115.h"
#include "miyota_driver.h"

/* 4 Main Essential Hybrid Watch Screens (Pure, Minimalist & Elegant) */
typedef enum {
    SW_SCREEN_HOME = 0,       /* 1. Accueil Minimaliste : Date, Pas du jour, Jauge fine, Batterie, BLE */
    SW_SCREEN_HEART,          /* 2. Cardio : Cœur 1-bit battant, Grand BPM, Onde PPG fine */
    SW_SCREEN_ACTIVITY,       /* 3. Activité : Podomètre LSM6DSOX, Barre % objectif, km & kcal */
    SW_SCREEN_NOTIF,          /* 4. Notifications : Alertes Smartphone ANCS (SMS, Appels) */
    SW_SCREEN_COUNT
} SW_ScreenId_t;

/* Button Event Types (2h Top = S2/PC5, 4h Bottom = S1/PC4) */
typedef enum {
    SW_BTN_EVT_NONE = 0,
    SW_BTN_EVT_2H_SHORT,      /* Top button short click (< 600 ms): Next screen */
    SW_BTN_EVT_2H_LONG,       /* Top button long press (>= 600 ms): Sleep / Back Home */
    SW_BTN_EVT_4H_SHORT,      /* Bottom button short click: Trigger action / Measure */
    SW_BTN_EVT_4H_LONG,       /* Bottom button long press: Secondary action / Reset */
    SW_BTN_EVT_BOTH_LONG      /* 2h + 4h held together (>= 1000 ms): 12:00 Calibration */
} SW_ButtonEvent_t;

/* Simulated or Real Sensor & System Telemetry */
typedef struct {
    /* RTC Time & Date */
    uint8_t  hours;
    uint8_t  minutes;
    uint8_t  seconds;
    uint8_t  day;
    uint8_t  month;
    char     weekday_str[4];  /* e.g. "DIM" */

    /* Power & Connectivity */
    uint8_t  battery_pct;     /* 0..100 % */
    uint16_t battery_mv;      /* 3000..4200 mV */
    bool     qi_charging;     /* BQ51003 + BQ24040 ~CHG low */
    bool     ble_connected;   /* STM32WB BLE link active */
    bool     survival_mode;   /* True if battery_pct < 5% (Analog hands only) */

    /* Biomedical & Motion Sensors (Garmin Health Engine) */
    uint8_t  heart_bpm;       /* MAX30101 BPM */
    uint8_t  spo2_pct;        /* MAX30101 SpO2 % */
    uint8_t  body_battery;    /* Garmin Body Battery score 0..100 */
    uint8_t  stress_score;    /* Real-time HRV Stress score 0..100 */
    uint8_t  hr_min;          /* Daily Min BPM (e.g. 48) */
    uint8_t  hr_max;          /* Daily Max BPM (e.g. 126) */
    bool     hr_measuring;    /* +5V_HR boost enabled (/EN_5V_HR on PB0) */
    int16_t  temp_deci_c;     /* TMP117 temperature in 0.1 C (e.g. 367 = 36.7 C) */
    uint32_t steps_count;     /* LSM6DSOX step counter */
    uint32_t steps_goal;      /* Default 10000 */
    uint16_t ambient_lux;     /* VEML6030 ambient light in lux */

    /* Smartphone Notification (ANCS) */
    uint8_t  notif_unread;
    char     notif_title[16]; /* e.g. "APPEL ENTRANT" or "SMS - ALICE" */
    char     notif_body[32];  /* e.g. "ON SE VOIT A 18H ?" */

    /* UI & Haptic state */
    SW_ScreenId_t current_screen;
    bool          display_awake;
    uint32_t      last_activity_ms;
    uint32_t      haptic_until_ms;
    uint8_t       ppg_phase;
} SW_SystemState_t;

/* Public Application API */
void SmileWatch_Init(I2C_HandleTypeDef *hi2c);
void SmileWatch_LoopTask(uint32_t now_ms);
void SmileWatch_HandleButtonEvent(SW_ButtonEvent_t evt, uint32_t now_ms);
void SmileWatch_PushNotification(const char *title, const char *body, uint32_t now_ms);
void SmileWatch_RenderCurrentScreen(void);

SW_SystemState_t* SmileWatch_GetState(void);
Miyota_Handle_t*  SmileWatch_GetMiyota(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_SMILEWATCH_UI_H_ */
