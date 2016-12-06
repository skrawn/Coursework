/*
 * display.h
 *
 * Created: 11/25/2016 1:19:37 PM
 *  Author: Sean
 */ 


#ifndef DISPLAY_H_
#define DISPLAY_H_

#include "tm1640.h"

typedef struct {
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
} display_state_t;

void display_init(void);
void display_idle(void);
void display_update_1Hz(void);
void display_update_33Hz(void);
void display_led_test_1Hz(void);
display_state_t *display_get_display_state(void);


#endif /* DISPLAY_H_ */