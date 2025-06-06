#include <stdint.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "led_matrix.h"

// pulse a pin high then low
void pulse(uint32_t pin) {
    nrf_gpio_pin_set(pin);
    nrf_delay_us(200);
    nrf_gpio_pin_clear(pin);
    nrf_delay_us(200);
}

void set_row_address(uint8_t row) {
    nrf_gpio_pin_write(A_PIN, 0);
    nrf_gpio_pin_write(B_PIN, 0);
    nrf_gpio_pin_write(C_PIN, 0);
    nrf_gpio_pin_write(D_PIN, 0);
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

    // init RGB pins to OFF (HIGH = LED OFF)
    nrf_gpio_pin_set(R1_PIN);
    nrf_gpio_pin_set(G1_PIN);
    nrf_gpio_pin_set(B1_PIN);
    nrf_gpio_pin_set(R2_PIN);
    nrf_gpio_pin_set(G2_PIN);
    nrf_gpio_pin_set(B2_PIN);

    // init control pins
    nrf_gpio_pin_clear(CLK_PIN);
    nrf_gpio_pin_clear(LAT_PIN);
    nrf_gpio_pin_set(OE_PIN);  // disable output initially
}

void display_row(uint8_t row) {
    nrf_gpio_pin_set(OE_PIN); // disable output

    // set row address
    set_row_address(row);

    for (int i = 0; i < 32; i++) {
        // Set RGB pins LOW to turn LEDs ON
        nrf_gpio_pin_clear(R1_PIN);
        // nrf_gpio_pin_clear(G1_PIN);
        // nrf_gpio_pin_clear(B1_PIN);
        nrf_gpio_pin_clear(R2_PIN);
        // nrf_gpio_pin_clear(G2_PIN);
        // nrf_gpio_pin_clear(B2_PIN);

       
        pulse(CLK_PIN);
    }
     // after all the pixels have been shifted in we set the LATCH pin high, 
    // and then on the next clock-divided cycle we set the LATCH back low
    nrf_gpio_pin_set(LAT_PIN);
    nrf_delay_us(200);
    nrf_gpio_pin_clear(LAT_PIN);
    

    // disable output 
    nrf_gpio_pin_clear(OE_PIN);

}

