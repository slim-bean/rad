idf_component_register(
    SRCS "main.c" "display.c" "geiger.c" "click.c" "battery.c"
    INCLUDE_DIRS "."
    REQUIRES driver esp_adc esp_lcd esp_pm esp_timer
)
