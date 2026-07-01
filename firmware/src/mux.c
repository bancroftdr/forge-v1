#include "mux.h"
#include "config.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"

void mux_init(void) {
    const uint8_t pins[] = {
        MUX_S0_PIN, MUX_S1_PIN, MUX_S2_PIN, MUX_S3_PIN, MUX_EN_PIN
    };
    for (int i = 0; i < 5; i++) {
        gpio_init(pins[i]);
        gpio_set_dir(pins[i], GPIO_OUT);
        gpio_put(pins[i], 0);
    }
    mux_disable(); /* Start disabled */
    mux_select(0);
}

void mux_select(uint8_t channel) {
    /* Write 4-bit channel address to S3:S0 */
    gpio_put(MUX_S0_PIN, (channel >> 0) & 0x01);
    gpio_put(MUX_S1_PIN, (channel >> 1) & 0x01);
    gpio_put(MUX_S2_PIN, (channel >> 2) & 0x01);
    gpio_put(MUX_S3_PIN, (channel >> 3) & 0x01);
}

void mux_enable(void) {
    gpio_put(MUX_EN_PIN, 0); /* Active LOW */
}

void mux_disable(void) {
    gpio_put(MUX_EN_PIN, 1);
}