#include "sequencer.h"
#include "midi.h"
#include "config.h"
#include "pico/time.h"
#include <string.h>

static const uint8_t DEFAULT_NOTES[SEQ_STEPS] = {
    36, 38, 40, 42, 44, 46, 49, 51,
    41, 43, 45, 47, 48, 50, 52, 55,
};

static sequencer_t seq;
static uint32_t last_step_us = 0;
static uint32_t last_clock_us = 0;
static uint32_t step_interval_us = 0;
static uint32_t clock_interval_us = 0;

static uint32_t bpm_to_step_us(uint8_t bpm) {
    return 60000000UL / (uint32_t)bpm / 4UL;
}

static uint32_t bpm_to_clock_us(uint8_t bpm) {
    return 60000000UL / (uint32_t)bpm / (uint32_t)SEQ_PPQN;
}

void sequencer_init(void) {
    memset(&seq, 0, sizeof(seq));
    seq.bpm = SEQ_DEFAULT_BPM;
    seq.mode = SEQ_MODE_STOPPED;
    sequencer_clear();
    step_interval_us = bpm_to_step_us(seq.bpm);
    clock_interval_us = bpm_to_clock_us(seq.bpm);
}

void sequencer_clear(void) {
    for (int i = 0; i < SEQ_STEPS; i++) {
        seq.steps[i].note = DEFAULT_NOTES[i];
        seq.steps[i].velocity = 100;
        seq.steps[i].active = false;
    }
}

void sequencer_play(void) {
    if (seq.mode != SEQ_MODE_STOPPED) return;
    seq.mode = SEQ_MODE_PLAYING;
    seq.current_step = 0;
    last_step_us = time_us_32();
    last_clock_us = last_step_us;
    midi_start();
}

void sequencer_stop(void) {
    if (seq.mode == SEQ_MODE_STOPPED) return;
    seq.mode = SEQ_MODE_STOPPED;
    midi_stop();
    midi_all_notes_off();
    seq.current_step = 0;
}

void sequencer_record_start(void) {
    seq.mode = SEQ_MODE_RECORDING;
    seq.record_step = 0;
}

void sequencer_record_pad(uint8_t pad_index, uint8_t velocity) {
    if (seq.mode != SEQ_MODE_RECORDING) return;
    if (pad_index >= SEQ_STEPS) return;
    seq.steps[seq.record_step].note = DEFAULT_NOTES[pad_index];
    seq.steps[seq.record_step].velocity = velocity;
    seq.steps[seq.record_step].active = true;
    seq.record_step = (seq.record_step + 1) % SEQ_STEPS;
}

void sequencer_set_bpm(uint8_t bpm) {
    if (bpm < SEQ_MIN_BPM) bpm = SEQ_MIN_BPM;
    if (bpm > SEQ_MAX_BPM) bpm = SEQ_MAX_BPM;
    seq.bpm = bpm;
    step_interval_us = bpm_to_step_us(bpm);
    clock_interval_us = bpm_to_clock_us(bpm);
}

void sequencer_tick(void) {
    if (seq.mode == SEQ_MODE_STOPPED) return;

    uint32_t now = time_us_32();

    if ((now - last_clock_us) >= clock_interval_us) {
        last_clock_us += clock_interval_us;
        midi_clock();
    }

    if ((now - last_step_us) >= step_interval_us) {
        last_step_us += step_interval_us;

        uint8_t prev = (seq.current_step == 0)
            ? SEQ_STEPS - 1
            : seq.current_step - 1;
        if (seq.steps[prev].active) {
            midi_note_off(MIDI_CHANNEL, seq.steps[prev].note, 0);
        }

        if (seq.steps[seq.current_step].active) {
            midi_note_on(MIDI_CHANNEL,
                seq.steps[seq.current_step].note,
                seq.steps[seq.current_step].velocity);
        }

        seq.current_step = (seq.current_step + 1) % SEQ_STEPS;
    }
}

sequencer_t* sequencer_get(void) {
    return &seq;
}