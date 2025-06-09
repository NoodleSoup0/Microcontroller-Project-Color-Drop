#include "infrared.h"
#include "nrf_delay.h"
#include <stdio.h>
#include "screen.h"
#include "font5x7.h"  
#include <stdlib.h>  // for rand()


#include <math.h>

#define MAX_RAIN 50
#define CHAR_HEIGHT 7
#define DROP_SPEED 2

typedef struct {
    int x;
    int y;
    char c;
    int active;
    int bouncing;
    int velocity;
} RainDrop;

static RainDrop rain_drops[MAX_RAIN];


static const nrf_twi_mngr_t* i2c_manager = NULL;

void amg8833_init(const nrf_twi_mngr_t* i2c) {
  i2c_manager = i2c;

  // Optional: Set to 10 Hz mode
  uint8_t set_fps[] = {0x02, 0x00};  // 0 = 10Hz
  nrf_twi_mngr_transfer_t const fps_transfer[] = {
      NRF_TWI_MNGR_WRITE(AMG8833_ADDR, set_fps, sizeof(set_fps), 0),
  };

  ret_code_t result = nrf_twi_mngr_perform(i2c_manager, NULL, fps_transfer, 1, NULL);
  if (result != NRF_SUCCESS) {
    printf("Failed to set AMG8833 frame rate: %lx\n", result);
  }
}

// Helper: Read 128 bytes (64 pixels × 2 bytes)
bool amg8833_read_pixels(float *pixels_out) {
  uint8_t reg = AMG8833_PIXEL_BASE;
  uint8_t buffer[AMG8833_PIXEL_COUNT * AMG8833_BYTES_PER_PIXEL] = {0};

  nrf_twi_mngr_transfer_t const transfers[] = {
      NRF_TWI_MNGR_WRITE(AMG8833_ADDR, &reg, 1, NRF_TWI_MNGR_NO_STOP),
      NRF_TWI_MNGR_READ(AMG8833_ADDR, buffer, sizeof(buffer), 0),
  };

  ret_code_t result = nrf_twi_mngr_perform(i2c_manager, NULL, transfers, 2, NULL);
  if (result != NRF_SUCCESS) {
    printf("Failed to read AMG8833 pixels: %lx\n", result);
    return false;
  }

  for (int i = 0; i < AMG8833_PIXEL_COUNT; ++i) {
    uint16_t raw = buffer[i * 2] | (buffer[i * 2 + 1] << 8);
    // Convert raw 12-bit signed data to temperature in Celsius
    if (raw & 0x800) {  // negative temperature
      raw = raw & 0x7FF;
      pixels_out[i] = -1.0f * raw * 0.25f;
    } else {
      pixels_out[i] = raw * 0.25f;
    }
  }

  return true;
}

void spawn_rain_drop() {
    for (int i = 0; i < MAX_RAIN; i++) {
        if (!rain_drops[i].active) {
            rain_drops[i].x = rand() % 8;
            rain_drops[i].y = -CHAR_HEIGHT; // Start above the screen
            rain_drops[i].c = 'A' + rand() % 26;
            rain_drops[i].active = 1;
            rain_drops[i].bouncing = 0;
            rain_drops[i].velocity = DROP_SPEED;
            printf("Spawning char '%c' at column %d\n", rain_drops[i].c, rain_drops[i].x);

            break;
            
        }
    }
}


void draw_char(int x, int y, char c, uint16_t fg, uint16_t bg) {
    const uint8_t *bitmap = font5x7[c - 32];
    for (int i = 0; i < 5; i++) {
        uint8_t line = bitmap[i];
        for (int j = 0; j < 7; j++) {
            uint16_t color = (line & 1) ? fg : bg;
            draw_pixel(x + i, y + j, color);  // Implement draw_pixel()
            line >>= 1;
        }
    }
}


// RIGHT SIDE UP heatmap layout
// 57 58 59 60 61 62 63 64
// 49 50 51 52 53 54 55 56
// 41 42 43 44 45 46 47 48
// 33 34 35 36 37 38 39 40
// 25 26 27 28 29 30 31 32
// 17 18 19 20 21 22 23 24
//  9 10 11 12 13 14 15 16
//  1  2  3  4  5  6  7  8

