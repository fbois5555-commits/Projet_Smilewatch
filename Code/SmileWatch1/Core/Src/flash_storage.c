/**
  ******************************************************************************
  * @file           : flash_storage.c
  * @brief          : Non-volatile Page-127 Flash storage on STM32WB5MMGH6
  ******************************************************************************
  */

#include "flash_storage.h"
#include "stm32wbxx_hal.h"
#include <string.h>

/* 8-byte aligned record serialized to doublewords */
typedef struct {
    uint32_t magic;
    uint32_t analog_pos_sec;
    uint32_t steps_goal;
    int16_t  temp_base_deci_c;
    uint16_t mlc_thresh_mg;
    uint16_t sport_sessions_done;
    uint8_t  sport_type;
    uint8_t  mlc_confidence_pct;
    uint8_t  stride_cm;
    uint8_t  vo2_max;
    uint8_t  recovery_hours;
    uint8_t  mlc_trees_used;
    uint8_t  split_6h_mode;
    uint8_t  wrist_nav_enabled;
    uint8_t  fall_sos_enabled;
    uint8_t  smart_wake_enabled;
    uint8_t  alarm_enabled;
    uint8_t  alarm_hour;
    uint8_t  alarm_min;
    uint8_t  history_count;
    uint8_t  history_head;
    uint8_t  pill_hand_pause_enabled;
    uint16_t hrv_base_ms;
    uint8_t  rhr_base_bpm;
    uint8_t  baseline_nights;
    int8_t   calib_error_pct;
    uint8_t  power_profile;
    uint8_t  auto_sport_enabled;
    uint8_t  night_mode_enabled;
    uint8_t  sedentary_alert_enabled;
    uint8_t  auto_brightness_enabled;
    uint8_t  look_to_wake_enabled;
    uint8_t  coach_enabled;
    uint8_t  target_hr_zone;
    uint16_t training_load_acute;
    uint16_t training_load_chronic;
    int8_t   tz2_offset_half_h;
    uint8_t  tz2_enabled;
    uint16_t daily_head;
    uint16_t daily_count;
    uint16_t daily_unsynced_count;
    uint8_t  last_sync_day;
    uint8_t  last_sync_month;
    uint8_t  last_sync_hour;
    uint8_t  last_sync_min;
    uint8_t  haptic_level;
    uint8_t  auto_gym_set_enabled;
    SW_AlarmItem_t alarms[SW_ALARMS_MAX];
    SW_DailyRecord_t daily_history[SW_DAILY_HIST_MAX];
    char     ice_name[12];
    char     ice_phone[16];
    SW_WorkoutRecord_t workout_history[SW_HISTORY_MAX];
    uint32_t checksum;
} SW_FlashPersist_t;

static uint32_t FlashStorage_ComputeChecksum(const SW_FlashPersist_t *rec)
{
    const uint8_t *bytes = (const uint8_t *)rec;
    uint32_t sum = 0x13572468UL;
    uint32_t len = (uint32_t)(sizeof(SW_FlashPersist_t) - sizeof(uint32_t));
    for (uint32_t i = 0UL; i < len; i++) {
        sum = (sum ^ (uint32_t)bytes[i]) * 16777619UL;
    }
    return sum;
}

