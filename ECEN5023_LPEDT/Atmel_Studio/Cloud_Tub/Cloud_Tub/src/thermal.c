/*
 * thermal.c
 *
 * Created: 11/13/2016 8:11:44 PM
 *  Author: Sean
 */ 

#include <stdbool.h>

#include "main.h"
#include "status_codes.h"
#include "thermal.h"

#define MIN_SETPOINT_TEMP_F       68  // F
#define MAX_SETPOINT_TEMP_F       104 // F

#define MIN_SETPOINT_TEMP_C       20  // C   
#define MAX_SETPOINT_TEMP_C       40  // C

uint8_t temp_set_point = 80;    // F
uint8_t water_temp = 255;

bool temp_valid = false;
bool degrees_F  = true;

void thermal_init(void)
{
    // Initialize relay drivers

    // Initialize ADC

}

void thermal_change_scale(bool new_scale)
{
    if (degrees_F != new_scale) {
        if (new_scale) 
            water_temp = celsius_to_fahrenheit(water_temp);
        else
            water_temp = fahrenheit_to_celsius(water_temp);
    }

}

enum status_code thermal_set_temperature(uint8_t temperature)
{        
    uint8_t temp_max, temp_min;

    if (degrees_F) {
        temp_min = MIN_SETPOINT_TEMP_F;
        temp_max = MAX_SETPOINT_TEMP_F;
    }
    else {
        temp_min = MIN_SETPOINT_TEMP_C;
        temp_max = MAX_SETPOINT_TEMP_C;
    }
    
    if ((temperature >= temp_min) && (temperature <= temp_max)) {
        temp_set_point = temperature;
        return STATUS_OK;
    }
    else
        return STATUS_ERR_BAD_DATA;
}

uint8_t thermal_get_temperature(void)
{
    return temp_set_point;
}

uint8_t thermal_get_water_temp(void)
{    
	return water_temp;
}
