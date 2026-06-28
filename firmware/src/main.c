/*
 * main.c — FORGE v1 firmware.
 *
 * ⚠ DAY 3 MINIMAL BUILD ⚠
 * This is a temporary, stripped-down main for proving USB MIDI works
 * in isolation. It sends one test note every 2 seconds — no pads,
 * no mux, no sequencer yet.
 *
 * When the breadboard is wired, restore the full main from the master
 * doc §6.14: add mux/pad/sequencer includes + init calls, replace the
 * test-note loop below with pad_scan() / sequencer_tick().
 */
#include "pico/stdlib.h"
#include "bsp/board.h"
#include "tusb.h"

#include "config.h"
#include "midi.h"

int main(void) {
    board_init();
    stdio_init_all();

    midi_init();
    tusb_init();          /* USB stack — must come after board_init() */

    uint32_t last_note_us = 0;
    bool note_is_on = false;

    while (true) {
        tud_task();       /* TinyUSB housekeeping — call every loop */
        midi_task();      /* Flush MIDI TX buffer */

        /* ── Temporary test note: toggle note 60 every 2 seconds ── */
        uint32_t now = time_us_32();
        if ((now - last_note_us) >= 2000000) {
            last_note_us = now;
            if (!note_is_on) {
                midi_note_on(MIDI_CHANNEL, 60, 100);
                note_is_on = true;
            } else {
                midi_note_off(MIDI_CHANNEL, 60, 0);
                note_is_on = false;
            }
        }
    }
    return 0;
}