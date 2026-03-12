#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_check.h"
#include "esp_log.h"
#include "pins.h"
#include "battery.h"

static const char *TAG = "battery";

#define VBAT_ADC_UNIT    ADC_UNIT_1
#define VBAT_ADC_CHAN    ADC_CHANNEL_6   /* GPIO34 */
#define VBAT_ATTEN       ADC_ATTEN_DB_12
#define VBAT_SAMPLES     16
#define VBAT_DIVIDER_NUM 2               /* FireBeetle 2 onboard 100k/100k */

static adc_oneshot_unit_handle_t adc_handle;
static adc_cali_handle_t         cali_handle;
static bool                      cali_ok;


void battery_init(void)
{
    adc_oneshot_unit_init_cfg_t unit_cfg = {
        .unit_id = VBAT_ADC_UNIT,
    };
    ESP_ERROR_CHECK(adc_oneshot_new_unit(&unit_cfg, &adc_handle));

    adc_oneshot_chan_cfg_t chan_cfg = {
        .bitwidth = ADC_BITWIDTH_DEFAULT,
        .atten    = VBAT_ATTEN,
    };
    ESP_ERROR_CHECK(adc_oneshot_config_channel(adc_handle, VBAT_ADC_CHAN, &chan_cfg));

    adc_cali_line_fitting_config_t cali_cfg = {
        .unit_id  = VBAT_ADC_UNIT,
        .atten    = VBAT_ATTEN,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    esp_err_t err = adc_cali_create_scheme_line_fitting(&cali_cfg, &cali_handle);
    cali_ok = (err == ESP_OK);
    if (!cali_ok)
        ESP_LOGW(TAG, "ADC calibration unavailable (0x%x), using raw values", err);

    ESP_LOGI(TAG, "ADC1 CH%d ready (GPIO %d), divider ratio %d:1",
             VBAT_ADC_CHAN, PIN_VBAT, VBAT_DIVIDER_NUM);
}


uint16_t battery_read_mv(void)
{
    int32_t sum = 0;

    for (int i = 0; i < VBAT_SAMPLES; i++) {
        int raw;
        if (adc_oneshot_read(adc_handle, VBAT_ADC_CHAN, &raw) == ESP_OK) {
            if (cali_ok) {
                int mv;
                adc_cali_raw_to_voltage(cali_handle, raw, &mv);
                sum += mv;
            } else {
                sum += (raw * 3300) / 4095;
            }
        }
    }

    uint16_t pin_mv = (uint16_t)(sum / VBAT_SAMPLES);
    return pin_mv * VBAT_DIVIDER_NUM;
}
