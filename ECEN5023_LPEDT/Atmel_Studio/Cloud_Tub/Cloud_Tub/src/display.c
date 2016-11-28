/*
 * display.c
 *
 * Created: 11/25/2016 1:19:04 PM
 *  Author: Sean
 */ 

#include <stdio.h>

#include "display.h"
#include "tm1640.h"
#include "wtc6508.h"

#define BUTTON_LOCK_UNLOCK      0x80
#define BUTTON_WATER_PUMP       0x40
#define BUTTON_HEATER           0x20
#define BUTTON_AIR_PUMP         0x04
#define BUTTON_DOWN             0x02
#define BUTTON_UP               0x01

#define DISPLAY_TEST_MODE       (1)

static void display_button_lock_unlock_handler(void)
{

}

static void display_button_water_pump_handler(void)
{

}

static void display_button_heater_handler(void)
{

}

static void display_button_air_pump_handler(void)
{

}

static void display_button_down_handler(void)
{

}

static void display_button_up_handler(void)
{

}

typedef struct {
    uint8_t button_mask;
    void (*handler)(void);
} display_handler_t;

static const display_handler_t display_handlers[] = {
    { BUTTON_LOCK_UNLOCK,   display_button_lock_unlock_handler },
    { BUTTON_WATER_PUMP,    display_button_water_pump_handler },
    { BUTTON_HEATER,        display_button_heater_handler },
    { BUTTON_AIR_PUMP,      display_button_air_pump_handler },
    { BUTTON_DOWN,          display_button_down_handler },
    { BUTTON_UP,            display_button_up_handler },
    { 0, NULL },
};

void display_init(void)
{
    wtc6508_init();   
    tm1640_init();
}


void display_update_50Hz(void)
{
    uint8_t status = 0;
    uint8_t button_mask = 0x1;
    
    // Check for user input
    if (wtc6508_read(&status)) {
        printf("Error! Unable to read button status\n");
        status = 0;
    }

    // Handle user input
    if (status) {
        if (status & button_mask) {

        }
    }

#if !DISPLAY_TEST_MODE    
    // Get temperature and error statuses

    // Update display if necessary
#endif
}

const tm1640_seg_t tm1640_segments[] = {
    SEG_OFF, SEG_a, SEG_b, SEG_c, SEG_d, SEG_e, SEG_f, SEG_g,
    SEG_0, SEG_1, SEG_2, SEG_3, SEG_4, SEG_5, SEG_6, SEG_7, SEG_8, SEG_9, 
    SEG_C, SEG_F, SEG_E, SEG_H, SEG_h
};

void display_led_test_1Hz(void)
{
#if DISPLAY_TEST_MODE    
    static uint8_t grid_number = 0;
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
