#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "rom/ets_sys.h"
#include "pins.h"
#include "geiger.h"
#include "click.h"

static portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;

static volatile uint8_t  bin_counts[GEIGER_WINDOW_SECONDS];
static volatile uint8_t  current_bin;
static volatile uint8_t  current_bin_count;
static volatile uint32_t total_count;
static volatile uint8_t  noise_flag;
static volatile uint8_t  seconds_elapsed;

static TaskHandle_t click_task_handle;
static uint8_t last_window;

static esp_timer_handle_t second_timer;


static void IRAM_ATTR sig_isr(void *arg)
{
    int ns = gpio_get_level(PIN_GEIGER_NS);
    if (ns == 0) {
        portENTER_CRITICAL_ISR(&spinlock);
        if (current_bin_count < 255)
            current_bin_count++;
        total_count++;
        portEXIT_CRITICAL_ISR(&spinlock);

        BaseType_t woken = pdFALSE;
        vTaskNotifyGiveFromISR(click_task_handle, &woken);
        portYIELD_FROM_ISR(woken);
    }
    noise_flag = (ns != 0);
}


static void second_tick(void *arg)
{
    portENTER_CRITICAL(&spinlock);
    current_bin = (current_bin + 1) % GEIGER_WINDOW_SECONDS;
    bin_counts[current_bin] = current_bin_count;
    current_bin_count = 0;
    if (seconds_elapsed < GEIGER_WINDOW_SECONDS)
        seconds_elapsed++;
    portEXIT_CRITICAL(&spinlock);
}


static void click_task(void *arg)
{
    while (1) {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        click_start();
        vTaskDelay(pdMS_TO_TICKS(2));
        click_stop();
    }
}


void geiger_init(void)
{
    memset((void *)bin_counts, 0, sizeof(bin_counts));
    current_bin = 0;
    current_bin_count = 0;
    total_count = 0;
    noise_flag = 0;
    seconds_elapsed = 0;

    gpio_config_t sig_cfg = {
        .pin_bit_mask = (1ULL << PIN_GEIGER_SIG),
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_NEGEDGE,
    };
    gpio_config(&sig_cfg);

    gpio_config_t ns_cfg = {
        .pin_bit_mask = (1ULL << PIN_GEIGER_NS),
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    gpio_config(&ns_cfg);

    gpio_install_isr_service(ESP_INTR_FLAG_IRAM);
    gpio_isr_handler_add(PIN_GEIGER_SIG, sig_isr, NULL);

    const esp_timer_create_args_t timer_args = {
        .callback = second_tick,
        .name     = "geiger_1hz",
    };
    esp_timer_create(&timer_args, &second_timer);
    esp_timer_start_periodic(second_timer, 1000000);

    xTaskCreate(click_task, "click", 2048, NULL, 5, &click_task_handle);
}


uint16_t geiger_get_cpm(void)
{
    uint16_t long_sum = 0;
    uint16_t short_sum = 0;
    uint8_t elapsed;
    uint8_t cb;

    portENTER_CRITICAL(&spinlock);
    elapsed = seconds_elapsed;
    cb = current_bin;
    for (int i = 0; i < GEIGER_WINDOW_SECONDS; i++)
        long_sum += bin_counts[i];
    long_sum += current_bin_count;

    /* Sum the most recent GEIGER_SHORT_WINDOW bins + current partial */
    short_sum = current_bin_count;
    uint8_t short_bins = (elapsed < GEIGER_SHORT_WINDOW) ? elapsed : GEIGER_SHORT_WINDOW;
    for (uint8_t i = 0; i < short_bins; i++) {
        uint8_t idx = (cb + GEIGER_WINDOW_SECONDS - i) % GEIGER_WINDOW_SECONDS;
        short_sum += bin_counts[idx];
    }
    portEXIT_CRITICAL(&spinlock);

    if (elapsed == 0) {
        last_window = 0;
        return 0;
    }

    /* Not enough data yet — just extrapolate what we have */
    if (elapsed < GEIGER_SHORT_WINDOW) {
        last_window = elapsed;
        return (uint16_t)(((uint32_t)long_sum * 60UL) / elapsed);
    }

    uint16_t long_cpm  = (elapsed < GEIGER_WINDOW_SECONDS)
                       ? (uint16_t)(((uint32_t)long_sum * 60UL) / elapsed)
                       : long_sum;
    uint16_t short_cpm = (uint16_t)(((uint32_t)short_sum * 60UL) / (short_bins + 1));

    /* If short-term differs from long-term by more than 50%, use short-term */
    uint32_t diff = (short_cpm > long_cpm)
                  ? (short_cpm - long_cpm)
                  : (long_cpm - short_cpm);
    if (long_cpm > 0 && diff * 2 > long_cpm) {
        last_window = GEIGER_SHORT_WINDOW;
        return short_cpm;
    }

    last_window = (elapsed < GEIGER_WINDOW_SECONDS) ? elapsed : GEIGER_WINDOW_SECONDS;
    return long_cpm;
}


uint32_t geiger_get_dose_x100(void)
{
    uint16_t cpm = geiger_get_cpm();
    return ((uint32_t)cpm * 100UL) / GEIGER_CPM_PER_USV;
}


uint32_t geiger_get_total(void)
{
    uint32_t t;
    portENTER_CRITICAL(&spinlock);
    t = total_count;
    portEXIT_CRITICAL(&spinlock);
    return t;
}


bool geiger_is_noisy(void)
{
    return noise_flag != 0;
}


uint8_t geiger_get_window(void)
{
    return last_window;
}


void geiger_reset(void)
{
    portENTER_CRITICAL(&spinlock);
    memset((void *)bin_counts, 0, sizeof(bin_counts));
    current_bin = 0;
    current_bin_count = 0;
    total_count = 0;
    seconds_elapsed = 0;
    portEXIT_CRITICAL(&spinlock);
}
