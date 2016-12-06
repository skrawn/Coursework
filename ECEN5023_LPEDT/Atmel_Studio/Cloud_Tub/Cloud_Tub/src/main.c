/**
 * \file
 *
 * \brief PubNub Example.
 *
 * Copyright (c) 2016 Atmel Corporation. All rights reserved.
 *
 * \asf_license_start
 *
 * \page License
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * 3. The name of Atmel may not be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
 * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \asf_license_stop
 *
 */

/** \mainpage
 * \section intro Introduction
 * This example demonstrates the use of the SAMW25 Xplained Pro with I/O1 Xplained Pro
 * board to communicate with pubnub. <br>
 * It uses the following hardware:
 * - SAMW25 Xplained Pro.
 * - I/O1 Xplained Pro on EXT1.
 *
 * \section files Main Files
 * - main.c : Initialize the SAMW25 and communicate with PubNub cloud.
 *
 * \section usage Usage
 * -# To connect to the internet Access Point using hardcoded credentials
 * -# MAIN_ENABLE_WEB_PROVISIONING macro must be commented in main.h, then
 * -# set the following macro:
 * \code
 *    #define MAIN_WLAN_SSID     "SSID"
 *    #define MAIN_WLAN_AUTH     M2M_WIFI_SEC_WPA_PSK
 *    #define MAIN_WLAN_PSK      "PASSWORD"
 * \endcode
 *
 * -# Alternatively keep MAIN_ENABLE_WEB_PROVISIONING macro to enable the
 * -# Wi-Fi web provisioning and follow instruction displayed on UART.
 *
 * -# Build the program and download it into the board.
 * -# On the computer, open and configure a terminal application as the follows.
 * \code
 *    Baud Rate : 115200
 *    Data : 8bit
 *    Parity bit : none
 *    Stop bit : 1bit
 *    Flow control : none
 * \endcode
 * -# Start the application.
 * -# In the terminal window, the following text should appear:
 * \code
 *    -- SAMW25 PubNub example --
 *    -- SAMW25_XPLAINED_PRO --
 *    -- Compiled: xxx xx xxxx xx:xx:xx --
 * \endcode
 *
 * -# Once the Wi-Fi is provisioned with internet access connect to PubNub
 * -# website http://www.pubnub.com and login with your account (free).
 * -# Start the application. Go to your Admin Portal and click on Debug Console.
 * -# From here:
 * -#  Enter Channel, Publish and Subscribe key as specified on the UART.
 * -#  Ensure SSL is enabled.
 * -#  Connect to the cloud by pressing the subscribe button.
 *
 * -# After reporting the first data to PubNub server, it is then possible to connect
 * -# to https://freeboard.io to configure a dashboard and to show reported data.
 *
 * \section compinfo Compilation Information
 * This software was written for the GNU GCC compiler using Atmel Studio 6.2
 * Other compilers may or may not work.
 *
 * \section contactinfo Contact Information
 * For further information, visit
 * <A href="http://www.atmel.com">Atmel</A>.\n
 */


#include "main.h"
#include "PubNub.h"

#include "display.h"
#include "thermal.h"
#include "wifi.h"

#define STRING_EOL    "\r\n"
#define STRING_HEADER "-- SAMW25 PubNub example --"STRING_EOL	\
	"-- "BOARD_NAME " --"STRING_EOL	\
	"-- Compiled: "__DATE__ " "__TIME__ " --"STRING_EOL



/*#define TASK_3S_PRIORITY        (tskIDLE_PRIORITY + 1)
#define TASK_1S_PRIORITY        (tskIDLE_PRIORITY + 1)
#define TASK_100HZ_PRIORITY     (tskIDLE_PRIORITY + 2)*/

#define TASK_3S_PRIORITY        (tskIDLE_PRIORITY + 1)
#define TASK_1S_PRIORITY        (tskIDLE_PRIORITY + 1)
#define TASK_50HZ_PRIORITY     (tskIDLE_PRIORITY + 1)

/** UART module for debug. */
static struct usart_module cdc_uart_module;

/**
 * \brief Configure UART console.
 */
static void configure_console(void)
{
	struct usart_config usart_conf;

	usart_get_config_defaults(&usart_conf);
	usart_conf.mux_setting = EDBG_CDC_SERCOM_MUX_SETTING;
	usart_conf.pinmux_pad0 = EDBG_CDC_SERCOM_PINMUX_PAD0;
	usart_conf.pinmux_pad1 = EDBG_CDC_SERCOM_PINMUX_PAD1;
	usart_conf.pinmux_pad2 = EDBG_CDC_SERCOM_PINMUX_PAD2;
	usart_conf.pinmux_pad3 = EDBG_CDC_SERCOM_PINMUX_PAD3;
	usart_conf.baudrate    = 115200;

	stdio_serial_init(&cdc_uart_module, EDBG_CDC_MODULE, &usart_conf);
	usart_enable(&cdc_uart_module);
}

