/*
 * display.c
 *
 * Created: 11/25/2016 1:19:04 PM
 *  Author: Sean
 */ 

#include <stdbool.h>
#include <stdio.h>

#include "asf.h"
#include "display.h"
#include "main.h"
#include "thermal.h"
#include "tm1640.h"
#include "wtc6508.h"

#define BUTTON_LOCK_UNLOCK      0x01
#define BUTTON_TIMER            0x02
#define BUTTON_WATER_PUMP       0x04
#define BUTTON_C_F              0x08
#define BUTTON_HEATER           0x10
#define BUTTON_AIR_PUMP         0x20
#define BUTTON_DOWN             0x40
#define BUTTON_UP               0x80

#define TOUCH_DETECT_DELAY      5

#define DISPLAY_TEST_MODE       (0)

#define DISPLAY_LOCK_TIME       10000   // 5 minutes in 30ms increments
// Unlock time essentially 3 seconds, but needs to take into account the ~150ms
// delay between touch detection. 
#define DISPLAY_UNLOCK_TIME     16     // 3 seconds in 180ms increments

// When making a setting change, the display will blink for 5 seconds
#define DISPLAY_BLINK_TIME      5

#define DISPLAY_CONFIG_LENGTH   5

#define WATER_TEMP_HYST         1

// Lights controlled by grid 3
#define LED_PUMP            SEG_b
#define LED_HEATON          SEG_c
#define LED_HEATOFF         SEG_d
#define LED_TIMER           SEG_e
#define LED_LOCK            SEG_f
#define LED_TIMER_SET       SEG_g
#define LED_CLOCK           SEG_DP 

// Lights controlled by grid 4
#define LED_DEG_C           SEG_a
#define LED_DEG_F           SEG_b
#define LED_BUBBLES         SEG_c

struct display_state {
    tm1640_seg_t char_display[3];        
    bool degrees_F;
    bool timer_set;
    bool timer_on;
    bool heater_on;
    bool bubbles_on;
    bool pump_on;

    bool display_update;
    bool display_blink;
    uint8_t display_blink_timer;

    bool display_locked;
    uint32_t display_lock_timer;
    uint32_t display_unlock_timer;
} display_state;

const tm1640_seg_t num_to_seg[] = {
    SEG_0, SEG_1, SEG_2, SEG_3, SEG_4, SEG_5, SEG_6, SEG_7, SEG_8, SEG_9
};

SemaphoreHandle_t display_update_mutex;

static void display_update(bool update)
{
    taskENTER_CRITICAL();
    
    display_state.display_update = update;
    display_state.display_blink = false;
    display_state.display_blink_timer = 0;
    
    taskEXIT_CRITICAL();
}

static void display_number_to_seg(uint8_t num, uint8_t *chars)
{
    if (num / 100)
        chars[0] = num_to_seg[num/100];
    else
        chars[0] = SEG_OFF;

    num -= (num / 100) * 100;    
    chars[1] = num_to_seg[num / 10];

    num -= (num / 10) * 10;    
    chars[2] = num_to_seg[num];
}

static void display_button_lock_unlock_handler(void)
{
    if (display_state.display_locked) {
        if (display_state.display_unlock_timer++ > DISPLAY_UNLOCK_TIME) {
            xSemaphoreGive(buzzer_sem);
            display_state.display_locked = false;
            display_state.display_unlock_timer = 0;
            display_state.display_lock_timer = 0;
            display_update(true);            
        }                
    }
}

static void display_timer_handler(void)
{
    if (!display_state.display_locked) {
        display_state.display_lock_timer = 0;
        xSemaphoreGive(buzzer_sem);
    }

}

static void display_button_water_pump_handler(void)
{
    if (!display_state.display_locked) {
        xSemaphoreGive(buzzer_sem);
        display_state.display_lock_timer = 0;    
        
        // Water and air pump cannot run at the same time
        if (display_state.bubbles_on) {
            display_state.bubbles_on = false;

            thermal_turn_on_air_pump(false);
        }
        
        if (display_state.pump_on) {
            display_state.pump_on = false;
            thermal_turn_on_water_pump(false);

            // Pump is already on. Open pump and heater relays (if heater is on)
            if (display_state.heater_on) {
                display_state.heater_on = false;
                thermal_turn_on_heater(false);
            }                        
        }
        else {
            display_state.pump_on = true;

            thermal_turn_on_water_pump(true);
        }
                
        display_update(true);
    }
}

