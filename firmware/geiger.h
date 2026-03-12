#ifndef GEIGER_H
#define GEIGER_H

#include <stdint.h>
#include <stdbool.h>

/*
 * Radiation Watch Pocket Geiger Type 5 driver.
 *
 * SIG (radiation pulse)  → PE4  (active-LOW, ~100 µs pulse)
 * NS  (noise/vibration)  → PE5  (active-LOW while noisy)
 *
 * Both pins use PCINT0 (PORTE pin-change interrupt), shared
 * with the joystick LEFT/RIGHT on PE2/PE3.
 *
 * CPM is computed over a 60-second sliding window (60 × 1 s bins).
 * Dose rate uses the Type 5 sensitivity factor for Cs-137:
 *   µSv/h = CPM / 53.032
 */

#define GEIGER_SIG_PIN  PE4
#define GEIGER_NS_PIN   PE5

#define GEIGER_WINDOW_SECONDS  60

#define GEIGER_CPM_PER_USV  53

void     geiger_init(void);
void     geiger_pin_isr(void);
void     geiger_second_tick(void);

uint16_t geiger_get_cpm(void);
uint16_t geiger_get_dose_x100(void);
uint32_t geiger_get_total(void);
bool     geiger_is_noisy(void);

bool     geiger_click_pending(void);
void     geiger_click_ack(void);

void     geiger_reset(void);

#endif
