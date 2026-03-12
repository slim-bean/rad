#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "pins.h"
#include "geiger.h"

static portMUX_TYPE spinlock = portMUX_INITIALIZER_UNLOCKED;

static volatile uint8_t  bin_counts[GEIGER_WINDOW_SECONDS];
static volatile uint8_t  current_bin;
static volatile uint8_t  current_bin_count;
static volatile uint32_t total_count;
static volatile uint8_t  noise_flag;
static volatile uint8_t  click_flag;
static volatile uint8_t  seconds_elapsed;

static esp_timer_handle_t second_timer;


static void IRAM_ATTR sig_isr(void *arg)
{
    int ns = gpio_get_level(PIN_GEIGER_NS);
    if (ns == 0) {
        portENTER_CRITICAL_ISR(&spinlock);
        if (current_bin_count < 255)
            current_bin_count++;
        total_count++;
        click_flag = 1;
        portEXIT_CRITICAL_ISR(&spinlock);
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


void geiger_init(void)
{
    memset((void *)bin_counts, 0, sizeof(bin_counts));
    current_bin = 0;
    current_bin_count = 0;
    total_count = 0;
    noise_flag = 0;
    click_flag = 0;
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
}


uint16_t geiger_get_cpm(void)
{
    uint16_t sum = 0;
    uint8_t elapsed;

    portENTER_CRITICAL(&spinlock);
    elapsed = seconds_elapsed;
    for (int i = 0; i < GEIGER_WINDOW_SECONDS; i++)
        sum += bin_counts[i];
    sum += current_bin_count;
    portEXIT_CRITICAL(&spinlock);

    if (elapsed == 0)
        return 0;
    if (elapsed < GEIGER_WINDOW_SECONDS)
        return (uint16_t)(((uint32_t)sum * 60UL) / elapsed);
    return sum;
}


uint16_t geiger_get_dose_x100(void)
{
    uint16_t cpm = geiger_get_cpm();
    return (uint16_t)(((uint32_t)cpm * 100UL) / GEIGER_CPM_PER_USV);
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


bool geiger_click_pending(void)
{
    return click_flag != 0;
}


void geiger_click_ack(void)
{
    click_flag = 0;
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
