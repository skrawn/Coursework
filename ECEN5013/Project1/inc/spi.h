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
	SPI_STATUS_ERROR = 3,
} spi_status_t;

/**
 * @brief Initializes the SPI interface
 */
void spi_init(void);

/**
 * @brief Performs an SPI transfer.
 * @param tx_buf Pointer to the transmit buffer.
 * @param tx_len Length of data to transfer
 * @param rx_buf Pointer to the receive buffer.
 * @return Returns SPI_STATUS_OK if there were no errors.
 */
spi_status_t spi_transfer_buf(uint8_t *tx_buf, size_t tx_len, uint8_t *rx_buf);

/**
 * @brief Performs an SPI transmit. Received data is discarded.
 * @param tx_buf Pointer to the transmit buffer.
 * @param tx_len Length of data to transfer
 * @return Returns SPI_STATUS_OK if there were no errors.
 */
spi_status_t spi_send_buf(uint8_t *tx_buf, size_t tx_len);

/**
 * @brief Transfer a single byte over SPI
 * @param tx_byte Byte to transmit
 * @param rx_byte Pointer to receive the received byte
 * @return Returns SPI_STATUS_OK if there were no errors.
 */
spi_status_t spi_transfer_byte(uint8_t tx_byte, uint8_t *rx_byte);

/**
 * @brief Transfer a single byte over SPI and discards the received byte.
 * @param tx_byte Byte to transmit
 * @return Returns SPI_STATUS_OK if there were no errors.
 */
spi_status_t spi_send_byte(uint8_t tx_byte);


#endif /* INC_SPI_H_ */
