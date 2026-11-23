/**
  ******************************************************************************
  * @file           : ble_smilewatch.c
  * @brief          : SmileWatch GATT Handlers for STM32WB55 STM32_WPAN Stack
  ******************************************************************************
  */

#include "ble_smilewatch.h"
#include <string.h>

void BLE_SmileWatch_Init(void)
{
    /* Register 128-bit GATT service & characteristics in STM32_WPAN aci_gatt_add_service() */
}

void BLE_SmileWatch_OnCtsTimeUpdate(uint8_t hours, uint8_t minutes, uint8_t seconds,
                                    uint8_t day, uint8_t month, int8_t tz_half_h)
{
    SW_SystemState_t *st = SmileWatch_GetState();
    if (st == NULL) return;
    st->hours             = (uint8_t)(hours % 24U);
    st->minutes           = (uint8_t)(minutes % 60U);
    st->seconds           = (uint8_t)(seconds % 60U);
    st->day               = (day >= 1U && day <= 31U) ? day : st->day;
    st->month             = (month >= 1U && month <= 12U) ? month : st->month;
    st->tz2_offset_half_h = tz_half_h;
    st->ble_connected     = true;
}

void BLE_SmileWatch_OnConfigWrite(const uint8_t *data, uint16_t len, uint32_t now_ms)
{
    SW_SystemState_t *st = SmileWatch_GetState();
    if ((st == NULL) || (data == NULL) || (len < 4U)) return;
    st->night_mode_enabled      = (data[0] != 0U);
    st->sedentary_alert_enabled = (data[1] != 0U);
    st->look_to_wake_enabled    = (data[2] != 0U);
    if (data[3] != 0U) {
        SmileWatch_SetCinemaMode(true, 120U, now_ms);
    } else {
        SmileWatch_SetCinemaMode(false, 0U, now_ms);
    }
}

void BLE_SmileWatch_OnSyncAck(void)
{
    SmileWatch_MarkWebSynced();
}

uint16_t BLE_SmileWatch_BuildHistoryPacket(uint8_t *out_buf, uint16_t max_len)
{
    const SW_SystemState_t *st = SmileWatch_GetState();
    if ((st == NULL) || (out_buf == NULL) || (max_len < 6U)) return 0U;
    out_buf[0] = (uint8_t)(st->daily_unsynced_count & 0xFFU);
    out_buf[1] = (uint8_t)((st->daily_unsynced_count >> 8U) & 0xFFU);
    out_buf[2] = st->last_sync_day;
    out_buf[3] = st->last_sync_month;
    out_buf[4] = st->last_sync_hour;
    out_buf[5] = st->last_sync_min;
    return 6U;
}
