#pragma once
#include <stdint.h>
#include <stdbool.h>
/*
 * pad.h — FSR pad scanning, velocity detection, and state machine.
 *
 * State machine per pad:
 * IDLE → DEBOUNCE → VELOCITY_WINDOW → HELD → IDLE
 */
typedef enum {
    PAD_STATE_IDLE = 0,
    PAD_STATE_DEBOUNCE = 1,
    PAD_STATE_VELOCITY_WINDOW = 2,
    PAD_STATE_HELD = 3,
} pad_state_t;

typedef struct {
    pad_state_t state;
    uint16_t adc_raw;
    uint16_t peak_adc;
    uint8_t velocity;
    uint8_t note;
    uint8_t debounce_count;
    uint32_t window_start_us;
    bool note_on_sent;
} pad_t;

void pad_init(void);
void pad_scan(void);
uint8_t pad_adc_to_velocity(uint16_t adc_peak);
pad_t* pad_get(uint8_t index);