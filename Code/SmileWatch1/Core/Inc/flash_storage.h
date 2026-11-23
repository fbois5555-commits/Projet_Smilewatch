/**
  ******************************************************************************
  * @file           : flash_storage.h
  * @brief          : Non-volatile Flash persistence (Page 127, 0x0807F000) for
  *                   STM32WB5MMGH6 (settings, MLC weights, Miyota pos & history)
  ******************************************************************************
  */

#ifndef INC_FLASH_STORAGE_H_
#define INC_FLASH_STORAGE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "smilewatch_ui.h"

#define SW_FLASH_MAGIC        0x53575642UL /* "SWVB" */
#define SW_FLASH_PAGE_INDEX   127U         /* Last 4 KB page of 512 KB App Flash */
#define SW_FLASH_PAGE_ADDR    0x0807F000UL

bool FlashStorage_Load(SW_SystemState_t *state, Miyota_Handle_t *miyota);
bool FlashStorage_Save(const SW_SystemState_t *state, const Miyota_Handle_t *miyota);

#ifdef __cplusplus
}
#endif

#endif /* INC_FLASH_STORAGE_H_ */
