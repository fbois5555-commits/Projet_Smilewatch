/**
  ******************************************************************************
  * @file           : smilewatch_ui.h
  * @brief          : SmileWatch V2 Application State Machine & 8-Screen UI (x3 sub-views)
  *                   Buttons: 2h Top (S2 / PC5) & 4h Bottom (S1 / PC4)
  *                   Display: 88x48 PMOLED (CH1115 at 6 o'clock) with Split-Screen 6h
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

/* 8 Main Hybrid Watch Screens x 3 Sub-views (24 total screens) */
typedef enum {
    SW_SCREEN_HOME = 0,       /* 1. Accueil : A=Minimaliste, B=Dashboard, C=Energie/Autonomie */
    SW_SCREEN_HEART,          /* 2. Cardio & SpO2 : A=BPM+PPG, B=SpO2+Min/Max, C=Graphe 24h */
    SW_SCREEN_HEALTH,         /* 3. Sante & T° : A=TMP117 ±0.1C, B=BodyBat/Stress, C=Sommeil */
    SW_SCREEN_ACTIVITY,       /* 4. Activite : A=Podometre+%, B=Dist+Kcal, C=Histo 7j */
    SW_SCREEN_SPORT,          /* 5. Sport Autonome : A=Muscu MLC, B=Natation 25m, C=Course */
    SW_SCREEN_CHRONO,         /* 6. Chrono & Timer : A=Chrono 1/10s, B=Minuteur, C=Lap/Preset */
    SW_SCREEN_NOTIF,          /* 7. Notifs & GPS : A=ANCS Multi, B=GPS Turn-by-Turn, C=Meteo+RDV */
    SW_SCREEN_SMART,          /* 8. Smart & Sys : A=HID Media/Photo, B=Alarme/Doux, C=FOTA/Cap */
    SW_SCREEN_CONFIG,         /* 9. Config & Histo : A=Nav/SOS/Doux, B=Historique, C=I2C/Flash */
    SW_SCREEN_COUNT
} SW_ScreenId_t;

#define SW_SPORT_COUNT 19U
#define SW_HISTORY_MAX 8U

typedef struct {
    uint8_t  sport_type;     /* 0..9 */
    uint16_t duration_sec;   /* Session duration (s) */
    uint8_t  avg_bpm;        /* Average BPM */
    uint16_t kcal;           /* Estimated kcal */
    uint8_t  recovery_h;     /* Calculated recovery hours */
} SW_WorkoutRecord_t;

