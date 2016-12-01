/*
 * tm1640.c
 *
 * Created: 9/26/2016 7:19:49 PM
 *  Author: Sean
 *
 * Titan Micro Electronics TM1640 LED Drive Control Driver
 */ 

#include "main.h"
#include "pinmux.h"
#include "spi.h"
#include "spi_interrupt.h"
#include "tm1640.h"
#include "wtc6508.h"

#define DISPLAY_MUTEX_TIMEOUT     pdMS_TO_TICKS(5)

#define TM1640_BAUD_RATE    15000

#define TM1640_BIT_TIME     1   // us

#ifndef XPLAINED
#define TM1640_SERCOM       SERCOM4
#define TM1640_IRQ          SERCOM4_IRQn

#define TM1640_PINMUX_PAD0  PINMUX_UNUSED
#define TM1640_PINMUX_PAD1  PINMUX_UNUSED
#define TM1640_PINMUX_PAD2  PINMUX_PB10D_SERCOM4_PAD2
#define TM1640_PINMUX_PAD3  PINMUX_UNUSED

#define TM1640_DOUT_PIN     PIN_PB10
#define TM1640_CLK_PIN      PIN_PA09

#else
#define TM1640_SERCOM       SERCOM1
#define TM1640_IRQ          SERCOM1_IRQn

#define TM1640_PINMUX_PAD0
#define TM1640_PINMUX_PAD1
#define TM1640_PINMUX_PAD2
#define TM1640_PINMUX_PAD3

#endif

#define CTRL_CMD            0x80
#define CTRL_CMD_DISP_ON    0x08
#define CTRL_CMD_DISP_OFF   0x00
#define CTRL_CMD_MASK       0xC0

#define DATA_CMD_ADDR_INC   0x40
#define DATA_CMD_FIXED      0x44
#define DATA_CMD_NORMAL     0x40

#define ADDR_CMD_ADDR0      0xC0

struct spi_module tm1640_module;

struct {
    uint8_t set_data;
    uint8_t set_addr;
    uint8_t disp_data[TM1640_GRIDS];
    uint8_t brigtness;
} tm1640_display_pkt;

typedef enum {
    STATE_IDLE,
    STATE_SET_DATA,
    STATE_SET_ADDR,
    STATE_CONTROL
} tm1640_state_t;

volatile tm1640_state_t tm1640_state;
volatile uint8_t transfer_complete = 1;
SemaphoreHandle_t tm1640_sem;
uint8_t wtc_bus_dummy_data[TM1640_GRIDS + 1] = {0};

static inline void tm1640_start(void)
{
    // Switch the CLK pin over to GPIO
    struct port_config gpio_conf;  
    gpio_conf.direction = PORT_PIN_DIR_OUTPUT;
    gpio_conf.input_pull = PORT_PIN_PULL_UP;
    gpio_conf.powersave = false;
    
    port_pin_set_config(TM1640_CLK_PIN, &gpio_conf);

    // Bring DOUT and CLK low for 1us each    
    port_pin_set_output_level(TM1640_DOUT_PIN, 0);
    delay_us(TM1640_BIT_TIME);
    port_pin_set_output_level(TM1640_CLK_PIN, 0);    
}

static inline void tm1640_stop(void)
{
    // Switch the DO and CLK pins over to GPIOs    
    struct system_pinmux_config clk_conf;        
        
    // Bring DOUT low for 1us, then bring DOUT and CLK high for 1us each    
    port_pin_set_output_level(TM1640_DOUT_PIN, 0);
    delay_us(TM1640_BIT_TIME);
    port_pin_set_output_level(WTC6508_CLK_GPIO, 1);
    delay_us(TM1640_BIT_TIME);
    port_pin_set_output_level(TM1640_DOUT_PIN, 1);    

    // Restore peripheral control
    system_pinmux_get_config_defaults(&clk_conf);
    clk_conf.direction = SYSTEM_PINMUX_PIN_DIR_INPUT;
    clk_conf.mux_position = WTC6508_PINMUX_PAD1 & 0xFFFF;
    system_pinmux_pin_set_config(WTC6508_PINMUX_PAD1 >> 16, &clk_conf);    
}

static inline void tm1640_write(uint8_t byte)
{
    uint8_t bit;

    // TM1640 requires LSB to be transmitted first
    for (bit = 0; bit < 8; bit++) {
        if ((byte >> bit) & 0x01) 
            port_pin_set_output_level(TM1640_DOUT_PIN, 1);    
        else
            port_pin_set_output_level(TM1640_DOUT_PIN, 0);    

        delay_us(TM1640_BIT_TIME);
        port_pin_set_output_level(TM1640_CLK_PIN, 1);
        delay_us(TM1640_BIT_TIME);
        port_pin_set_output_level(TM1640_CLK_PIN, 0);
        delay_us(TM1640_BIT_TIME);
    }
}


static enum status_code tm1640_write_cmd(uint8_t cmd, uint8_t data)
{
    taskENTER_CRITICAL();
    
    tm1640_start();
    tm1640_write((cmd & CTRL_CMD_MASK) | (data & ~CTRL_CMD_MASK));
    tm1640_stop();

    taskEXIT_CRITICAL();

    return STATUS_OK;
}

void tm1640_init(void)
{
    struct port_config gpio_conf;
    gpio_conf.direction = PORT_PIN_DIR_OUTPUT;
    gpio_conf.input_pull = PORT_PIN_PULL_UP;
    gpio_conf.powersave = false;
    port_pin_set_config(TM1640_DOUT_PIN, &gpio_conf);
    port_pin_set_output_level(TM1640_DOUT_PIN, 1);
}


enum status_code tm1640_display_on(uint8_t on)
{
    uint8_t display_on = CTRL_CMD_DISP_OFF;

    if (on) 
        display_on = CTRL_CMD_DISP_ON | BRIGHT_3; 

    return tm1640_write_cmd(CTRL_CMD, display_on); 
}

enum status_code tm1640_set_display(uint8_t *disp, uint8_t length, tm1640_brightness_t brightness)
{
    enum status_code status = STATUS_OK;
    struct port_config di_conf;    
    struct system_pinmux_config pin_conf;
    uint8_t grids;
       
    // Take the display bus mutex
    if (!xSemaphoreTake(display_mutex, portMAX_DELAY)) {
        // Timeout waiting for semaphore. Just return
        return STATUS_ERR_TIMEOUT;
    }

    taskENTER_CRITICAL();

    system_pinmux_get_config_defaults(&pin_conf);
    pin_conf.direction = SYSTEM_PINMUX_PIN_DIR_INPUT;
    pin_conf.mux_position = PINMUX_PA08C_SERCOM0_PAD0 & 0xFFFF;

    // Data input pin for the WTC6508 needs to be set to a GPIO so it isn't driven
    // when the clock is generated for the TM1640
    di_conf.direction = PORT_PIN_DIR_INPUT;
    di_conf.input_pull = PORT_PIN_PULL_UP;
    di_conf.powersave = false;
    port_pin_set_config(WTC6508_DI_GPIO, &di_conf);

    tm1640_start();
    tm1640_write(ADDR_CMD_ADDR0);
    for (grids = 0; grids < length; grids++) {
        tm1640_write(disp[grids]);
    }
    tm1640_stop();                    

    // Restore pin function to the WTC6508 SPI
    system_pinmux_pin_set_config(PINMUX_PA08C_SERCOM0_PAD0 >> 16, &pin_conf);       
    
    taskEXIT_CRITICAL();                 

    xSemaphoreGive(display_mutex);     
    return status;
}

