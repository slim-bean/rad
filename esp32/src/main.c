#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_pm.h"
#include "esp_system.h"
#include "pins.h"
#include "display.h"
#include "geiger.h"
#include "click.h"
#include "battery.h"

static const char *TAG = "geiger";

#define APP_CPU_FREQ_MHZ      160
#define RESET_ACK_POLL_MS     20
#define RESET_ACK_DEBOUNCE_MS 40

/* ------------------------------------------------------------------ */
/*  Layout constants for 320×240 landscape                             */
/* ------------------------------------------------------------------ */
#define CPM_X       20
#define CPM_Y       10
#define CPM_SCALE   2       /* 28×48 base × 2 = 56×96 per digit */
#define CPM_DIG_W   (28 * CPM_SCALE)
#define CPM_DIG_GAP 4
#define CPM_LABEL_X 20
#define CPM_LABEL_Y 108

#define DOSE_X      20
#define DOSE_Y      134
#define MREM_X      180
#define MREM_Y      134

#define BAR_X       20
#define BAR_Y       172
#define BAR_W       280
#define BAR_H       16
#define BAR_MAX_CPM 200

#define INFO_X      20
#define INFO_Y      210

#define NOISE_X     200
#define NOISE_Y     108

#define BATT_X      240
#define BATT_Y      210

/* ------------------------------------------------------------------ */
/*  Screen state — only redraw values that changed                     */
/* ------------------------------------------------------------------ */
static uint16_t prev_cpm   = 0xFFFF;
static uint32_t prev_dose  = 0xFFFFFFFF;
static uint32_t prev_mrem  = 0xFFFFFFFF;
static uint32_t prev_total = 0xFFFFFFFF;
static bool     prev_noise = false;
static uint16_t prev_batt  = 0xFFFF;
static uint8_t  prev_win   = 0xFF;


static const char *reset_reason_name(esp_reset_reason_t reason)
{
    switch (reason) {
    case ESP_RST_POWERON:    return "POWER ON";
    case ESP_RST_SW:         return "SOFTWARE";
    case ESP_RST_PANIC:      return "PANIC";
    case ESP_RST_INT_WDT:    return "INT WDT";
    case ESP_RST_TASK_WDT:   return "TASK WDT";
    case ESP_RST_WDT:        return "WDT";
    case ESP_RST_BROWNOUT:   return "BROWNOUT";
    case ESP_RST_PWR_GLITCH: return "PWR GLITCH";
    case ESP_RST_CPU_LOCKUP: return "CPU LOCKUP";
    case ESP_RST_DEEPSLEEP:  return "DEEPSLEEP";
    case ESP_RST_UNKNOWN:    return "UNKNOWN";
    default:                 return "OTHER";
    }
}


static bool should_show_reset_notice(esp_reset_reason_t reason)
{
    switch (reason) {
    case ESP_RST_PANIC:
    case ESP_RST_INT_WDT:
    case ESP_RST_TASK_WDT:
    case ESP_RST_WDT:
    case ESP_RST_BROWNOUT:
    case ESP_RST_PWR_GLITCH:
    case ESP_RST_CPU_LOCKUP:
        return true;
    default:
        return false;
    }
}


