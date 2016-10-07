
#ifndef _UART_H_
#define _UART_H_

typedef enum {
	UART_RX_BUF_EMPTY = -2,
	UART_TX_BUF_FULL = -1,
	UART_OK = 0
} uart_status_t;

void uart_init(void);

#endif

