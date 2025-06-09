#include "nrf_gpio.h"
#include "nrfx_spim.h"
#include "nrf_delay.h"
#include <stdio.h>

#define RESET_PIN  12
#define DC_PIN     8
#define SCK_PIN    13
#define MISO_PIN   14
#define MOSI_PIN   15
#define CS_PIN     16

const nrfx_spim_t SPIM_INST = NRFX_SPIM_INSTANCE(2);

// initialize the SPIM peripheral
void spim_init(void) {
    nrfx_spim_config_t spim_config = NRFX_SPIM_DEFAULT_CONFIG;
    spim_config.sck_pin = SCK_PIN;
    spim_config.mosi_pin = MOSI_PIN;
    spim_config.miso_pin = MISO_PIN;
    spim_config.frequency = NRF_SPIM_FREQ_8M;
    spim_config.mode = NRF_SPIM_MODE_0;
    spim_config.irq_priority = 0;

    nrfx_spim_init(&SPIM_INST, &spim_config, NULL, NULL);

    // configure output pins
    nrf_gpio_cfg_output(CS_PIN);
    nrf_gpio_cfg_output(DC_PIN);
    nrf_gpio_cfg_output(RESET_PIN);

    nrf_gpio_pin_set(CS_PIN);
}
// cs control functions
static void cs_select(void) {
    nrf_gpio_pin_clear(CS_PIN);
}

static void cs_deselect(void) {
    nrf_gpio_pin_set(CS_PIN);
}

// write command
void display_write_command(uint8_t cmd) {
    nrf_gpio_pin_clear(DC_PIN); // command mode
    cs_select();

    nrfx_spim_xfer_desc_t xfer = {
        .p_tx_buffer = &cmd,
        .tx_length = 1,
        .p_rx_buffer = NULL,
        .rx_length = 0
    };
    nrfx_err_t err = nrfx_spim_xfer(&SPIM_INST, &xfer, 0);
    if (err != NRFX_SUCCESS) {
        printf("SPI transfer failed with error: 0x%08X\n", err);
      
    }


    cs_deselect();
}

// write data
void display_write_data(uint8_t* data, size_t length) {
    nrf_gpio_pin_set(DC_PIN); // data mode
    cs_select();

    nrfx_spim_xfer_desc_t xfer = {
        .p_tx_buffer = data,
        .tx_length = length,
        .p_rx_buffer = NULL,
        .rx_length = 0
    };
    nrfx_err_t err = nrfx_spim_xfer(&SPIM_INST, &xfer, 0);
    if (err != NRFX_SUCCESS) {
        printf("SPI transfer failed with error: 0x%08X\n", err);
    }


    cs_deselect();
}

// Helper to send command + data
void display_send_command_with_data(uint8_t cmd, const uint8_t *data, size_t len) {
    display_write_command(cmd);
    if (len) display_write_data((uint8_t *)data, len);
}

// reset display
void display_reset(void) {
    nrf_gpio_pin_set(RESET_PIN);
    nrf_delay_ms(10);
    nrf_gpio_pin_clear(RESET_PIN);
    nrf_delay_ms(20);
    nrf_gpio_pin_set(RESET_PIN);
    nrf_delay_ms(150);
}


