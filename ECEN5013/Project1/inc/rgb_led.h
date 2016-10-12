/*
 * rgb_led.h
 *
 *  Created on: Oct 9, 2016
 *      Author: Sean
 */

#ifndef SOURCE_RGB_LED_H_
#define SOURCE_RGB_LED_H_

#include <stdint.h>

/**
 * @brief Initializes the RGB LED
 */ 
void rgb_led_init(void);

/**
 * @brief Initializes the RGB LED
 * @param red Sets the red color brightness
 * @param green Sets the green color brightness
 * @param blue Sets the blue color brightness
 */ 
void rgb_led_set_brightness(uint8_t red, uint8_t green, uint8_t blue);

#endif /* SOURCE_RGB_LED_H_ */
