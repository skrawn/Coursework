/*
 * thermal.c
 *
 * Created: 11/13/2016 8:11:44 PM
 *  Author: Sean
 */ 

#include <stdbool.h>

#include "asf.h"
#include "main.h"
#include "status_codes.h"
#include "thermal.h"

#define MIN_SETPOINT_TEMP_F         68  // F
#define MAX_SETPOINT_TEMP_F         104 // F

#define MIN_SETPOINT_TEMP_C         20  // C   
#define MAX_SETPOINT_TEMP_C         40  // C

#define HEATER_1_GPIO               PIN_PA06
#define HEATER_2_GPIO               PIN_PA25
#define AIR_PUMP_GPIO               PIN_PA24
#define WATER_PUMP_GPIO             PIN_PA18

#define RELAY_ON_DELAY              10  // ms
#define RELAY_OFF_DELAY             5 // ms

#define WATER_TEMP_GPIO             PIN_PA10
#define WATER_TEMP_AN_CH            ADC_POSITIVE_INPUT_PIN18
#define WATER_TEMP_CH_NUM           18

#define PUMP_HALL_EFF_GPIO          PIN_PA07
#define PUMP_HALL_EFF_AN_CH         ADC_POSITIVE_INPUT_PIN7
#define PUMP_HALL_EFF_CH_NUM        7

#define WATER_TEMP_INVALID          0xFF

uint32_t analog_channels[] = {
    WATER_TEMP_AN_CH, PUMP_HALL_EFF_AN_CH
};

uint16_t adc_result_buf[WATER_TEMP_CH_NUM - PUMP_HALL_EFF_CH_NUM] = {0};

typedef struct {
    uint8_t temperature;
    uint16_t digital_value;
} lut_t;

const lut_t water_temp_LUT[] = {
    // Degrees F, Digital Value
    {0,  197}, {1,  200}, {2,  202}, {3,  204}, {4,  206},
    {5,  208}, {6,  210}, {7,  212}, {8,  214}, {9,  217},
    {10,  219}, {11,  221}, {12,  224}, {13,  226}, {14,  229},
    {15,  231}, {16,  234}, {17,  237}, {18,  239}, {19,  242},
    {20,  245}, {21,  248}, {22,  251}, {23,  254}, {24,  257},
    {25,  260}, {26,  264}, {27,  267}, {28,  271}, {29,  274},
    {30,  278}, {31,  282}, {32,  286}, {33,  290}, {34,  294},
    {35,  298}, {36,  302}, {37,  307}, {38,  312}, {39,  317},
    {40,  321}, {41,  326}, {42,  331}, {43,  337}, {44,  343},
    {45,  348}, {46,  354}, {47,  361}, {48,  367}, {49,  374},
    {50,  380}, {51,  388}, {52,  395}, {53,  403}, {54,  411},
    {55,  419}, {56,  428}, {57,  437}, {58,  446}, {59,  456},
    {60,  466}, {61,  477}, {62,  488}, {63,  500}, {64,  513},
    {65,  526}, {66,  536}, {67,  547}, {68,  558}, {69,  570},
    {70,  582}, {71,  593}, {72,  604}, {73,  617}, {74,  629},
    {75,  642}, {76,  656}, {77,  670}, {78,  685}, {79,  700},
    {80,  716}, {81,  729}, {82,  742}, {83,  756}, {84,  770},
    {85,  784}, {86,  800}, {87,  816}, {88,  833}, {89,  850},
    {90,  868}, {91,  882}, {92,  897}, {93,  912}, {94,  927},
    {95,  943}, {96,  960}, {97,  977}, {98,  995}, {99,  1016},
    {100, 1034}, {101, 1053}, {102, 1074}, {103, 1095}, {104, 1117}   
};

SemaphoreHandle_t therm_sem = NULL;

struct adc_module adc_instance;

struct thermal_state {
    bool heater_1_state;
    bool heater_2_state;
    bool air_pump_state;
    bool water_pump_state;

    bool temp_valid;
    bool degrees_F;

    uint8_t temp_set_point;
    uint8_t water_temp;

} thermal_state;

