/**
 * @file nRF24L01.h
 * @author Sean Donohue
 * @date 27 Oct 2016
 * @brief Driver for nRF24L01 WiFi
 */

#ifndef INC_NRF24L01_H_
#define INC_NRF24L01_H_

#include <stddef.h>
#include <stdint.h>

// nRF24L01 Register Map
#define NRF24L01_REG_CONFIG			0x00
#define NRF24L01_REG_EN_AA			0x01
#define NRF24L01_REG_EN_RXADDR		0x02
#define NRF24L01_REG_SETUP_AW		0x03
#define NRF24L01_REG_SETUP_RETR		0x04
#define NRF24L01_REG_RF_CH			0x05
#define NRF24L01_REG_RF_SETUP		0x06
#define NRF24L01_REG_STATUS			0x07
#define NRF24L01_REG_OBSERVE_TX		0x08
#define NRF24L01_REG_RPD			0x09
#define NRF24L01_REG_RX_ADDR_P0		0x0A
#define NRF24L01_REG_RX_ADDR_P1		0x0B
#define NRF24L01_REG_RX_ADDR_P2		0x0C
#define NRF24L01_REG_RX_ADDR_P3		0x0D
#define NRF24L01_REG_RX_ADDR_P4		0x0E
#define NRF24L01_REG_RX_ADDR_P6		0x0F
#define NRF24L01_REG_TX_ADDR		0x10
#define NRF24L01_REG_RX_PW_P0		0x11
#define NRF24L01_REG_RX_PW_P1		0x12
#define NRF24L01_REG_RX_PW_P2		0x13
#define NRF24L01_REG_RX_PW_P3		0x14
#define NRF24L01_REG_RX_PW_P4		0x15
#define NRF24L01_REG_RX_PW_P5		0x16
#define NRF24L01_REG_FIFO_STATUS	0x17
#define NRF24L01_REG_DYNPD			0x1C
#define NRF24L01_REG_FEATURE		0x1D

#define NRF24L01_MAX_ADDR_LEN		5	// bytes
#define NRF24L01_MIN_ADDR_LEN		3	// bytes
#define NRF24L01_NUM_RX_ADDR		6	// bytes
#define NRF24L01_MAX_PAYLOAD_LEN	32	// bytes

typedef enum {
	NRF_STATUS_OK = 0,
	NRF_STATUS_INV_CONFIG = -1,
	NRF_STATUS_TIMEOUT = -2,
	NRF_STATUS_INV_ADDR = -3,
	NRF_STATUS_SPI_ERR = -4,
} nrf_status_t;

typedef enum {
	NRF_POWER_n18 = 0,
	NRF_POWER_n12,
	NRF_POWER_n6,
	NRF_POWER_0
} nrf_pwr_t;

typedef struct {
	union {
		uint8_t reg;
		struct {
			uint8_t PRIM_RX		:1;
			uint8_t PWR_UP		:1;
			uint8_t CRCO		:1;
			uint8_t EN_CRC		:1;
			uint8_t MASK_MAX_RT	:1;
			uint8_t MASK_TX_DS	:1;
			uint8_t MASK_RX_DR	:1;
			uint8_t res0		:1;
		} bits;
	};
} nrf_config_t;

typedef struct {
	nrf_config_t config;
	nrf_pwr_t output_power;
	uint8_t power_state;
	uint8_t address_width;
	uint8_t tx_addr[NRF24L01_MAX_ADDR_LEN];
	uint8_t rx_addr[NRF24L01_NUM_RX_ADDR][NRF24L01_MAX_ADDR_LEN];
	void (*gpio_irq_handler)(void);
} nrf_t;

nrf_t *nrf_init(nrf_config_t *config, nrf_pwr_t pwr, uint8_t *tx_addr, uint8_t tx_addr_len);

nrf_status_t nrf_set_power(nrf_t *nrf, uint8_t power_on);

nrf_status_t nrf_write_register(nrf_t *nrf, uint8_t reg, uint8_t *buf, size_t buf_len);

nrf_status_t nrf_read_register(nrf_t *nrf, uint8_t reg, uint8_t *buf, size_t buf_len);

nrf_status_t nrf_tx_data(nrf_t *nrf, uint8_t *tx_buf, size_t tx_len);

nrf_status_t nrf_rx_data(nrf_t *nrf, uint8_t *rx_buf, size_t *rx_len, uint8_t index);

nrf_status_t nrf_flush_tx_fifo(nrf_t *nrf);

nrf_status_t nrf_set_tx_address(nrf_t *nrf, uint8_t *tx_addr, uint8_t tx_addr_len);

nrf_status_t nrf_set_rx_address(nrf_t *nrf, uint8_t *rx_addr, uint8_t rx_addr_len, uint8_t index);


#endif /* INC_NRF24L01_H_ */
