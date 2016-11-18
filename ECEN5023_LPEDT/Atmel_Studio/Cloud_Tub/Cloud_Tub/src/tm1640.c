/*
 * tm1640.c
 *
 * Created: 9/26/2016 7:19:49 PM
 *  Author: Sean
 *
 * Titan Micro Electronics TM1640 LED Drive Control Driver
 */ 

#include "pinmux.h"
#include "spi.h"
#include "spi_interrupt.h"
#include "tm1640.h"

#define TM1640_BAUD_RATE    200000

#ifndef XPLAINED
#define TM1640_SERCOM       SERCOM4
#define TM1640_IRQ          SERCOM4_IRQn
#else
#define TM1640_SERCOM       SERCOM1
#define TM1640_IRQ          SERCOM1_IRQn
#endif

#define DATA_CMD_ADDR_INC   0x40
#define DATA_CMD_FIXED      0x44
#define DATA_CMD_NORMAL     0x40

#define ADDR_CMD_ADDR0      0xC0

struct spi_module spi_module;

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

static tm1640_state_t tm1640_get_state(void)
{    
    // State will be updated from an interrupt so disable interrupts
    // to make this interaction atomic.
    tm1640_state_t temp_state;
    NVIC_DisableIRQ(TM1640_IRQ);
    temp_state = tm1640_state;
    NVIC_EnableIRQ(TM1640_IRQ);
    return temp_state;
}

static void spi_cb_buffer_transmitted(struct spi_module *const module) 
{
    // This function will be called from an interrupt context
    switch (tm1640_state) {
    case STATE_SET_DATA:
        tm1640_state = STATE_SET_ADDR;
        spi_write_buffer_job(&spi_module, &tm1640_display_pkt.set_addr, TM1640_GRIDS + 1);
        spi_write(&spi_module, (uint16_t) tm1640_display_pkt.set_data);
        break;

    case STATE_SET_ADDR:
        tm1640_state = STATE_CONTROL;
        spi_write_buffer_job(&spi_module, &tm1640_display_pkt.brigtness, 1);
        break;

    case STATE_CONTROL:
    default:
        tm1640_state = STATE_IDLE;
        break;
    }
}

static void spi_cb_error(struct spi_module *const module) 
{
    printf("Spi error!\n");
    while(1) {}
}

void tm1640_init(void)
{
    struct spi_config config;
    struct system_pinmux_config do_pin;
    struct system_pinmux_config sck_pin;

    do_pin.powersave = false;
    do_pin.direction = SYSTEM_PINMUX_PIN_DIR_OUTPUT;
    do_pin.input_pull = SYSTEM_PINMUX_PIN_PULL_UP;

    sck_pin.powersave = false;
    sck_pin.direction = SYSTEM_PINMUX_PIN_DIR_OUTPUT;
    sck_pin.input_pull = SYSTEM_PINMUX_PIN_PULL_UP;

    config.character_size = SPI_CHARACTER_SIZE_8BIT;
    config.data_order = SPI_DATA_ORDER_LSB;
    config.mode = SPI_MODE_MASTER;
    config.transfer_mode = SPI_TRANSFER_MODE_3;
    config.select_slave_low_detect_enable = false;
#ifndef XPLAINED
    config.mux_setting = SPI_SIGNAL_MUX_SETTING_E;    

    //config.pinmux_pad0 = (uint32_t) PORT->Group[1].PMUX[10/2];
    
    // PB10 - DO, PB11 - SCK
    do_pin.mux_position = 0x3;
    sck_pin.mux_position = 0x3;
    system_pinmux_pin_set_config(32 + 10, &do_pin);
    system_pinmux_pin_set_config(32 + 11, &sck_pin);
#else
    //config.mux_setting = SPI_SIGNAL_MUX_SETTING_O;
    config.mux_setting = SPI_SIGNAL_MUX_SETTING_E;

    // PA16 - DO, PA19 - SCK
    do_pin.mux_position = 0x2;    
    sck_pin.mux_position = 0x2;
    //system_pinmux_pin_set_config(16, &do_pin);
    //system_pinmux_pin_set_config(19, &sck_pin);
#endif
    config.receiver_enable = false;
    config.master_slave_select_enable = false;
    config.run_in_standby = true;
    config.mode_specific.master.baudrate = TM1640_BAUD_RATE;
    config.generator_source = GCLK_GENERATOR_0;

    if (spi_init(&spi_module, TM1640_SERCOM, &config)) {
        printf("Failed to initialize TM1640_SERCOM!\n");
        return;
    }

    spi_register_callback(&spi_module, spi_cb_buffer_transmitted, SPI_CALLBACK_BUFFER_TRANSMITTED);      
    spi_enable_callback(&spi_module, SPI_CALLBACK_BUFFER_TRANSMITTED);  
    spi_register_callback(&spi_module, spi_cb_error, SPI_CALLBACK_ERROR);    
    spi_enable_callback(&spi_module, SPI_CALLBACK_ERROR);

    NVIC_EnableIRQ(TM1640_IRQ);
    spi_enable(&spi_module);
}

enum status_code tm1640_set_display(tm1640_display_t *disp, tm1640_brightness_t brightness)
{
    enum status_code status;

    // Transactions have to be broken up into 3 segments because the data and clock need to go high
    // for a period of time between the data command, the address/display data, and the display control.
    // Start off with a single byte transfer and then keep the transaction moving with the callback.
    if (tm1640_get_state() == STATE_IDLE) {
        tm1640_state = STATE_SET_DATA;

        memcpy(&tm1640_display_pkt.disp_data, disp->grid, TM1640_GRIDS);
        tm1640_display_pkt.set_addr = ADDR_CMD_ADDR0;
        tm1640_display_pkt.set_data = DATA_CMD_ADDR_INC;
        tm1640_display_pkt.brigtness = brightness;

        status = spi_write_buffer_job(&spi_module, &tm1640_display_pkt.set_data, 1);

        // First byte needs to be started manually
        if (!status) 
            return spi_write(&spi_module, (uint16_t) tm1640_display_pkt.set_data);
        else
            return status;
    }
    else
        return STATUS_BUSY;
}


