/**
  ******************************************************************************
  * @file           : oled_ch1115.c
  * @brief          : Driver & 1-bit Graphics Engine for Topwin QG-8848TSWYG02
  *                   (CH1115 controller, 88x48 pixels, I2C1 on PB8/PB9)
  ******************************************************************************
  */

#include "oled_ch1115.h"
#include <string.h>
#include <stdlib.h>
#include <math.h>

/* 528-byte 1-bit page-organized VRAM: 6 pages x 88 columns */
static uint8_t s_vram[OLED_PAGES][OLED_WIDTH];
static bool s_oled_powered = false;

/* Built-in 8x8 1-bit icons (row-major, MSB left) */
static const uint8_t k_icons_8x8[OLED_ICON_COUNT][8] = {
    /* OLED_ICON_HEART */
    { 0x00, 0x66, 0xFF, 0xFF, 0xFF, 0x7E, 0x3C, 0x18 },
    /* OLED_ICON_THERMO */
    { 0x18, 0x24, 0x24, 0x24, 0x24, 0x7E, 0x7E, 0x3C },
    /* OLED_ICON_SHOE */
    { 0x00, 0x30, 0x78, 0x7C, 0xFE, 0xFF, 0xFF, 0x00 },
    /* OLED_ICON_BELL */
    { 0x18, 0x3C, 0x3C, 0x3C, 0x7E, 0xFF, 0x18, 0x00 },
    /* OLED_ICON_BLE */
    { 0x18, 0x54, 0x38, 0x10, 0x38, 0x54, 0x18, 0x00 },
    /* OLED_ICON_BOLT */
    { 0x0C, 0x18, 0x30, 0x7E, 0x18, 0x30, 0x60, 0x00 },
    /* OLED_ICON_WATCH */
    { 0x3C, 0x42, 0x99, 0xA1, 0xA1, 0x99, 0x42, 0x3C }
};

