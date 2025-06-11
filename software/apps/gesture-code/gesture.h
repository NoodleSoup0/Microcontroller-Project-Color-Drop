#pragma once
#include "nrf_drv_twi.h"
#include "nrf_twi_mngr.h"

#include <stdbool.h>
#include <stdint.h>

#define APDS9960_ADDRESS 0x39
#define APDS9960_ID 0x92

#define APDS9960_ENABLE 0x80
#define APDS9960_GPENTH 0xA0
#define APDS9960_GEXTH 0xA1
#define APDS9960_GCONF1 0xA2
#define APDS9960_GCONF2 0xA3
#define APDS9960_GCONF4 0xAB
#define APDS9960_GSTATUS 0xAF
#define APDS9960_GFIFO_U 0xFC
#define APDS9960_GFIFO_D 0xFD
#define APDS9960_GFIFO_L 0xFE
#define APDS9960_GFIFO_R 0xFF
#define APDS9960_GFLVL 0xAE
#define APDS9960_GCONF3 0xAA
#define APDS9960_GPULSE 0xA6
#define APDS9960_GOFFSET_U 0xA7
#define APDS9960_GOFFSET_D 0xA8
#define APDS9960_GOFFSET_L 0xA9
#define APDS9960_GOFFSET_R 0xAA
#define APDS9960_CONFIG3 0x9F

typedef enum
{
    GESTURE_NONE,
    GESTURE_UP,
    GESTURE_DOWN,
    GESTURE_LEFT,
    GESTURE_RIGHT
} gesture_t;

void apds9960_init(const nrf_twi_mngr_t *i2c_mngr);
bool apds9960_enable_gesture_sensor(void);
bool apds9960_is_gesture_available(void);
gesture_t apds9960_read_gesture(void);
uint8_t apds9960_read_register(uint8_t reg);
void apds9960_write_register(uint8_t reg, uint8_t data);
extern volatile bool g_twi_xfer_done;