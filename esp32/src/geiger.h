#pragma once

#include <stdint.h>
#include <stdbool.h>

#define GEIGER_WINDOW_SECONDS  60
#define GEIGER_SHORT_WINDOW    5
#define GEIGER_CPM_PER_USV     53

void     geiger_init(void);
uint16_t geiger_get_cpm(void);
uint32_t geiger_get_dose_x100(void);
uint32_t geiger_get_total(void);
bool     geiger_is_noisy(void);
uint8_t  geiger_get_window(void);
void     geiger_reset(void);
