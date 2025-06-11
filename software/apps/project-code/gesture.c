#include "gesture.h"
#include "nrf_delay.h"
#include <stdlib.h>

#include <stdio.h>

static const nrf_twi_mngr_t *m_i2c_mngr = NULL;

void apds9960_init(const nrf_twi_mngr_t *i2c_mngr)
{
  m_i2c_mngr = i2c_mngr;
}

uint8_t apds9960_read_register(uint8_t reg)
{
  if (m_i2c_mngr == NULL)
  {
    printf("ERROR: I2C manager not initialized!\n");
    return 0;
  }

  uint8_t reg_buf;
  uint8_t value = 0;
  reg_buf = reg;

  nrf_twi_mngr_transfer_t transfers[] = {
      NRF_TWI_MNGR_WRITE(APDS9960_ADDRESS, &reg_buf, 1, NRF_TWI_MNGR_NO_STOP),
      NRF_TWI_MNGR_READ(APDS9960_ADDRESS, &value, 1, 0)};

  ret_code_t err = nrf_twi_mngr_perform(m_i2c_mngr, NULL, transfers, 2, NULL);
  if (err != NRF_SUCCESS)
  {
    printf("I2C READ failed reg=0x%02X, err=0x%lx\n", reg, err);
    return 0;
  }

  return value;
}

void apds9960_write_register(uint8_t reg, uint8_t data)
{
  if (!m_i2c_mngr)
  {
    printf("ERROR: I2C manager NULL\n");
    return;
  }

  uint8_t buffer[2];
  buffer[0] = reg;
  buffer[1] = data;

  nrf_twi_mngr_transfer_t xfer = NRF_TWI_MNGR_WRITE(APDS9960_ADDRESS, buffer, 2, 0);
  ret_code_t err = nrf_twi_mngr_perform(m_i2c_mngr, NULL, &xfer, 1, NULL);
  if (err != NRF_SUCCESS)
  {
    printf("I2C write to 0x%02X failed (err=0x%lx)\n", reg, err);
  }
}

bool apds9960_enable_gesture_sensor(void)
{
  apds9960_write_register(APDS9960_ENABLE, 0x01);
  nrf_delay_ms(10);

  apds9960_write_register(0x9F, 0x01);
  apds9960_write_register(APDS9960_GPENTH, 30);
  apds9960_write_register(APDS9960_GEXTH, 50);
  apds9960_write_register(APDS9960_GCONF1, 0x41);
  apds9960_write_register(APDS9960_GCONF2, 0x41);
  apds9960_write_register(0x90, 0x24);
  apds9960_write_register(0xA6, 0x49);
  apds9960_write_register(0xAA, 0x02);
  apds9960_write_register(0x9F, 0xC0);

  apds9960_write_register(0xA7, 0x00);
  apds9960_write_register(0xA8, 0x00);
  apds9960_write_register(0xA9, 0x00);
  apds9960_write_register(0xAA, 0x00);
  apds9960_write_register(0xAA, 0x00);

  nrf_delay_ms(5);

  apds9960_write_register(APDS9960_ENABLE, 0x05);
  nrf_delay_ms(5);

  apds9960_write_register(APDS9960_ENABLE, 0x45);
  nrf_delay_ms(5);

  apds9960_write_register(APDS9960_GCONF4, 0x01);
  nrf_delay_ms(10);

  uint8_t gconf4 = apds9960_read_register(APDS9960_GCONF4);
  uint8_t enable = apds9960_read_register(APDS9960_ENABLE);

  return (gconf4 == 0x01 && (enable & 0x45) == 0x45);
}

bool apds9960_is_gesture_available(void)
{
  uint8_t status = apds9960_read_register(APDS9960_GSTATUS);
  return (status & 0x01);
}

gesture_t apds9960_read_gesture(void)
{
  uint8_t l_sum = 0, r_sum = 0;
  uint8_t fifo_level = apds9960_read_register(APDS9960_GFLVL);

  if (fifo_level == 0 || fifo_level > 32)
  {
    return GESTURE_NONE;
  }

  for (int i = 0; i < fifo_level; i++)
  {
    apds9960_read_register(APDS9960_GFIFO_U);
    apds9960_read_register(APDS9960_GFIFO_D);
    l_sum += apds9960_read_register(APDS9960_GFIFO_L);
    r_sum += apds9960_read_register(APDS9960_GFIFO_R);
  }

  int lr_diff = (int)r_sum - (int)l_sum;

  if (abs(lr_diff) < 30)
  {
    return GESTURE_NONE;
  }

  return (lr_diff > 0) ? GESTURE_RIGHT : GESTURE_LEFT;
}