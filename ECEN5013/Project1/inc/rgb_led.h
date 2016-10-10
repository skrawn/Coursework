/*
 * rgb_led.h
 *
 *  Created on: Oct 9, 2016
 *      Author: Sean
 */

#ifndef SOURCE_RGB_LED_H_
#define SOURCE_RGB_LED_H_

#include <stdint.h>

void rgb_led_init(void);
void rgb_led_set_brightness(uint8_t red, uint8_t green, uint8_t blue);

#endif /* SOURCE_RGB_LED_H_ */
