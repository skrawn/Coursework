/**
 * @file spi.c
 * @author Sean Donohue
 * @date 23 Oct 2016
 * @brief SPI functions for the KL25Z
 */

#ifndef INC_SPI_H_
#define INC_SPI_H_

#include <stddef.h>
#include <stdint.h>

typedef enum {
	SPI_STATUS_OK = 0,
	SPI_STATUS_NO_SUPPORT = -1,
	SPI_STATUS_TIMEOUT = -2,
} spi_status_t;


void spi_init(void);


spi_status_t spi_transfer_buf(uint8_t *tx_buf, size_t tx_len, uint8_t *rx_buf);


spi_status_t spi_send_buf(uint8_t *tx_buf, size_t tx_len);


spi_status_t spi_transfer_byte(uint8_t tx_byte, uint8_t *rx_byte);


spi_status_t spi_send_byte(uint8_t tx_byte);


#endif /* INC_SPI_H_ */
