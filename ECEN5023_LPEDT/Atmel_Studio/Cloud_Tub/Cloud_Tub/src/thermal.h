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

void thermal_init(void);
void thermal_3s(void);
void thermal_change_scale(bool new_scale);
enum status_code thermal_set_temperature(uint8_t temperature);
uint8_t thermal_get_temperature(void);
uint8_t thermal_get_water_temp(void);
void thermal_turn_on_heater(bool on);
void thermal_turn_on_water_pump(bool on);
void thermal_turn_on_air_pump(bool on);

#endif /* THERMAL_H_ */