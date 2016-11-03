/**
 * @file spi.c
 * @author Sean Donohue
 * @date 23 Oct 2016
 * @brief SPI functions for the KL25Z
 */

#ifdef FRDM

#include "MKL25Z4.h"
#include "profiler.h"
#include "spi.h"

#define CS_LOW		0
#define CS_HI		1

#define SPI_TIMEOUT		1000	// timeout after 1000us = 1ms
#define CSN_PIN_NUM		17
#define CE_PIN_NUM		3

static spi_status_t spi_write_byte(uint8_t byte);
static spi_status_t spi_read_byte(uint8_t *byte);
static inline void spi1_cs_assert(uint8_t high);
static inline void spi_timeout_clear(void);
static inline uint8_t spi_timeout(void);

volatile uint32_t spi_timeout_start = 0xFFFFFFFF;
volatile uint32_t spi_timeout_end;

void spi_init(void)
{
	// Initializes SPI1 for the nRF24L01+ WiFi module
	// Ungate the clock for SPI1
	SIM->SCGC4 |= SIM_SCGC4_SPI1(1);

	// The KL25Z had some custom modifications to it to make the nRF24L01+
	// work in header J2. The pin map is as follows:
	// PTA17 - CSN
	// PTE1 - MISO
	// PTD3 - CE
	// PTD5 - SCK
	// PTD6 - MOSI
	// PTD7 - IRQ
	// Ungate the clock to Port A, D and E
	SIM->SCGC5 |= SIM_SCGC5_PORTA(1) | SIM_SCGC5_PORTD(1) | SIM_SCGC5_PORTE(1);

	// PORTA17 - Alt Mode 1 - GPIO, Pull Up Enable
	PORTA->PCR[17] = 0x00000000 |
			PORT_PCR_MUX(1) 	|
			PORT_PCR_PS(1) 	|
			PORT_PCR_PE(1);
	// Make the pin GP output
	GPIOA->PDDR |= (1 << CSN_PIN_NUM);
	// Bring CSN high
	GPIOA->PSOR |= (1 << CSN_PIN_NUM);

	// PORTD3 - Alt Mode 1 - GPIO, Pull Up Enable
	PORTD->PCR[3] = 0x00000000 |
			PORT_PCR_MUX(1) |
			PORT_PCR_PS(1) |
			PORT_PCR_PE(1);
	GPIOD->PDDR |= (1 << CE_PIN_NUM);
	// Bring CE High
	GPIOD->PSOR |= (1 << CE_PIN_NUM);

	// PORTD7 - Alt Mode 1 - GPIO, Input, Enable Filter, Interrupt on rising edge
	PORTD->PCR[7] = 0x00000000 |
			PORT_PCR_MUX(1) |
			PORT_PCR_PFE(1) |
			PORT_PCR_IRQC(9);

	// PORTE1 - Alt Mode 5 - SPI1_MISO
	PORTE->PCR[1] = (PORTE->PCR[1] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(5);
	// PORTD5 - Alt Mode 2 - SPI1_SCK
	PORTD->PCR[5] = (PORTD->PCR[5] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(2);
	// PORTD6 - Alt Mode 2 - SPI1_MOSI
	PORTD->PCR[6] = (PORTD->PCR[6] & ~PORT_PCR_MUX_MASK) | PORT_PCR_MUX(2);

	// Setup SPI for master mode
	SPI1->C1 = 0x00 |
			SPI_C1_MSTR(1) |
			SPI_C1_CPOL(0) |	// Active high clock (idle low)
			SPI_C1_CPHA(0);		// First edge occurs in middle of data

	// Set the baud rate for 1MHz
	// 48MHz / (3) / (16) = 1MHz
	SPI1->BR |= 0x00 |
			SPI_BR_SPPR(2) |
			SPI_BR_SPR(3);

	// Enable Port D interrupt - Move somewhere else?
	NVIC_EnableIRQ(PORTD_IRQn);

	// Enable SPI1
	SPI1->C1 |= SPI_C1_SPE(1);
}

spi_status_t spi_transfer_buf(uint8_t *tx_buf, size_t tx_len, uint8_t *rx_buf)
{
	spi_status_t status = SPI_STATUS_OK;

	spi1_cs_assert(CS_LOW);

	while (tx_len > 0 && status == SPI_STATUS_OK) {
		status |= spi_write_byte(*(tx_buf++));
		status |= spi_read_byte(rx_buf++);
		tx_len--;
	}

	spi1_cs_assert(CS_HI);

	return status;
}

spi_status_t spi_send_buf(uint8_t *tx_buf, size_t tx_len)
{
	uint8_t dummy;
	spi_status_t status = SPI_STATUS_OK;

	spi1_cs_assert(CS_LOW);
	while (tx_len-- > 0 && status == SPI_STATUS_OK) {
		status |= spi_write_byte(*(tx_buf++));
		status |= spi_read_byte(&dummy);
	}
	spi1_cs_assert(CS_HI);

	return status;
}

spi_status_t spi_transfer_byte(uint8_t tx_byte, uint8_t *rx_byte)
{
	spi_status_t status = SPI_STATUS_OK;

	spi1_cs_assert(CS_LOW);
	status = spi_write_byte(tx_byte);
	status |= spi_read_byte(rx_byte);
	spi1_cs_assert(CS_HI);

	return status;
}

spi_status_t spi_send_byte(uint8_t tx_byte)
{
	spi_status_t status = SPI_STATUS_OK;
	uint8_t dummy;

	spi1_cs_assert(CS_LOW);
	status = spi_write_byte(tx_byte);
	status |= spi_read_byte(&dummy);
	spi1_cs_assert(CS_HI);

	return status;
}

static spi_status_t spi_write_byte(uint8_t byte)
{
	spi_timeout_clear();

	// Wait for the transmit register to be empty
	while (!(SPI1->S & SPI_S_SPTEF_MASK) && !spi_timeout()) {}

	if (spi_timeout()) 
		return SPI_STATUS_TIMEOUT;	

	// Write a byte to the transmit register
	SPI1->D = byte;
	return SPI_STATUS_OK;
}

static spi_status_t spi_read_byte(uint8_t *byte)
{
	spi_timeout_clear();

	while (!(SPI1->S & SPI_S_SPRF_MASK) && !spi_timeout()) {}

	if (spi_timeout()) 
		return SPI_STATUS_TIMEOUT;

	*byte = SPI1->D;
	return SPI_STATUS_OK;
}

static inline void spi1_cs_assert(uint8_t high)
{
	if(high)
		GPIOA->PSOR |= (1 << CSN_PIN_NUM);
	else
		GPIOA->PCOR |= (1 << CSN_PIN_NUM);
}

static inline void spi_timeout_clear(void)
{
	spi_timeout_start = 0xFFFFFFFF;
}

static inline uint8_t spi_timeout(void)
{
	if (spi_timeout_start == 0xFFFFFFFF) {
		spi_timeout_start = profiler_get_time();
		return 0;
	}
	else {
		spi_timeout_end = profiler_get_time();
		return (profiler_get_time_diff(spi_timeout_start, spi_timeout_end) > SPI_TIMEOUT);
	}
}

void PORTD_IRQHandler(void)
{
	// Handle nRF24L0+ interrupt
	uint32_t portd = PORTD->ISFR;
	PORTD->ISFR = portd;

	// May move this into nRF driver?
	if (portd & 0x7) {

	}
}

#elif defined(BBB)

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

static const char *device = "/dev/spidev1.0";

void spi_init(void)
{

}


spi_status_t spi_transfer_buf(uint8_t *tx_buf, size_t tx_len, uint8_t *rx_buf);


spi_status_t spi_send_buf(uint8_t *tx_buf, size_t tx_len);


spi_status_t spi_transfer_byte(uint8_t tx_byte, uint8_t *rx_byte);


spi_status_t spi_send_byte(uint8_t tx_byte, uint8_t *rx_byte);

#endif


