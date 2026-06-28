#include "midi.h"
#include "config.h"
#include "tusb.h"

void midi_init(void) {
    /* TinyUSB initialization is done in main via tusb_init() */
}

static void midi_send(const uint8_t* msg, uint8_t len) {
    if (!tud_midi_mounted()) return;
    tud_midi_stream_write(MIDI_CABLE_NUM, msg, len);
}

void midi_note_on(uint8_t channel, uint8_t note, uint8_t velocity) {
    uint8_t msg[3] = {
        (uint8_t)(0x90 | (channel & 0x0F)),
        (uint8_t)(note & 0x7F),
        (uint8_t)(velocity & 0x7F)
    };
    midi_send(msg, 3);
}

void midi_note_off(uint8_t channel, uint8_t note, uint8_t velocity) {
    uint8_t msg[3] = {
        (uint8_t)(0x80 | (channel & 0x0F)),
        (uint8_t)(note & 0x7F),
        (uint8_t)(velocity & 0x7F)
    };
    midi_send(msg, 3);
}

void midi_clock(void)  { uint8_t m = 0xF8; midi_send(&m, 1); }
void midi_start(void)  { uint8_t m = 0xFA; midi_send(&m, 1); }
void midi_stop(void)   { uint8_t m = 0xFC; midi_send(&m, 1); }

void midi_all_notes_off(void) {
    for (uint8_t ch = 0; ch < 16; ch++) {
        uint8_t msg[3] = { (uint8_t)(0xB0 | ch), 123, 0 };
        midi_send(msg, 3);
    }
}

void midi_task(void) {
    /* Nothing to do — tud_task() in the main loop handles MIDI TX.
       (Older TinyUSB had tud_midi_stream_flush(); removed in newer SDK.) */
}