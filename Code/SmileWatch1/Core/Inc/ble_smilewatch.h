/**
  ******************************************************************************
  * @file           : ble_smilewatch.h
  * @brief          : SmileWatch Custom GATT Service & SIG Profiles (STM32WB55)
  ******************************************************************************
  */

#ifndef INC_BLE_SMILEWATCH_H_
#define INC_BLE_SMILEWATCH_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "smilewatch_ui.h"

/* 128-bit SmileWatch Custom Service UUID: 534D5730-0001-4C45-8000-00805F9B34FB */
#define SW_BLE_SVC_UUID_BASE   {0xFB,0x34,0x9B,0x5F,0x80,0x00,0x00,0x80,0x45,0x4C,0x01,0x00,0x30,0x57,0x4D,0x53}

typedef enum {
    SW_BLE_CHAR_CONFIG_RX = 0, /* Write: night, sedentary, cinema, look-to-wake, ICE */
    SW_BLE_CHAR_ALARM_RX,      /* Write: 3 programmable alarms */
    SW_BLE_CHAR_HISTORY_TX,    /* Notify/Read: 365-day FIFO & workout records */
    SW_BLE_CHAR_SYNC_ACK       /* Write: marks Web/Cloud backup done */
} SW_BleCharId_t;

void BLE_SmileWatch_Init(void);
void BLE_SmileWatch_OnCtsTimeUpdate(uint8_t hours, uint8_t minutes, uint8_t seconds,
                                    uint8_t day, uint8_t month, int8_t tz_half_h);
void BLE_SmileWatch_OnConfigWrite(const uint8_t *data, uint16_t len, uint32_t now_ms);
void BLE_SmileWatch_OnSyncAck(void);
uint16_t BLE_SmileWatch_BuildHistoryPacket(uint8_t *out_buf, uint16_t max_len);

#ifdef __cplusplus
}
#endif

#endif /* INC_BLE_SMILEWATCH_H_ */
