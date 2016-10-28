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

#define SPI_TIMEOUT	1000	// timeout after 1000us/1ms

static spi_status_t spi_write_byte(SPI_Type *spi, uint8_t byte);
static spi_status_t spi_read_byte(SPI_Type *spi, uint8_t *byte);
static inline void spi1_cs_assert(uint8_t high);
static inline void spi_timeout_clear(void);
static inline uint8_t spi_timeout(void);

uint32_t spi_timeout_start = 0xFFFFFFFF;

void spi_init(void)
{
	// Initializes SPI1 for the nRF24L01+ WiFi module
	// Ungate the clock for SPI1
	SIM->SCGC4 |= SIM_SCGC4_SPI1(1);

	// The KL25Z had some custom modifications to it to make the nRF24L01+
	// work in header J2. The pin map is as follows:
	// PTA17 - CSN
	// PTE1 - MISO
	// PTE31 - VCC
	// PTD3 - CE
	// PTD5 - SCK
	// PTD6 - MOSI
	// PTD7 - IRQ
	// Ungate the clock to Port A, D and E
	SIM->SCGC5 |= SIM_SCGC5_PORTA(1) | SIM_SCGC5_PORTD(1) | SIM_SCGC5_PORTE(1);

	// PORTE31 - Alt Mode 1 - GPIO
	// This is VCC for the nRF24L0+ so it needs high drive strength and slow slew rate
	PORTE->PCR[31] = 0x00000000 |
			PORT_PCR_MUX(1) |
			PORT_PCR_DSE(1) |
			PORT_PCR_SRE(1);
	// Bring VCC High
	PORTE->GPCHR |= PORT_GPCHR_GPWD(15) | PORT_GPCHR_GPWE(15);

	// PORTA17 - Alt Mode 1 - GPIO, Pull Up Enable
	PORTA->PCR[17] = 0x00000000 |
			PORT_PCR_MUX(1) 	|
			PORT_PCR_PS(1) 	|
			PORT_PCR_PE(1);
	// Bring CSN high
	PORTA->GPCHR |= PORT_GPCHR_GPWD(1) | PORT_GPCHR_GPWE(1);

	// PORTD3 - Alt Mode 1 - GPIO, Pull Up Enable
	PORTD->PCR[3] = 0x00000000 |
			PORT_PCR_MUX(1) |
			PORT_PCR_PS(1) |
			PORT_PCR_PE(1);
	// Bring CE High
	PORTD->GPCLR |= PORT_GPCHR_GPWD(3) | PORT_GPCHR_GPWE(3);

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
	SPI1->C1 |= 0x00 |
			SPI_C1_MSTR(1) |
			SPI_C1_CPOL(0) |
			SPI_C1_CPHA(0);

	// Set the baud rate for 2MHz
	// 48MHz / (3) / (8) = 2MHz
	SPI1->BR |= 0x00 |
			SPI_BR_SPPR(2) |
			SPI_BR_SPR(2);

	// Enable Port D interrupt - Move somewhere else?
	NVIC_EnableIRQ(PORTD_IRQn);

	// Enable SPI1
	SPI1->C1 |= SPI_C1_SPE(1);
}

spi_status_t spi_transfer_buf(SPI_Type *spi, uint8_t *tx_buf, size_t tx_len, uint8_t *rx_buf)
{
	void (*cs_assert)(uint8_t high);
	spi_status_t status = SPI_STATUS_OK;

	if (spi == SPI1)
		cs_assert = spi1_cs_assert;
	else
		return SPI_STATUS_NO_SUPPORT;

	cs_assert(CS_LOW);

	while (tx_len > 0 && status == SPI_STATUS_OK) {
		status |= spi_write_byte(spi, *(tx_buf++));
		status |= spi_read_byte(spi, rx_buf++);
		tx_len--;
	}

	cs_assert(CS_HI);

	return status;
}

spi_status_t spi_send_buf(SPI_Type *spi, uint8_t *tx_buf, size_t tx_len)
{
	uint8_t dummy;
	void (*cs_assert)(uint8_t high);
	spi_status_t status = SPI_STATUS_OK;

	if (spi == SPI1)
		cs_assert = spi1_cs_assert;
	else
		return SPI_STATUS_NO_SUPPORT;

	cs_assert(CS_LOW);
	while (tx_len > 0 && status == SPI_STATUS_OK) {
		status |= spi_write_byte(spi, *(tx_buf++));
		status |= spi_read_byte(spi, &dummy);
		tx_len--;
	}
	cs_assert(CS_HI);

	return status;
}

spi_status_t spi_transfer_byte(SPI_Type *spi, uint8_t tx_byte, uint8_t *rx_byte)
{
	void (*cs_assert)(uint8_t high);
	spi_status_t status = SPI_STATUS_OK;

	if (spi == SPI1)
		cs_assert = spi1_cs_assert;
	else
		return SPI_STATUS_NO_SUPPORT;

	cs_assert(CS_LOW);
	status = spi_write_byte(spi, tx_byte);
	status |= spi_read_byte(spi, rx_byte);
	cs_assert(CS_HI);

	return status;
}

spi_status_t spi_send_byte(SPI_Type *spi, uint8_t tx_byte, uint8_t *rx_byte)
{
	void (*cs_assert)(uint8_t high);
	spi_status_t status = SPI_STATUS_OK;
	uint8_t dummy;

	if (spi == SPI1)
		cs_assert = spi1_cs_assert;
	else
		return SPI_STATUS_NO_SUPPORT;

	cs_assert(CS_LOW);
	status = spi_write_byte(spi, tx_byte);
	status |= spi_read_byte(spi, &dummy);
	cs_assert(CS_HI);

	return status;
}

static spi_status_t spi_write_byte(SPI_Type *spi, uint8_t byte)
{
	spi_timeout_clear();

	// Wait for the transmit register to be empty
	while ((spi->S & SPI_S_SPTEF_MASK) && !spi_timeout()) {}

	if (spi_timeout())
		return SPI_STATUS_TIMEOUT;

	// Write a byte to the transmit register
	spi->D = byte;
	return SPI_STATUS_OK;
}

static spi_status_t spi_read_byte(SPI_Type *spi, uint8_t *byte)
{
	spi_timeout_clear();

	while ((spi->S & SPI_S_SPRF_MASK) && !spi_timeout()) {}

	if (spi_timeout())
		return SPI_STATUS_TIMEOUT;

	*byte = spi->D;
	return SPI_STATUS_OK;
}

static inline void spi1_cs_assert(uint8_t high)
{
	if(high)
		PORTA->GPCHR |= PORT_GPCHR_GPWD(1) | PORT_GPCHR_GPWE(1);
	else
		PORTA->GPCHR |= PORT_GPCHR_GPWD(0) | PORT_GPCHR_GPWE(1);
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
	else
		return (profiler_get_time_diff(spi_timeout_start, profiler_get_time()) > SPI_TIMEOUT);
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

#endif


