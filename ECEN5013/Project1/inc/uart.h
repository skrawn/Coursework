
#ifndef _UART_H_
#define _UART_H_

typedef enum {
	UART_RX_BUF_EMPTY = -2,
	UART_TX_BUF_FULL = -1,
	UART_OK = 0
} uart_status_t;

void uart_init(void);
uart_status_t uart_put_tx_buf(uint8_t *buf, uint16_t tx_size);
uint16_t uart_pop_rx_buf(uint8_t *buf);
uint8_t uart_tx_buffer_full(void);
uint8_t uart_rx_buffer_not_empty(void);

#endif