bool FlashStorage_Load(SW_SystemState_t *state, Miyota_Handle_t *miyota)
{
    if ((state == NULL) || (miyota == NULL)) return false;

    SW_FlashPersist_t rec;
    memcpy(&rec, (const void *)SW_FLASH_PAGE_ADDR, sizeof(rec));
    if (rec.magic != SW_FLASH_MAGIC) {
        return false;
    }
    if (FlashStorage_ComputeChecksum(&rec) != rec.checksum) {
        return false;
    }

    miyota->analog_pos_sec     = rec.analog_pos_sec % MIYOTA_SEC_PER_12H;
    state->steps_goal          = (rec.steps_goal > 0UL) ? rec.steps_goal : 10000UL;
    state->temp_base_deci_c    = rec.temp_base_deci_c;
    state->mlc_thresh_mg       = rec.mlc_thresh_mg;
    state->sport_sessions_done = rec.sport_sessions_done;
    state->sport_type          = (uint8_t)(rec.sport_type % SW_SPORT_COUNT);
    state->mlc_confidence_pct  = rec.mlc_confidence_pct;
    state->stride_cm           = rec.stride_cm;
    state->vo2_max             = rec.vo2_max;
    state->recovery_hours      = rec.recovery_hours;
    state->mlc_trees_used      = rec.mlc_trees_used;
    state->split_6h_mode       = (uint8_t)(rec.split_6h_mode % 3U);
    state->wrist_nav_enabled       = (rec.wrist_nav_enabled != 0U);
    state->fall_sos_enabled        = (rec.fall_sos_enabled != 0U);
    state->smart_wake_enabled      = (rec.smart_wake_enabled != 0U);
    state->pill_hand_pause_enabled = (rec.pill_hand_pause_enabled != 0U);
    state->alarm_enabled       = (rec.alarm_enabled != 0U);
    state->alarm_hour          = (uint8_t)(rec.alarm_hour % 24U);
    state->alarm_min           = (uint8_t)(rec.alarm_min % 60U);

    state->hrv_base_ms         = (rec.hrv_base_ms < 20U) ? 20U :
                                 (rec.hrv_base_ms > 120U) ? 120U : rec.hrv_base_ms;
    state->rhr_base_bpm        = (rec.rhr_base_bpm < 42U) ? 42U :
                                 (rec.rhr_base_bpm > 80U) ? 80U : rec.rhr_base_bpm;
    state->baseline_nights     = rec.baseline_nights;
    state->calib_error_pct     = (rec.calib_error_pct < -50) ? -50 :
                                 (rec.calib_error_pct > 50) ? 50 : rec.calib_error_pct;
    state->power_profile       = (uint8_t)(rec.power_profile % 4U);
    state->auto_sport_enabled  = (rec.auto_sport_enabled != 0U);
    state->night_mode_enabled  = (rec.night_mode_enabled != 0U);
    state->sedentary_alert_enabled = (rec.sedentary_alert_enabled != 0U);
    state->auto_brightness_enabled = (rec.auto_brightness_enabled != 0U);
    state->look_to_wake_enabled    = (rec.look_to_wake_enabled != 0U);
    state->coach_enabled           = (rec.coach_enabled != 0U);
    state->target_hr_zone          = (rec.target_hr_zone >= 1U && rec.target_hr_zone <= 5U) ? rec.target_hr_zone : 3U;
    state->training_load_acute     = rec.training_load_acute;
    state->training_load_chronic   = rec.training_load_chronic;
    state->tz2_offset_half_h       = rec.tz2_offset_half_h;
    state->tz2_enabled             = (rec.tz2_enabled != 0U);
    if (rec.daily_count <= SW_DAILY_HIST_MAX) {
        state->daily_count          = rec.daily_count;
        state->daily_head           = (uint16_t)(rec.daily_head % SW_DAILY_RING_CAP);
        state->daily_unsynced_count = (rec.daily_unsynced_count <= SW_DAILY_HIST_MAX) ? rec.daily_unsynced_count : rec.daily_count;
        state->sync_needed_alert    = (state->daily_unsynced_count >= 295U);
        state->last_sync_day        = rec.last_sync_day;
        state->last_sync_month      = rec.last_sync_month;
        state->last_sync_hour       = rec.last_sync_hour;
        state->last_sync_min        = rec.last_sync_min;
        state->haptic_level         = (rec.haptic_level <= 2U) ? rec.haptic_level : 1U;
        state->auto_gym_set_enabled = (rec.auto_gym_set_enabled != 0U);
        memcpy(state->alarms, rec.alarms, sizeof(state->alarms));
        memcpy(state->daily_history, rec.daily_history, sizeof(state->daily_history));
    }
    if (rec.ice_name[0] != '\0') {
        strncpy(state->ice_name, rec.ice_name, sizeof(state->ice_name) - 1U);
        state->ice_name[sizeof(state->ice_name) - 1U] = '\0';
    }
    if (rec.ice_phone[0] != '\0') {
        strncpy(state->ice_phone, rec.ice_phone, sizeof(state->ice_phone) - 1U);
        state->ice_phone[sizeof(state->ice_phone) - 1U] = '\0';
    }

    if (rec.history_count <= SW_HISTORY_MAX) {
        state->history_count = rec.history_count;
        state->history_head  = (uint8_t)(rec.history_head % SW_HISTORY_MAX);
        memcpy(state->workout_history, rec.workout_history, sizeof(state->workout_history));
    }
    return true;
}

