/*
 * thermal.h
 *
 * Created: 11/29/2016 8:38:38 PM
 *  Author: Sean
 */ 

#ifndef THERMAL_H_
#define THERMAL_H_

#include <stdint.h>

void thermal_init(void);
void thermal_change_scale(bool new_scale);
enum status_code thermal_set_temperature(uint8_t temperature);
uint8_t thermal_get_temperature(void);
uint8_t thermal_get_water_temp(void);


#endif /* THERMAL_H_ */