/* Compact 5x7 ASCII Font (0x20 Space to 0x5A 'Z') */
static const uint8_t k_font5x7[][5] = {
    {0x00,0x00,0x00,0x00,0x00}, /* 0x20 ' ' */
    {0x00,0x00,0x5F,0x00,0x00}, /* 0x21 '!' */
    {0x00,0x07,0x00,0x07,0x00}, /* 0x22 '"' */
    {0x14,0x7F,0x14,0x7F,0x14}, /* 0x23 '#' */
    {0x24,0x2A,0x7F,0x2A,0x12}, /* 0x24 '$' */
    {0x23,0x13,0x08,0x64,0x62}, /* 0x25 '%' */
    {0x36,0x49,0x55,0x22,0x50}, /* 0x26 '&' */
    {0x00,0x05,0x03,0x00,0x00}, /* 0x27 ''' */
    {0x00,0x1C,0x22,0x41,0x00}, /* 0x28 '(' */
    {0x00,0x41,0x22,0x1C,0x00}, /* 0x29 ')' */
    {0x14,0x08,0x3E,0x08,0x14}, /* 0x2A '*' */
    {0x08,0x08,0x3E,0x08,0x08}, /* 0x2B '+' */
    {0x00,0x50,0x30,0x00,0x00}, /* 0x2C ',' */
    {0x08,0x08,0x08,0x08,0x08}, /* 0x2D '-' */
    {0x00,0x60,0x60,0x00,0x00}, /* 0x2E '.' */
    {0x20,0x10,0x08,0x04,0x02}, /* 0x2F '/' */
    {0x3E,0x51,0x49,0x45,0x3E}, /* 0x30 '0' */
    {0x00,0x42,0x7F,0x40,0x00}, /* 0x31 '1' */
    {0x42,0x61,0x51,0x49,0x46}, /* 0x32 '2' */
    {0x21,0x41,0x45,0x4B,0x31}, /* 0x33 '3' */
    {0x18,0x14,0x12,0x7F,0x10}, /* 0x34 '4' */
    {0x27,0x45,0x45,0x45,0x39}, /* 0x35 '5' */
    {0x3C,0x4A,0x49,0x49,0x30}, /* 0x36 '6' */
    {0x01,0x71,0x09,0x05,0x03}, /* 0x37 '7' */
    {0x36,0x49,0x49,0x49,0x36}, /* 0x38 '8' */
    {0x06,0x49,0x49,0x29,0x1E}, /* 0x39 '9' */
    {0x00,0x36,0x36,0x00,0x00}, /* 0x3A ':' */
    {0x00,0x56,0x36,0x00,0x00}, /* 0x3B ';' */
    {0x08,0x14,0x22,0x41,0x00}, /* 0x3C '<' */
    {0x14,0x14,0x14,0x14,0x14}, /* 0x3D '=' */
    {0x00,0x41,0x22,0x14,0x08}, /* 0x3E '>' */
    {0x02,0x01,0x51,0x09,0x06}, /* 0x3F '?' */
    {0x32,0x49,0x79,0x41,0x3E}, /* 0x40 '@' */
    {0x7E,0x11,0x11,0x11,0x7E}, /* 0x41 'A' */
    {0x7F,0x49,0x49,0x49,0x36}, /* 0x42 'B' */
    {0x3E,0x41,0x41,0x41,0x22}, /* 0x43 'C' */
    {0x7F,0x41,0x41,0x22,0x1C}, /* 0x44 'D' */
    {0x7F,0x49,0x49,0x49,0x41}, /* 0x45 'E' */
    {0x7F,0x09,0x09,0x09,0x01}, /* 0x46 'F' */
    {0x3E,0x41,0x49,0x49,0x7A}, /* 0x47 'G' */
    {0x7F,0x08,0x08,0x08,0x7F}, /* 0x48 'H' */
    {0x00,0x41,0x7F,0x41,0x00}, /* 0x49 'I' */
    {0x20,0x40,0x41,0x3F,0x01}, /* 0x4A 'J' */
    {0x7F,0x08,0x14,0x22,0x41}, /* 0x4B 'K' */
    {0x7F,0x40,0x40,0x40,0x40}, /* 0x4C 'L' */
    {0x7F,0x02,0x0C,0x02,0x7F}, /* 0x4D 'M' */
    {0x7F,0x04,0x08,0x10,0x7F}, /* 0x4E 'N' */
    {0x3E,0x41,0x41,0x41,0x3E}, /* 0x4F 'O' */
    {0x7F,0x09,0x09,0x09,0x06}, /* 0x50 'P' */
    {0x3E,0x41,0x51,0x21,0x5E}, /* 0x51 'Q' */
    {0x7F,0x09,0x19,0x29,0x46}, /* 0x52 'R' */
    {0x46,0x49,0x49,0x49,0x31}, /* 0x53 'S' */
    {0x01,0x01,0x7F,0x01,0x01}, /* 0x54 'T' */
    {0x3F,0x40,0x40,0x40,0x3F}, /* 0x55 'U' */
    {0x1F,0x20,0x40,0x20,0x1F}, /* 0x56 'V' */
    {0x3F,0x40,0x38,0x40,0x3F}, /* 0x57 'W' */
    {0x63,0x14,0x08,0x14,0x63}, /* 0x58 'X' */
    {0x07,0x08,0x70,0x08,0x07}, /* 0x59 'Y' */
    {0x61,0x51,0x49,0x45,0x43}  /* 0x5A 'Z' */
};

static void OLED_WriteCmd(I2C_HandleTypeDef *hi2c, uint8_t cmd)
{
    if (hi2c == NULL) return;
    uint8_t buf[2] = { 0x00U, cmd }; /* Co=0, D/C#=0 */
    (void)HAL_I2C_Master_Transmit(hi2c, OLED_I2C_ADDR, buf, 2U, 10U);
}

void OLED_PowerOn(void)
{
    /* Enable Q2/Q1 high-side switch (/Ecran/GPIO_EC on PD7) */
    HAL_GPIO_WritePin(OLED_PWR_EN_GPIO_Port, OLED_PWR_EN_Pin, GPIO_PIN_SET);
    HAL_Delay(5U);
    /* Hardware Reset pulse on /Ecran/RES (PB4) */
    HAL_GPIO_WritePin(OLED_RES_GPIO_Port, OLED_RES_Pin, GPIO_PIN_RESET);
    HAL_Delay(10U);
    HAL_GPIO_WritePin(OLED_RES_GPIO_Port, OLED_RES_Pin, GPIO_PIN_SET);
    HAL_Delay(10U);
    s_oled_powered = true;
}

