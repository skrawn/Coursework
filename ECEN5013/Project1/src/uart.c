/**
 * @file uart.c
 * @author Sean Donohue
 * @date 12 Sep 2016
 * @brief Functions to control UART0 on the KL25Z
 */

#ifdef FRDM

#include <stdlib.h>
#include <string.h>
#include "circ_buf.h"
#include "MKL25Z4.h"
#include "rgb_led.h"
#include "uart.h"

static inline void uart_enable_tx_interrupt(void);
static inline void uart_disable_tx_interrupt(void);
static inline void uart_enable_rx_interrupt(void);
static char *my_strstr(char *buf, int character);

const uint8_t INVALID_INPUT[] = "Error: Invalid input: ";

cb_t tx_cb;
uint8_t tx_buf[TX_BUFFER_LENGTH] = {0};
cb_t rx_cb;
uint8_t rx_buf[RX_BUFFER_LENGTH] = {0};

volatile uint8_t txOnGoing = 0;

void uart_init(void)
{
	// UART0 uses PTA1 and PTA2. Ungate the PORTA clock
	SIM->SCGC5 |= SIM_SCGC5_PORTA(1);

	// Select alt mode 2 for PTA1 and PTA2 to select UART0
	PORTA->PCR[1] = (PORTA->PCR[1] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(2);
	PORTA->PCR[2] = (PORTA->PCR[2] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(2);

    // Set UART0 clock source to MCGPLLCLK/2
    SIM->SOPT2 |= SIM_SOPT2_UART0SRC(1);
    // Ungate the UART0 clock
    SIM->SCGC4 |= SIM_SCGC4_UART0(1);

    // Calculate baud rate. Clock source to the UART is running at 48Hz.
    // Baudrate = 48M/((OSR+1)*BR). Using OSR = 0x1F and BR = 13 gives a baudrate
    // of 48M/((31+1)*13) = 115384
    UART0->BDL = 13;
    UART0->BDH = 0x00 |
    		UART0_BDH_SBNS(0); // One stop bit.
    UART0->C4 = UART0_C4_OSR(0x1F);

	// Initialize the TX and RX circular buffers
	tx_cb.buf = tx_buf;
	tx_cb.head = tx_cb.tail = tx_buf;
	tx_cb.length = TX_BUFFER_LENGTH;
	tx_cb.num_items = 0;

	rx_cb.buf = rx_buf;
	rx_cb.head = rx_cb.tail = rx_buf;
	rx_cb.length = RX_BUFFER_LENGTH;
	rx_cb.num_items = 0;

	// Enable the transmitter and receiver
	UART0->C2 |= UART0_C2_TE_MASK | UART0_C2_RE_MASK;

	// Enable the receiver interrupt
	uart_enable_rx_interrupt();

	NVIC_EnableIRQ(UART0_IRQn);

	// Clear status flags
	UART0->S1 |= 0x1F;
}

uart_status_t uart_put_tx_buf(uint8_t *buf, uint16_t tx_size)
{
	int8_t return_val = 0;
#ifdef VERBOSE
	uint8_t tx_byte;

	// Disable IRQs when loading the transmit buffer
	NVIC_EnableIRQ(UART0_IRQn);
	while (tx_size > 0) {
		if (cb_full(&tx_cb) == cb_status_ok) {
			cb_push(&tx_cb, *(buf++));
			tx_size--;
		}
		else {
			tx_size = 0;
			return_val = UART_TX_BUF_FULL;
		}
	}

	// Check if a transfer is currently in progress. If not, start one
	if (!txOnGoing) {
		cb_pop(&tx_cb, &tx_byte);
		UART0->D = tx_byte;
		// Enable transmit interrupt.
		uart_enable_tx_interrupt();
		txOnGoing = 1;
	}
	NVIC_EnableIRQ(UART0_IRQn);
#endif

	return return_val;
}

uint16_t uart_pop_rx_buf(uint8_t *buf)
{
	uint16_t rx_size = 0;

	// Disable IRQs when removing from the receive buffer
	NVIC_EnableIRQ(UART0_IRQn);
	while (cb_empty(&rx_cb) != cb_status_empty) {
		cb_pop(&rx_cb, (buf++));
		rx_size++;
	}
	NVIC_EnableIRQ(UART0_IRQn);

	return rx_size;
}

uint8_t uart_tx_buf_full(void)
{
	cb_status_t buf_status;
	// Disable IRQs when checking the state of the buffers
	NVIC_EnableIRQ(UART0_IRQn);
	buf_status = cb_full(&tx_cb);
	NVIC_EnableIRQ(UART0_IRQn);
	return (buf_status == cb_status_full ? 1 : 0);
}

uint8_t uart_tx_buf_empty(void)
{
	cb_status_t buf_status;
	// Disable IRQs when checking the state of the buffers
	NVIC_EnableIRQ(UART0_IRQn);
	buf_status = cb_empty(&tx_cb);
	NVIC_EnableIRQ(UART0_IRQn);
	return (buf_status == cb_status_empty ? 1 : 0);
}

uint8_t uart_rx_buf_not_empty(void)
{
	cb_status_t buf_status;
	// Disable IRQs when checking the state of the buffers
	NVIC_EnableIRQ(UART0_IRQn);
	buf_status = cb_empty(&rx_cb);
	NVIC_EnableIRQ(UART0_IRQn);
	return (buf_status == cb_status_empty ? 0 : 1);
}

void uart_wait_tx_buf_empty(void)
{
	uint32_t i;
	while (!uart_tx_buf_empty()) {
		for (i = 0; i < 10000; i++) {}
	}
}

static inline void uart_enable_tx_interrupt(void)
{
	UART0->C2 |= UART0_C2_TIE(1);
}

static inline void uart_disable_tx_interrupt(void)
{
	UART0->C2 = (UART0->C2 & ~UART0_C2_TIE_MASK);
}

static inline void uart_enable_rx_interrupt(void)
{
	UART0->C2 |= UART0_C2_RIE(1);
}

// Library version doesn't work?
static char *my_strstr(char *buf, int character)
{
	while (*buf != '\0' && *buf != (char) character) {
		buf++;
	}
	if (*buf == (char) character)
		return buf;
	else
		return NULL;
}

void UART0_DriverIRQHandler(void)
{
	// Get and clear the current interrupts
	uint8_t s1_flags, s2_flags, data, i, red, green, blue, input_valid = 1;
	uint8_t cmd_buf[RX_BUFFER_LENGTH + 1] = {0};
	char *str_ptr, *cmd_ptr, brightness[4] = {0};
	int space_char = 0x20;
	s1_flags = UART0->S1;
	UART0->S1 = s1_flags;

	s2_flags = UART0->S2;
	UART0->S2 = s2_flags;

	// Check for a receive interrupt
	if (s1_flags & UART0_S1_RDRF_MASK) {
		data = UART0->D;
		// Find a return character, indicating the end of a command
		if (data == '\r') {
			// Dequeue the receive circular buffer into a local buffer
			for (;;) {
				i = 0;
				while (cb_pop(&rx_cb, &cmd_buf[i]) != cb_status_empty && i < RX_BUFFER_LENGTH) {
					i++;
				}

				// Find integers with spaces in between them
				cmd_ptr = (char *) cmd_buf;
				str_ptr = my_strstr((char *) cmd_ptr, space_char);
				if (str_ptr == NULL) {
					input_valid = 0;
					break;
				}

				// If the offset from the space to the beginning of the buffer is
				// greater than 3, it's invalid input
				if ((str_ptr - cmd_ptr) > 3) {
					input_valid = 0;
					break;
				}

				// Copy the characters from the beginning of the buffer to the space
				// to get the red value
				memcpy(brightness, cmd_ptr, str_ptr - cmd_ptr);

				// Using the standard library version because it will be faster
				red = (uint8_t) atoi(brightness);

				cmd_ptr = str_ptr + 1;
				// Look for the green value
				str_ptr = my_strstr((char *) cmd_ptr, ' ');
				if (str_ptr == NULL) {
					input_valid = 0;
					break;
				}

				// If the offset from the space to the beginning of the buffer is
				// greater than 3, it's invalid input
				if ((str_ptr - cmd_ptr) > 3) {
					input_valid = 0;
					break;
				}

				// Copy the characters from the beginning of the buffer to the space
				// to get the red value
				memset(brightness, 0, sizeof(brightness));
				memcpy(brightness, cmd_ptr, str_ptr - cmd_ptr);

				// Using the standard library version because it will be faster
				green = (uint8_t) atoi(brightness);

				cmd_ptr = str_ptr + 1;
				// For the end of the buffer
				str_ptr = my_strstr((char *) cmd_ptr, '\0');
				if (str_ptr == NULL) {
					input_valid = 0;
					break;
				}

				// If the offset from the space to the beginning of the buffer is
				// greater than 3, it's invalid input
				if ((str_ptr - cmd_ptr) > 3) {
					input_valid = 0;
					break;
				}

				// Copy the characters from the beginning of the buffer to the space
				// to get the red value
				memset(brightness, 0, sizeof(brightness));
				memcpy(brightness, cmd_ptr, str_ptr - cmd_ptr);

				// Using the standard library version because it will be faster
				blue = (uint8_t) atoi(brightness);

				rgb_led_set_brightness(red, green, blue);
				break;

			}
			if (!input_valid) {
				cmd_buf[i + 1] = '\n';
				uart_put_tx_buf((uint8_t *) INVALID_INPUT, sizeof(INVALID_INPUT));
				uart_put_tx_buf(cmd_buf, i + 1);
			}
		}
		else {
			// Otherwise, just put it in the RX buffer.
			cb_push(&rx_cb, data);
		}
	}

	// Check for a transmit data register empty interrupt
	if (s1_flags & UART0_S1_TDRE_MASK) {
		// Keep transmitting if the buffer is full
		if (cb_empty(&tx_cb) != cb_status_empty) {
			cb_pop(&tx_cb, &data);
			UART0->D = data;
		}
		else {
			txOnGoing = 0;
			uart_disable_tx_interrupt();
		}
	}

}

#endif