static void display_cf_handler(void)
{
    if (!display_state.display_locked) {
        xSemaphoreGive(buzzer_sem);
        display_state.display_lock_timer = 0;        
        display_state.degrees_F ^= 1;
        thermal_change_scale(display_state.degrees_F);
        display_update(true);        
    }
}

static void display_button_heater_handler(void)
{
    if (!display_state.display_locked) {
        xSemaphoreGive(buzzer_sem);
        display_state.display_lock_timer = 0;
        display_state.heater_on ^= 1;        

        if (display_state.bubbles_on) {
            display_state.bubbles_on = false;

            thermal_turn_on_air_pump(false);
        }

        // Pump must be running before turning on heater
        if (!display_state.pump_on) {            
            display_state.pump_on = true;

            thermal_turn_on_water_pump(true);            
        }

        thermal_turn_on_heater(display_state.heater_on);

        display_update(true);
    }
}

static void display_button_air_pump_handler(void)
{
    if (!display_state.display_locked) {
        // Water pump/heater cannot run while air pump is on
        display_state.bubbles_on ^= 1;

        if (display_state.pump_on && display_state.bubbles_on) {                        
            display_state.pump_on = false;
            display_state.heater_on = false;

            thermal_turn_on_heater(false);
            thermal_turn_on_water_pump(false);
        }   
        
        thermal_turn_on_air_pump(display_state.bubbles_on);

        xSemaphoreGive(buzzer_sem);
        display_state.display_lock_timer = 0;                   

        display_update(true);
    }
}

static void display_button_down_handler(void)
{
    uint8_t current_temp;

    if (!display_state.display_locked) {
        display_state.display_lock_timer = 0;

        // Try to set the display temperature
        current_temp = thermal_get_temperature();
        if (thermal_set_temperature(current_temp-1) == STATUS_OK) {
            xSemaphoreGive(buzzer_sem);
            display_update(false);
            display_state.display_blink = true;
            display_state.display_blink_timer = 0;            
        }
    }
}

static void display_button_up_handler(void)
{
    uint8_t current_temp;

    if (!display_state.display_locked) {
        display_state.display_lock_timer = 0;

        // Try to set the display temperature
        current_temp = thermal_get_temperature();
        if (thermal_set_temperature(current_temp+1) == STATUS_OK) {
            xSemaphoreGive(buzzer_sem);            
            display_update(false);
            display_state.display_blink = true;
            display_state.display_blink_timer = 0;            
        }
    }
}

static void display_set_display(uint8_t *new_conf)
{    
    new_conf[0] = display_state.char_display[0];
    new_conf[1] = display_state.char_display[1];
    new_conf[2] = display_state.char_display[2];
        
    if (display_state.bubbles_on)
        new_conf[4] |= LED_BUBBLES;

    if (display_state.degrees_F)
        new_conf[4] |= LED_DEG_F;
    else
        new_conf[4] |= LED_DEG_C;

    if (display_state.pump_on) {
        new_conf[3] |= LED_PUMP;
        new_conf[3] |= LED_TIMER;
        if (display_state.heater_on)
            new_conf[3] |= LED_HEATON;
        else
            new_conf[3] |= LED_HEATOFF;
    }

    if (display_state.timer_set)
        new_conf[3] |= LED_TIMER_SET;

    if (display_state.display_locked)
        new_conf[3] |= LED_LOCK;

    if (display_state.timer_on)
        new_conf[3] |= LED_CLOCK;    
}

typedef struct {
    uint8_t button_mask;
    void (*handler)(void);
} display_handler_t;

static const display_handler_t display_handlers[] = {
    { BUTTON_LOCK_UNLOCK,   display_button_lock_unlock_handler },
    { BUTTON_WATER_PUMP,    display_button_water_pump_handler },
    { BUTTON_C_F,           display_cf_handler },
    { BUTTON_HEATER,        display_button_heater_handler },
    { BUTTON_AIR_PUMP,      display_button_air_pump_handler },
    { BUTTON_DOWN,          display_button_down_handler },
    { BUTTON_UP,            display_button_up_handler },
    {BUTTON_TIMER,          display_timer_handler },
    { 0, NULL },
};

void display_init(void)
{
    wtc6508_init();   
    tm1640_init();

    memset(&display_state, 0, sizeof(display_state));

    // Set the initial display state
    display_state.char_display[0] = SEG_0;
    display_state.char_display[1] = SEG_0;
    display_state.char_display[2] = SEG_0;

    display_state.degrees_F = true;        
    display_state.display_update = true;    

    display_update_mutex = xSemaphoreCreateMutex();
}