/* Button Event Types (2h Top = S2/PC5, 4h Bottom = S1/PC4) */
typedef enum {
    SW_BTN_EVT_NONE = 0,
    SW_BTN_EVT_2H_SHORT,      /* Top button short click (< 600 ms): Next screen */
    SW_BTN_EVT_2H_LONG,       /* Top button long press (>= 600 ms): Back Home / Sleep */
    SW_BTN_EVT_4H_SHORT,      /* Bottom button short click: Next sub-view (A -> B -> C) */
    SW_BTN_EVT_4H_LONG,       /* Bottom button long press: Primary action / Start-Stop */
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

    /* Biomedical & Motion Sensors */
    uint8_t  heart_bpm;       /* MAX30101 BPM */
    uint8_t  spo2_pct;        /* MAX30101 SpO2 % */
    uint8_t  body_battery;    /* Body Battery score 0..100 */
    uint8_t  stress_score;    /* Real-time HRV Stress score 0..100 */
    uint8_t  hr_min;          /* Daily Min BPM (e.g. 48) */
    uint8_t  hr_max;          /* Daily Max BPM (e.g. 126) */
    bool     hr_measuring;    /* +5V_HR boost enabled (/EN_5V_HR on PB0) */
    int16_t  temp_deci_c;     /* TMP117 temperature in 0.1 C (e.g. 366 = 36.6 C) */
    int16_t  temp_base_deci_c;/* TMP117 calibrated baseline in 0.1 C (e.g. 365 = 36.5 C) */
    uint32_t steps_count;     /* LSM6DSOX step counter */
    uint32_t steps_goal;      /* Default 10000 */
    uint16_t ambient_lux;     /* VEML6030 ambient light in lux */

    /* Sleep Tracking */
    uint16_t sleep_min;       /* Total sleep minutes (e.g. 462 = 7h42) */
    uint8_t  sleep_score;     /* Sleep quality score 0..100 */
    uint8_t  sleep_deep_pct;  /* Deep sleep % */
    uint8_t  sleep_rem_pct;   /* REM sleep % */

    /* Autonomous Sport (MLC LSM6DSOX - 10 Disciplines + Adaptive AI & Recovery) */
    uint8_t  sport_type;          /* 0..9: MUSCU, COURSE, NAGE25M, VELO, HIIT, RANDO, TENNIS, BOXE, RAMEUR, YOGA */
    uint16_t sport_metric;        /* Sport counter (strokes, shots, D+ m, speed*10, etc.) */
    uint8_t  mlc_confidence_pct;  /* Adaptive LSM6DSOX MLC recognition accuracy (72..99 %) */
    uint16_t mlc_thresh_mg;       /* Auto-calibrated IMU threshold in mg (1150..1350 mg) */
    uint8_t  stride_cm;           /* Auto-calibrated running stride length in cm (70..85 cm) */
    uint8_t  recovery_hours;      /* Calculated post-workout recovery time in hours (4..72 h) */
    uint8_t  vo2_max;             /* Estimated VO2max (ml/kg/min) */
    uint16_t sport_sessions_done; /* Completed/calibrated sport sessions count */
    bool     sport_active;        /* Active sport session running */
    uint16_t sport_sec;           /* Elapsed sport duration (s) */
    uint8_t  gym_sets;            /* Weight training sets */
    uint8_t  gym_reps;            /* Repetitions in current set */
    uint8_t  gym_rest_sec;        /* Rest countdown (s) */
    uint8_t  swim_laps;           /* 25m pool laps */
    uint8_t  swim_stroke;         /* 0 = CRAWL, 1 = BRASSE */
    uint16_t run_pace_sec;        /* Pace in sec/km (e.g. 315 = 5'15"/km) */

    /* Stopwatch & Timer */
    bool     chrono_running;
    uint32_t chrono_tenths;     /* Stopwatch tenths of a second */
    uint32_t chrono_lap_tenths; /* Last recorded lap */
    bool     timer_running;
    uint16_t timer_rem_sec;     /* Remaining timer seconds */
    uint16_t timer_preset_sec;  /* Preset duration (300 / 600 / 1500 s) */

    /* Smartphone Notification (ANCS), GPS Navigation, Weather & Calendar */
    uint8_t  notif_unread;
    uint8_t  notif_index;
    char     notif_title[16];
    char     notif_body[32];
    bool     nav_active;      /* True when a GPS turn-by-turn route is active */
    uint8_t  nav_dir;         /* 0=STRAIGHT, 1=RIGHT, 2=LEFT, 3=ROUNDABOUT, 4=ARRIVAL */
    uint16_t nav_dist_m;      /* Distance to next maneuver in meters (0..999 m) */
    char     nav_street[16];  /* Target street or instruction (e.g. "RUE PONTOISE") */
    char     nav_eta[8];      /* Estimated arrival time (e.g. "13:28") */
    int8_t   weather_temp_c;
    uint8_t  weather_rain_pct;
    char     weather_desc[12];
    char     calendar_time[8];
    char     calendar_title[16];

    /* Smart BLE HID, Haptic Alarm, Cloud FOTA & STM32WB5MMG Capacity Telemetry */
    bool     media_playing;
    uint8_t  hid_mode;            /* 0 = MUSIQUE, 1 = PHOTO */
    char     media_track[16];
    uint16_t hid_shots;
    bool     alarm_enabled;
    uint8_t  alarm_hour;
    uint8_t  alarm_min;
    bool     ota_active;          /* True while BLE FOTA update is streaming */
    uint8_t  ota_progress_pct;    /* 0..100 % BLE FOTA progress */
    uint8_t  fw_major;            /* Firmware major version (2) */
    uint8_t  fw_minor;            /* Firmware minor version (1 -> 2/3 after OTA) */
    uint8_t  flash_used_pct;      /* STM32WB5MMG App Flash used % (6 %) */
    uint8_t  ram_used_pct;        /* STM32WB5MMG SRAM used % (2 %) */
    uint8_t  mlc_trees_used;      /* LSM6DSOX active MLC decision trees (4..8 of 8) */

    /* UI, Annex Split-Screen 6h & Haptic state */
    SW_ScreenId_t current_screen;
    uint8_t       sub_view;        /* 0, 1, or 2 */
    bool          split_6h;        /* Active split-screen avoidance state */
    uint8_t       split_6h_mode;   /* Annex option: 0=OFF (default), 1=AUTO 6h, 2=FORCED ON */
    bool          split_6h_force;  /* Legacy helper synced with split_6h_mode == 2 */
    bool          display_awake;
    uint32_t      last_activity_ms;
    uint32_t      haptic_until_ms;
    uint8_t       ppg_phase;

    /* Workout History Ring Buffer */
    SW_WorkoutRecord_t workout_history[SW_HISTORY_MAX];
    uint8_t            history_count;
    uint8_t            history_head;
    uint8_t            history_view_idx;

    /* Configurable Settings, Smart Wake & Fall SOS */
    bool               wrist_nav_enabled;       /* Hands-free wrist flick navigation */
    bool               fall_sos_enabled;        /* Fall detection + 10s SOS alert */
    bool               smart_wake_enabled;      /* 20-min light sleep gentle alarm */
    bool               pill_hand_pause_enabled; /* Pause minute hand before OLED pill (:24..:35) when screen ON */
    bool               pill_hand_paused;        /* True while hand is currently parked in front of pill */
    bool               smart_wake_triggered;    /* True once smart wake fired today */
    bool               sos_active;           /* Active SOS alert overlay */
    uint8_t            sos_countdown_sec;    /* 10..0 countdown before BLE alert */
    bool               sos_sent;             /* True once SOS BLE alert transmitted */

    /* Real Hardware I2C/Flash Status (populated by Sensors_Init & FlashStorage) */
    bool               hw_imu_ok;
    bool               hw_hr_ok;
    bool               hw_temp_ok;
    bool               hw_als_ok;
    bool               hw_drv_ok;
    bool               hw_flash_ok;

    /* Physiological Engine & Auto-Learned Baselines */
    uint16_t           hrv_rmssd_ms;        /* Current HRV RMSSD (ms) */
    uint16_t           hrv_base_ms;         /* Auto-learned EMA HRV baseline (ms) */
    uint8_t            rhr_base_bpm;        /* Auto-learned EMA Resting HR baseline */
    uint8_t            sleep_stage;         /* 0=EVEIL, 1=LEGER, 2=PROFOND, 3=REM */
    uint8_t            readiness_state;     /* 0=FORME, 1=MODERE, 2=REPOS! */
    uint8_t            baseline_nights;     /* Calm learning nights accumulated */

    /* BLE Ground-Truth Calibration & User Feedback */
    uint32_t           steps_at_last_calib; /* Watch steps snapshot at last BLE sync */
    int8_t             calib_error_pct;     /* Last BLE/feedback error (-50..+50 %) */
    uint16_t           last_ble_gps_dist_m; /* Last phone GPS distance received (m) */
    uint8_t            imu_fp_rejects;      /* IMU vs Cardio false-positive rejections */

    /* Real-Time LIR3048 Power Budget (280 mAh / 252 mAh usable) */
    uint8_t            power_profile;       /* 0=SURVIE, 1=ECO, 2=STANDARD, 3=SPORT */
    uint16_t           oled_lit_pixels;     /* Active lit pixels (0..4224) */
    uint32_t           current_inst_ua;     /* Instantaneous draw (uA) */
    uint16_t           current_avg_ua;      /* Daily projected average draw (uA) */
    uint16_t           autonomy_days;       /* Estimated days remaining */

    /* Zero-App Auto-Sport, Autonomous Track Calibration & Emergency ICE */
    bool               auto_sport_enabled;     /* Zero-click sport classifier */
    uint8_t            detected_activity;      /* 0=REPOS, 1=MARCHE, 2=COURSE, 3=MUSCU, 4=VELO, 5=NAGE */
    bool               track_calib_active;     /* 400m / 1000m autonomous calibration active */
    uint16_t           track_calib_dist_m;     /* Target distance (400 or 1000 m) */
    uint32_t           track_calib_start_steps;/* Steps snapshot at calibration start */
    char               ice_name[12];           /* Emergency ICE contact name */
    char               ice_phone[16];          /* Emergency ICE contact phone */

    /* Night/DND, Sedentary Alert, Tabata, Coherence, Triathlon & HRR60 */
    bool               night_mode_enabled;     /* DND / Night mode (23h..07h or forced) */
    bool               sedentary_alert_enabled;/* Optional 50-min inactivity reminder */
    uint8_t            sedentary_min;          /* Minutes of continuous stillness */
    bool               sedentary_alert_active; /* Brief OLED "BOUGER!" banner */
    bool               tri_active;             /* Triathlon multi-stage mode */
    uint8_t            tri_stage;              /* 0=NAGE, 1=T1, 2=VELO, 3=T2, 4=COURSE */
    uint16_t           tri_total_sec;          /* Total triathlon elapsed seconds */
    bool               tabata_work;            /* True=20s WORK, False=10s REST */
    uint8_t            tabata_round;           /* 1..8 rounds */
    uint8_t            tabata_sec_left;        /* Countdown 20..1 / 10..1 */
    uint8_t            breath_phase_sec;       /* 0..9 (0..4=INSPIR, 5..9=EXPIR) */
    bool               hrr_active;             /* 60s post-workout HRR measurement */
    uint8_t            hrr_sec_left;           /* 60..0 countdown */
    uint8_t            hrr_start_bpm;          /* BPM at workout stop */
    uint8_t            hrr_drop_bpm;           /* Measured 60s BPM drop */

    /* Adaptive OLED, Look-to-Wake, STOP2, Live Coach & Wake Summary */
    bool               auto_brightness_enabled;/* VEML6030 -> CH1115 auto-contrast */
    uint8_t            oled_contrast;          /* Current CH1115 contrast (40..255) */
    bool               look_to_wake_enabled;   /* Wake OLED only on look-at-watch gesture */
    bool               stop2_active;           /* True when STM32WB55 can enter STOP2 */
    bool               coach_enabled;          /* Live HR-zone haptic coach + optimizer */
    uint8_t            target_hr_zone;         /* 1..5 target HR zone */
    uint8_t            current_hr_zone;        /* 1..5 live HR zone */
    uint8_t            coach_status;           /* 0=ZONE OK, 1=ACCELERER, 2=RALENTIR! */
    uint8_t            coach_rec_code;         /* 0=Z2 REC, 1=Z3 END, 2=Z4 SEUIL, 3=HIIT, 4=REPOS! */
    uint16_t           training_load_acute;    /* 7-day EMA TRIMP load */
    uint16_t           training_load_chronic;  /* 28-day EMA TRIMP load */
    bool               pre_illness_alert;      /* Temp + RHR + HRV morning warning */
    bool               wake_summary_active;    /* Morning summary card on HOME */
    bool               wake_summary_shown_today;/* Prevents re-triggering same day */
    uint8_t            last_sleep_min;         /* Estimated sleep duration (x10 min) */
} SW_SystemState_t;