void OLED_PowerOff(void)
{
    /* Cut VBAT supply via Q1/Q2 to prevent sleep leakage (Topwin datasheet p. 10) */
    HAL_GPIO_WritePin(OLED_RES_GPIO_Port, OLED_RES_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(OLED_PWR_EN_GPIO_Port, OLED_PWR_EN_Pin, GPIO_PIN_RESET);
    s_oled_powered = false;
}

void OLED_Init(I2C_HandleTypeDef *hi2c)
{
    OLED_PowerOn();

    /* Topwin QG-8848TSWYG02 (CH1115) Official Initialization Sequence (Datasheet p. 12) */
    static const uint8_t init_cmds[] = {
        0xAEU,       /* Display OFF */
        0x00U, 0x10U,/* Lower / Higher column address */
        0x40U,       /* Display start line = 0 */
        0xB0U,       /* Page address = 0 */
        0x81U, 0x80U,/* Contrast control (default 0x80) */
        0x82U, 0x00U,/* IREF resistor ratio */
        0xA1U,       /* Segment remap (A0/A1) */
        0xA2U,       /* Normal display */
        0xA4U,       /* Entire display ON from RAM */
        0xA6U,       /* Normal (non-inverted) display */
        0xA8U, 0x2FU,/* Multiplex ratio = 48 - 1 (0x2F) */
        0xC8U,       /* COM output scan direction (C0/C8) */
        0xD3U, 0x08U,/* Display offset */
        0xD5U, 0xF0U,/* Oscillator frequency & divide ratio */
        0xD9U, 0x22U,/* Pre-charge period */
        0xDAU, 0x12U,/* COM pins hardware configuration */
        0xDBU, 0x40U,/* VCOMH deselect level */
        0xADU, 0x8BU,/* CH1115 DC-DC Charge Pump Enable (7.5V internal) */
        0x32U,       /* Pump voltage 7.4V..8.0V */
        0xAFU        /* Display ON */
    };

    for (uint32_t i = 0U; i < sizeof(init_cmds); i++) {
        OLED_WriteCmd(hi2c, init_cmds[i]);
    }

    OLED_Clear();
    OLED_Flush(hi2c);
}

void OLED_SetContrast(I2C_HandleTypeDef *hi2c, uint8_t contrast)
{
    OLED_WriteCmd(hi2c, 0x81U);
    OLED_WriteCmd(hi2c, contrast);
}

void OLED_Clear(void)
{
    memset(s_vram, 0, sizeof(s_vram));
}

void OLED_Flush(I2C_HandleTypeDef *hi2c)
{
    if ((hi2c == NULL) || (!s_oled_powered)) return;

    uint8_t tx_buf[1U + OLED_WIDTH];
    tx_buf[0] = 0x40U; /* Co=0, D/C#=1 (RAM Data stream) */

    for (uint8_t page = 0U; page < OLED_PAGES; page++) {
        uint8_t col = OLED_COL_OFFSET;
        OLED_WriteCmd(hi2c, (uint8_t)(0xB0U | page));
        OLED_WriteCmd(hi2c, (uint8_t)(0x00U | (col & 0x0FU)));
        OLED_WriteCmd(hi2c, (uint8_t)(0x10U | ((col >> 4U) & 0x0FU)));

        memcpy(&tx_buf[1], s_vram[page], OLED_WIDTH);
        (void)HAL_I2C_Master_Transmit(hi2c, OLED_I2C_ADDR, tx_buf, sizeof(tx_buf), 20U);
    }
}

void OLED_DrawPixel(int16_t x, int16_t y, uint8_t color)
{
    if ((x < 0) || (x >= (int16_t)OLED_WIDTH) || (y < 0) || (y >= (int16_t)OLED_HEIGHT)) {
        return;
    }
    uint8_t page = (uint8_t)(y >> 3);
    uint8_t bit  = (uint8_t)(1U << (y & 0x07));
    if (color != 0U) {
        s_vram[page][x] |= bit;
    } else {
        s_vram[page][x] &= (uint8_t)(~bit);
    }
}

void OLED_DrawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color)
{
    int16_t dx = (int16_t)abs(x1 - x0);
    int16_t sx = (x0 < x1) ? 1 : -1;
    int16_t dy = (int16_t)(-abs(y1 - y0));
    int16_t sy = (y0 < y1) ? 1 : -1;
    int16_t err = dx + dy;

    while (1) {
        OLED_DrawPixel(x0, y0, color);
        if ((x0 == x1) && (y0 == y1)) break;
        int16_t e2 = (int16_t)(2 * err);
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

void OLED_DrawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color)
{
    if ((w <= 0) || (h <= 0)) return;
    OLED_DrawLine(x, y, x + w - 1, y, color);
    OLED_DrawLine(x, y + h - 1, x + w - 1, y + h - 1, color);
    OLED_DrawLine(x, y, x, y + h - 1, color);
    OLED_DrawLine(x + w - 1, y, x + w - 1, y + h - 1, color);
}

void OLED_FillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color)
{
    for (int16_t j = y; j < (y + h); j++) {
        for (int16_t i = x; i < (x + w); i++) {
            OLED_DrawPixel(i, j, color);
        }
    }
}

