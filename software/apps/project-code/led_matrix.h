#ifndef LED_MATRIX_H
#define LED_MATRIX_H

// --------------------
// RGB Data Pins
// --------------------
#define R1_PIN   0    // P0 - Red, upper half
#define R2_PIN   1    // P1 - Red, lower half
#define G1_PIN   2    // P2 - Green, upper half
#define G2_PIN   3    // P3 - Green, lower half
#define B1_PIN   4    // P4 - Blue, upper half
#define B2_PIN   5   // P10 - Blue, lower half

// --------------------
// Row Address Pins (A–D)
// --------------------
#define A_PIN    6    // P5 - Row address A
#define B_PIN    7    // P6 - Row address B
#define C_PIN    11    // P7 - Row address C
#define D_PIN    12    // P8 - Row address D

// --------------------
// Control Pins
// --------------------
#define LAT_PIN  14   // P11 - Latch
#define OE_PIN   15   // P12 - Output Enable (active LOW)
#define CLK_PIN  13   // P13 - Clock

// Setup all necessary GPIO pins
void setup_pins(void);

// Display a single row with all pixels ON
void display_row(uint8_t row);

// Set the row address (A, B, C, D pins)
void set_row_address(uint8_t row);

// Shift in 32 pairs of RGB pixels (upper & lower) – all ON
void shift_all_pixels_on(void);

// Pulse a GPIO pin (for CLK, etc.)
void pulse(uint32_t pin);

#endif // LED_MATRIX_H
