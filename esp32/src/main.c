#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_pm.h"
#include "rom/ets_sys.h"
#include "pins.h"
#include "display.h"
#include "geiger.h"
#include "click.h"
#include "battery.h"

static const char *TAG = "geiger";

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
static uint16_t prev_dose  = 0xFFFF;
static uint16_t prev_mrem  = 0xFFFF;
static uint32_t prev_total = 0xFFFFFFFF;
static bool     prev_noise = false;
static uint16_t prev_batt  = 0xFFFF;


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
    snprintf(buf, sizeof(buf), "%5u", cpm > 99999 ? 99999 : cpm);

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


static void draw_dose(uint16_t dose_x100)
{
    if (dose_x100 == prev_dose) return;
    prev_dose = dose_x100;

    char buf[10];
    if (dose_x100 > 9999) dose_x100 = 9999;
    snprintf(buf, sizeof(buf), "%2u.%02u", dose_x100 / 100, dose_x100 % 100);

    display_draw_string(DOSE_X, DOSE_Y + 20, buf, COL_YELLOW, COL_BLACK, 2);
}


static void draw_mrem(uint16_t mrem_x1000)
{
    if (mrem_x1000 == prev_mrem) return;
    prev_mrem = mrem_x1000;

    char buf[10];
    if (mrem_x1000 > 9999) mrem_x1000 = 9999;
    snprintf(buf, sizeof(buf), "%1u.%03u", mrem_x1000 / 1000, mrem_x1000 % 1000);

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

    esp_pm_config_t pm_cfg = {
        .max_freq_mhz = 80,
        .min_freq_mhz = 40,
        .light_sleep_enable = false,
    };
    ESP_ERROR_CHECK(esp_pm_configure(&pm_cfg));
    ESP_LOGI(TAG, "DFS enabled: %d–%d MHz", pm_cfg.min_freq_mhz, pm_cfg.max_freq_mhz);

    display_init();
    click_init();
    geiger_init();
    battery_init();

    draw_static_labels();

    ESP_LOGI(TAG, "Ready — waiting for radiation events on GPIO %d", PIN_GEIGER_SIG);
    ESP_LOGI(TAG, "NS pin = GPIO %d, current level = %d", PIN_GEIGER_NS, gpio_get_level(PIN_GEIGER_NS));

    uint32_t loop_count = 0;

    while (1) {
        if (geiger_click_pending()) {
            geiger_click_ack();
            click_start();
            ets_delay_us(1500);
            click_stop();
        }

        uint16_t cpm    = geiger_get_cpm();
        uint16_t dose   = geiger_get_dose_x100();
        bool     noisy  = geiger_is_noisy();
        uint32_t total  = geiger_get_total();

        draw_cpm(cpm);
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
