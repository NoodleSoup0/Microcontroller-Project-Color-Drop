#include <stdint.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "screen.h"


void setup_pins(void) {
    nrf_drv_spi_config_t spi_config = NRF_DRV_SPI_DEFAULT_CONFIG;
    spi_config.ss_pin   = CS_PIN;
    spi_config.miso_pin = MISO_PIN;
    spi_config.mosi_pin = MOSI_PIN;
    spi_config.sck_pin  = SCK_PIN;
    spi_config.frequency = NRF_DRV_SPI_FREQ_1M;
    spi_config.mode = NRF_DRV_SPI_MODE_0;
    spi_config.bit_order = NRF_DRV_SPI_BIT_ORDER_MSB_FIRST;

    nrf_drv_spi_init(&spi, &spi_config, NULL, NULL);
}