uint16_t temperature_to_color(float temp) {
    // Clamp between 20 and 40°C for display
    if (temp < 20) temp = 20;
    if (temp > 40) temp = 40;

    // Normalize temp to 0-255
    uint8_t intensity = (uint8_t)(255 * (temp - 20) / 20);

    // Create a gradient: Blue → Green → Red
    uint8_t r = 0, g = 0, b = 0;
    if (intensity < 128) {
        b = 255 - 2 * intensity;
        g = 2 * intensity;
    } else {
        g = 255 - 2 * (intensity - 128);
        r = 2 * (intensity - 128);
    }

    // Convert RGB888 to RGB565
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

void update_rain_drops(float *thermal_pixels) {
    for (int i = 0; i < MAX_RAIN; i++) {
        if (!rain_drops[i].active) continue;

        int grid_row = (rain_drops[i].y + CHAR_HEIGHT) / 40;
        int col = rain_drops[i].x;
        int index = grid_row * 8 + col;

        float temp = (grid_row >= 0 && grid_row < 8) ? thermal_pixels[index] : 0;

        if (temp > 23.0f && rain_drops[i].y + CHAR_HEIGHT >= grid_row * 40) {
            // Bounce logic
            if (!rain_drops[i].bouncing) {
                rain_drops[i].bouncing = 1;
                rain_drops[i].velocity = -DROP_SPEED * 2;  // bounce upward
            }
        }

        rain_drops[i].y += rain_drops[i].velocity;

        if (rain_drops[i].bouncing) {
            rain_drops[i].velocity++;  // simulate gravity
            if (rain_drops[i].velocity > DROP_SPEED) {
                rain_drops[i].bouncing = 0;
                rain_drops[i].velocity = DROP_SPEED;
            }
        }

        if (rain_drops[i].y > 320) {
            rain_drops[i].active = 0;
        }
    }
}

void draw_rain_drops() {
    for (int i = 0; i < MAX_RAIN; i++) {
        if (rain_drops[i].active) {
            int x_pixel = rain_drops[i].x * 30 + 10;  // Centered in block
            int y_pixel = rain_drops[i].y;
            draw_char(x_pixel, y_pixel, rain_drops[i].c, 0xFFFF, 0x0000);  // white on black
            printf("Rain char '%c' at x=%d y=%d\n", rain_drops[i].c, x_pixel, y_pixel);

        }
    }
}


void draw_heatmap_with_rain(float *pixels) {
    for (int row = 0; row < 8; row++) {
        for (int col = 0; col < 8; col++) {
            int index = row * 8 + col;
            float temp = pixels[index];
            uint16_t bg_color = temperature_to_color(temp);

            // Block coordinates
            int x = col * 30;
            int y = row * 40;

            // Default: just draw filled color block
            draw_filled_rect(x, y, 30, 40, bg_color);

            // Now check: is a rain drop here?
            for (int i = 0; i < MAX_RAIN; i++) {
                if (!rain_drops[i].active) continue;

                // Check if this drop is inside this block
                int drop_col = rain_drops[i].x;
                int drop_y = rain_drops[i].y;

                if (drop_col == col && drop_y >= y && drop_y < y + 40) {
                    int char_x = x + 10;
                    int char_y = drop_y;

                    draw_char(char_x, char_y, rain_drops[i].c, 0xFFFF, bg_color);  // white on temperature color
                }
            }
        }
    }
}


void infrared_timer_callback(void* p_context) {
    extern float thermal_pixels[64];

    if (amg8833_read_pixels(thermal_pixels)) {
        update_rain_drops(thermal_pixels);
        draw_heatmap_with_rain(thermal_pixels);  // Unified render

        // Spawn new drop occasionally
        if (rand() % 2 == 0) {
            spawn_rain_drop();
        }
    } else {
        printf("Failed to read AMG8833 data.\n");
    }
}
