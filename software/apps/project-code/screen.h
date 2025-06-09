#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>
#include "nrfx_spim.h"

// Pin definitions for micro:bit v2
#define RESET_PIN  12
#define DC_PIN     8
#define SCK_PIN    13
#define MISO_PIN   14  
#define MOSI_PIN   15
#define CS_PIN     16

// extern const nrfx_spim_t SPIM_INST;

// void spim_init(void);
// void ili9341_reset(void);
// void ili9341_init(void);
// void send_command(uint8_t cmd);
// void send_data(uint8_t *data, size_t length);
// void set_address_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
// void draw_pixel(uint16_t x, uint16_t y, uint16_t color);
// uint16_t rgb_to_565(uint8_t r, uint8_t g, uint8_t b);
// void fill_screen(uint16_t color);

// void test_spi_write_with_led(void);

#endif // SCREEN_H