static void task_3s(void *args)
{    
    TickType_t lastTimer;
    TickType_t delay_time = pdMS_TO_TICKS(3000);

    lastTimer = xTaskGetTickCount();
    for ( ;; )
    {
        vTaskDelayUntil(&lastTimer, delay_time);        
                    
        thermal_3s();
        wifi_task_3s();
    }
}

static void task_1s(void *args)
{
    TickType_t lastTimer;

    lastTimer = xTaskGetTickCount();
    TickType_t delay_time = pdMS_TO_TICKS(500);

    while (1) {
        vTaskDelayUntil(&lastTimer, delay_time);

        wifi_task_1s();    
               
        display_update_1Hz();
    }

}

#include "tm1640.h"
static void task_33Hz(void *args)
{
    TickType_t lastTimer;

    // Turn on the display
    tm1640_display_on(1);

    lastTimer = xTaskGetTickCount();
    TickType_t delay_time = pdMS_TO_TICKS(MAIN_33HZ_TASK_INTERVAL);        

    while(1) {
        vTaskDelayUntil(&lastTimer, delay_time);

        display_update_33Hz();

    }
}

static void task_Buzzer(void *args)
{
    struct tc_module buzz_module;
    struct tc_config buzz_config;    

    tc_get_config_defaults(&buzz_config);
    buzz_config.clock_source = GCLK_GENERATOR_5; // ~500kHz
    buzz_config.run_in_standby = true;
    buzz_config.counter_size = TC_COUNTER_SIZE_8BIT;
    buzz_config.pwm_channel[TC_COMPARE_CAPTURE_CHANNEL_1].enabled = true;
    buzz_config.pwm_channel[TC_COMPARE_CAPTURE_CHANNEL_1].pin_out = (PINMUX_PB11E_TC5_WO1) >> 16;
    buzz_config.pwm_channel[TC_COMPARE_CAPTURE_CHANNEL_1].pin_mux = PINMUX_PB11E_TC5_WO1;
    buzz_config.counter_8_bit.period = (500000 / BUZZER_FREQUENCY) / 2;
    buzz_config.counter_8_bit.value = 0;

    tc_init(&buzz_module, TC5, &buzz_config);    

    while(1) {
        xSemaphoreTake(buzzer_sem, portMAX_DELAY);

        // When another task gives up the semaphore, the buzzer will run 
        // for the set delay time.
        tc_set_count_value(&buzz_module, 0);
        tc_enable(&buzz_module);        
        vTaskDelay(pdMS_TO_TICKS(BUZZER_ON_TIME));
        tc_disable(&buzz_module);
    }
}

void vApplicationIdleHook(void);
void vApplicationIdleHook(void)
{         
    wifi_task_idle();
    display_idle();
}

void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName );
void vApplicationStackOverflowHook( TaskHandle_t xTask, char *pcTaskName )
{
    printf("Stack overflow!: %s\n\r", pcTaskName);
    while (1) {}
}

void vApplicationMallocFailedHook(void);
void vApplicationMallocFailedHook(void)
{
    printf("Malloc failed!\n\r");
    while(1) {}
}


/**
 * \brief Main application function.
 *
 * Initialize board and WINC1500 Wi-Fi module.
 * Publish and subscribe event to pubnub.
 *
 * \return program return value.
 */
int main(void)
{
	/* Initialize the board. */
	system_init();

	/* Initialize the UART console. */
	configure_console();

	/* Initialize the delay driver. */
	delay_init();	

    /* Initialize the display */
    display_init();	

    wifi_init();	    

    thermal_init();

    xTaskCreate(task_3s, "task_3s", configMINIMAL_STACK_SIZE, 0, TASK_3S_PRIORITY, NULL);
    xTaskCreate(task_1s, "task_1s", configMINIMAL_STACK_SIZE, 0, TASK_1S_PRIORITY, NULL);    
    xTaskCreate(task_33Hz, "task_33Hz", configMINIMAL_STACK_SIZE, 0, TASK_50HZ_PRIORITY, NULL); 
    xTaskCreate(task_Buzzer, "task_Buzzer", 100, 0, tskIDLE_PRIORITY, NULL);
    display_mutex = xSemaphoreCreateMutex();
    buzzer_sem = xSemaphoreCreateBinary();

    vTaskStartScheduler();

    while(1) {}

	return 0;
}

void HardFault_Handler(void)
{
    __asm volatile
    (
        "mrs r0,psp         \n"
        "ldr r1,[r0,#24]    \n"    // r1 will contain the address where the hard fault occurred
    );    
}