bool FlashStorage_Save(const SW_SystemState_t *state, const Miyota_Handle_t *miyota)
{
    if ((state == NULL) || (miyota == NULL)) return false;

    SW_FlashPersist_t rec;
    memset(&rec, 0, sizeof(rec));
    rec.magic               = SW_FLASH_MAGIC;
    rec.analog_pos_sec      = miyota->analog_pos_sec;
    rec.steps_goal          = state->steps_goal;
    rec.temp_base_deci_c    = state->temp_base_deci_c;
    rec.mlc_thresh_mg       = state->mlc_thresh_mg;
    rec.sport_sessions_done = state->sport_sessions_done;
    rec.sport_type          = state->sport_type;
    rec.mlc_confidence_pct  = state->mlc_confidence_pct;
    rec.stride_cm           = state->stride_cm;
    rec.vo2_max             = state->vo2_max;
    rec.recovery_hours      = state->recovery_hours;
    rec.mlc_trees_used      = state->mlc_trees_used;
    rec.split_6h_mode       = state->split_6h_mode;
    rec.wrist_nav_enabled       = state->wrist_nav_enabled ? 1U : 0U;
    rec.fall_sos_enabled        = state->fall_sos_enabled ? 1U : 0U;
    rec.smart_wake_enabled      = state->smart_wake_enabled ? 1U : 0U;
    rec.pill_hand_pause_enabled = state->pill_hand_pause_enabled ? 1U : 0U;
    rec.alarm_enabled       = state->alarm_enabled ? 1U : 0U;
    rec.alarm_hour          = state->alarm_hour;
    rec.alarm_min           = state->alarm_min;
    rec.hrv_base_ms         = state->hrv_base_ms;
    rec.rhr_base_bpm        = state->rhr_base_bpm;
    rec.baseline_nights     = state->baseline_nights;
    rec.calib_error_pct     = state->calib_error_pct;
    rec.power_profile       = state->power_profile;
    rec.auto_sport_enabled  = state->auto_sport_enabled ? 1U : 0U;
    rec.night_mode_enabled  = state->night_mode_enabled ? 1U : 0U;
    rec.sedentary_alert_enabled = state->sedentary_alert_enabled ? 1U : 0U;
    rec.auto_brightness_enabled = state->auto_brightness_enabled ? 1U : 0U;
    rec.look_to_wake_enabled    = state->look_to_wake_enabled ? 1U : 0U;
    rec.coach_enabled           = state->coach_enabled ? 1U : 0U;
    rec.target_hr_zone          = state->target_hr_zone;
    rec.training_load_acute     = state->training_load_acute;
    rec.training_load_chronic   = state->training_load_chronic;
    rec.tz2_offset_half_h       = state->tz2_offset_half_h;
    rec.tz2_enabled             = state->tz2_enabled ? 1U : 0U;
    rec.daily_head              = state->daily_head;
    rec.daily_count             = state->daily_count;
    rec.daily_unsynced_count    = state->daily_unsynced_count;
    rec.last_sync_day           = state->last_sync_day;
    rec.last_sync_month         = state->last_sync_month;
    rec.last_sync_hour          = state->last_sync_hour;
    rec.last_sync_min           = state->last_sync_min;
    rec.haptic_level            = state->haptic_level;
    rec.auto_gym_set_enabled    = state->auto_gym_set_enabled ? 1U : 0U;
    memcpy(rec.alarms, state->alarms, sizeof(rec.alarms));
    memcpy(rec.daily_history, state->daily_history, sizeof(rec.daily_history));
    strncpy(rec.ice_name, state->ice_name, sizeof(rec.ice_name) - 1U);
    strncpy(rec.ice_phone, state->ice_phone, sizeof(rec.ice_phone) - 1U);
    rec.history_count       = state->history_count;
    rec.history_head        = state->history_head;
    memcpy(rec.workout_history, state->workout_history, sizeof(rec.workout_history));
    rec.checksum            = FlashStorage_ComputeChecksum(&rec);

    if (HAL_FLASH_Unlock() != HAL_OK) {
        return false;
    }

    FLASH_EraseInitTypeDef erase = {0};
    uint32_t page_error = 0UL;
    erase.TypeErase = FLASH_TYPEERASE_PAGES;
    erase.Page      = SW_FLASH_PAGE_INDEX;
    erase.NbPages   = 1U;

    if (HAL_FLASHEx_Erase(&erase, &page_error) != HAL_OK) {
        (void)HAL_FLASH_Lock();
        return false;
    }

    const uint8_t *raw = (const uint8_t *)&rec;
    uint32_t total = (uint32_t)sizeof(rec);
    bool ok = true;
    for (uint32_t offset = 0UL; offset < total; offset += 8UL) {
        uint64_t dword = 0xFFFFFFFFFFFFFFFFULL;
        uint32_t chunk = ((total - offset) >= 8UL) ? 8UL : (total - offset);
        memcpy(&dword, &raw[offset], chunk);
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, SW_FLASH_PAGE_ADDR + offset, dword) != HAL_OK) {
            ok = false;
            break;
        }
    }

    (void)HAL_FLASH_Lock();
    return ok;
}
