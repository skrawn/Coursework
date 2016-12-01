/*
 * wtc6508.c
 *
 * Created: 11/19/2016 3:12:39 PM
 *  Author: Sean
 */ 

#include <stdio.h>

#include "delay.h"
#include "main.h"
#include "pinmux.h"
#include "wtc6508.h"

#define WTC65808_SERCOM         SERCOM0
#define WTC6508_IRQ             SERCOM0_IRQn

#define WTC6508_BAUD            15000  // Hz

#define DISPLAY_MUTEX_TIMEOUT   pdMS_TO_TICKS(5)

// Must run at 2kHz to 20kHz with at least 15 ms between transactions

void wtc6508_init(void)
{
    struct spi_config config;
    spi_get_config_defaults(&config);

    config.pinmux_pad0 = WTC6508_PINMUX_PAD0;
    config.pinmux_pad1 = WTC6508_PINMUX_PAD1;
    config.pinmux_pad2 = WTC6508_PINMUX_PAD2;
    config.pinmux_pad3 = WTC6508_PINMUX_PAD3;
    config.mux_setting = SPI_SIGNAL_MUX_SETTING_I;
    config.select_slave_low_detect_enable = false;
    config.mode_specific.master.baudrate = WTC6508_BAUD;
    config.transfer_mode = SPI_TRANSFER_MODE_3;    
    config.generator_source = GCLK_GENERATOR_4;
    config.run_in_standby = false;

    if (spi_init(&wtc6508_module, WTC65808_SERCOM, &config)) {
        printf("failed to initialize WTC6508!\n");
        return;
    }

    spi_enable(&wtc6508_module);
}

enum status_code wtc6508_read(uint8_t *status)
{
    enum status_code ret;
    struct port_config di_conf;    
    struct port_config clk_conf;
    struct system_pinmux_config pin_conf;

    clk_conf.direction = PORT_PIN_DIR_OUTPUT;
    clk_conf.input_pull = PORT_PIN_PULL_UP;
    clk_conf.powersave = false;   

    di_conf.direction = PORT_PIN_DIR_INPUT;
    di_conf.input_pull = PORT_PIN_PULL_UP;
    di_conf.powersave = false;

    system_pinmux_get_config_defaults(&pin_conf);
    pin_conf.direction = SYSTEM_PINMUX_PIN_DIR_INPUT;

    // Take the display bus mutex
    if (!xSemaphoreTake(display_mutex, portMAX_DELAY)) {
        // Timeout waiting for semaphore. Just return
        return STATUS_ERR_TIMEOUT;
    }

    taskENTER_CRITICAL();

    // Start and stop bits need to be manually sent. Switch the data and clock
    // lines to GPIOs. Timing is critical here, so suspend all other tasks
    //vTaskSuspendAll();

    port_pin_set_config(WTC6508_CLK_GPIO, &clk_conf);
    port_pin_set_config(WTC6508_DI_GPIO, &di_conf);

    // If the DI pin is already low, then the bus is still busy from the last TM1640 transaction.
    // Skip the read.
    if (!port_pin_get_input_level(WTC6508_DI_GPIO)) {
        xSemaphoreGive(display_mutex);
        taskEXIT_CRITICAL();
        return STATUS_BUSY;
    }

    // Need a 10us - 22us clock pulse. Delay low for 6us
    port_pin_set_output_level(WTC6508_CLK_GPIO, 0);        
    delay_us(3);
    port_pin_set_output_level(WTC6508_CLK_GPIO, 1);        

    // Give pin control back to SPI module again 
    pin_conf.mux_position = WTC6508_PINMUX_PAD0 & 0xFFFF;
    system_pinmux_pin_set_config(WTC6508_PINMUX_PAD0 >> 16, &pin_conf);
    pin_conf.mux_position = WTC6508_PINMUX_PAD1 & 0xFFFF;
    system_pinmux_pin_set_config(WTC6508_PINMUX_PAD1 >> 16, &pin_conf);

    ret = spi_read_buffer_wait(&wtc6508_module, status, 1, 0);

    port_pin_set_config(WTC6508_CLK_GPIO, &clk_conf);
    port_pin_set_config(WTC6508_DI_GPIO, &di_conf);

    // Need a 10us - 22us clock pulse. Delay low for 6us
    port_pin_set_output_level(WTC6508_CLK_GPIO, 0);    
    delay_us(3);
    port_pin_set_output_level(WTC6508_CLK_GPIO, 1);    

    // Give pin control back to SPI module again
    pin_conf.mux_position = WTC6508_PINMUX_PAD0 & 0xFFFF;
    system_pinmux_pin_set_config(WTC6508_PINMUX_PAD0 >> 16, &pin_conf);
    pin_conf.mux_position = WTC6508_PINMUX_PAD1 & 0xFFFF;
    system_pinmux_pin_set_config(WTC6508_PINMUX_PAD1 >> 16, &pin_conf);        

    //xTaskResumeAll();

    // Give the display mutex back
    xSemaphoreGive(display_mutex);
    taskEXIT_CRITICAL();

    return ret;
}


