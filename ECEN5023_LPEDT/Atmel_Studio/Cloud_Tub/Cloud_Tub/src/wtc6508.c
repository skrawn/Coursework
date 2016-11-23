/*
 * wtc6508.c
 *
 * Created: 11/19/2016 3:12:39 PM
 *  Author: Sean
 */ 

#include "i2c_master.h"
#include "i2c_master_interrupt.h"
#include "wtc6508.h"

#define WTC65808_SERCOM         SERCOM0
#define WTC6508_IRQ             SERCOM0_IRQn
#define WTC6508_PINMUX_PAD0     PINMUX_PA08C_SERCOM0_PAD0
#define WTC6508_PINMUX_PAD1     PINMUX_PA09C_SERCOM0_PAD1

#define WTC6508_BAUD            10  // kHz

struct i2c_master_module wtc6508_module;

// Must run at 2kHz to 20kHz with at least 15 ms between transactions

void wtc6508_init(void)
{
    struct i2c_master_config config;
    i2c_master_get_config_defaults(&config);

    config.pinmux_pad0 = WTC6508_PINMUX_PAD0;
    config.pinmux_pad1 = WTC6508_PINMUX_PAD1;
    config.baud_rate = WTC6508_BAUD;
    config.generator_source = GCLK_GENERATOR_4;

    if (i2c_master_init(&wtc6508_module, WTC65808_SERCOM, &config)) {
        printf("failed to initialize WTC6508!\n");
        return;
    }
}

enum status_code wtc6508_read(uint8_t *status)
{
    enum status_code ret;
    
    ret = i2c_master_read_byte(&wtc6508_module, status);
    i2c_master_send_stop(&wtc6508_module);
    return ret;
}
