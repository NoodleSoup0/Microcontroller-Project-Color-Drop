#pragma once

#include "nrf_twi_mngr.h"

// AMG8833 I2C address (fixed)
#define AMG8833_ADDR 0x69

// Registers
#define AMG8833_PIXEL_BASE 0x80  // First pixel temp register
#define AMG8833_PIXEL_COUNT 64   // Total number of pixels
#define AMG8833_BYTES_PER_PIXEL 2

// Function Prototypes
void amg8833_init(const nrf_twi_mngr_t* i2c);
bool amg8833_read_pixels(float *pixels_out);
void print_amg8833_ascii_heatmap(float *pixels);
void print_amg8833_heatmap(float *pixels);
void infrared_timer_callback(void *context);
uint16_t temperature_to_color(float temp);
