/*
 * thermal.h
 *
 * Created: 11/29/2016 8:38:38 PM
 *  Author: Sean
 */ 

#ifndef THERMAL_H_
#define THERMAL_H_

#include <stdbool.h>
#include <stdint.h>

/**
 * @brief Initializes the thermal functions
 */
void thermal_init(void);

/**
 * @brief 3 second thermal task
 */
void thermal_3s(void);

/**
 * @brief Changes the display scale
 * @param new_scale Scale to change to
 */
void thermal_change_scale(bool new_scale);

/**
 * @brief Sets the water temperature set point
 * @param temperature New temperature to set the water to.
 * @return Return STATUS_OK if the temperature is valid
 */
enum status_code thermal_set_temperature(uint8_t temperature);

/**
 * @brief Return the current water temperature set point
 * @return Returns the current water temperature set point
 */
uint8_t thermal_get_temperature(void);

/**
 * @brief Return the current water temperature
 * @return Returns the current water temperature
 */
uint8_t thermal_get_water_temp(void);

/**
 * @brief Turns the heaters on or off
 * @param on Turns the heaters on or off
 */
void thermal_turn_on_heater(bool on);

/**
 * @brief Turns the water pump on or off
 * @param on Turns the water pump on or off
 */
void thermal_turn_on_water_pump(bool on);

/**
 * @brief Turns the air pump on or off
 * @param on Turns the air pump on or off
 */
void thermal_turn_on_air_pump(bool on);

#endif /* THERMAL_H_ */