void OLED_DrawCircle(int16_t x0, int16_t y0, int16_t r, uint8_t color)
{
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    OLED_DrawPixel(x0, y0 + r, color);
    OLED_DrawPixel(x0, y0 - r, color);
    OLED_DrawPixel(x0 + r, y0, color);
    OLED_DrawPixel(x0 - r, y0, color);

    while (x < y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;
        OLED_DrawPixel(x0 + x, y0 + y, color);
        OLED_DrawPixel(x0 - x, y0 + y, color);
        OLED_DrawPixel(x0 + x, y0 - y, color);
        OLED_DrawPixel(x0 - x, y0 - y, color);
        OLED_DrawPixel(x0 + y, y0 + x, color);
        OLED_DrawPixel(x0 - y, y0 + x, color);
        OLED_DrawPixel(x0 + y, y0 - x, color);
        OLED_DrawPixel(x0 - y, y0 - x, color);
    }
}

void OLED_FillCircle(int16_t x0, int16_t y0, int16_t r, uint8_t color)
{
    for (int16_t y = -r; y <= r; y++) {
        for (int16_t x = -r; x <= r; x++) {
            if ((x * x + y * y) <= (r * r)) {
                OLED_DrawPixel(x0 + x, y0 + y, color);
            }
        }
    }
}

void OLED_DrawRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint8_t color)
{
    if ((w <= 0) || (h <= 0)) return;
    if (r <= 0) { OLED_DrawRect(x, y, w, h, color); return; }
    if (r * 2 > w) r = w / 2;
    if (r * 2 > h) r = h / 2;

    OLED_DrawLine(x + r, y, x + w - r - 1, y, color);
    OLED_DrawLine(x + r, y + h - 1, x + w - r - 1, y + h - 1, color);
    OLED_DrawLine(x, y + r, x, y + h - r - 1, color);
    OLED_DrawLine(x + w - 1, y + r, x + w - 1, y + h - r - 1, color);

    for (int16_t i = 0; i <= r; i++) {
        int16_t dy = (int16_t)(sqrtf((float)(r * r - i * i)) + 0.5f);
        OLED_DrawPixel(x + r - i, y + r - dy, color);
        OLED_DrawPixel(x + w - 1 - r + i, y + r - dy, color);
        OLED_DrawPixel(x + r - i, y + h - 1 - r + dy, color);
        OLED_DrawPixel(x + w - 1 - r + i, y + h - 1 - r + dy, color);
    }
}

void OLED_FillRoundRect(int16_t x, int16_t y, int16_t w, int16_t h, int16_t r, uint8_t color)
{
    if ((w <= 0) || (h <= 0)) return;
    if (r <= 0) { OLED_FillRect(x, y, w, h, color); return; }
    if (r * 2 > w) r = w / 2;
    if (r * 2 > h) r = h / 2;

    OLED_FillRect(x + r, y, w - 2 * r, h, color);
    for (int16_t dy = 0; dy <= r; dy++) {
        int16_t dx = (int16_t)(sqrtf((float)(r * r - (r - dy) * (r - dy))) + 0.5f);
        OLED_DrawLine(x + r - dx, y + dy, x + r - 1, y + dy, color);
        OLED_DrawLine(x + w - 1 - r + 1, y + dy, x + w - 1 - r + dx, y + dy, color);
        OLED_DrawLine(x + r - dx, y + h - 1 - dy, x + r - 1, y + h - 1 - dy, color);
        OLED_DrawLine(x + w - 1 - r + 1, y + h - 1 - dy, x + w - 1 - r + dx, y + h - 1 - dy, color);
    }
}

void OLED_DrawBitmap(int16_t x, int16_t y, const uint8_t *bmp, uint8_t w, uint8_t h, uint8_t color)
{
    if (bmp == NULL) return;
    uint8_t byte_width = (uint8_t)((w + 7U) / 8U);
    for (uint8_t j = 0U; j < h; j++) {
        for (uint8_t i = 0U; i < w; i++) {
            uint8_t b = bmp[j * byte_width + (i >> 3U)];
            if ((b & (uint8_t)(0x80U >> (i & 7U))) != 0U) {
                OLED_DrawPixel(x + (int16_t)i, y + (int16_t)j, color);
            }
        }
    }
}

