/*
 * main.c — FORGE v1 firmware.
 *
 * ⚠ DAY 4 BUTTON TEST ⚠
 * Reads a single pushbutton on GP15 (internal pull-up) and sends
 * a MIDI note on press / note-off on release. Proves the full
 * chain: physical input → firmware → USB MIDI → DAW sound.
 *
 * When the breadboard mux/FSR grid is wired, restore the full main
 * from master doc §6.14 (mux/pad/sequencer init + pad_scan()).
 */
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "bsp/board.h"
#include "tusb.h"

#include "config.h"
#include "midi.h"

#define BUTTON_PIN   15
#define TEST_NOTE    36   /* GM Bass Drum — change to taste */

int main(void) {
    board_init();
    stdio_init_all();

    /* Button input with internal pull-up:
       not pressed = HIGH (1), pressed = LOW (0) */
    gpio_init(BUTTON_PIN);
    gpio_set_dir(BUTTON_PIN, GPIO_IN);
    gpio_pull_up(BUTTON_PIN);

    midi_init();
    tusb_init();          /* USB stack — after board_init() */

    bool note_is_on = false;

    while (true) {
        tud_task();       /* TinyUSB housekeeping — every loop */
        midi_task();

        /* Active-low: pressed when the pin reads 0 */
        bool pressed = (gpio_get(BUTTON_PIN) == 0);

        if (pressed && !note_is_on) {
            midi_note_on(MIDI_CHANNEL, TEST_NOTE, 100);
            note_is_on = true;
        } else if (!pressed && note_is_on) {
            midi_note_off(MIDI_CHANNEL, TEST_NOTE, 0);
            note_is_on = false;
        }

        sleep_ms(1);      /* Light debounce + CPU breather */
    }
    return 0;
}