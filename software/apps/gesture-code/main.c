#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "nrf_twi_mngr.h"
#include "color_scheme.h"

#include "gesture.h"

#include "microbit_v2.h"

// Global I2C manager and app timer for infrared sensor
NRF_TWI_MNGR_DEF(twi_mngr_instance, 1, 0);

int main(void)
{
    printf("Board started!\n");

    nrf_drv_twi_config_t i2c_config = NRF_DRV_TWI_DEFAULT_CONFIG;
    i2c_config.scl = I2C_QWIIC_SCL; // EDGE_P19
    i2c_config.sda = I2C_QWIIC_SDA; // EDGE_P20
    i2c_config.frequency = NRF_TWIM_FREQ_100K;
    i2c_config.interrupt_priority = 0;

    // GESTURE SENSOR
    nrf_twi_mngr_init(&twi_mngr_instance, &i2c_config);
    apds9960_init(&twi_mngr_instance);
    printf("INITIALIZED I2C MANAGER\n");
    uint8_t id = apds9960_read_register(APDS9960_ID);
    printf("APDS9960 ID = 0x%02X\n", id);

    for (uint8_t addr = 1; addr < 127; addr++)
    {
        uint8_t dummy = 0;
        nrf_twi_mngr_transfer_t xfer = NRF_TWI_MNGR_WRITE(addr, &dummy, 1, 0);
        ret_code_t err = nrf_twi_mngr_perform(&twi_mngr_instance, NULL, &xfer, 1, NULL);
        if (err == NRF_SUCCESS)
        {
            printf("Device found at 0x%02X\n", addr);
        }
    }

    if (!apds9960_enable_gesture_sensor())
    {
        printf("Gesture sensor init failed!\n");
    }

    while (1)
    {

        if (apds9960_is_gesture_available())
        {
            gesture_t g = apds9960_read_gesture();
            if (g == GESTURE_LEFT || g == GESTURE_RIGHT)
            {
                printf("Gesture detected\n");
            }
        }

        nrf_delay_ms(100);
    }
}