void display_init(void) {
    // Reset display first
    display_write_command(0x01);  // SW reset
    nrf_delay_ms(150);

    // Initialization sequence adapted from your array

    // 0xEF, 3 bytes
    uint8_t ef_data[] = {0x03, 0x80, 0x02};
    display_send_command_with_data(0xEF, ef_data, sizeof(ef_data));

    // 0xCF, 3 bytes
    uint8_t cf_data[] = {0x00, 0xC1, 0x30};
    display_send_command_with_data(0xCF, cf_data, sizeof(cf_data));

    // 0xED, 4 bytes
    uint8_t ed_data[] = {0x64, 0x03, 0x12, 0x81};
    display_send_command_with_data(0xED, ed_data, sizeof(ed_data));

    // 0xE8, 3 bytes
    uint8_t e8_data[] = {0x85, 0x00, 0x78};
    display_send_command_with_data(0xE8, e8_data, sizeof(e8_data));

    // 0xCB, 5 bytes
    uint8_t cb_data[] = {0x39, 0x2C, 0x00, 0x34, 0x02};
    display_send_command_with_data(0xCB, cb_data, sizeof(cb_data));

    // 0xF7, 1 byte
    uint8_t f7_data[] = {0x20};
    display_send_command_with_data(0xF7, f7_data, sizeof(f7_data));

    // 0xEA, 2 bytes
    uint8_t ea_data[] = {0x00, 0x00};
    display_send_command_with_data(0xEA, ea_data, sizeof(ea_data));

    // Power control VRH[5:0] (0xC0)
    uint8_t pwctr1_data[] = {0x23};
    display_send_command_with_data(0xC0, pwctr1_data, sizeof(pwctr1_data));

    // Power control SAP[2:0];BT[3:0] (0xC1)
    uint8_t pwctr2_data[] = {0x10};
    display_send_command_with_data(0xC1, pwctr2_data, sizeof(pwctr2_data));

    // VCM control (0xC5)
    uint8_t vmctr1_data[] = {0x3E, 0x28};
    display_send_command_with_data(0xC5, vmctr1_data, sizeof(vmctr1_data));

    // VCM control 2 (0xC7)
    uint8_t vmctr2_data[] = {0x86};
    display_send_command_with_data(0xC7, vmctr2_data, sizeof(vmctr2_data));

    // Memory Access Control (0x36)
    uint8_t madctl_data[] = {0x48};
    display_send_command_with_data(0x36, madctl_data, sizeof(madctl_data));

    // Vertical Scroll Zero (0x37)
    uint8_t vscrsadd_data[] = {0x00};
    display_send_command_with_data(0x37, vscrsadd_data, sizeof(vscrsadd_data));

    // Pixel Format Set (0x3A)
    uint8_t pixfmt_data[] = {0x55};
    display_send_command_with_data(0x3A, pixfmt_data, sizeof(pixfmt_data));

    // Frame Rate Control (0xB1)
    uint8_t frmctr1_data[] = {0x00, 0x18};
    display_send_command_with_data(0xB1, frmctr1_data, sizeof(frmctr1_data));

    // Display Function Control (0xB6)
    uint8_t dfunctr_data[] = {0x08, 0x82, 0x27};
    display_send_command_with_data(0xB6, dfunctr_data, sizeof(dfunctr_data));

    // 3Gamma Function Disable (0xF2)
    uint8_t f2_data[] = {0x00};
    display_send_command_with_data(0xF2, f2_data, sizeof(f2_data));

    // Gamma curve selected (0x26)
    uint8_t gammaset_data[] = {0x01};
    display_send_command_with_data(0x26, gammaset_data, sizeof(gammaset_data));

    // Positive Gamma Correction (0xE0) - 15 bytes
    uint8_t gmctrp1_data[] = {
        0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08,
        0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03,
        0x0E, 0x09, 0x00
    };
    display_send_command_with_data(0xE0, gmctrp1_data, sizeof(gmctrp1_data));

    // Negative Gamma Correction (0xE1) - 15 bytes
    uint8_t gmctrn1_data[] = {
        0x00, 0x0E, 0x14, 0x03, 0x11, 0x07,
        0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C,
        0x31, 0x36, 0x0F
    };
    display_send_command_with_data(0xE1, gmctrn1_data, sizeof(gmctrn1_data));

    // Exit Sleep (0x11)
    display_write_command(0x11);
    nrf_delay_ms(150);

    // Display ON (0x29)
    display_write_command(0x29);
    nrf_delay_ms(150);
}



int main(void) {
    printf("Starting SPIM example...\n");
    spim_init();

    

    while (1) {
        nrf_delay_ms(10);
        display_init();
    }
}
