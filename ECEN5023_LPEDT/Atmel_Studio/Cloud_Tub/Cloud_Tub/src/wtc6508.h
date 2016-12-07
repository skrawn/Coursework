/*
 * wtc6508.h
 *
 * Created: 11/19/2016 3:13:00 PM
 *  Author: Sean
 */ 

#include <stdint.h>

#include "spi.h"
#include "status_codes.h"

#ifndef WTC6508_H_
#define WTC6508_H_

#define WTC6508_DI_GPIO         PIN_PA08
#define WTC6508_CLK_GPIO        PIN_PA09

#define WTC6508_PINMUX_PAD0     PINMUX_PA08C_SERCOM0_PAD0
#define WTC6508_PINMUX_PAD1     PINMUX_PA09C_SERCOM0_PAD1
#define WTC6508_PINMUX_PAD2     PINMUX_UNUSED
#define WTC6508_PINMUX_PAD3     PINMUX_UNUSED

/**
 * @brief Initializes the WTC6508
 */
void wtc6508_init(void);

/**
 * @brief Reads the button state from the WTC6508
 * @param status Pointer to the status buffer
 * @return Returns STATUS_OK if there were no faults
 */
enum status_code wtc6508_read(uint8_t *status);

#endif /* WTC6508_H_ */