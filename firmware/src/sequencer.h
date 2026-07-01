#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "config.h"
/*
 * sequencer.h — 16-step MIDI step sequencer.
 */
#define SEQ_PPQN 24 /* Pulses per quarter note (MIDI standard) */

typedef struct {
    uint8_t note;
    uint8_t velocity;
    bool active;
} seq_step_t;

typedef enum {
    SEQ_MODE_STOPPED = 0,
    SEQ_MODE_PLAYING = 1,
    SEQ_MODE_RECORDING = 2,
} seq_mode_t;

typedef struct {
    seq_step_t steps[SEQ_STEPS];
    uint8_t current_step;
    uint8_t record_step;
    uint8_t bpm;
    seq_mode_t mode;
} sequencer_t;

void sequencer_init(void);
void sequencer_tick(void);
void sequencer_play(void);
void sequencer_stop(void);
void sequencer_record_start(void);
void sequencer_record_pad(uint8_t pad_index, uint8_t velocity);
void sequencer_set_bpm(uint8_t bpm);
void sequencer_clear(void);
sequencer_t* sequencer_get(void);