static void ack_button_init(void)
{
    gpio_config_t btn_cfg = {
        .pin_bit_mask = (1ULL << PIN_ACK_BUTTON),
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    ESP_ERROR_CHECK(gpio_config(&btn_cfg));
}


static bool ack_button_pressed(void)
{
    return gpio_get_level(PIN_ACK_BUTTON) == 0;
}


static void show_reset_notice_if_needed(esp_reset_reason_t reason)
{
    if (!should_show_reset_notice(reason))
        return;

    display_fill_rect(0, 0, TFT_WIDTH, TFT_HEIGHT, COL_BLACK);
    display_draw_string(24, 24, "RESET DETECTED", COL_RED, COL_BLACK, 2);
    display_draw_string(24, 72, "REASON", COL_GREY, COL_BLACK, 2);
    display_draw_string(24, 96, reset_reason_name(reason), COL_YELLOW, COL_BLACK, 2);
    display_draw_string(24, 152, "PRESS BTN GPIO27", COL_WHITE, COL_BLACK, 2);
    display_draw_string(24, 176, "TO CONTINUE", COL_WHITE, COL_BLACK, 2);

    for (;;) {
        if (ack_button_pressed()) {
            vTaskDelay(pdMS_TO_TICKS(RESET_ACK_DEBOUNCE_MS));
            if (ack_button_pressed()) {
                while (ack_button_pressed())
                    vTaskDelay(pdMS_TO_TICKS(RESET_ACK_POLL_MS));
                break;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(RESET_ACK_POLL_MS));
    }
}


static void draw_static_labels(void)
{
    display_fill_rect(0, 0, TFT_WIDTH, TFT_HEIGHT, COL_BLACK);

    display_draw_string(CPM_LABEL_X, CPM_LABEL_Y, "CPM", COL_GREY, COL_BLACK, 2);

    display_draw_string(DOSE_X, DOSE_Y, "uSv/h", COL_GREY, COL_BLACK, 2);
    display_draw_string(MREM_X, MREM_Y, "mrem/h", COL_GREY, COL_BLACK, 2);

    display_fill_rect(BAR_X - 2, BAR_Y - 2, BAR_W + 4, BAR_H + 4, COL_GREY);
    display_fill_rect(BAR_X, BAR_Y, BAR_W, BAR_H, COL_BLACK);

    display_draw_string(INFO_X, INFO_Y, "N:", COL_GREY, COL_BLACK, 2);
}


static void draw_cpm(uint16_t cpm)
{
    if (cpm == prev_cpm) return;
    prev_cpm = cpm;

    char buf[6];
    snprintf(buf, sizeof(buf), "%5u", (unsigned)cpm);

    uint16_t color = COL_GREEN;
    if (cpm > 100) color = COL_YELLOW;
    if (cpm > 1000) color = COL_RED;

    int x = CPM_X;
    for (int i = 0; i < 5; i++) {
        if (buf[i] == ' ')
            display_fill_rect(x, CPM_Y, CPM_DIG_W, 48 * CPM_SCALE, COL_BLACK);
        else
            display_draw_7seg(x, CPM_Y, buf[i] - '0', color, COL_BLACK, CPM_SCALE);
        x += CPM_DIG_W + CPM_DIG_GAP;
    }
}


static void draw_dose(uint32_t dose_x100)
{
    if (dose_x100 == prev_dose) return;
    prev_dose = dose_x100;

    char buf[10];
    if (dose_x100 > 99999) dose_x100 = 99999;
    snprintf(buf, sizeof(buf), "%3lu.%02lu", (unsigned long)(dose_x100 / 100), (unsigned long)(dose_x100 % 100));

    display_draw_string(DOSE_X, DOSE_Y + 20, buf, COL_YELLOW, COL_BLACK, 2);
}


static void draw_mrem(uint32_t mrem_x1000)
{
    if (mrem_x1000 == prev_mrem) return;
    prev_mrem = mrem_x1000;

    char buf[10];
    if (mrem_x1000 > 99999) mrem_x1000 = 99999;
    snprintf(buf, sizeof(buf), "%2lu.%03lu", (unsigned long)(mrem_x1000 / 1000), (unsigned long)(mrem_x1000 % 1000));

    display_draw_string(MREM_X, MREM_Y + 20, buf, COL_YELLOW, COL_BLACK, 2);
}


static void draw_bar(uint16_t cpm)
{
    int filled = (cpm * BAR_W) / BAR_MAX_CPM;
    if (filled > BAR_W) filled = BAR_W;

    uint16_t color = COL_GREEN;
    if (cpm > 100) color = COL_YELLOW;
    if (cpm > 1000) color = COL_RED;

    if (filled > 0)
        display_fill_rect(BAR_X, BAR_Y, filled, BAR_H, color);
    if (filled < BAR_W)
        display_fill_rect(BAR_X + filled, BAR_Y, BAR_W - filled, BAR_H, COL_BLACK);
}


static void draw_total(uint32_t total)
{
    if (total == prev_total) return;
    prev_total = total;

    char buf[12];
    snprintf(buf, sizeof(buf), "%-10lu", (unsigned long)(total > 9999999UL ? 9999999UL : total));

    display_draw_string(INFO_X + 32, INFO_Y, buf, COL_WHITE, COL_BLACK, 2);
}


static void draw_noise(bool noisy)
{
    if (noisy == prev_noise) return;
    prev_noise = noisy;

    if (noisy)
        display_draw_string(NOISE_X, NOISE_Y, " NOISE ", COL_BLACK, COL_RED, 2);
    else
        display_fill_rect(NOISE_X, NOISE_Y, 7 * 16, 16, COL_BLACK);
}


static void draw_window(uint8_t win)
{
    if (win == prev_win) return;
    prev_win = win;

    char buf[6];
    snprintf(buf, sizeof(buf), "%2us", win);
    display_draw_string(CPM_LABEL_X + 56, CPM_LABEL_Y, buf, COL_GREY, COL_BLACK, 2);
}


static void draw_battery(uint16_t mv)
{
    uint16_t rounded = (mv / 10) * 10;
    if (rounded == prev_batt) return;
    prev_batt = rounded;

    uint16_t color = COL_GREEN;
    if (mv < 3500) color = COL_YELLOW;
    if (mv < 3300) color = COL_RED;

    char buf[8];
    snprintf(buf, sizeof(buf), "%u.%02uV", mv / 1000, (mv % 1000) / 10);

    display_draw_string(BATT_X, BATT_Y, buf, color, COL_BLACK, 2);
}


void app_main(void)
{
    ESP_LOGI(TAG, "Geiger counter starting");

    esp_reset_reason_t reset_reason = esp_reset_reason();
    ack_button_init();

    esp_pm_config_t pm_cfg = {
        .max_freq_mhz = APP_CPU_FREQ_MHZ,
        .min_freq_mhz = APP_CPU_FREQ_MHZ,
        .light_sleep_enable = false,
    };
    ESP_ERROR_CHECK(esp_pm_configure(&pm_cfg));
    ESP_LOGI(TAG, "CPU fixed at %d MHz", APP_CPU_FREQ_MHZ);
    ESP_LOGI(TAG, "Last reset reason: %s", reset_reason_name(reset_reason));

    display_init();
    show_reset_notice_if_needed(reset_reason);
    click_init();
    geiger_init();
    battery_init();

    draw_static_labels();

    ESP_LOGI(TAG, "Ready — waiting for radiation events on GPIO %d", PIN_GEIGER_SIG);
    ESP_LOGI(TAG, "NS pin = GPIO %d, current level = %d", PIN_GEIGER_NS, gpio_get_level(PIN_GEIGER_NS));

    uint32_t loop_count = 0;

    while (1) {
        uint16_t cpm    = geiger_get_cpm();
        uint8_t  win    = geiger_get_window();
        uint32_t dose   = geiger_get_dose_x100();
        bool     noisy  = geiger_is_noisy();
        uint32_t total  = geiger_get_total();

        draw_cpm(cpm);
        draw_window(win);
        draw_dose(dose);
        draw_mrem(dose);    /* mrem_x1000 == dose_x100 numerically */
        draw_bar(cpm);
        draw_total(total);
        draw_noise(noisy);

        if (++loop_count % 8 == 0)    /* ~2 s */
            draw_battery(battery_read_mv());

        if (loop_count % 20 == 0)     /* ~5 s */
            ESP_LOGI(TAG, "cpm=%u total=%lu noise=%d ns_pin=%d",
                     cpm, (unsigned long)total, noisy,
                     gpio_get_level(PIN_GEIGER_NS));

        vTaskDelay(pdMS_TO_TICKS(250));
    }
}
