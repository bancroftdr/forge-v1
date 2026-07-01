#include "pad.h"
#include "mux.h"
#include "midi.h"
#include "config.h"
#include "hardware/adc.h"
#include "pico/time.h"
#include "pico/stdlib.h"
#include <math.h>
#include <string.h>

/* ── GM drum note mapping (pad 0 = bottom-left, pad 15 = top-right) */
static const uint8_t PAD_NOTE_MAP[NUM_PADS] = {
    /* Row 3 — bottom row (pads 0–3) */
    36, 38, 40, 42,
    /* Row 2 (pads 4–7) */
    44, 46, 49, 51,
    /* Row 1 (pads 8–11) */
    41, 43, 45, 47,
    /* Row 0 — top row (pads 12–15) */
    48, 50, 52, 55,
};

static pad_t pads[NUM_PADS];

uint8_t pad_adc_to_velocity(uint16_t adc_peak) {
    if (adc_peak <= VELOCITY_THRESHOLD) return 1;
    if (adc_peak >= VELOCITY_MAX_ADC) return 127;
    float range = (float)(VELOCITY_MAX_ADC - VELOCITY_THRESHOLD);
    float norm = (float)(adc_peak - VELOCITY_THRESHOLD) / range;
    float curve = logf(1.0f + norm * (float)(M_E - 1.0));
    uint8_t vel = (uint8_t)(curve * 127.0f);
    if (vel < 1) vel = 1;
    if (vel > 127) vel = 127;
    return vel;
}

void pad_init(void) {
    adc_init();
    adc_gpio_init(ADC_GPIO_PIN);
    adc_select_input(ADC_CHANNEL);
    memset(pads, 0, sizeof(pads));
    for (uint8_t i = 0; i < NUM_PADS; i++) {
        pads[i].state = PAD_STATE_IDLE;
        pads[i].note = PAD_NOTE_MAP[i];
    }
    mux_enable();
}

static uint16_t read_pad_adc(uint8_t pad_index) {
    mux_select(pad_index);
    sleep_us(10);
    return adc_read();
}

void pad_scan(void) {
    uint32_t now_us = time_us_32();
    for (uint8_t i = 0; i < NUM_PADS; i++) {
        pad_t* p = &pads[i];
        p->adc_raw = read_pad_adc(i);

        switch (p->state) {
            case PAD_STATE_IDLE:
                if (p->adc_raw > VELOCITY_THRESHOLD) {
                    p->debounce_count = 1;
                    p->state = PAD_STATE_DEBOUNCE;
                }
                break;

            case PAD_STATE_DEBOUNCE:
                if (p->adc_raw > VELOCITY_THRESHOLD) {
                    p->debounce_count++;
                    if (p->debounce_count >= DEBOUNCE_SAMPLES) {
                        p->peak_adc = p->adc_raw;
                        p->window_start_us = now_us;
                        p->state = PAD_STATE_VELOCITY_WINDOW;
                    }
                } else {
                    p->debounce_count = 0;
                    p->state = PAD_STATE_IDLE;
                }
                break;

            case PAD_STATE_VELOCITY_WINDOW:
                if (p->adc_raw > p->peak_adc) {
                    p->peak_adc = p->adc_raw;
                }
                if ((now_us - p->window_start_us) >=
                    (uint32_t)(VELOCITY_WINDOW_MS * 1000)) {
                    p->velocity = pad_adc_to_velocity(p->peak_adc);
                    p->note_on_sent = true;
                    p->state = PAD_STATE_HELD;
                    midi_note_on(MIDI_CHANNEL, p->note, p->velocity);
                }
                break;

            case PAD_STATE_HELD:
                if (p->adc_raw < (VELOCITY_THRESHOLD - VELOCITY_HYSTERESIS)) {
                    midi_note_off(MIDI_CHANNEL, p->note, 0);
                    p->note_on_sent = false;
                    p->peak_adc = 0;
                    p->debounce_count = 0;
                    p->state = PAD_STATE_IDLE;
                }
                break;
        }
    }
}

pad_t* pad_get(uint8_t index) {
    if (index >= NUM_PADS) return NULL;
    return &pads[index];
}