static void task_Thermal(void *args)
{
    while(1) {
        xSemaphoreTake(therm_sem, portMAX_DELAY);

        if (port_pin_get_output_level(HEATER_1_GPIO) != thermal_state.heater_1_state) {
            if (port_pin_get_output_level(HEATER_1_GPIO)) {
                port_pin_set_output_level(HEATER_1_GPIO, 0);
                vTaskDelay(pdMS_TO_TICKS(RELAY_OFF_DELAY));
            }
            else {
                port_pin_set_output_level(HEATER_1_GPIO, 1);
                vTaskDelay(pdMS_TO_TICKS(RELAY_ON_DELAY));
            }
        }

        if (port_pin_get_output_level(HEATER_2_GPIO) != thermal_state.heater_2_state) {
            if (port_pin_get_output_level(HEATER_2_GPIO)) {
                port_pin_set_output_level(HEATER_2_GPIO, 0);
                vTaskDelay(pdMS_TO_TICKS(RELAY_OFF_DELAY));
            }
            else {
                port_pin_set_output_level(HEATER_2_GPIO, 1);
                vTaskDelay(pdMS_TO_TICKS(RELAY_ON_DELAY));
            }
        }
         
        if (port_pin_get_output_level(AIR_PUMP_GPIO) != thermal_state.air_pump_state) {
            if (port_pin_get_output_level(AIR_PUMP_GPIO)) {
                port_pin_set_output_level(AIR_PUMP_GPIO, 0);
                vTaskDelay(pdMS_TO_TICKS(RELAY_OFF_DELAY));
            }
            else {
                port_pin_set_output_level(AIR_PUMP_GPIO, 1);
                vTaskDelay(pdMS_TO_TICKS(RELAY_ON_DELAY));
            }
        }

        if (port_pin_get_output_level(WATER_PUMP_GPIO) != thermal_state.water_pump_state) {
            if (port_pin_get_output_level(WATER_PUMP_GPIO)) {
                port_pin_set_output_level(WATER_PUMP_GPIO, 0);
                vTaskDelay(pdMS_TO_TICKS(RELAY_OFF_DELAY));
            }
            else {
                port_pin_set_output_level(WATER_PUMP_GPIO, 1);
                vTaskDelay(pdMS_TO_TICKS(RELAY_ON_DELAY));
            }
        }             
    }
}

static uint8_t thermal_look_up_water_temp(uint16_t digital_value)
{
    uint32_t i = 0;    

    while (i < (sizeof(water_temp_LUT) / sizeof(lut_t))) {
        if (digital_value <= water_temp_LUT[i].digital_value) {
            return water_temp_LUT[i].temperature;        
        }
        i++;
    }

    // If no valid value is found, return an invalid result
    return WATER_TEMP_INVALID;
}

void thermal_init(void)
{
    memset(&thermal_state, 0, sizeof(thermal_state));
    thermal_state.degrees_F = true;
    thermal_state.temp_set_point = 80;  //F
    thermal_state.water_temp = WATER_TEMP_INVALID;

    struct port_config gpio_config;
    gpio_config.direction = PORT_PIN_DIR_OUTPUT_WTH_READBACK;
    gpio_config.input_pull = PORT_PIN_PULL_DOWN;
    gpio_config.powersave = false;

    // Initialize relay drivers
    port_pin_set_config(HEATER_1_GPIO, &gpio_config);
    port_pin_set_config(HEATER_2_GPIO, &gpio_config);
    port_pin_set_config(AIR_PUMP_GPIO, &gpio_config);
    port_pin_set_config(WATER_PUMP_GPIO, &gpio_config);
    
    // Initialize ADC
    struct adc_config config_adc;
    adc_get_config_defaults(&config_adc);
    
    config_adc.clock_prescaler = ADC_CLOCK_PRESCALER_DIV512;
    config_adc.reference = ADC_REFERENCE_INTVCC1;   
    config_adc.resolution = ADC_RESOLUTION_12BIT;
    config_adc.clock_source = GCLK_GENERATOR_0;
    config_adc.positive_input = WATER_TEMP_AN_CH;
    config_adc.gain_factor = ADC_GAIN_FACTOR_DIV2;
    
    adc_init(&adc_instance, ADC, &config_adc);

    adc_regular_ain_channel(analog_channels, sizeof(analog_channels) / sizeof(uint32_t));

    adc_enable(&adc_instance);

    if (xTaskCreate(task_Thermal, "task_Therm", configMINIMAL_STACK_SIZE, 0, tskIDLE_PRIORITY, NULL) != pdPASS) {
        printf("Failed to create task_Thermal thread!\n");
    }

    therm_sem = xSemaphoreCreateBinary();    
    if (therm_sem == NULL) {
        printf("Failed to create therm_sem!\n");
    }

    // Start the conversion
    adc_start_conversion(&adc_instance);
}

