#pragma once
#include <stdint.h>
/*
 * midi.h — USB MIDI output via TinyUSB.
 */
void midi_init(void);
void midi_note_on(uint8_t channel, uint8_t note, uint8_t velocity);
void midi_note_off(uint8_t channel, uint8_t note, uint8_t velocity);
void midi_clock(void);
void midi_start(void);
void midi_stop(void);
void midi_all_notes_off(void);
void midi_task(void);