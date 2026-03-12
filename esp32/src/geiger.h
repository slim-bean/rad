#pragma once

#include <stdint.h>
#include <stdbool.h>

#define GEIGER_WINDOW_SECONDS  60
#define GEIGER_CPM_PER_USV     53

void     geiger_init(void);
uint16_t geiger_get_cpm(void);
uint16_t geiger_get_dose_x100(void);
uint32_t geiger_get_total(void);
bool     geiger_is_noisy(void);
bool     geiger_click_pending(void);
void     geiger_click_ack(void);
void     geiger_reset(void);
