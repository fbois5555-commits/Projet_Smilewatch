/**
  ******************************************************************************
  * @file           : miyota_driver.c
  * @brief          : Bipolar Lavet Stepper Motor Driver for Modified Miyota 2035
  *                   Implements alternating polarity pulses + LOW-LOW Lenz braking
  ******************************************************************************
  */

#include "miyota_driver.h"

static void Miyota_ApplyElectroDynamicBrake(void)
{
    /* Both coil terminals shorted to GND (LOW-LOW): Lenz current damps rotor bounce */
    HAL_GPIO_WritePin(MIYOTA_HP_GPIO_Port, MIYOTA_HP_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(MIYOTA_HM_GPIO_Port, MIYOTA_HM_Pin, GPIO_PIN_RESET);
}

void Miyota_Init(Miyota_Handle_t *hm)
{
    if (hm == NULL) return;
    hm->next_polarity     = 0U;
    hm->pulse_active      = false;
    hm->pulse_start_ms    = 0U;
    hm->last_step_ms      = 0U;
    hm->analog_pos_sec    = 0U; /* Default 12:00:00 at assembly */
    hm->target_pos_sec    = 0U;
    hm->total_steps_count = 0U;

    Miyota_ApplyElectroDynamicBrake();
}

void Miyota_TriggerSingleStep(Miyota_Handle_t *hm, uint32_t now_ms)
{
    if ((hm == NULL) || hm->pulse_active) return;

    if (hm->next_polarity == 0U) {
        /* Even step: H+ = HIGH (3.3V), H- = LOW (0V) */
        HAL_GPIO_WritePin(MIYOTA_HP_GPIO_Port, MIYOTA_HP_Pin, GPIO_PIN_SET);
        HAL_GPIO_WritePin(MIYOTA_HM_GPIO_Port, MIYOTA_HM_Pin, GPIO_PIN_RESET);
        hm->next_polarity = 1U;
    } else {
        /* Odd step: H+ = LOW (0V), H- = HIGH (3.3V) */
        HAL_GPIO_WritePin(MIYOTA_HP_GPIO_Port, MIYOTA_HP_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(MIYOTA_HM_GPIO_Port, MIYOTA_HM_Pin, GPIO_PIN_SET);
        hm->next_polarity = 0U;
    }

    hm->pulse_active      = true;
    hm->pulse_start_ms    = now_ms;
    hm->last_step_ms      = now_ms;
    hm->analog_pos_sec    = (hm->analog_pos_sec + 1UL) % MIYOTA_SEC_PER_12H;
    hm->total_steps_count++;
}

void Miyota_Task(Miyota_Handle_t *hm, uint32_t now_ms)
{
    if (hm == NULL) return;

    /* 1. Cut impulse after MIYOTA_IMPULSE_MS (10 ms) and apply LOW-LOW Lenz brake */
    if (hm->pulse_active) {
        if ((now_ms - hm->pulse_start_ms) >= MIYOTA_IMPULSE_MS) {
            Miyota_ApplyElectroDynamicBrake();
            hm->pulse_active = false;
        }
        return;
    }

    /* 2. Fast-forward catch-up if physical hands lag behind RTC target time */
    if (hm->analog_pos_sec != hm->target_pos_sec) {
        if ((now_ms - hm->last_step_ms) >= MIYOTA_FAST_PERIOD_MS) {
            Miyota_TriggerSingleStep(hm, now_ms);
        }
    }
}

void Miyota_SetCalibrated12h00(Miyota_Handle_t *hm, uint32_t current_rtc_sec_12h)
{
    if (hm == NULL) return;
    hm->analog_pos_sec = 0UL; /* Hands confirmed at 12:00:00 */
    hm->target_pos_sec = current_rtc_sec_12h % MIYOTA_SEC_PER_12H;
}

void Miyota_SyncTargetTime(Miyota_Handle_t *hm, uint8_t hours, uint8_t minutes, uint8_t seconds)
{
    if (hm == NULL) return;
    uint32_t h12 = (uint32_t)(hours % 12U);
    hm->target_pos_sec = (h12 * 3600UL) + ((uint32_t)minutes * 60UL) + (uint32_t)seconds;
}
