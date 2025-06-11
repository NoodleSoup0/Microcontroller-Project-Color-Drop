#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "app_timer.h"

#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_twi_mngr.h"

#include "led_matrix.h"
#include "infrared.h"
#include "screen.h"
#include "matrix_rain.h"
#include "gesture.h"
#include "color_scheme.h"

#include "microbit_v2.h"

// Global I2C manager and app timer for infrared sensor
NRF_TWI_MNGR_DEF(twi_mngr_instance, 1, 0);
APP_TIMER_DEF(infrared_timer);

float thermal_pixels[64];

int main(void)
{
    printf("Board Started!\n");

    // Initialize I2C peripheral
    nrf_drv_twi_config_t i2c_config = NRF_DRV_TWI_DEFAULT_CONFIG;
    i2c_config.scl = I2C_QWIIC_SCL; // EDGE_P19
    i2c_config.sda = I2C_QWIIC_SDA; // EDGE_P20
    i2c_config.frequency = NRF_TWIM_FREQ_100K;
    i2c_config.interrupt_priority = 0;

    // INFRARED SENSOR
    nrf_twi_mngr_init(&twi_mngr_instance, &i2c_config);
    amg8833_init(&twi_mngr_instance);

    app_timer_init();
    app_timer_create(&infrared_timer, APP_TIMER_MODE_REPEATED, infrared_timer_callback);
    app_timer_start(infrared_timer, APP_TIMER_TICKS(600), NULL);

    // TFT LCD DISPLAY
    spim_init();
    display_init();
    init_rain();

    while (1)
    {
        update_rain();
        nrf_delay_ms(10);
    }
}
