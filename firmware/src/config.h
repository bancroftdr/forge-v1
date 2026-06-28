#pragma once
/*
 * config.h — All compile-time constants for the FORGE firmware.
 * Change values here; never scatter magic numbers through source files.
 */

/* ── Pad hardware ─────────────────────────────────────────────── */
#define NUM_PADS                16
#define PAD_ROWS                4
#define PAD_COLS                4

/* ── Multiplexer GPIO (CD74HC4067) ───────────────────────────── */
#define MUX_S0_PIN              2    /* Select bit 0 */
#define MUX_S1_PIN              3    /* Select bit 1 */
#define MUX_S2_PIN              4    /* Select bit 2 */
#define MUX_S3_PIN              5    /* Select bit 3 */
#define MUX_EN_PIN              6    /* Enable, active LOW */

/* ── ADC ─────────────────────────────────────────────────────── */
#define ADC_GPIO_PIN            26   /* GP26 = ADC0 */
#define ADC_CHANNEL             0
#define ADC_MAX                 4095 /* 12-bit */
#define ADC_VREF_MV             3300 /* mV */

/* ── Velocity detection ──────────────────────────────────────── */
/* ADC count below which a pad is considered unpressed */
#define VELOCITY_THRESHOLD      500
/* ADC count representing maximum velocity (127) */
#define VELOCITY_MAX_ADC        3900
/* Hysteresis: pad must drop below (THRESHOLD - HYSTERESIS) to release */
#define VELOCITY_HYSTERESIS     100
/* Time after threshold crossing to track ADC peak, in ms */
#define VELOCITY_WINDOW_MS      10
/* Minimum consecutive above-threshold samples before triggering */
#define DEBOUNCE_SAMPLES        3
/* Scan interval per pad in microseconds */
#define PAD_SCAN_INTERVAL_US    1000 /* 1ms per pad, 16ms per full scan */

/* ── MIDI ────────────────────────────────────────────────────── */
#define MIDI_CHANNEL            9    /* Channel 10 (0-indexed) = GM Drums */
#define MIDI_NOTE_BASE          36   /* C2 = GM Bass Drum 1 */
#define MIDI_CABLE_NUM          0

/* ── Sequencer ───────────────────────────────────────────────── */
#define SEQ_STEPS               16
#define SEQ_DEFAULT_BPM         120
#define SEQ_MIN_BPM             40
#define SEQ_MAX_BPM             240

/* ── LED GPIO ────────────────────────────────────────────────── */
#define LED_POWER_PIN           19   /* Green: lit when powered */
#define LED_SEQ_PIN             20   /* Red: blinks on sequencer beat */