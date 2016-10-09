

// This code is only valid for the FRDM board
#ifdef FRDM

#include "circ_buf.h"
#include "MKL25Z4.h"
#include "uart.h"

static inline void uart_enable_tx_interrupt(void);
static inline void uart_disable_tx_interrupt(void);
static inline void uart_enable_rx_interrupt(void);

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

void UART0_DriverIRQHandler(void)
{
	// Get and clear the current interrupts
	uint8_t s1_flags, s2_flags, data;
	s1_flags = UART0->S1;
	UART0->S1 = s1_flags;

	s2_flags = UART0->S2;
	UART0->S2 = s2_flags;

	// Check for a receive interrupt
	if (s1_flags & UART0_S1_RDRF_MASK) {
		data = UART0->D;
		cb_push(&rx_cb, data);
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

