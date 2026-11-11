/**
  ******************************************************************************
  * @file           : smilewatch_ui.c
  * @brief          : SmileWatch V2 Application State Machine & Modern Wear OS UI
  *                   88x48 1-bit PMOLED Rendering (Topwin QG-8848TSWYG02 / CH1115)
  ******************************************************************************
  */

#include "smilewatch_ui.h"
#include <stdio.h>
#include <string.h>

#define SW_OLED_TIMEOUT_MS          12000UL  /* Auto-sleep OLED after 12 s inactivity */
#define SW_BTN_LONG_MS              600UL    /* Long press threshold */
#define SW_BTN_DEBOUNCE_MS          30UL

static I2C_HandleTypeDef *s_hi2c = NULL;
static SW_SystemState_t   s_state;
static Miyota_Handle_t    s_miyota;

/* Internal button polling state */
static bool     s_btn2h_prev = false;
static bool     s_btn4h_prev = false;
static uint32_t s_btn2h_down_ms = 0U;
static uint32_t s_btn4h_down_ms = 0U;
static bool     s_btn2h_long_fired = false;
static bool     s_btn4h_long_fired = false;
static uint32_t s_last_rtc_tick_ms = 0U;
static uint32_t s_last_render_ms   = 0U;

/* Simulated PPG waveform lookup (22 samples, amplitude 0..16 px) */
static const uint8_t k_ppg_wave[22] = {
    4, 4, 5, 3, 2, 8, 15, 16, 11, 6, 4, 6, 8, 7, 5, 4, 4, 4, 4, 4, 4, 4
};

static void SmileWatch_TriggerHaptic(uint32_t now_ms, uint32_t duration_ms)
{
    HAL_GPIO_WritePin(DRV_EN_GPIO_Port, DRV_EN_Pin, GPIO_PIN_SET);
    s_state.haptic_until_ms = now_ms + duration_ms;
}

/* ============================================================================
 * SCREEN 1: ACCUEIL MINIMALISTE & ÉPURÉ (STYLE HYBRIDE CHIC)
 * ==========================================================================*/
static void Render_ScreenHome(void)
{
    char buf[24];

    /* Date centrée en haut */
    (void)snprintf(buf, sizeof(buf), "%s %02u", s_state.weekday_str, s_state.day);
    OLED_DrawStr(24, 4, buf, 1, 1);

    /* Ligne fine de séparation */
    OLED_DrawLine(10, 16, 77, 16, 1);

    /* Statut central : Pas du jour en chiffres nets */
    (void)snprintf(buf, sizeof(buf), "%lu", (unsigned long)s_state.steps_count);
    OLED_DrawStr(20, 20, buf, 2, 1);
    OLED_DrawStr(64, 26, "PAS", 1, 1);

    /* Ligne fine de progression pas (y=36) */
    OLED_DrawRoundRect(10, 35, 68, 4, 2, 1);
    uint32_t pct = (s_state.steps_goal > 0U) ? ((s_state.steps_count * 100UL) / s_state.steps_goal) : 0UL;
    if (pct > 100UL) pct = 100UL;
    if (pct > 0U) {
        OLED_FillRoundRect(11, 36, (int16_t)((pct * 66UL) / 100UL), 2, 1);
    }

    /* Bas : Batterie discrète à gauche, BLE à droite */
    (void)snprintf(buf, sizeof(buf), "%u%%", s_state.battery_pct);
    OLED_DrawStr(10, 41, buf, 1, 1);
    OLED_DrawRect(28, 41, 8, 5, 1);
    OLED_FillRect(36, 42, 1, 3, 1);
    OLED_FillRect(29, 42, (int16_t)((s_state.battery_pct * 6U) / 100U), 3, 1);

    if (s_state.ble_connected) {
        OLED_DrawIcon(72, 40, OLED_ICON_BLE, 1);
    }
}

/* ============================================================================
 * SCREEN 2: CARDIO & FRÉQUENCE CARDIAQUE ÉPURÉE
 * ==========================================================================*/
static void Render_ScreenHeart(void)
{
    char buf[24];

    /* Titre discret */
    OLED_DrawStr(28, 2, "CARDIO", 1, 1);
    OLED_DrawLine(8, 11, 79, 11, 1);

    /* Cœur 1-bit élégant et battant */
    OLED_DrawBitmap(8, 17, k_garmin_heart_10x9, 10, 9, 1);

    /* Grand chiffre BPM */
    (void)snprintf(buf, sizeof(buf), "%u", s_state.heart_bpm);
    OLED_DrawStr(26, 15, buf, 2, 1);
    OLED_DrawStr(60, 22, "BPM", 1, 1);

    /* Onde cardiaque continue très fine au bas */
    int16_t prev_y = 38;
    for (int16_t x = 0; x < 88; x++) {
        uint8_t idx = (uint8_t)((x + s_state.ppg_phase) % 22U);
        int16_t y = (int16_t)(43 - (k_ppg_wave[idx] * 8) / 16);
        if (x > 0) OLED_DrawLine((int16_t)(x - 1), prev_y, x, y, 1);
        prev_y = y;
    }
}