/* Public Application API */
void SmileWatch_Init(I2C_HandleTypeDef *hi2c);
void SmileWatch_LoopTask(uint32_t now_ms);
bool SmileWatch_NeedsFastTick(void);
void SmileWatch_HandleLookToWake(bool looking_at_watch, uint32_t now_ms);
void SmileWatch_ShowWakeSummary(uint32_t now_ms);
void SmileWatch_HandleButtonEvent(SW_ButtonEvent_t evt, uint32_t now_ms);
void SmileWatch_PushNotification(const char *title, const char *body, uint32_t now_ms);
void SmileWatch_HandleWristFlick(uint32_t now_ms);
void SmileWatch_TriggerFallSos(uint32_t now_ms);
void SmileWatch_SaveWorkoutSession(void);
void SmileWatch_UpdatePhysioEngine(bool imu_still, uint32_t now_ms);
void SmileWatch_ApplyBleCalibration(uint16_t phone_gps_dist_m,
                                    uint32_t phone_steps,
                                    uint32_t watch_steps_window,
                                    uint32_t now_ms);
void SmileWatch_SubmitSportFeedback(int8_t rep_delta, int8_t corrected_sport, uint32_t now_ms);
void SmileWatch_CheckImuHrCoherence(uint16_t recent_steps_per_min, uint32_t now_ms);
void SmileWatch_AutoDetectSport(uint16_t imu_energy_mg,
                                uint16_t imu_gyro_dps,
                                uint16_t spm,
                                uint32_t now_ms);
void SmileWatch_ToggleTrackCalibration(uint16_t dist_m, uint32_t now_ms);
void SmileWatch_StartTriathlon(uint32_t now_ms);
void SmileWatch_NextTriathlonStage(uint32_t now_ms);
void SmileWatch_TriggerSedentaryAlert(uint32_t now_ms);
void SmileWatch_UpdatePowerEstimate(uint32_t now_ms);
void SmileWatch_RenderCurrentScreen(void);
void SmileWatch_DrawHeader(const char *title, uint8_t sub_idx);

SW_SystemState_t* SmileWatch_GetState(void);
Miyota_Handle_t*  SmileWatch_GetMiyota(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_SMILEWATCH_UI_H_ */
