/**
 * @file nRF24L01.c
 * @author Sean Donohue
 * @date 27 Oct 2016
 * @brief Driver for nRF24L01 WiFi
 */

#include <stdlib.h>
#include <string.h>

#include "nRF24L01.h"
#include "spi.h"

#define READ_REG_MASK			0x00
#define WRITE_REG_MASK			0x20

#define CMD_R_RX_PAYLOAD		0x61
#define CMD_W_TX_PAYLOAD		0xA0
#define CMD_FLUSH_TX			0xE1
#define CMD_FLUSH_RX			0xE2
#define CMD_REUSE_TX_PL			0xE3
#define CMD_R_RX_PL_WID			0x60
#define CMD_W_ACK_PAYLOAD		0xA8
#define CMD_W_TX_PAYLOAD_NOACK	0xB0
#define CMD_NOP					0xFF


nrf_t *nrf_init(nrf_config_t *config, nrf_pwr_t pwr, uint8_t *tx_addr, uint8_t tx_addr_len)
{
	nrf_t *nrf;
	uint8_t tx_buf[10] = {0};

	if (config == NULL || tx_addr == NULL || tx_addr_len > NRF24L01_MAX_ADDR_LEN ||
			tx_addr_len < NRF24L01_MIN_ADDR_LEN)
		return (nrf_t *) NRF_STATUS_INV_CONFIG;

	nrf = malloc(sizeof(nrf_t));
	if (nrf == NULL)
		return (nrf_t *) NRF_STATUS_INV_CONFIG;

	memset(nrf, 0, sizeof(nrf_t));

	// Write the config registers
	tx_buf[0] = NRF24L01_REG_CONFIG | WRITE_REG_MASK;
	tx_buf[1] = config->reg;
	if (spi_send_buf(tx_buf, 2))
		return NULL;
	nrf->config.reg = config->reg;

	// Set the transmit power
	tx_buf[0] = NRF24L01_REG_RF_SETUP | WRITE_REG_MASK;
	tx_buf[1] = 0x00 |
			(0 << 3) |	// Sets the RF Data Rate to 1Mbps
			(pwr << 1);
	if (spi_send_buf(tx_buf, 2))
		return NULL;
	nrf->output_power = pwr;

	// Set the address width
	tx_buf[0] = NRF24L01_REG_SETUP_AW | WRITE_REG_MASK;
	tx_buf[1] = tx_addr_len - 2;
	if (spi_send_buf(tx_buf, 2))
		return NULL;
	nrf->address_width = tx_addr_len;

	nrf_set_tx_address(nrf, tx_addr, tx_addr_len);

	return nrf;
}

nrf_status_t nrf_set_power(nrf_t *nrf, uint8_t power_on)
{
	return NRF_STATUS_OK;
}

nrf_status_t nrf_write_register(nrf_t *nrf, uint8_t reg, uint8_t *buf, size_t buf_len)
{
	uint8_t tx_buf[10] = {0};

	tx_buf[0] = reg | WRITE_REG_MASK;
	memcpy(tx_buf, buf, buf_len);
	if (spi_send_buf(tx_buf, buf_len + 1))
		return NRF_STATUS_SPI_ERR;

	return NRF_STATUS_OK;
}

nrf_status_t nrf_read_register(nrf_t *nrf, uint8_t reg, uint8_t *buf, size_t buf_len)
{
	uint8_t tx_buf[10] = {0};
	tx_buf[0] = reg | READ_REG_MASK;
	if (spi_transfer_buf(tx_buf, buf_len, buf))
		return NRF_STATUS_SPI_ERR;

	return NRF_STATUS_OK;
}

nrf_status_t nrf_tx_data(nrf_t *nrf, uint8_t *tx_buf, size_t tx_len)
{
	return NRF_STATUS_OK;
}

nrf_status_t nrf_rx_data(nrf_t *nrf, uint8_t *rx_buf, size_t *rx_len, uint8_t index)
{
	return NRF_STATUS_OK;
}

nrf_status_t nrf_flush_tx_fifo(nrf_t *nrf)
{
	return NRF_STATUS_OK;
}

nrf_status_t nrf_set_tx_address(nrf_t *nrf, uint8_t *tx_addr, uint8_t tx_addr_len)
{
	uint8_t tx_buf[6] = {0};
	uint8_t i, j;

	if (tx_addr == NULL || tx_addr_len < NRF24L01_MIN_ADDR_LEN ||
			tx_addr_len > NRF24L01_MAX_ADDR_LEN)
		return NRF_STATUS_INV_ADDR;

	// Set the transmit address. Must be sent LSB first
	tx_buf[0] = NRF24L01_REG_TX_ADDR | WRITE_REG_MASK;
	j = 1;
	for (i = (tx_addr_len - 1); i < 0xFF; i--) {
		tx_buf[j++] = *(tx_addr + i);
	}
	if (spi_send_buf(tx_buf, 1 + tx_addr_len))
		return NRF_STATUS_SPI_ERR;

	memcpy(nrf->tx_addr, tx_addr, tx_addr_len);

	return NRF_STATUS_OK;
}

nrf_status_t nrf_set_rx_address(nrf_t *nrf, uint8_t *rx_addr, uint8_t rx_addr_len, uint8_t index)
{
	uint8_t tx_buf[6] = {0};
	uint8_t i, j;

	if (rx_addr == NULL || rx_addr_len < NRF24L01_MIN_ADDR_LEN ||
			rx_addr_len > NRF24L01_MAX_ADDR_LEN || index >= NRF24L01_NUM_RX_ADDR)
		return NRF_STATUS_INV_ADDR;

	// Set the receiver address. Must be sent LSB first
	tx_buf[0] = (NRF24L01_REG_RX_ADDR_P0 | WRITE_REG_MASK) + index;
	j = 1;
	for (i = (rx_addr_len - 1); i > 0; i--) {
		tx_buf[j++] = *(rx_addr + i);
	}
	if (spi_send_buf(tx_buf, 1 + rx_addr_len))
		return NRF_STATUS_SPI_ERR;

	memcpy(nrf->rx_addr[index], rx_addr, rx_addr_len);

	return NRF_STATUS_OK;
}




