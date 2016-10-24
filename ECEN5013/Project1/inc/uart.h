/**
 * @file uart.h
 * @author Sean Donohue
 * @date 12 Sep 2016
 * @brief Functions to control UART0 on the KL25Z
 */

#ifndef _UART_H_
#define _UART_H_

#define TX_BUFFER_LENGTH	1024
#define RX_BUFFER_LENGTH	32

#define CMD_BYTE_LEN		1
#define LENGTH_LEN			1
#define CHECKSUM_LEN		2

typedef enum {
	UART_RX_BUF_EMPTY = -2,
	UART_TX_BUF_FULL = -1,
	UART_OK = 0
} uart_status_t;

typedef enum Cmds_t{
	CMD_NONE = 0,
	GET_TEMP,
	SET_SPEED,
	LED_CONFIG
} Cmds;

typedef struct CI_Msg_t{
	Cmds command;
	uint8_t length;
	// The length here accounts for the total size of the RX buffer
	// minus the command, length and checksum field
	uint8_t data[RX_BUFFER_LENGTH - CMD_BYTE_LEN - LENGTH_LEN -
				 CHECKSUM_LEN];
	uint16_t checksum;
} CI_Msg;

/**
 * @brief Initializes UART0 at 115200 baud
 * @param cb Pointer to the circular buffer.
 * @param element Pointer to the byte to store the returned data.
 * @return Returns cb_status_ok if the item was returned and 
 * buffer is not empty.
 */ 
void uart_init(void);

/**
 * @brief Puts an array of data in the transmit buffer.
 * @param buf Pointer to the data.
 * @param tx_size Number of bytes to buffer.
 * @return Returns UART_OK if the buffer is not full.
 */ 
uart_status_t uart_put_tx_buf(uint8_t *buf, uint16_t tx_size);

/**
 * @brief Removes the contents of the receive circular buffer and puts
 * them into buf.
 * @param buf Pointer to the buffer.
 * @return Returns the number of items in the receive buffer.
 */ 
uint16_t uart_pop_rx_buf(uint8_t *buf);

/**
 * @brief Checks if the transmit buffer is full.
 * @return Returns 1 if the transmit buffer is full.
 */ 
uint8_t uart_tx_buf_full(void);

/**
 * @brief Checks if the transmit buffer is empty
 * @return Returns 1 if the transmit buffer is full.
 */ 
uint8_t uart_tx_buf_empty(void);

/**
 * @brief Checks if the receive buffer is not empty.
 * @return Returns 1 if the receive buffer is not empty.
 */ 
uint8_t uart_rx_buf_not_empty(void);

#ifdef FRDM
/**
 * @brief Blocks and waits for the transmit buffer to empty.
 */ 
void uart_wait_tx_buf_empty(void);
#else
static inline void uart_wait_tx_buf_empty(void) { return; }
#endif

#endif

