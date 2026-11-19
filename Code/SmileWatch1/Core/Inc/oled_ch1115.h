/**
  ******************************************************************************
  * @file           : oled_ch1115.h
  * @brief          : Driver for Topwin QG-8848TSWYG02 0.5" PMOLED (88x48 px)
  *                   Controller: Sino Wealth CH1115 (I2C 0x3C)
  *                   Hardware: SmileWatch V2 (STM32WB55VGYx WLCSP-100)
  ******************************************************************************
  */

#ifndef INC_OLED_CH1115_H_
#define INC_OLED_CH1115_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "main.h"

/* Physical Display Dimensions (Topwin QG-8848TSWYG02) */
#define OLED_WIDTH              88U
#define OLED_HEIGHT             48U
#define OLED_PAGES              (OLED_HEIGHT / 8U)   /* 6 pages of 8 vertical bits */
#define OLED_VRAM_SIZE          (OLED_WIDTH * OLED_PAGES) /* 528 bytes */

/* CH1115 column offset to center 88 columns inside 128-column internal RAM */
#define OLED_COL_OFFSET         20U
#define OLED_I2C_ADDR_7B        0x3CU
#define OLED_I2C_ADDR           (OLED_I2C_ADDR_7B << 1U)

/* Built-in 8x8 Icons for SmileWatch UI */
typedef enum {
    OLED_ICON_HEART = 0,
    OLED_ICON_THERMO,
    OLED_ICON_SHOE,
    OLED_ICON_BELL,
    OLED_ICON_BLE,
    OLED_ICON_BOLT,
    OLED_ICON_WATCH,
    OLED_ICON_COUNT
} OLED_IconId_t;

/* Public API */
void OLED_PowerOn(void);
void OLED_PowerOff(void);
void OLED_Init(I2C_HandleTypeDef *hi2c);
void OLED_SetContrast(I2C_HandleTypeDef *hi2c, uint8_t contrast);
void OLED_Clear(void);
void OLED_Flush(I2C_HandleTypeDef *hi2c);

/* 1-bit Framebuffer Drawing Primitives (identical in C firmware and Web Simulator) */
void OLED_DrawPixel(int16_t x, int16_t y, uint8_t color);
void OLED_DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color);
void OLED_DrawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color);
void OLED_FillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color);
void OLED_DrawCircle(int16_t x0, int16_t y0, int16_t r, uint8_t color);
void OLED_FillCircle(int16_t x0, int16_t y0, int16_t r, uint8_t color);
void OLED_DrawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint8_t color);
void OLED_FillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint8_t color);
void OLED_DrawBitmap(int16_t x, int16_t y, const uint8_t *bmp, uint8_t w, uint8_t h, uint8_t color);
void OLED_DrawIcon(int16_t x, int16_t y, OLED_IconId_t icon_id, uint8_t color);
void OLED_DrawChar(int16_t x, int16_t y, char c, uint8_t scale, uint8_t color);
void OLED_DrawStr(int16_t x, int16_t y, const char *str, uint8_t scale, uint8_t color);

/* Access raw 528-byte page buffer [6][88] */
const uint8_t* OLED_GetFramebuffer(void);
bool OLED_IsPoweredOn(void);
uint16_t OLED_CountLitPixels(void);

#ifdef __cplusplus
}
#endif

#endif /* INC_OLED_CH1115_H_ */