void thermal_3s(void)
{
    uint16_t result;
    uint8_t temp_water_temp;

    if (adc_get_status(&adc_instance) & ADC_STATUS_RESULT_READY) {
        adc_read(&adc_instance, &result);

        // Check the thermistor LUT
        temp_water_temp = thermal_look_up_water_temp(result);

        // Set water temperature (converting to whatever our units are set to)
        if (temp_water_temp != WATER_TEMP_INVALID) {
            thermal_state.water_temp = temp_water_temp;

            // Turn on the heater if the pump is on and the temperature is below the set point
            if (thermal_state.water_temp < thermal_state.temp_set_point &&
                 thermal_state.water_pump_state && 
                !(thermal_state.heater_1_state && thermal_state.heater_2_state)) {
                thermal_turn_on_heater(true);
            }
        }

        // Start the next conversion
        adc_start_conversion(&adc_instance);
    }
}

void thermal_change_scale(bool new_scale)
{
    if (thermal_state.degrees_F != new_scale) {
        thermal_state.degrees_F = new_scale;
        if (new_scale) {
            if (thermal_state.water_temp != WATER_TEMP_INVALID) 
                thermal_state.water_temp = celsius_to_fahrenheit(thermal_state.water_temp);       
                
            thermal_state.temp_set_point = celsius_to_fahrenheit(thermal_state.temp_set_point);
        }
        else {
            if (thermal_state.water_temp != WATER_TEMP_INVALID)
                thermal_state.water_temp = fahrenheit_to_celsius(thermal_state.water_temp);

            thermal_state.temp_set_point = fahrenheit_to_celsius(thermal_state.temp_set_point);
        }
    }

}

enum status_code thermal_set_temperature(uint8_t temperature)
{        
    uint8_t temp_max, temp_min, temp_scaled;

    temp_scaled = thermal_state.temp_set_point;

    if (thermal_state.degrees_F) {
        temp_min = MIN_SETPOINT_TEMP_F;
        temp_max = MAX_SETPOINT_TEMP_F;
    }
    else {        
        temp_scaled = fahrenheit_to_celsius(temp_scaled);
        temp_min = MIN_SETPOINT_TEMP_C;
        temp_max = MAX_SETPOINT_TEMP_C;
    }
    
    if ((temperature >= temp_min) && (temperature <= temp_max)) {
        // Only store/manipulate with degrees F
        if (!thermal_state.degrees_F) {
            thermal_state.temp_set_point = celsius_to_fahrenheit(temperature);
        }
        else
            thermal_state.temp_set_point = temperature;

        return STATUS_OK;
    }
    else
        return STATUS_ERR_BAD_DATA;
}

uint8_t thermal_get_temperature(void)
{
    return thermal_state.temp_set_point;
}

uint8_t thermal_get_water_temp(void)
{    
    uint8_t return_temp = thermal_state.water_temp;

    if (!thermal_state.degrees_F) {
        return_temp = fahrenheit_to_celsius(thermal_state.water_temp);
    }

	return return_temp;    
}

void thermal_turn_on_heater(bool on)
{
    thermal_state.heater_1_state = on;
    thermal_state.heater_2_state = on;
    xSemaphoreGive(therm_sem);
}

void thermal_turn_on_water_pump(bool on)
{
    thermal_state.water_pump_state = on;
    xSemaphoreGive(therm_sem);
}

void thermal_turn_on_air_pump(bool on)
{
    thermal_state.air_pump_state = on;
    xSemaphoreGive(therm_sem);
}