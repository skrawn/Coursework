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
//#define TM1640_PINMUX_PAD3  PINMUX_PB11D_SERCOM4_PAD3

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
    static BaseType_t wakeTask;

    // This function will be called from an interrupt context
    wakeTask = pdFALSE;
    switch (tm1640_state) {
    case STATE_SET_DATA:
        tm1640_state = STATE_SET_ADDR;
        spi_write_buffer_job(&tm1640_module, &tm1640_display_pkt.set_addr, TM1640_GRIDS + 1);
        spi_write_buffer_job(&wtc6508_module, wtc_bus_dummy_data, TM1640_GRIDS + 1);
        spi_write(&tm1640_module, (uint16_t) tm1640_display_pkt.set_addr);
        spi_write(&wtc6508_module, (uint16_t) wtc_bus_dummy_data[0]);
        break;

    case STATE_SET_ADDR:
        tm1640_state = STATE_CONTROL;
                       
        tm1640_module.dir = SPI_DIRECTION_WRITE;
        tm1640_module.hw->SPI.INTFLAG.reg = 0x1F;
        tm1640_module.hw->SPI.INTENSET.reg |= SPI_INTERRUPT_FLAG_TX_COMPLETE;
        
        spi_write(&tm1640_module, (uint16_t) tm1640_display_pkt.brigtness);
        spi_write(&wtc6508_module, 0);
        break;

    case STATE_CONTROL:
    default:
        tm1640_state = STATE_IDLE;        
        xSemaphoreGiveFromISR(tm1640_sem, &wakeTask);
        break;
    }

    portYIELD_FROM_ISR(wakeTask);
}

static void spi_cb_error(struct spi_module *const module) 
{
    printf("Spi error!\n");
    while(1) {}
}

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
    //delay_us(1);
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
    //delay_us(1);

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

    // Take the display bus mutex
    /*if (!xSemaphoreTake(display_mutex, portMAX_DELAY)) {
        // Timeout waiting for semaphore. Just return
        return STATUS_ERR_TIMEOUT;
    }*/
    
    // Timing is critical here, so suspend all other tasks
    //vTaskSuspendAll();
    
    tm1640_start();
    tm1640_write((cmd & CTRL_CMD_MASK) | (data & ~CTRL_CMD_MASK));
    tm1640_stop();

    //xTaskResumeAll();

    // Give the display mutex back
    //xSemaphoreGive(display_mutex);

    taskEXIT_CRITICAL();

    return STATUS_OK;
}

void tm1640_init(void)
{
    /*struct spi_config config;

    config.character_size = SPI_CHARACTER_SIZE_8BIT;
    config.data_order = SPI_DATA_ORDER_LSB;
    config.mode = SPI_MODE_MASTER;
    config.transfer_mode = SPI_TRANSFER_MODE_3;
    config.select_slave_low_detect_enable = false;
#ifndef XPLAINED
    config.mux_setting = SPI_SIGNAL_MUX_SETTING_E;    
#else    
    config.mux_setting = SPI_SIGNAL_MUX_SETTING_E;
#endif
    config.receiver_enable = false;
    config.master_slave_select_enable = false;
    config.run_in_standby = true;
    config.mode_specific.master.baudrate = TM1640_BAUD_RATE;    
    config.generator_source = GCLK_GENERATOR_4;
    config.pinmux_pad0 = TM1640_PINMUX_PAD0;
    config.pinmux_pad1 = TM1640_PINMUX_PAD1;
    config.pinmux_pad2 = TM1640_PINMUX_PAD2;
    config.pinmux_pad3 = TM1640_PINMUX_PAD3;

    if (spi_init(&tm1640_module, TM1640_SERCOM, &config)) {
        printf("Failed to initialize TM1640_SERCOM!\n");
        return;
    }

    spi_register_callback(&tm1640_module, spi_cb_buffer_transmitted, SPI_CALLBACK_BUFFER_TRANSMITTED);      
    spi_enable_callback(&tm1640_module, SPI_CALLBACK_BUFFER_TRANSMITTED);  
    spi_register_callback(&tm1640_module, spi_cb_error, SPI_CALLBACK_ERROR);    
    spi_enable_callback(&tm1640_module, SPI_CALLBACK_ERROR);

    NVIC_EnableIRQ(TM1640_IRQ);
    spi_enable(&tm1640_module);

    tm1640_sem = xSemaphoreCreateBinary(); */

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

enum status_code tm1640_set_display(tm1640_display_t *disp, tm1640_brightness_t brightness)
{
    enum status_code status;
    struct port_config di_conf;    
    struct system_pinmux_config pin_conf;

    // Transactions have to be broken up into 3 segments because the data and clock need to go high
    // for a period of time between the data command, the address/display data, and the display control.
    // Start off with a single byte transfer and then keep the transaction moving with the callback.
    if (tm1640_get_state() == STATE_IDLE) {
        tm1640_state = STATE_SET_DATA;

        memcpy(&tm1640_display_pkt.disp_data, disp->grid, TM1640_GRIDS);
        tm1640_display_pkt.set_addr = ADDR_CMD_ADDR0;
        tm1640_display_pkt.set_data = DATA_CMD_ADDR_INC;
        tm1640_display_pkt.brigtness = brightness;
       
        // Take the display bus mutex
        if (!xSemaphoreTake(display_mutex, portMAX_DELAY)) {
            // Timeout waiting for semaphore. Just return
            return STATUS_ERR_TIMEOUT;
        }

        transfer_complete = 0;

        system_pinmux_get_config_defaults(&pin_conf);
        pin_conf.direction = SYSTEM_PINMUX_PIN_DIR_INPUT;
        pin_conf.mux_position = PINMUX_PA08C_SERCOM0_PAD0 & 0xFFFF;

        // Data input pin for the WTC6508 needs to be set to a GPIO so it isn't driven
        // when the clock is generated for the TM1640
        di_conf.direction = PORT_PIN_DIR_INPUT;
        di_conf.input_pull = PORT_PIN_PULL_UP;
        di_conf.powersave = false;
        port_pin_set_config(WTC6508_DI_GPIO, &di_conf);
        
        SercomSpi *const hw = &tm1640_module.hw->SPI;
        hw->INTFLAG.reg = 0x1F;
        tm1640_module.dir = SPI_DIRECTION_WRITE;
        hw->INTENSET.reg |= SPI_INTERRUPT_FLAG_TX_COMPLETE;                

        status = spi_write(&tm1640_module, (uint16_t) tm1640_display_pkt.set_data);
        spi_write(&wtc6508_module, 0);

        if (!status) {
            xSemaphoreTake(tm1640_sem, portMAX_DELAY);
        }
 
        // Ensure the WTC6508 bus has finished transmitting before giving it up
        while (!(wtc6508_module.hw->SPI.INTFLAG.reg & SPI_INTERRUPT_FLAG_TX_COMPLETE)) {}

        // Restore pin function to the WTC6508 SPI
        system_pinmux_pin_set_config(PINMUX_PA08C_SERCOM0_PAD0 >> 16, &pin_conf);                        

        xSemaphoreGive(display_mutex);     
        return status;
    }
    else
        return STATUS_BUSY;
}