/* ============================================================================
 * SCREEN 3: ACTIVITÉ & OBJECTIF DU JOUR
 * ==========================================================================*/
static void Render_ScreenActivity(void)
{
    char buf[24];

    /* En-tête */
    OLED_DrawBitmap(8, 2, k_garmin_shoe_12x9, 12, 9, 1);
    OLED_DrawStr(24, 2, "ACTIVITE", 1, 1);
    OLED_DrawLine(6, 12, 81, 12, 1);

    /* Grand chiffre des pas */
    (void)snprintf(buf, sizeof(buf), "%lu", (unsigned long)s_state.steps_count);
    OLED_DrawStr(10, 16, buf, 2, 1);

    /* Objectif % */
    uint32_t pct = (s_state.steps_goal > 0U) ? ((s_state.steps_count * 100UL) / s_state.steps_goal) : 0UL;
    if (pct > 100UL) pct = 100UL;
    (void)snprintf(buf, sizeof(buf), "%u%%", (unsigned int)pct);
    OLED_DrawStr(64, 21, buf, 1, 1);

    /* Barre de progression arrondie */
    OLED_DrawRoundRect(6, 31, 76, 5, 2, 1);
    if (pct > 0U) {
        OLED_FillRoundRect(7, 32, (int16_t)((pct * 74UL) / 100UL), 3, 1);
    }

    /* Distance & Calories */
    uint32_t dist_hm = (s_state.steps_count * 7UL) / 1000UL;
    uint32_t kcal    = (s_state.steps_count * 4UL) / 100UL;
    (void)snprintf(buf, sizeof(buf), "%lu.%lukm  %lukcal",
                   (unsigned long)(dist_hm / 10UL),
                   (unsigned long)(dist_hm % 10UL),
                   (unsigned long)kcal);
    OLED_DrawStr(10, 39, buf, 1, 1);
}

/* ============================================================================
 * SCREEN 4: NOTIFICATION SMARTPHONE ÉPURÉE
 * ==========================================================================*/
static void Render_ScreenNotif(void)
{
    OLED_DrawBitmap(8, 2, k_garmin_bubble_11x9, 11, 9, 1);
    OLED_DrawStr(24, 2, "MESSAGE", 1, 1);
    OLED_DrawLine(6, 12, 81, 12, 1);

    if (s_state.notif_unread == 0U) {
        OLED_DrawStr(16, 20, "AUCUN", 2, 1);
        OLED_DrawStr(12, 36, "MESSAGE", 1, 1);
    } else {
        OLED_DrawStr(8, 16, s_state.notif_title, 1, 1);
        OLED_DrawLine(8, 25, 79, 25, 1);

        char l1[14];
        char l2[14];
        strncpy(l1, s_state.notif_body, 13U); l1[13] = '\0';
        OLED_DrawStr(8, 28, l1, 1, 1);
        if (strlen(s_state.notif_body) > 13U) {
            strncpy(l2, &s_state.notif_body[13], 13U); l2[13] = '\0';
            OLED_DrawStr(8, 38, l2, 1, 1);
        }
    }
}

void SmileWatch_RenderCurrentScreen(void)
{
    OLED_Clear();

    if (!s_state.display_awake || s_state.survival_mode) {
        OLED_Flush(s_hi2c);
        return;
    }

    switch (s_state.current_screen) {
        case SW_SCREEN_HOME:     Render_ScreenHome();     break;
        case SW_SCREEN_HEART:    Render_ScreenHeart();    break;
        case SW_SCREEN_ACTIVITY: Render_ScreenActivity(); break;
        case SW_SCREEN_NOTIF:    Render_ScreenNotif();    break;
        default:                 Render_ScreenHome();     break;
    }

    OLED_Flush(s_hi2c);
}

