
/* ============================================================================
 * 1. EXACT 1-BIT CH1115 (88x48) FRAMEBUFFER & 5x7 ASCII FONT ENGINE
 *    Bit-for-bit identical to Code/SmileWatch1/Core/Src/oled_ch1115.c
 * ==========================================================================*/
const OLED_WIDTH  = 88;
const OLED_HEIGHT = 48;
const OLED_PAGES  = 6;
const s_vram = Array.from({ length: OLED_PAGES }, () => new Uint8Array(OLED_WIDTH));

const OLED_ICON_HEART  = 0;
const OLED_ICON_THERMO = 1;
const OLED_ICON_SHOE   = 2;
const OLED_ICON_BELL   = 3;
const OLED_ICON_BLE    = 4;
const OLED_ICON_BOLT   = 5;
const OLED_ICON_WATCH  = 6;

const k_icons_8x8 = [
    [ 0x00, 0x66, 0xFF, 0xFF, 0xFF, 0x7E, 0x3C, 0x18 ], // HEART
    [ 0x18, 0x24, 0x24, 0x24, 0x24, 0x7E, 0x7E, 0x3C ], // THERMO
    [ 0x00, 0x30, 0x78, 0x7C, 0xFE, 0xFF, 0xFF, 0x00 ], // SHOE
    [ 0x18, 0x3C, 0x3C, 0x3C, 0x7E, 0xFF, 0x18, 0x00 ], // BELL
    [ 0x18, 0x54, 0x38, 0x10, 0x38, 0x54, 0x18, 0x00 ], // BLE
    [ 0x0C, 0x18, 0x30, 0x7E, 0x18, 0x30, 0x60, 0x00 ], // BOLT
    [ 0x3C, 0x42, 0x99, 0xA1, 0xA1, 0x99, 0x42, 0x3C ]  // WATCH
];

const k_font5x7 = [
    [0x00,0x00,0x00,0x00,0x00],[0x00,0x00,0x5F,0x00,0x00],[0x00,0x07,0x00,0x07,0x00],[0x14,0x7F,0x14,0x7F,0x14],
    [0x24,0x2A,0x7F,0x2A,0x12],[0x23,0x13,0x08,0x64,0x62],[0x36,0x49,0x55,0x22,0x50],[0x00,0x05,0x03,0x00,0x00],
    [0x00,0x1C,0x22,0x41,0x00],[0x00,0x41,0x22,0x1C,0x00],[0x14,0x08,0x3E,0x08,0x14],[0x08,0x08,0x3E,0x08,0x08],
    [0x00,0x50,0x30,0x00,0x00],[0x08,0x08,0x08,0x08,0x08],[0x00,0x60,0x60,0x00,0x00],[0x20,0x10,0x08,0x04,0x02],
    [0x3E,0x51,0x49,0x45,0x3E],[0x00,0x42,0x7F,0x40,0x00],[0x42,0x61,0x51,0x49,0x46],[0x21,0x41,0x45,0x4B,0x31],
    [0x18,0x14,0x12,0x7F,0x10],[0x27,0x45,0x45,0x45,0x39],[0x3C,0x4A,0x49,0x49,0x30],[0x01,0x71,0x09,0x05,0x03],
    [0x36,0x49,0x49,0x49,0x36],[0x06,0x49,0x49,0x29,0x1E],[0x00,0x36,0x36,0x00,0x00],[0x00,0x56,0x36,0x00,0x00],
    [0x08,0x14,0x22,0x41,0x00],[0x14,0x14,0x14,0x14,0x14],[0x00,0x41,0x22,0x14,0x08],[0x02,0x01,0x51,0x09,0x06],
    [0x32,0x49,0x79,0x41,0x3E],[0x7E,0x11,0x11,0x11,0x7E],[0x7F,0x49,0x49,0x49,0x36],[0x3E,0x41,0x41,0x41,0x22],
    [0x7F,0x41,0x41,0x22,0x1C],[0x7F,0x49,0x49,0x49,0x41],[0x7F,0x09,0x09,0x09,0x01],[0x3E,0x41,0x49,0x49,0x7A],
    [0x7F,0x08,0x08,0x08,0x7F],[0x00,0x41,0x7F,0x41,0x00],[0x20,0x40,0x41,0x3F,0x01],[0x7F,0x08,0x14,0x22,0x41],
    [0x7F,0x40,0x40,0x40,0x40],[0x7F,0x02,0x0C,0x02,0x7F],[0x7F,0x04,0x08,0x10,0x7F],[0x3E,0x41,0x41,0x41,0x3E],
    [0x7F,0x09,0x09,0x09,0x06],[0x3E,0x41,0x51,0x21,0x5E],[0x7F,0x09,0x19,0x29,0x46],[0x46,0x49,0x49,0x49,0x31],
    [0x01,0x01,0x7F,0x01,0x01],[0x3F,0x40,0x40,0x40,0x3F],[0x1F,0x20,0x40,0x20,0x1F],[0x3F,0x40,0x38,0x40,0x3F],
    [0x63,0x14,0x08,0x14,0x63],[0x07,0x08,0x70,0x08,0x07],[0x61,0x51,0x49,0x45,0x43]
];

const k_ppg_wave = [4, 4, 5, 3, 2, 8, 15, 16, 11, 6, 4, 6, 8, 7, 5, 4, 4, 4, 4, 4, 4, 4];