void display_idle(void)
{
    uint8_t disp_conf[DISPLAY_CONFIG_LENGTH] = {0};

    if (display_state.display_update && !display_state.display_blink) {
        display_update(false);

        // Perform a display update        
        display_set_display(disp_conf);

        tm1640_set_display(disp_conf, DISPLAY_CONFIG_LENGTH, BRIGHT_MAX);
    }
}

void display_update_1Hz(void)
{
    static bool blink_state = false;
    uint8_t disp_conf[DISPLAY_CONFIG_LENGTH] = {0};
    uint8_t temperature;

    if (display_state.display_blink) {

        display_set_display(disp_conf);
        // Only the character display should blink        
        if (blink_state) {
            disp_conf[0] = SEG_OFF;
            disp_conf[1] = SEG_OFF;
            disp_conf[2] = SEG_OFF;
        }
        else {
            temperature = thermal_get_temperature();
            display_number_to_seg(temperature, disp_conf);
        }

        if (display_state.display_blink_timer++ >= DISPLAY_BLINK_TIME) {
            display_state.display_blink_timer = 0;
            display_state.display_blink = 0;
            blink_state = false;
        }
        else
            blink_state ^= 1;
                        
        tm1640_set_display(disp_conf, DISPLAY_CONFIG_LENGTH, BRIGHT_MAX);
    }
    else {
        // Check to see if the temperature changed
        temperature = thermal_get_water_temp();
        if (temperature != 0xFF) {
            // 255 represents an invalid/un-updated temperature
            display_number_to_seg(temperature, (uint8_t *) &display_state.char_display[0]);
        }        
        else {
            display_state.char_display[0] = SEG_0;
            display_state.char_display[1] = SEG_0;
            display_state.char_display[2] = SEG_0;
        }
        display_update(true);
    }    

    if (display_state.heater_on) {
        if (thermal_get_water_temp() >= (thermal_get_temperature() + WATER_TEMP_HYST)) {
            display_state.heater_on = false;            
            thermal_turn_on_heater(false);
            display_update(true);
        }
    }
    else {
        if (display_state.pump_on) {
            if (thermal_get_water_temp() < (thermal_get_temperature() - WATER_TEMP_HYST)) {
                display_state.heater_on = true;                
                thermal_turn_on_heater(true);
                display_update(true);
            }
        }
    }
}

void display_update_33Hz(void)
{
    static uint8_t touch_detect_delay = 0;

    uint8_t status = 0;    
    uint8_t i = 0;
    
    if (!touch_detect_delay) {
        // Check for user input
        if (wtc6508_read(&status)) {
            printf("Error! Unable to read button status\n");
            status = 0;
        }

        // Handle user input
        if (status) {          
            touch_detect_delay = TOUCH_DETECT_DELAY;            

            while (display_handlers[i].button_mask) {
                if ((display_handlers[i].button_mask & status) && 
                     display_handlers[i].handler != NULL ) {
                     display_handlers[i].handler();
                }
                i++;
            }
        }
        else if (display_state.display_unlock_timer > 0)
            // Reset the display unlock timer because the user isn't holding the button anymore
            display_state.display_unlock_timer = 0;
    }
    else
        touch_detect_delay--;

    if (!display_state.display_locked && display_state.display_lock_timer++ > DISPLAY_LOCK_TIME) {
        display_state.display_locked = true;
        display_update(true);
    }
}

const tm1640_seg_t tm1640_segments[] = {
    SEG_OFF, SEG_a, SEG_b, SEG_c, SEG_d, SEG_e, SEG_f, SEG_g, SEG_DP,
//    SEG_0, SEG_1, SEG_2, SEG_3, SEG_4, SEG_5, SEG_6, SEG_7, SEG_8, SEG_9, 
//    SEG_C, SEG_F, SEG_E, SEG_H, SEG_h
};

void display_led_test_1Hz(void)
{
#if DISPLAY_TEST_MODE    
    static uint8_t grid_number = 3;
    static uint8_t segment_number = 0;

    tm1640_display_t disp_conf = {0};    
    disp_conf.grid[grid_number] = tm1640_segments[segment_number];
    tm1640_set_display(&disp_conf, BRIGHT_MAX);

    segment_number = (segment_number + 1) % (sizeof(tm1640_segments) / sizeof(tm1640_seg_t));

    if (segment_number == 0) {
        grid_number = (grid_number + 1) % TM1640_GRIDS;    
    }       

#endif
}

void display_buzzer_beep(void)
{



}
