/*
 * tm1640.h
 *
 * Created: 9/26/2016 7:20:15 PM
 *  Author: Sean
 *
 * Titan Micro Electronics TM1640 LED Drive Control Driver
 */ 


#ifndef TM1640_H_
#define TM1640_H_

#define TM1640_SEGMENTS     8
#define TM1640_GRIDS        16

// Encoding for common segments
typedef enum {
    SEG_OFF = 0,

    // Individual Segment Controls
    SEG_a = 0x01,
    SEG_b = 0x02,
    SEG_c = 0x04,
    SEG_d = 0x08,
    SEG_e = 0x10,
    SEG_f = 0x20,
    SEG_g = 0x40,
    SEG_DP = 0x80,

    // Numbers
    SEG_0 = 0x3F,
    SEG_1 = 0x06,
    SEG_2 = 0x5B,
    SEG_3 = 0x4F,
    SEG_4 = 0x66,
    SEG_5 = 0x6D,
    SEG_6 = 0x7D,
    SEG_7 = 0x07,
    SEG_8 = 0x7F,
    SEG_9 = 0x6F,

    // Letters
    SEG_C = 0x39,
    SEG_F = 0x71,
    SEG_E = 0x79,    
    SEG_H = 0x76,
    SEG_h = 0x74
} tm1640_seg_t;

typedef struct {
    tm1640_seg_t grid[TM1640_GRIDS];
}tm1640_display_t;

typedef enum {
    BRIGHT_MIN = 0x88,
    BRIGHT_1,
    BRIGHT_2,
    BRIGHT_3,
    BRIGHT_4,
    BRIGHT_5,
    BRIGHT_6,    
    BRIGHT_MAX
} tm1640_brightness_t;

// Adds the decimal point
#define DP_MASK     0x80   

void tm1640_init(void);
enum status_code tm1640_set_display(uint8_t *disp, uint8_t length, tm1640_brightness_t brightness);
enum status_code tm1640_display_on(uint8_t on);

#endif /* TM1640_H_ */