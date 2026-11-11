/**
  ******************************************************************************
  * @file           : miyota_driver.h
  * @brief          : Bipolar Lavet Stepper Motor Driver for Modified Miyota 2035
  *                   Pins: PA7 (TP6 H+) and PB5 (TP7 H-) on Carte_V2
  ******************************************************************************
  */

#ifndef INC_MIYOTA_DRIVER_H_
#define INC_MIYOTA_DRIVER_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "main.h"

/* Calibrated impulse width for 3.3V GPIO drive (10 ms + LOW-LOW Lenz braking) */
#define MIYOTA_IMPULSE_MS           10U
#define MIYOTA_FAST_PERIOD_MS       45U   /* ~22 Hz max fast-forward catchup rate */
#define MIYOTA_SEC_PER_12H          43200UL

typedef struct {
    uint8_t  next_polarity;      /* Alternates 0 (H+ > H-) and 1 (H- > H+) */
    bool     pulse_active;       /* True during 10 ms active drive window */
    uint32_t pulse_start_ms;     /* Tick when current impulse started */
    uint32_t last_step_ms;       /* Tick of last completed step */
    uint32_t analog_pos_sec;     /* Physical hand position in seconds [0..43199] (12h) */
    uint32_t target_pos_sec;     /* Target RTC position in seconds [0..43199] (12h) */
    uint32_t total_steps_count;  /* Cumulative steps since boot */
} Miyota_Handle_t;

void Miyota_Init(Miyota_Handle_t *hm);
void Miyota_TriggerSingleStep(Miyota_Handle_t *hm, uint32_t now_ms);
void Miyota_Task(Miyota_Handle_t *hm, uint32_t now_ms);
void Miyota_SetCalibrated12h00(Miyota_Handle_t *hm, uint32_t current_rtc_sec_12h);
void Miyota_SyncTargetTime(Miyota_Handle_t *hm, uint8_t hours, uint8_t minutes, uint8_t seconds);

#ifdef __cplusplus
}
#endif

#endif /* INC_MIYOTA_DRIVER_H_ */
