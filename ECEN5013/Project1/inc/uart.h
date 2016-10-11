
#ifndef _UART_H_
#define _UART_H_

#define TX_BUFFER_LENGTH	1024
#define RX_BUFFER_LENGTH	16

typedef enum {
	UART_RX_BUF_EMPTY = -2,
	UART_TX_BUF_FULL = -1,
	UART_OK = 0
} uart_status_t;

void uart_init(void);
uart_status_t uart_put_tx_buf(uint8_t *buf, uint16_t tx_size);
uint16_t uart_pop_rx_buf(uint8_t *buf);
uint8_t uart_tx_buf_full(void);
uint8_t uart_tx_buf_empty(void);
uint8_t uart_rx_buf_not_empty(void);

#ifdef FRDM
void uart_wait_tx_buf_empty(void);
#else
static inline void uart_wait_tx_buf_empty(void) {}
#endif

#endif