void OLED_DrawIcon(int16_t x, int16_t y, OLED_IconId_t icon_id, uint8_t color)
{
    if (icon_id >= OLED_ICON_COUNT) return;
    OLED_DrawBitmap(x, y, k_icons_8x8[icon_id], 8U, 8U, color);
}

static uint8_t OLED_GlyphPx(const uint8_t *glyph, int8_t col, int8_t row)
{
    if ((col < 0) || (col >= 5) || (row < 0) || (row >= 7)) {
        return 0U;
    }
    return ((glyph[col] & (uint8_t)(1U << (uint8_t)row)) != 0U) ? 1U : 0U;
}

void OLED_DrawChar(int16_t x, int16_t y, char c, uint8_t scale, uint8_t color)
{
    if (scale == 0U) scale = 1U;
    if ((c >= 'a') && (c <= 'z')) {
        c = (char)(c - ('a' - 'A'));
    }
    if ((c < 0x20) || (c > 0x5A)) {
        c = '?';
    }
    const uint8_t *glyph = k_font5x7[(uint8_t)c - 0x20U];
    if (scale == 2U) {
        /* HD Scale2x / EPX sub-pixel smoothing on 10x14 grid (eliminates staircase edges) */
        for (int8_t col = 0; col < 5; col++) {
            for (int8_t row = 0; row < 7; row++) {
                uint8_t p = OLED_GlyphPx(glyph, col, row);
                uint8_t a = OLED_GlyphPx(glyph, col, (int8_t)(row - 1)); /* Up */
                uint8_t b = OLED_GlyphPx(glyph, (int8_t)(col + 1), row); /* Right */
                uint8_t l = OLED_GlyphPx(glyph, (int8_t)(col - 1), row); /* Left */
                uint8_t d = OLED_GlyphPx(glyph, col, (int8_t)(row + 1)); /* Down */

                uint8_t e0 = ((l == a) && (l != d) && (a != b)) ? a : p;
                uint8_t e1 = ((a == b) && (a != l) && (b != d)) ? b : p;
                uint8_t e2 = ((d == l) && (d != b) && (l != a)) ? l : p;
                uint8_t e3 = ((b == d) && (b != a) && (d != l)) ? d : p;

                int16_t px = (int16_t)(x + col * 2);
                int16_t py = (int16_t)(y + row * 2);
                if (e0 != 0U) OLED_DrawPixel(px, py, color);
                if (e1 != 0U) OLED_DrawPixel((int16_t)(px + 1), py, color);
                if (e2 != 0U) OLED_DrawPixel(px, (int16_t)(py + 1), color);
                if (e3 != 0U) OLED_DrawPixel((int16_t)(px + 1), (int16_t)(py + 1), color);
            }
        }
        return;
    }

    for (uint8_t col = 0U; col < 5U; col++) {
        uint8_t line = glyph[col];
        for (uint8_t row = 0U; row < 7U; row++) {
            if ((line & (uint8_t)(1U << row)) != 0U) {
                if (scale == 1U) {
                    OLED_DrawPixel(x + (int16_t)col, y + (int16_t)row, color);
                } else {
                    OLED_FillRect(x + (int16_t)(col * scale),
                                  y + (int16_t)(row * scale),
                                  (int16_t)scale, (int16_t)scale, color);
                }
            }
        }
    }
}

void OLED_DrawStr(int16_t x, int16_t y, const char *str, uint8_t scale, uint8_t color)
{
    if (str == NULL) return;
    if (scale == 0U) scale = 1U;
    int16_t cur_x = x;
    while (*str != '\0') {
        OLED_DrawChar(cur_x, y, *str, scale, color);
        cur_x += (int16_t)(6U * scale);
        str++;
    }
}

const uint8_t* OLED_GetFramebuffer(void)
{
    return &s_vram[0][0];
}

bool OLED_IsPoweredOn(void)
{
    return s_oled_powered;
}

uint16_t OLED_CountLitPixels(void)
{
    if (!s_oled_powered) {
        return 0U;
    }
    uint16_t count = 0U;
    for (uint8_t page = 0U; page < OLED_PAGES; page++) {
        for (uint8_t col = 0U; col < OLED_WIDTH; col++) {
            uint8_t b = s_vram[page][col];
            while (b != 0U) {
                b &= (uint8_t)(b - 1U);
                count++;
            }
        }
    }
    return count;
}
