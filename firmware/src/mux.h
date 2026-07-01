#pragma once
#include <stdint.h>
/*
 * mux.h — CD74HC4067 16:1 analog multiplexer driver.
 * Controls which FSR voltage divider is connected to the ADC.
 */
void mux_init(void);
void mux_select(uint8_t channel); /* Select channel 0–15 */
void mux_enable(void);            /* Pull EN low */
void mux_disable(void);           /* Pull EN high */