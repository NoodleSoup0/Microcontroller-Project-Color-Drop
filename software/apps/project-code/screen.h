#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>
#include "nrfx_spim.h"

// Pin definitions for micro:bit v2
#define RESET_PIN EDGE_P12
#define DC_PIN EDGE_P8
#define SCK_PIN EDGE_P13
#define MISO_PIN EDGE_P14
#define MOSI_PIN EDGE_P15
#define CS_PIN EDGE_P16

extern const nrfx_spim_t SPIM_INST;

// Function prototypes
void spim_init(void);
void display_write_command(uint8_t cmd);
void display_write_data(uint8_t* data, size_t length);
void display_send_command_with_data(uint8_t cmd, const uint8_t *data, size_t len);
void display_reset(void);
void display_init(void);
void set_address_window(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);
void draw_pixel(uint16_t x, uint16_t y, uint16_t color);
void draw_filled_rect(int x, int y, int w, int h, uint16_t color);


#endif // SCREEN_H
