#include "infrared.h"
#include "nrf_delay.h"
#include <stdio.h>

#include <math.h>

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

//Use this to show TEMP values in °C in heatmap
void print_amg8833_heatmap(float *pixels) {
    printf("\nAMG8833 TEMP Heatmap (°C):\n");
    for (int row = 7; row >= 0; row--) {         // bottom to top
        for (int col = 0; col < 8; col++) {      // left to right
            int index = row * 8 + col;           // natural indexing
            printf("%5.1f", pixels[index]);
            if (col < 7) printf(" ");
        }
        printf("\n");
    }
}

char pixel_to_symbol(float temp) {
    if (temp > 32.0) return '@';  // Hot (hand center)
    if (temp > 30.0) return '#';  // Warm (edges of hand)
    if (temp > 28.0) return '*';  // Mild
    if (temp > 23.0) return '*';
    return '.';
}

// UPSIDE DOWN SENSOR heatmap layout
//  8  7  6  5  4  3  2  1
// 16 15 14 13 12 11 10  9
// 24 23 22 21 20 19 18 17
// 32 31 30 29 28 27 26 25
// 40 39 38 37 36 35 34 33
// 48 47 46 45 44 43 42 41
// 56 55 54 53 52 51 50 49
// 64 63 62 61 60 59 58 57
// void print_amg8833_ascii_heatmap(float *pixels) {
//     printf("\nAMG8833 Thermal View (1–64, flipped columns):\n");

//     for (int row = 0; row < 8; row++) {          // top to bottom
//         for (int col = 7; col >= 0; col--) {     // right to left
//             int index = row * 8 + col;           // raw data index (0–63)
//             char symbol = pixel_to_symbol(pixels[index]);
//             printf("%c ", symbol);
//         }
//         printf("\n");
//     }
// }

// RIGHT SIDE UP heatmap layout
// 57 58 59 60 61 62 63 64
// 49 50 51 52 53 54 55 56
// 41 42 43 44 45 46 47 48
// 33 34 35 36 37 38 39 40
// 25 26 27 28 29 30 31 32
// 17 18 19 20 21 22 23 24
//  9 10 11 12 13 14 15 16
//  1  2  3  4  5  6  7  8
void print_amg8833_ascii_heatmap(float *pixels) {
    printf("\nAMG8833 Pixel Number Layout (1–64):\n");
    for (int row = 7; row >= 0; row--) {         // bottom to top
        for (int col = 0; col < 8; col++) {      // left to right
            int index = row * 8 + col;           // natural indexing
            char symbol = pixel_to_symbol(pixels[index]);
            printf("%c ", symbol);
        }
        printf("\n");
    }
}

void infrared_timer_callback(void* p_context) {
    extern float thermal_pixels[64];  // if not in same file

    if (amg8833_read_pixels(thermal_pixels)) {
        print_amg8833_ascii_heatmap(thermal_pixels);
    } else {
        printf("Failed to read AMG8833 data.\n");
    }
}
