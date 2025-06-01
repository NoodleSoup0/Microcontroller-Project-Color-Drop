#include <stdint.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "led_matrix.h"

// Pulse a pin high then low
void pulse(uint32_t pin) {
    nrf_gpio_pin_set(pin);
    nrf_delay_us(2);
    nrf_gpio_pin_clear(pin);
}

void set_row_address(uint8_t row) {
    nrf_gpio_pin_write(A_PIN, (row >> 3) & 1);
    nrf_gpio_pin_write(B_PIN, (row >> 2) & 1);
    nrf_gpio_pin_write(C_PIN, (row >> 1) & 1);
    nrf_gpio_pin_write(D_PIN, (row >> 0) & 1);
}

void shift_all_pixels_on(void) {
    // Shift in 32 pixels for top half and 32 for bottom half
    // Each pixel has 3 bits (RGB) but we just set all ON (LOW)
    // HUB75 panel expects the data in the order:
    // R1 G1 B1 (upper half pixel)
    // R2 G2 B2 (lower half pixel)

    for (int i = 0; i < 32; i++) {
        // Set RGB pins LOW to turn LEDs ON for this pixel pair
        nrf_gpio_pin_clear(R1_PIN);
        nrf_gpio_pin_clear(G1_PIN);
        nrf_gpio_pin_clear(B1_PIN);
        nrf_gpio_pin_clear(R2_PIN);
        nrf_gpio_pin_clear(G2_PIN);
        nrf_gpio_pin_clear(B2_PIN);

        // Pulse clock to shift these bits in
        pulse(CLK_PIN);
    }
}


void setup_pins(void) {
    // Configure RGB pins as outputs
    nrf_gpio_cfg_output(R1_PIN);
    nrf_gpio_cfg_output(G1_PIN);
    nrf_gpio_cfg_output(B1_PIN);
    nrf_gpio_cfg_output(R2_PIN);
    nrf_gpio_cfg_output(G2_PIN);
    nrf_gpio_cfg_output(B2_PIN);

    // Configure row address pins as outputs
    nrf_gpio_cfg_output(A_PIN);
    nrf_gpio_cfg_output(B_PIN);
    nrf_gpio_cfg_output(C_PIN);
    nrf_gpio_cfg_output(D_PIN);

    // Configure control pins as outputs
    nrf_gpio_cfg_output(CLK_PIN);
    nrf_gpio_cfg_output(LAT_PIN);
    nrf_gpio_cfg_output(OE_PIN);

    // Initialize RGB pins to OFF (HIGH = LED OFF)
    nrf_gpio_pin_set(R1_PIN);
    nrf_gpio_pin_set(G1_PIN);
    nrf_gpio_pin_set(B1_PIN);
    nrf_gpio_pin_set(R2_PIN);
    nrf_gpio_pin_set(G2_PIN);
    nrf_gpio_pin_set(B2_PIN);

    // Initialize control pins
    nrf_gpio_pin_clear(CLK_PIN);
    nrf_gpio_pin_clear(LAT_PIN);
    nrf_gpio_pin_set(OE_PIN);  // Disable output initially
}

void display_row(uint8_t row) {
    nrf_gpio_pin_set(OE_PIN); // Disable output

    // Shift pixel data for row (all pixels ON here)
    shift_all_pixels_on();

    // Latch data
    nrf_gpio_pin_set(LAT_PIN);
    nrf_delay_us(5);
    nrf_gpio_pin_clear(LAT_PIN);

    // Set row address
    set_row_address(row);

    // Enable output
    nrf_gpio_pin_clear(OE_PIN);

    // Hold the row for a bit (brightness control here)
    nrf_delay_ms(2);
}

