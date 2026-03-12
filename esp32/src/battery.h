#pragma once

#include <stdint.h>

void     battery_init(void);
uint16_t battery_read_mv(void);