void SmileWatch_Init(I2C_HandleTypeDef *hi2c)
{
    s_hi2c = hi2c;
    memset(&s_state, 0, sizeof(s_state));

    s_state.hours          = 10U;
    s_state.minutes        = 10U;
    s_state.seconds        = 30U;
    s_state.day            = 8U;
    s_state.month          = 11U;
    strncpy(s_state.weekday_str, "DIM", sizeof(s_state.weekday_str) - 1U);

    s_state.battery_pct    = 84U;
    s_state.battery_mv     = 4020U;
    s_state.qi_charging    = false;
    s_state.ble_connected  = true;
    s_state.survival_mode  = false;

    s_state.heart_bpm      = 72U;
    s_state.spo2_pct       = 98U;
    s_state.hr_measuring   = true;
    s_state.body_battery   = 78U;
    s_state.stress_score   = 24U;
    s_state.hr_min         = 48U;
    s_state.hr_max         = 126U;
    s_state.temp_deci_c    = 366; /* 36.6 C */
    s_state.steps_count    = 6420UL;
    s_state.steps_goal     = 10000UL;
    s_state.ambient_lux    = 250U;

    s_state.notif_unread   = 1U;
    strncpy(s_state.notif_title, "SMS - ALEXIS", sizeof(s_state.notif_title) - 1U);
    strncpy(s_state.notif_body,  "RDV FABLAB MARDI 13H30 OK", sizeof(s_state.notif_body) - 1U);

    s_state.current_screen = SW_SCREEN_HOME;
    s_state.display_awake  = true;

    HAL_GPIO_WritePin(TCA_EN_GPIO_Port, TCA_EN_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(HR_5V_EN_GPIO_Port, HR_5V_EN_Pin, GPIO_PIN_SET);

    Miyota_Init(&s_miyota);
    Miyota_SyncTargetTime(&s_miyota, s_state.hours, s_state.minutes, s_state.seconds);
    s_miyota.analog_pos_sec = s_miyota.target_pos_sec;

    OLED_Init(s_hi2c);
    SmileWatch_RenderCurrentScreen();
}

void SmileWatch_HandleButtonEvent(SW_ButtonEvent_t evt, uint32_t now_ms)
{
    if (evt == SW_BTN_EVT_NONE) return;

    s_state.last_activity_ms = now_ms;

    if (!s_state.display_awake && !s_state.survival_mode) {
        s_state.display_awake = true;
        OLED_PowerOn();
        OLED_Init(s_hi2c);
        SmileWatch_TriggerHaptic(now_ms, 35U);
        SmileWatch_RenderCurrentScreen();
        return;
    }

    switch (evt) {
        case SW_BTN_EVT_2H_SHORT:
            s_state.current_screen = (SW_ScreenId_t)((s_state.current_screen + 1U) % SW_SCREEN_COUNT);
            SmileWatch_TriggerHaptic(now_ms, 25U);
            break;

        case SW_BTN_EVT_2H_LONG:
            if (s_state.current_screen != SW_SCREEN_HOME) {
                s_state.current_screen = SW_SCREEN_HOME;
            } else {
                s_state.display_awake = false;
                OLED_PowerOff();
            }
            SmileWatch_TriggerHaptic(now_ms, 60U);
            break;

        case SW_BTN_EVT_4H_SHORT:
            SmileWatch_TriggerHaptic(now_ms, 30U);
            if (s_state.current_screen == SW_SCREEN_HEART) {
                s_state.hr_measuring = !s_state.hr_measuring;
                HAL_GPIO_WritePin(HR_5V_EN_GPIO_Port, HR_5V_EN_Pin,
                                  s_state.hr_measuring ? GPIO_PIN_SET : GPIO_PIN_RESET);
            } else if (s_state.current_screen == SW_SCREEN_NOTIF) {
                s_state.notif_unread = 0U;
            }
            break;

        case SW_BTN_EVT_4H_LONG:
            SmileWatch_TriggerHaptic(now_ms, 80U);
            if (s_state.current_screen == SW_SCREEN_ACTIVITY) {
                s_state.steps_count = 0UL;
            }
            break;

        case SW_BTN_EVT_BOTH_LONG:
            SmileWatch_TriggerHaptic(now_ms, 150U);
            s_miyota.analog_pos_sec = 0UL;
            s_miyota.target_pos_sec = 0UL;
            break;

        default:
            break;
    }

    SmileWatch_RenderCurrentScreen();
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
    if (!s_state.display_awake && !s_state.survival_mode) {
        s_state.display_awake = true;
        OLED_PowerOn();
        OLED_Init(s_hi2c);
    }
    SmileWatch_TriggerHaptic(now_ms, 180U);
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

    if ((now_ms - s_last_rtc_tick_ms) >= 1000UL) {
        s_last_rtc_tick_ms += 1000UL;
        s_state.seconds++;
        if (s_state.seconds >= 60U) {
            s_state.seconds = 0U;
            s_state.minutes++;
            if (s_state.minutes >= 60U) {
                s_state.minutes = 0U;
                s_state.hours = (uint8_t)((s_state.hours + 1U) % 24U);
            }
        }
        Miyota_SyncTargetTime(&s_miyota, s_state.hours, s_state.minutes, s_state.seconds);
    }

    Miyota_Task(&s_miyota, now_ms);

    if (s_state.display_awake && ((now_ms - s_state.last_activity_ms) >= SW_OLED_TIMEOUT_MS)) {
        s_state.display_awake = false;
        OLED_PowerOff();
    }

    if (s_state.display_awake && ((now_ms - s_last_render_ms) >= 100UL)) {
        s_last_render_ms = now_ms;
        s_state.ppg_phase = (uint8_t)((s_state.ppg_phase + 1U) % 22U);
        SmileWatch_RenderCurrentScreen();
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