/* Circle & Rounded Rect Primitives for Garmin UI */
function OLED_DrawCircle(x0, y0, r, color = 1) {
    x0 = Math.trunc(x0); y0 = Math.trunc(y0); r = Math.trunc(r);
    let f = 1 - r, ddF_x = 1, ddF_y = -2 * r, x = 0, y = r;
    OLED_DrawPixel(x0, y0 + r, color);
    OLED_DrawPixel(x0, y0 - r, color);
    OLED_DrawPixel(x0 + r, y0, color);
    OLED_DrawPixel(x0 - r, y0, color);
    while (x < y) {
        if (f >= 0) { y--; ddF_y += 2; f += ddF_y; }
        x++; ddF_x += 2; f += ddF_x;
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

function OLED_FillCircle(x0, y0, r, color = 1) {
    x0 = Math.trunc(x0); y0 = Math.trunc(y0); r = Math.trunc(r);
    for (let y = -r; y <= r; y++) {
        for (let x = -r; x <= r; x++) {
            if (x * x + y * y <= r * r) {
                OLED_DrawPixel(x0 + x, y0 + y, color);
            }
        }
    }
}

function OLED_DrawRoundRect(x, y, w, h, r, color = 1) {
    x = Math.trunc(x); y = Math.trunc(y); w = Math.trunc(w); h = Math.trunc(h); r = Math.trunc(r);
    OLED_DrawLine(x + r, y, x + w - r - 1, y, color);
    OLED_DrawLine(x + r, y + h - 1, x + w - r - 1, y + h - 1, color);
    OLED_DrawLine(x, y + r, x, y + h - r - 1, color);
    OLED_DrawLine(x + w - 1, y + r, x + w - 1, y + h - r - 1, color);
    // Draw 4 corners
    for (let i = 0; i <= r; i++) {
        let dy = Math.round(Math.sqrt(r * r - i * i));
        OLED_DrawPixel(x + r - i, y + r - dy, color);
        OLED_DrawPixel(x + w - 1 - r + i, y + r - dy, color);
        OLED_DrawPixel(x + r - i, y + h - 1 - r + dy, color);
        OLED_DrawPixel(x + w - 1 - r + i, y + h - 1 - r + dy, color);
    }
}

/* High-Definition Garmin Health 1-Bit Icons */
const k_garmin_heart_10x9 = [
    0x38, 0x00, 0x7C, 0x00, 0xFE, 0x00, 0xFF, 0x00, 0xFF, 0x00, 0x7E, 0x00, 0x3C, 0x00, 0x18, 0x00, 0x00, 0x00
];
const k_garmin_bodybat_12x10 = [
    0x3F, 0xC0, 0x20, 0x40, 0x23, 0x40, 0x27, 0x40, 0x2E, 0x40, 0x3C, 0x40, 0x28, 0x40, 0x20, 0x40, 0x3F, 0xC0, 0x0C, 0x00
];
const k_garmin_thermo_8x12 = [
    0x38, 0x44, 0x44, 0x54, 0x54, 0x54, 0x7C, 0xFE, 0xFE, 0xEE, 0x44, 0x38
];
const k_garmin_shoe_12x9 = [
    0x03, 0x00, 0x07, 0x80, 0x0F, 0xC0, 0x1F, 0xE0, 0x3F, 0xF0, 0x7F, 0xF0, 0xFF, 0xF0, 0x7F, 0xE0, 0x00, 0x00
];
const k_garmin_bubble_11x9 = [
    0x3F, 0x80, 0x7F, 0xC0, 0xE9, 0xE0, 0xE0, 0xE0, 0xE9, 0xE0, 0x7F, 0xC0, 0x3F, 0x80, 0x07, 0x00, 0x02, 0x00
];

function OLED_Clear() {
    for (let p = 0; p < OLED_PAGES; p++) s_vram[p].fill(0);
}

function OLED_DrawPixel(x, y, color = 1) {
    x = Math.trunc(x); y = Math.trunc(y);
    if (x < 0 || x >= OLED_WIDTH || y < 0 || y >= OLED_HEIGHT) return;
    const page = y >> 3;
    const bit  = 1 << (y & 7);
    if (color) s_vram[page][x] |= bit;
    else       s_vram[page][x] &= ~bit;
}

function OLED_DrawLine(x0, y0, x1, y1, color = 1) {
    x0 = Math.trunc(x0); y0 = Math.trunc(y0); x1 = Math.trunc(x1); y1 = Math.trunc(y1);
    const dx = Math.abs(x1 - x0), sx = x0 < x1 ? 1 : -1;
    const dy = -Math.abs(y1 - y0), sy = y0 < y1 ? 1 : -1;
    let err = dx + dy;
    while (true) {
        OLED_DrawPixel(x0, y0, color);
        if (x0 === x1 && y0 === y1) break;
        const e2 = 2 * err;
        if (e2 >= dy) { err += dy; x0 += sx; }
        if (e2 <= dx) { err += dx; y0 += sy; }
    }
}

function OLED_DrawRect(x, y, w, h, color = 1) {
    if (w <= 0 || h <= 0) return;
    OLED_DrawLine(x, y, x + w - 1, y, color);
    OLED_DrawLine(x, y + h - 1, x + w - 1, y + h - 1, color);
    OLED_DrawLine(x, y, x, y + h - 1, color);
    OLED_DrawLine(x + w - 1, y, x + w - 1, y + h - 1, color);
}

function OLED_FillRect(x, y, w, h, color = 1) {
    for (let j = y; j < y + h; j++) {
        for (let i = x; i < x + w; i++) OLED_DrawPixel(i, j, color);
    }
}

function OLED_DrawBitmap(x, y, bmp, w, h, color = 1) {
    const byteWidth = Math.ceil(w / 8);
    for (let j = 0; j < h; j++) {
        for (let i = 0; i < w; i++) {
            const b = bmp[j * byteWidth + (i >> 3)];
            if (b & (0x80 >> (i & 7))) OLED_DrawPixel(x + i, y + j, color);
        }
    }
}

function OLED_DrawIcon(x, y, icon_id, color = 1) {
    if (icon_id >= 0 && icon_id < k_icons_8x8.length) {
        OLED_DrawBitmap(x, y, k_icons_8x8[icon_id], 8, 8, color);
    }
}

function OLED_DrawChar(x, y, ch, scale = 1, color = 1) {
    scale = Math.max(1, Math.trunc(scale));
    let code = String(ch).toUpperCase().charCodeAt(0);
    if (code < 0x20 || code > 0x5A) code = 0x3F;
    const glyph = k_font5x7[code - 0x20];
    for (let col = 0; col < 5; col++) {
        const line = glyph[col];
        for (let row = 0; row < 7; row++) {
            if (line & (1 << row)) {
                if (scale === 1) OLED_DrawPixel(x + col, y + row, color);
                else OLED_FillRect(x + col * scale, y + row * scale, scale, scale, color);
            }
        }
    }
}

function OLED_DrawStr(x, y, str, scale = 1, color = 1) {
    str = String(str ?? "");
    let curX = x;
    for (let i = 0; i < str.length; i++) {
        OLED_DrawChar(curX, y, str[i], scale, color);
        curX += 6 * scale;
    }
}

/* ============================================================================
 * 2. SMILEWATCH SYSTEM STATE & MIYOTA 2035 STEPPING ENGINE
 * ==========================================================================*/

/* Garmin 24h Daily History Buffers (44 points for 88 px screen width) */
const k_hr_24h_history = [
    54, 52, 51, 50, 49, 48, 50, 52, 56, 62, 74, 82, 88, 95, 86, 78,
    72, 70, 75, 84, 92, 118, 126, 110, 96, 85, 80, 76, 74, 78, 85, 92,
    88, 82, 76, 72, 68, 65, 62, 58, 56, 55, 54, 52
];
const k_bodybat_history = [
    95, 92, 88, 85, 82, 78, 75, 71, 68, 64, 60, 55, 52, 48, 45, 42,
    46, 50, 48, 44, 40, 36, 32, 28, 25, 22, 20, 18, 16, 20, 24, 28,
    35, 42, 50, 60, 70, 78, 84, 88, 92, 94, 95, 96
];

const s_state = {
    body_battery: 78,
    stress_score: 24,
    hr_min: 48,
    hr_max: 126,

    hours: 10, minutes: 10, seconds: 30,
    day: 8, month: 11, weekday_str: "DIM",
    battery_pct: 84, battery_mv: 4020,
    qi_charging: false, ble_connected: true, survival_mode: false,
    heart_bpm: 72, spo2_pct: 98, hr_measuring: true,
    temp_deci_c: 366, steps_count: 6420, steps_goal: 10000, ambient_lux: 250,
    notif_unread: 1,
    notif_title: "SMS - ALEXIS",
    notif_body: "RDV FABLAB MARDI 13H30 OK",
    current_screen: 0,
    display_awake: true,
    ppg_phase: 0
};

const s_miyota = {
    next_polarity: 0,
    pulse_active: false,
    pulse_polarity: 0, // +1 (PA7=1,PB5=0), -1 (PA7=0,PB5=1), 0 (Lenz Brake)
    pulse_until_ms: 0,
    analog_pos_sec: (10 * 3600 + 10 * 60 + 30),
    target_pos_sec: (10 * 3600 + 10 * 60 + 30),
    last_step_ms: 0
};

let g_btn2h_pressed = false;
let g_btn4h_pressed = false;
let g_btn_down_time = { '2h': 0, '4h': 0 };
let g_haptic_active_until = 0;

function SmileWatch_DrawStatusBar(tag) {
    OLED_DrawStr(0, 0, tag, 1, 1);
    if (s_state.ble_connected) OLED_DrawIcon(52, 0, OLED_ICON_BLE, 1);
    if (s_state.qi_charging)   OLED_DrawIcon(62, 0, OLED_ICON_BOLT, 1);
    OLED_DrawRect(72, 1, 14, 6, 1);
    OLED_FillRect(86, 2, 2, 4, 1);
    const fill_w = Math.trunc((s_state.battery_pct * 10) / 100);
    if (fill_w > 0) OLED_FillRect(74, 3, fill_w, 2, 1);
    OLED_DrawLine(0, 9, 87, 9, 1);
}

/* Editable C Screen Sources (stored as C code and transpiled live) */
const k_default_c_screens = [
`static void Render_ScreenHome(void)
{
    /* GARMIN WATCHFACE - GLANCE STYLE */
    char buf[24];

    /* Top Status Bar: BLE connection dot & Battery pill */
    if (s_state.ble_connected) {
        OLED_FillCircle(3, 4, 1.5, 1);
        OLED_DrawIcon(6, 1, OLED_ICON_BLE, 1);
    }
    if (s_state.qi_charging) {
        OLED_DrawIcon(16, 1, OLED_ICON_BOLT, 1);
    }

    /* Garmin Battery Badge with % */
    snprintf(buf, sizeof(buf), "%u%%", s_state.battery_pct);
    OLED_DrawStr(54, 2, buf, 1, 1);
    OLED_DrawRoundRect(72, 1, 13, 7, 2, 1);
    OLED_FillRect(85, 3, 2, 3, 1);
    int16_t bfill = (s_state.battery_pct * 9) / 100;
    if (bfill > 0) OLED_FillRoundRect(74, 3, bfill, 3, 1, 1);

    /* Center Big Bold Time HH:MM */
    snprintf(buf, sizeof(buf), "%02u:%02u", s_state.hours, s_state.minutes);
    OLED_DrawStr(6, 12, buf, 2, 1);

    /* Seconds pill box */
    OLED_DrawRoundRect(66, 15, 18, 11, 3, 1);
    snprintf(buf, sizeof(buf), "%02u", s_state.seconds);
    OLED_DrawStr(69, 17, buf, 1, 1);

    /* Divider */
    OLED_DrawLine(2, 29, 85, 29, 1);

    /* Bottom Garmin Glance Metrics: [Date] | [HR Heart + BPM] | [Body Bat] */
    snprintf(buf, sizeof(buf), "%s %02u", s_state.weekday_str, s_state.day);
    OLED_DrawStr(2, 33, buf, 1, 1);

    /* Mini Live Heart Pulse Icon + Value */
    OLED_DrawBitmap(32, 32, k_garmin_heart_10x9, 10, 9, 1);
    snprintf(buf, sizeof(buf), "%u", s_state.heart_bpm);
    OLED_DrawStr(44, 33, buf, 1, 1);

    /* Body Battery pill */
    OLED_DrawRoundRect(64, 32, 22, 13, 3, 1);
    OLED_DrawStr(66, 35, "B:78", 1, 1);

    /* Steps Progress bar at very bottom (y=46..47) */
    uint32_t pct = Math.min(100, Math.trunc((s_state.steps_count * 100) / s_state.steps_goal));
    OLED_FillRect(2, 46, Math.trunc((pct * 84) / 100), 2, 1);
}`,
`static void Render_ScreenHeartSpo2(void)
{
    /* GARMIN ELEVATE HR & 24-HOUR DAILY OSCILLOGRAM */
    char buf[24];

    /* Header Bar: Heart Icon + LIVE BPM + Min/Max badges */
    OLED_DrawBitmap(2, 1, k_garmin_heart_10x9, 10, 9, 1);
    snprintf(buf, sizeof(buf), "%u", s_state.heart_bpm);
    OLED_DrawStr(14, 0, buf, 2, 1);
    OLED_DrawStr(50, 1, "BPM", 1, 1);

    /* Daily Min/Max: 48 / 126 BPM */
    snprintf(buf, sizeof(buf), "H:%u", s_state.hr_max || 126);
    OLED_DrawStr(54, 8, buf, 1, 1);
    snprintf(buf, sizeof(buf), "L:%u", s_state.hr_min || 48);
    OLED_DrawStr(70, 8, buf, 1, 1);

    /* Graph Frame (Daily Oscillogram: 0h to 24h across 88 px) */
    OLED_DrawLine(0, 17, 87, 17, 1);
    OLED_DrawLine(0, 43, 87, 43, 1);
    // 6h, 12h, 18h vertical dotted time markers
    for (let y = 18; y < 43; y += 4) {
        OLED_DrawPixel(22, y, 1);
        OLED_DrawPixel(44, y, 1);
        OLED_DrawPixel(66, y, 1);
    }

    /* Plot 24h HR Trend Curve & Area Fill */
    let prev_gx = 0;
    let prev_gy = 38;
    for (let i = 0; i < 44; i++) {
        let hr = k_hr_24h_history[i] || 70;
        // Map HR [45..135] to Y [42..19]
        let gy = 42 - Math.round(((hr - 45) * 23) / 90);
        if (gy < 19) gy = 19;
        if (gy > 42) gy = 42;
        let gx = i * 2;
        if (i > 0) {
            OLED_DrawLine(prev_gx, prev_gy, gx, gy, 1);
        }
        // Shading dots under peak
        if (hr > 85) {
            OLED_DrawLine(gx, gy, gx, 42, 1);
        }
        prev_gx = gx;
        prev_gy = gy;
    }

    /* Bottom Time Axis Labels */
    OLED_DrawStr(1, 44, "0H", 1, 1);
    OLED_DrawStr(20, 44, "6H", 1, 1);
    OLED_DrawStr(40, 44, "12H", 1, 1);
    OLED_DrawStr(62, 44, "18H", 1, 1);
    OLED_DrawStr(78, 44, "24", 1, 1);
}`,
`static void Render_ScreenTemp(void)
{
    /* GARMIN BODY BATTERY (TM) & STRESS LEVEL */
    char buf[24];

    /* Header: Body Battery Icon + Score 78/100 */
    OLED_DrawBitmap(2, 1, k_garmin_bodybat_12x10, 12, 10, 1);
    OLED_DrawStr(16, 1, "BODY BAT.", 1, 1);

    /* Big Value */
    snprintf(buf, sizeof(buf), "%u", s_state.body_battery || 78);
    OLED_DrawStr(68, 0, buf, 2, 1);

    /* Separator */
    OLED_DrawLine(0, 13, 87, 13, 1);

    /* Subtitle: Stress & Charge status */
    OLED_DrawStr(2, 16, "CHARGE +22  STRESS:BAS", 1, 1);

    /* 24-Hour Body Battery Drain & Charge Trend Curve */
    OLED_DrawLine(0, 24, 87, 24, 1);
    OLED_DrawLine(0, 41, 87, 41, 1);

    let prev_bx = 0;
    let prev_by = 30;
    for (let i = 0; i < 44; i++) {
        let bat = k_bodybat_history[i] || 50;
        // Map 0..100 to Y 40..25
        let by = 40 - Math.round((bat * 15) / 100);
        let bx = i * 2;
        if (i > 0) {
            OLED_DrawLine(prev_bx, prev_by, bx, by, 1);
        }
        prev_bx = bx;
        prev_by = by;
    }

    /* Bottom recovery indicators */
    OLED_DrawStr(2, 43, "REPOS: 8H12  ENERGIE OK", 1, 1);
}`,
`static void Render_ScreenSteps(void)
{
    /* GARMIN CLINICAL HEALTH - TMP117 & SpO2 PULSE OX */
    char buf[24];

    /* Header */
    OLED_DrawBitmap(2, 1, k_garmin_thermo_8x12, 8, 12, 1);
    OLED_DrawStr(14, 2, "SANTE CLINIQUE", 1, 1);
    OLED_DrawLine(0, 14, 87, 14, 1);

    /* Left Card: TMP117 High-Precision Skin Temperature */
    OLED_DrawRoundRect(1, 16, 42, 30, 3, 1);
    OLED_DrawStr(4, 18, "TEMP.", 1, 1);
    int16_t whole = Math.trunc(s_state.temp_deci_c / 10);
    int16_t frac  = Math.abs(s_state.temp_deci_c % 10);
    snprintf(buf, sizeof(buf), "%d.%d", whole, frac);
    OLED_DrawStr(4, 28, buf, 1, 1);
    OLED_DrawStr(30, 28, "C", 1, 1);
    OLED_DrawRect(28, 27, 2, 2, 1); // degree symbol
    OLED_DrawStr(4, 38, "+-0.1C", 1, 1);

    /* Right Card: SpO2 Pulse Oximeter */
    OLED_DrawRoundRect(45, 16, 42, 30, 3, 1);
    OLED_DrawStr(48, 18, "SpO2", 1, 1);
    snprintf(buf, sizeof(buf), "%u%%", s_state.spo2_pct);
    OLED_DrawStr(48, 28, buf, 1, 1);
    if (s_state.spo2_pct >= 95) {
        OLED_DrawStr(48, 38, "OPTIMAL", 1, 1);
    } else {
        OLED_DrawStr(48, 38, "ATTENT.", 1, 1);
    }
}`,
`static void Render_ScreenNotif(void)
{
    /* GARMIN FITNESS & INTENSITY MINUTES */
    char buf[24];

    /* Top Icon + Activity Title */
    OLED_DrawBitmap(2, 1, k_garmin_shoe_12x9, 12, 9, 1);
    OLED_DrawStr(16, 2, "PAS & ACTIVITE", 1, 1);
    OLED_DrawLine(0, 12, 87, 12, 1);

    /* Steps Count (Large Font) */
    snprintf(buf, sizeof(buf), "%lu", s_state.steps_count);
    OLED_DrawStr(4, 15, buf, 2, 1);

    /* Goal Arc/Progress Bar */
    uint32_t pct = Math.min(100, Math.trunc((s_state.steps_count * 100) / s_state.steps_goal));
    snprintf(buf, sizeof(buf), "/10k (%u%%)", pct);
    OLED_DrawStr(52, 20, buf, 1, 1);

    /* Rounded Garmin Bar */
    OLED_DrawRoundRect(4, 31, 80, 5, 2, 1);
    OLED_FillRoundRect(5, 32, Math.trunc((pct * 78) / 100), 3, 1, 1);

    /* Bottom Metrics Chips */
    uint32_t dist_hm = Math.trunc((s_state.steps_count * 7) / 1000);
    uint32_t kcal    = Math.trunc((s_state.steps_count * 4) / 100);
    snprintf(buf, sizeof(buf), "%lu.%ukm  %lukcal  38MIN", Math.trunc(dist_hm / 10), dist_hm % 10, kcal);
    OLED_DrawStr(4, 39, buf, 1, 1);
}`,
`static void Render_ScreenCalib(void)
{
    /* GARMIN NOTIFICATIONS & SMART GLANCES */
    OLED_DrawBitmap(2, 1, k_garmin_bubble_11x9, 11, 9, 1);
    OLED_DrawStr(16, 2, "NOTIFICATIONS", 1, 1);
    OLED_DrawLine(0, 12, 87, 12, 1);

    if (s_state.notif_unread == 0) {
        OLED_DrawRoundRect(8, 16, 72, 26, 4, 1);
        OLED_DrawStr(18, 22, "AUCUN", 1, 1);
        OLED_DrawStr(14, 31, "NOUVEAU MSG", 1, 1);
    } else {
        /* Garmin Message Notification Card */
        OLED_DrawRoundRect(2, 14, 84, 32, 4, 1);
        OLED_DrawStr(6, 17, s_state.notif_title, 1, 1);
        OLED_DrawLine(6, 26, 80, 26, 1);

        OLED_DrawStr(6, 29, s_state.notif_body.substring(0, 12), 1, 1);
        OLED_DrawStr(6, 37, s_state.notif_body.substring(12, 24), 1, 1);
    }
}`
];

const g_c_screens = [...k_default_c_screens];

/* Lightweight C-to-JS transpiler for live execution of C screen code */
function executeCScreenCode(cSource) {
    let js = cSource
        .replace(/static\s+void\s+\w+\s*\(\s*void\s*\)\s*\{/, '{')
        .replace(/char\s+buf\s*\[\s*\d+\s*\]\s*;/g, 'let buf = "";')
        .replace(/\(void\)\s*snprintf/g, 'snprintf')
        .replace(/snprintf\s*\(\s*buf\s*,\s*sizeof\s*\(\s*buf\s*\)\s*,/g, 'buf = c_sprintf(')
        .replace(/\b(?:uint8_t|uint16_t|uint32_t|int16_t|int32_t|int)\b/g, 'let');

    const c_sprintf = (fmt, ...args) => {
        let i = 0;
        return fmt.replace(/%0?(\d*)?(?:lu|u|d|s|%)/g, (m, width) => {
            if (m === '%%') return '%';
            const val = args[i++];
            if (m.endsWith('s')) return String(val ?? '');
            const num = Math.trunc(Number(val ?? 0));
            return width ? String(num).padStart(Number(width), '0') : String(num);
        });
    };

    const fn = new Function(
        's_state', 's_miyota', 'k_ppg_wave', 'c_sprintf',
        'SmileWatch_DrawStatusBar', 'OLED_Clear', 'OLED_DrawPixel', 'OLED_DrawLine',
        'OLED_DrawRect', 'OLED_FillRect', 'OLED_DrawIcon', 'OLED_DrawStr', 'OLED_DrawBitmap',
        'OLED_ICON_HEART', 'OLED_ICON_THERMO', 'OLED_ICON_SHOE', 'OLED_ICON_BELL',
        'OLED_ICON_BLE', 'OLED_ICON_BOLT', 'OLED_ICON_WATCH',
        js
    );

    fn(
        s_state, s_miyota, k_ppg_wave, c_sprintf,
        SmileWatch_DrawStatusBar, OLED_Clear, OLED_DrawPixel, OLED_DrawLine,
        OLED_DrawRect, OLED_FillRect, OLED_DrawIcon, OLED_DrawStr, OLED_DrawBitmap,
        OLED_ICON_HEART, OLED_ICON_THERMO, OLED_ICON_SHOE, OLED_ICON_BELL,
        OLED_ICON_BLE, OLED_ICON_BOLT, OLED_ICON_WATCH
    );
}

function renderCurrentScreen() {
    OLED_Clear();
    if (s_state.display_awake && !s_state.survival_mode) {
        try {
            executeCScreenCode(g_c_screens[s_state.current_screen]);
        } catch (err) {
            OLED_DrawStr(2, 10, "ERREUR CODE C:", 1, 1);
            OLED_DrawStr(2, 22, String(err.message).substring(0, 14), 1, 1);
        }
    }
    flushOLED();
}

function flushOLED() {
    const watchCanvas = document.getElementById('watch-oled-canvas');
    const zoomCanvas  = document.getElementById('zoom-oled-canvas');
    const wCtx = watchCanvas.getContext('2d');
    const zCtx = zoomCanvas.getContext('2d');

    wCtx.fillStyle = '#020408';
    wCtx.fillRect(0, 0, OLED_WIDTH, OLED_HEIGHT);

    zCtx.fillStyle = '#020409';
    zCtx.fillRect(0, 0, zoomCanvas.width, zoomCanvas.height);

    // Brightness modulation from VEML6030 ambient lux
    const alpha = s_state.display_awake && !s_state.survival_mode
        ? Math.min(1.0, 0.55 + (s_state.ambient_lux / 1000) * 0.45)
        : 0.0;

    const pxColor = `rgba(0, 238, 255, ${alpha.toFixed(2)})`;
    wCtx.fillStyle = pxColor;
    zCtx.fillStyle = pxColor;

    for (let y = 0; y < OLED_HEIGHT; y++) {
        const page = y >> 3;
        const bit  = 1 << (y & 7);
        for (let x = 0; x < OLED_WIDTH; x++) {
            if (s_vram[page][x] & bit) {
                wCtx.fillRect(x, y, 1, 1);
                zCtx.fillRect(x * 4, y * 4, 3.4, 3.4);
            }
        }
    }

    // Optional hardware page/pixel grid on zoom loupe
    if (document.getElementById('chk-pixel-grid').checked) {
        zCtx.strokeStyle = 'rgba(0, 229, 255, 0.22)';
        zCtx.lineWidth = 1;
        for (let p = 1; p < OLED_PAGES; p++) {
            zCtx.beginPath();
            zCtx.moveTo(0, p * 32);
            zCtx.lineTo(352, p * 32);
            zCtx.stroke();
        }
    }

    document.getElementById('loupe-power').innerText =
        (s_state.display_awake && !s_state.survival_mode)
        ? 'GPIO_EC (PD7) : ON (+3V8 -> 7.5V CH1115)'
        : 'GPIO_EC (PD7) : OFF (Veille 0 µA)';
}

/* ============================================================================
 * 3. BUTTON EVENTS, HAPTIC & WATCH HANDS RENDERING
 * ==========================================================================*/
const SCREEN_NAMES = ['GLANCE', 'FC 24H', 'BODY BAT', 'CLINIQUE', 'FITNESS', 'NOTIF'];

function triggerHaptic(ms) {
    g_haptic_active_until = performance.now() + ms;
    document.getElementById('watch-container').classList.add('vibrating');
    logI2C(`I2C1 [0x5A DRV2605L] -> GO Waveform (PA1 EN=1, ${ms} ms)`);
}

function triggerButtonEvent(type) {
    const now = performance.now();

    if (!s_state.display_awake && !s_state.survival_mode) {
        s_state.display_awake = true;
        triggerHaptic(35);
        logI2C(`GPIO [PD7 GPIO_EC=1, PB4 RES=1] -> CH1115 Wakeup Init (0x3C)`);
        renderCurrentScreen();
        return;
    }

    if (type === '2H_SHORT') {
        s_state.current_screen = (s_state.current_screen + 1) % 6;
        triggerHaptic(25);
        logI2C(`EXTI [PC5 S2 2h] Clic court -> Écran ${SCREEN_NAMES[s_state.current_screen]}`);
    } else if (type === '2H_LONG') {
        if (s_state.current_screen !== 0) {
            s_state.current_screen = 0;
            logI2C(`EXTI [PC5 S2 2h] Appui long -> Retour HOME`);
        } else {
            s_state.display_awake = false;
            logI2C(`GPIO [PD7 GPIO_EC=0] -> Extinction OLED CH1115 (Stop 2)`);
        }
        triggerHaptic(60);
    } else if (type === '4H_SHORT') {
        triggerHaptic(30);
        if (s_state.current_screen === 1) {
            s_state.hr_measuring = !s_state.hr_measuring;
            logI2C(`GPIO [PB0 EN_5V_HR=${s_state.hr_measuring ? 1 : 0}] -> MAX30101 (0x57) ${s_state.hr_measuring ? 'START' : 'STOP'}`);
        } else if (s_state.current_screen === 4) {
            s_state.notif_unread = 0;
            logI2C(`ANCS -> Notification acquittée par l'utilisateur`);
        } else if (s_state.current_screen === 5) {
            stepMiyotaOnce(now);
            s_miyota.target_pos_sec = s_miyota.analog_pos_sec;
            logI2C(`MIYOTA [PA7/PB5] -> Pas manuel +1s`);
        } else {
            logI2C(`I2C1 [0x48 TMP117 / 0x6A LSM6DSOX] -> Rafraîchissement forcé`);
        }
    } else if (type === '4H_LONG') {
        triggerHaptic(80);
        if (s_state.current_screen === 3) {
            s_state.steps_count = 0;
            logI2C(`I2C1 [0x6A LSM6DSOX] -> Remise à zéro compteur de pas`);
        } else if (s_state.current_screen === 5) {
            s_miyota.target_pos_sec = (s_miyota.target_pos_sec + 60) % 43200;
            logI2C(`MIYOTA [PA7/PB5] -> Avance rapide +60 pas (+1 min)`);
        }
    } else if (type === 'BOTH_LONG') {
        triggerHaptic(150);
        s_state.current_screen = 5;
        s_miyota.analog_pos_sec = 0; // 12:00:00 reference
        s_miyota.target_pos_sec = ((s_state.hours % 12) * 3600) + (s_state.minutes * 60) + s_state.seconds;
        logI2C(`CALIB [2h+4h] -> Zéro 12:00 validé ! Rattrapage auto 22 Hz vers ${String(s_state.hours).padStart(2,'0')}:${String(s_state.minutes).padStart(2,'0')}`);
    }

    updateScreenPills();
    renderCurrentScreen();
}

function onPusherDown(which) {
    g_btn_down_time[which] = performance.now();
    if (which === '2h') { g_btn2h_pressed = true; document.getElementById('pusher-2h').classList.add('pressed'); }
    if (which === '4h') { g_btn4h_pressed = true; document.getElementById('pusher-4h').classList.add('pressed'); }
}

function onPusherUp(which) {
    const start = g_btn_down_time[which];
    if (!start) return;
    const held = performance.now() - start;
    g_btn_down_time[which] = 0;
    if (which === '2h') { g_btn2h_pressed = false; document.getElementById('pusher-2h').classList.remove('pressed'); }
    if (which === '4h') { g_btn4h_pressed = false; document.getElementById('pusher-4h').classList.remove('pressed'); }
    triggerButtonEvent(held >= 550 ? `${which.toUpperCase()}_LONG` : `${which.toUpperCase()}_SHORT`);
}

function onPusherCancel(which) {
    g_btn_down_time[which] = 0;
    if (which === '2h') { g_btn2h_pressed = false; document.getElementById('pusher-2h').classList.remove('pressed'); }
    if (which === '4h') { g_btn4h_pressed = false; document.getElementById('pusher-4h').classList.remove('pressed'); }
}

function selectScreen(idx) {
    s_state.current_screen = idx;
    s_state.display_awake = true;
    updateScreenPills();
    loadSelectedCFunction(idx);
    renderCurrentScreen();
}

function updateScreenPills() {
    document.getElementById('hdr-screen').innerText = SCREEN_NAMES[s_state.current_screen];
    const pills = document.querySelectorAll('.screen-pill');
    pills.forEach((p, i) => p.classList.toggle('active', i === s_state.current_screen));
}

function stepMiyotaOnce(now) {
    s_miyota.pulse_active = true;
    s_miyota.pulse_polarity = (s_miyota.next_polarity === 0) ? 1 : -1;
    s_miyota.next_polarity = 1 - s_miyota.next_polarity;
    s_miyota.pulse_until_ms = now + 28; // visual scope width
    s_miyota.last_step_ms = now;
    s_miyota.analog_pos_sec = (s_miyota.analog_pos_sec + 1) % 43200;
    renderWatchFace();
}

function renderWatchFace() {
    const sec = s_miyota.analog_pos_sec;
    const minAngle  = ((sec % 3600) / 3600) * 360;
    const hourAngle = ((sec % 43200) / 43200) * 360;

    document.getElementById('svg-hour-hand').setAttribute('transform', `rotate(${hourAngle.toFixed(1)}, 157, 180)`);
    document.getElementById('svg-min-hand').setAttribute('transform', `rotate(${minAngle.toFixed(1)}, 157, 180)`);

    const ghost = document.getElementById('chk-ghost-hands').checked;
    document.getElementById('miyota-hands-group').setAttribute('opacity', ghost ? '0.28' : '1.0');

    let ah = Math.trunc(sec / 3600) % 12; if (ah === 0) ah = 12;
    const am = Math.trunc((sec % 3600) / 60);
    const as = sec % 60;
    document.getElementById('hdr-miyota').innerText =
        `${String(ah).padStart(2,'0')}:${String(am).padStart(2,'0')}:${String(as).padStart(2,'0')}`;
}

/* ============================================================================
 * 4. SENSORS, BLE EVENTS & LOGIC OSCILLOSCOPE
 * ==========================================================================*/
function updateSensor(kind, val) {
    val = Number(val);
    if (kind === 'bpm') {
        s_state.heart_bpm = val;
        document.getElementById('val-bpm').innerText = `${val} BPM`;
    } else if (kind === 'spo2') {
        s_state.spo2_pct = val;
        document.getElementById('val-spo2').innerText = `${val} %`;
    } else if (kind === 'temp') {
        s_state.temp_deci_c = val;
        document.getElementById('val-temp').innerText = `${(val/10).toFixed(1)} °C`;
    } else if (kind === 'steps') {
        s_state.steps_count = val;
        document.getElementById('val-steps').innerText = `${val} pas`;
    } else if (kind === 'bat') {
        s_state.battery_pct = val;
        s_state.battery_mv = Math.round(3000 + (val / 100) * 1200);
        s_state.survival_mode = (val < 5);
        document.getElementById('val-bat').innerText = `${val} % (${(s_state.battery_mv/1000).toFixed(2)} V)`;
    } else if (kind === 'lux') {
        s_state.ambient_lux = val;
        document.getElementById('val-lux').innerText = `${val} lux`;
    }
    renderCurrentScreen();
}

function simIncomingCall() {
    s_state.notif_title = "APPEL ENTRANT";
    s_state.notif_body  = "YANIS GRAVE   +33 6 12 34 56";
    s_state.notif_unread = 1;
    s_state.current_screen = 4;
    s_state.display_awake = true;
    triggerHaptic(220);
    updateScreenPills();
    renderCurrentScreen();
    logI2C(`BLE ANCS -> Notification d'appel entrant reçue`);
}

function simIncomingSMS() {
    s_state.notif_title = "SMS - ALEXIS";
    s_state.notif_body  = "RDV FABLAB MARDI 13H30 OK";
    s_state.notif_unread = 1;
    s_state.current_screen = 4;
    s_state.display_awake = true;
    triggerHaptic(140);
    updateScreenPills();
    renderCurrentScreen();
    logI2C(`BLE ANCS -> Notification SMS reçue`);
}

function simWristTilt() {
    s_state.display_awake = true;
    triggerHaptic(20);
    renderCurrentScreen();
    logI2C(`EXTI [PA10 INTSA] -> Geste lever de poignet détecté par MLC LSM6DSOX`);
}

function toggleQiCharger() {
    s_state.qi_charging = !s_state.qi_charging;
    logI2C(`GPIO [PB2 ~CHG=${s_state.qi_charging ? 0 : 1}] -> Charge sans fil Qi BQ51003 ${s_state.qi_charging ? 'ACTIVE (119 mA)' : 'COUPÉE'}`);
    renderCurrentScreen();
}

function desyncMiyotaHands() {
    s_miyota.analog_pos_sec = (s_miyota.analog_pos_sec - 25 + 43200) % 43200;
    renderWatchFace();
    logI2C(`MIYOTA -> Décalage simulé (-25 pas) : rattrapage automatique 22 Hz engagé`);
}

function toggleSurvivalMode() {
    if (!s_state.survival_mode) {
        document.getElementById('slider-bat').value = 3;
        updateSensor('bat', 3);
        logI2C(`SURVIE [< 5%] -> Coupure OLED + BLE + Capteurs (Aiguilles Miyota seules : 15.5 µA)`);
    } else {
        document.getElementById('slider-bat').value = 84;
        updateSensor('bat', 84);
        s_state.display_awake = true;
        renderCurrentScreen();
        logI2C(`BATTERIE [84%] -> Sortie du mode survie, réveil complet`);
    }
}

function logI2C(msg) {
    const box = document.getElementById('i2c-log');
    const now = new Date();
    const ts = `${String(now.getHours()).padStart(2,'0')}:${String(now.getMinutes()).padStart(2,'0')}:${String(now.getSeconds()).padStart(2,'0')}.${String(now.getMilliseconds()).padStart(3,'0')}`;
    const line = document.createElement('div');
    line.textContent = `[${ts}] ${msg}`;
    box.prepend(line);
    while (box.children.length > 30) box.removeChild(box.lastChild);
}

/* Scope History Buffer */
const SCOPE_LEN = 200;
const scopeHist = {
    hp: new Int8Array(SCOPE_LEN),
    hm: new Int8Array(SCOPE_LEN),
    btn2: new Int8Array(SCOPE_LEN),
    btn4: new Int8Array(SCOPE_LEN),
    hap: new Int8Array(SCOPE_LEN)
};

function updateOscilloscope(now) {
    // Shift history
    for (const k of Object.keys(scopeHist)) {
        scopeHist[k].copyWithin(0, 1);
    }
    const hp = (s_miyota.pulse_active && s_miyota.pulse_polarity === 1) ? 1 : 0;
    const hm = (s_miyota.pulse_active && s_miyota.pulse_polarity === -1) ? 1 : 0;
    scopeHist.hp[SCOPE_LEN - 1]   = hp;
    scopeHist.hm[SCOPE_LEN - 1]   = hm;
    scopeHist.btn2[SCOPE_LEN - 1] = g_btn2h_pressed ? 1 : 0;
    scopeHist.btn4[SCOPE_LEN - 1] = g_btn4h_pressed ? 1 : 0;
    scopeHist.hap[SCOPE_LEN - 1]  = (now < g_haptic_active_until) ? 1 : 0;

    document.getElementById('miyota-coil-status').innerText =
        hp ? 'Bobine : IMPULSION +3.3V (PA7=1, PB5=0)' :
        hm ? 'Bobine : IMPULSION -3.3V (PA7=0, PB5=1)' :
             'Bobine : FREIN ÉLECTRODYNAMIQUE LENZ (PA7=0, PB5=0)';

    const c = document.getElementById('scope-canvas');
    const ctx = c.getContext('2d');
    ctx.fillStyle = '#040812';
    ctx.fillRect(0, 0, c.width, c.height);

    const channels = [
        { key: 'hp',   label: 'PA7 (Miyota H+)', color: '#fde047', yBase: 32 },
        { key: 'hm',   label: 'PB5 (Miyota H-)', color: '#fb923c', yBase: 68 },
        { key: 'btn2', label: 'PC5 (BTN 2h S2)', color: '#00e5ff', yBase: 104 },
        { key: 'btn4', label: 'PC4 (BTN 4h S1)', color: '#34d399', yBase: 140 },
        { key: 'hap',  label: 'PA1 (DRV2605L)',  color: '#c084fc', yBase: 174 }
    ];

    channels.forEach(ch => {
        ctx.fillStyle = ch.color;
        ctx.font = '11px Consolas, monospace';
        ctx.fillText(ch.label, 8, ch.yBase - 4);

        ctx.strokeStyle = ch.color;
        ctx.lineWidth = 1.6;
        ctx.beginPath();
        const xStart = 130;
        const stepX = (c.width - xStart - 10) / SCOPE_LEN;
        for (let i = 0; i < SCOPE_LEN; i++) {
            const x = xStart + i * stepX;
            const y = ch.yBase - scopeHist[ch.key][i] * 18;
            if (i === 0) ctx.moveTo(x, y);
            else ctx.lineTo(x, y);
        }
        ctx.stroke();
    });

    // Estimate instantaneous current consumption
    let uA = 15.5; // Stop 2 + RTC + Miyota average
    if (s_state.display_awake && !s_state.survival_mode) uA += 9500;
    if (s_state.hr_measuring && s_state.current_screen === 1 && s_state.display_awake) uA += 12500;
    if (now < g_haptic_active_until) uA += 65000;
    document.getElementById('hdr-current').innerText =
        uA >= 1000 ? `${(uA / 1000).toFixed(1)} mA` : `${uA.toFixed(1)} µA`;
}

/* ============================================================================
 * 5. LIVE C CODE STUDIO & FULL SMILEWATCH_UI.C EXPORT
 * ==========================================================================*/
function loadSelectedCFunction(forceIdx) {
    const sel = document.getElementById('select-c-func');
    if (forceIdx !== undefined) sel.value = String(forceIdx);
    const idx = Number(sel.value);
    document.getElementById('c-code-textarea').value = g_c_screens[idx];
}

function onCodeEditorInput() {
    const idx = Number(document.getElementById('select-c-func').value);
    const code = document.getElementById('c-code-textarea').value;
    const status = document.getElementById('c-compile-status');
    try {
        g_c_screens[idx] = code;
        s_state.current_screen = idx;
        updateScreenPills();
        renderCurrentScreen();
        status.style.color = 'var(--accent-green)';
        status.innerText = '✅ Code C compilé et exécuté en temps réel sur la montre !';
    } catch (e) {
        status.style.color = 'var(--accent-red)';
        status.innerText = '❌ Erreur : ' + e.message;
    }
}

function applyLiveCCode() {
    onCodeEditorInput();
    showToast("Code C appliqué sur l'écran 88×48 !");
}

function buildFullCFileContent() {
    return `/**
  ******************************************************************************
  * @file           : smilewatch_ui.c
  * @brief          : Généré depuis le Studio Simulateur SmileWatch V2
  ******************************************************************************
  */
#include "smilewatch_ui.h"
#include <stdio.h>
#include <string.h>

${g_c_screens.join('\n\n')}
`;
}

function copyFullCFile() {
    navigator.clipboard.writeText(buildFullCFileContent()).then(() => {
        showToast("Code C complet copié pour STM32CubeIDE !");
    });
}

function downloadFullCFile() {
    const blob = new Blob([buildFullCFileContent()], { type: 'text/plain;charset=utf-8' });
    const a = document.createElement('a');
    a.href = URL.createObjectURL(blob);
    a.download = 'smilewatch_ui_custom.c';
    a.click();
    showToast("Fichier smilewatch_ui_custom.c téléchargé !");
}

/* ============================================================================
 * 6. 16x16 1-BIT BITMAP ICON EDITOR
 * ==========================================================================*/
const iconGrid = Array.from({ length: 16 }, () => new Uint8Array(16));

function renderIconEditor() {
    const c = document.getElementById('icon-draw-canvas');
    const ctx = c.getContext('2d');
    const cell = 14;
    ctx.fillStyle = '#040812';
    ctx.fillRect(0, 0, c.width, c.height);

    const bytes = [];
    for (let y = 0; y < 16; y++) {
        let b0 = 0, b1 = 0;
        for (let x = 0; x < 16; x++) {
            if (iconGrid[y][x]) {
                ctx.fillStyle = '#00e5ff';
                ctx.fillRect(x * cell + 1, y * cell + 1, cell - 2, cell - 2);
                if (x < 8) b0 |= (0x80 >> x);
                else       b1 |= (0x80 >> (x - 8));
            }
            ctx.strokeStyle = 'rgba(255,255,255,0.1)';
            ctx.strokeRect(x * cell, y * cell, cell, cell);
        }
        bytes.push(`0x${b0.toString(16).toUpperCase().padStart(2,'0')}`,
                   `0x${b1.toString(16).toUpperCase().padStart(2,'0')}`);
    }

    document.getElementById('icon-c-output').value =
        `static const uint8_t k_custom_icon_16x16[32] = {\n    ` +
        bytes.slice(0, 8).join(', ') + `,\n    ` +
        bytes.slice(8, 16).join(', ') + `,\n    ` +
        bytes.slice(16, 24).join(', ') + `,\n    ` +
        bytes.slice(24, 32).join(', ') + `\n};`;
}

function clearIconGrid() {
    for (let y = 0; y < 16; y++) iconGrid[y].fill(0);
    renderIconEditor();
}

function invertIconGrid() {
    for (let y = 0; y < 16; y++) {
        for (let x = 0; x < 16; x++) iconGrid[y][x] ^= 1;
    }
    renderIconEditor();
}

function previewIconOnWatch() {
    const bmp = new Uint8Array(32);
    for (let y = 0; y < 16; y++) {
        for (let x = 0; x < 16; x++) {
            if (iconGrid[y][x]) bmp[y * 2 + (x >> 3)] |= (0x80 >> (x & 7));
        }
    }
    OLED_DrawBitmap(68, 14, bmp, 16, 16, 1);
    flushOLED();
    showToast("Icône affichée sur l'OLED (x=68, y=14) !");
}

function copyIconCode() {
    navigator.clipboard.writeText(document.getElementById('icon-c-output').value).then(() => {
        showToast("Tableau C copié !");
    });
}

/* ============================================================================
 * 7. INITIALIZATION, DIAL SVG GENERATION & MAIN 20 FPS LOOP
 * ==========================================================================*/
function initDialIndices() {
    const g = document.getElementById('dial-indices');
    let html = '';
    // Exact colors from SolidWorks photo: #4e7a9b outline, #35536b fill
    for (let i = 0; i < 60; i++) {
        const ang = i * 6;
        const isHour = (i % 5 === 0);
        // Cutout at 6h covers indices from 26 to 34 minutes
        if (i >= 26 && i <= 34) {
            // Index 6h is a shorter baton just below the stadium window
            if (i === 30) {
                html += `<rect x="154" y="278" width="6" height="16" rx="1.5" fill="#344f66" stroke="#4f7f9f" stroke-width="1.2"/>`;
            }
            continue;
        }

        if (isHour) {
            // Bold hour batons matching CAD photo
            const w = (i === 0) ? 6.5 : 5.5;
            const h = (i === 0) ? 22 : 19;
            html += `<rect x="${157 - w/2}" y="67" width="${w}" height="${h}" rx="1.5" fill="#344f66" stroke="#4f7f9f" stroke-width="1.2" transform="rotate(${ang}, 157, 180)"/>`;
        } else {
            // Fine minute tick lines
            html += `<line x1="157" y1="68" x2="157" y2="76" stroke="#3d6482" stroke-width="1.2" stroke-linecap="round" transform="rotate(${ang}, 157, 180)"/>`;
        }
    }
    g.innerHTML = html;
}

function switchStudioTab(tabId, btn) {
    document.querySelectorAll('.tab-pane').forEach(p => p.classList.remove('active'));
    document.querySelectorAll('.studio-tabs .tab-btn').forEach(b => b.classList.remove('active'));
    document.getElementById(tabId).classList.add('active');
    btn.classList.add('active');
}

function showToast(msg) {
    const t = document.getElementById('toast');
    t.innerText = msg;
    t.style.display = 'block';
    setTimeout(() => { t.style.display = 'none'; }, 2200);
}

window.addEventListener('DOMContentLoaded', () => {
    initDialIndices();
    loadSelectedCFunction(0);
    renderWatchFace();
    renderCurrentScreen();

    // Seed smiley icon in 16x16 bitmap editor
    const smilePts = [[5,5],[10,5],[5,6],[10,6],[4,10],[5,11],[6,12],[7,12],[8,12],[9,12],[10,11],[11,10]];
    smilePts.forEach(([x,y]) => { iconGrid[y][x] = 1; });
    renderIconEditor();

    // Zoom canvas pixel coordinate inspector
    const zc = document.getElementById('zoom-oled-canvas');
    zc.addEventListener('mousemove', e => {
        const r = zc.getBoundingClientRect();
        const x = Math.min(87, Math.max(0, Math.floor((e.clientX - r.left) / (r.width / 88))));
        const y = Math.min(47, Math.max(0, Math.floor((e.clientY - r.top) / (r.height / 48))));
        const page = y >> 3;
        const bit  = y & 7;
        document.getElementById('loupe-coords').innerText =
            `Curseur : X=${x} Y=${y} | Page=${page} Bit=${bit} (VRAM[${page}][${x}]=0x${s_vram[page][x].toString(16).toUpperCase().padStart(2,'0')})`;
    });

    // Icon editor click handler
    const ic = document.getElementById('icon-draw-canvas');
    ic.addEventListener('mousedown', e => {
        const r = ic.getBoundingClientRect();
        const x = Math.min(15, Math.max(0, Math.floor((e.clientX - r.left) / 14)));
        const y = Math.min(15, Math.max(0, Math.floor((e.clientY - r.top) / 14)));
        iconGrid[y][x] ^= 1;
        renderIconEditor();
    });

    // Keyboard shortcuts (Up = 2h, Down = 4h)
    window.addEventListener('keydown', e => {
        if (document.activeElement.tagName === 'TEXTAREA') return;
        if (e.key === 'ArrowUp')   { e.preventDefault(); triggerButtonEvent('2H_SHORT'); }
        if (e.key === 'ArrowDown') { e.preventDefault(); triggerButtonEvent('4H_SHORT'); }
    });

    logI2C(`BOOT [STM32WB55VGYx] -> Init I2C1 (PB8/PB9), CH1115 (0x3C), Miyota (PA7/PB5)`);

    // 1 Hz RTC Clock & 20 Hz UI/Miyota/Scope Loop
    let lastSecTick = performance.now();
    setInterval(() => {
        const now = performance.now();

        if (now >= g_haptic_active_until) {
            document.getElementById('watch-container').classList.remove('vibrating');
        }

        if (s_miyota.pulse_active && now >= s_miyota.pulse_until_ms) {
            s_miyota.pulse_active = false;
            s_miyota.pulse_polarity = 0; // Lenz Brake
        }

        // Fast-forward catch-up (22 Hz) if hands lag behind RTC target
        if (!s_miyota.pulse_active && s_miyota.analog_pos_sec !== s_miyota.target_pos_sec) {
            if (now - s_miyota.last_step_ms >= 45) {
                stepMiyotaOnce(now);
            }
        }

        // 1 Hz RTC tick
        if (now - lastSecTick >= 1000) {
            lastSecTick += 1000;
            s_state.seconds++;
            if (s_state.seconds >= 60) {
                s_state.seconds = 0;
                s_state.minutes++;
                if (s_state.minutes >= 60) {
                    s_state.minutes = 0;
                    s_state.hours = (s_state.hours + 1) % 24;
                }
            }
            s_miyota.target_pos_sec = ((s_state.hours % 12) * 3600) + (s_state.minutes * 60) + s_state.seconds;
            if (!s_miyota.pulse_active && s_miyota.analog_pos_sec === (s_miyota.target_pos_sec + 43199) % 43200) {
                stepMiyotaOnce(now);
            }
        }

        s_state.ppg_phase = (s_state.ppg_phase + 1) % 22;
        renderCurrentScreen();
        updateOscilloscope(now);
    }, 60);
});

try {
    executeCScreenCode(g_c_screens[0]);
    console.log('SUCCESS on screen 0');
} catch (e) {
    console.error('ERROR on screen 0:', e);
}
