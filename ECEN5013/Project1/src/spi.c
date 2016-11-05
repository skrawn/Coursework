/**
 * @file spi.c
 * @author Sean Donohue
 * @date 23 Oct 2016
 * @brief SPI functions for the KL25Z
 */

#include "spi.h"

#define CS_LOW		0
#define CS_HI		1

#ifdef FRDM

#include "MKL25Z4.h"
#include "profiler.h"



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

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/spi/spidev.h>

#define TRANSFER_DELAY		1	//us
#define CS_GPIO_NUM			115

static void spi_set_cs(uint8_t high);

static const char *device = "/dev/spidev2.1";
static const char *cs_gpio_dir = "/sys/class/gpio/gpio115/direction";
static const char *cs_gpio_val = "/sys/class/gpio/gpio115/value";
static int spi_fd, gpio_fd;

// Mode is configured to defaults:
// CPHA = 0, CPOL = 0, Active low CS, etc...
static uint32_t mode = 0;
static uint32_t bits = 8;
static uint32_t speed = 1000000;	

void spi_init(void)
{
	int ret;
	char buf[10] = {0};

	spi_fd = open(device, O_RDWR);
	if (spi_fd < 0) {
		perror("Cannot open spidev1.0");
		abort();
	}

	ret = ioctl(spi_fd, SPI_IOC_WR_MODE32, &mode);
	if (ret == -1) {
		perror("Cannot set SPI mode");
		abort();
	}

	ret = ioctl(spi_fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
	if (ret == -1) {
		perror("Cannot set SPI bits");
		abort();
	}

	ret = ioctl(spi_fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
	if (ret == -1) {
		perror("Cannot set SPI speed");
		abort();
	}

	gpio_fd = open("/sys/class/gpio/export", O_WRONLY);
	if (gpio_fd < 0) {
		perror("Failed to open export");
		abort();
	}
	sprintf(buf, "%d", CS_GPIO_NUM);
	write(gpio_fd, buf, strlen(buf));
	close(gpio_fd);

	gpio_fd = open(cs_gpio_dir, O_WRONLY);
	write(gpio_fd, "out", 3);
	close(gpio_fd);

	gpio_fd = open(cs_gpio_val, O_WRONLY);
	write(gpio_fd, "1", 1);
	// Leave the file descriptor active
	//close(gpio_fd);
}

spi_status_t spi_transfer_buf(uint8_t *tx_buf, size_t tx_len, uint8_t *rx_buf)
{
	struct spi_ioc_transfer tr;
	int ret;

	tr.tx_buf = (unsigned long) tx_buf;
	tr.rx_buf = (unsigned long) rx_buf;
	tr.len = tx_len;
	tr.delay_usecs = TRANSFER_DELAY;
	tr.speed_hz = speed;
	tr.bits_per_word = bits;
	tr.cs_change = 1;

	spi_set_cs(CS_LOW);
	ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr);
	spi_set_cs(CS_HI);
	if (ret < 1) {
		perror("Failed to send spi message");
		return SPI_STATUS_ERROR;		
	}
	return SPI_STATUS_OK;
}

spi_status_t spi_send_buf(uint8_t *tx_buf, size_t tx_len)
{
	struct spi_ioc_transfer tr;
	int ret;

	tr.tx_buf = (unsigned long) tx_buf;
	tr.rx_buf = (unsigned long) NULL;
	tr.len = tx_len;
	tr.delay_usecs = TRANSFER_DELAY;
	tr.speed_hz = speed;
	tr.bits_per_word = bits;
	tr.cs_change = 1;

	spi_set_cs(CS_LOW);
	ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr);
	spi_set_cs(CS_HI);
	if (ret < 1) {
		perror("Failed to send spi message");
		return SPI_STATUS_ERROR;		
	}
	return SPI_STATUS_OK;
}

spi_status_t spi_transfer_byte(uint8_t tx_byte, uint8_t *rx_byte)
{
	struct spi_ioc_transfer tr;
	int ret;

	tr.tx_buf = (unsigned long) &tx_byte;
	tr.rx_buf = (unsigned long) rx_byte;
	tr.len = 1;
	tr.delay_usecs = TRANSFER_DELAY;
	tr.speed_hz = speed;
	tr.bits_per_word = bits;
	tr.cs_change = 1;

	spi_set_cs(CS_LOW);
	ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr);
	spi_set_cs(CS_HI);
	if (ret < 1) {
		perror("Failed to transfer spi byte");
		return SPI_STATUS_ERROR;		
	}
	return SPI_STATUS_OK;
}

spi_status_t spi_send_byte(uint8_t tx_byte)
{
	struct spi_ioc_transfer tr;
	int ret;

	tr.tx_buf = (unsigned long) &tx_byte;
	tr.rx_buf = (unsigned long) NULL;
	tr.len = 1;
	tr.delay_usecs = TRANSFER_DELAY;
	tr.speed_hz = speed;
	tr.bits_per_word = bits;
	tr.cs_change = 1;

	spi_set_cs(CS_LOW);
	ret = ioctl(spi_fd, SPI_IOC_MESSAGE(1), &tr);
	spi_set_cs(CS_HI);
	if (ret < 1) {
		perror("Failed to send spi byte");
		return SPI_STATUS_ERROR;		
	}
	return SPI_STATUS_OK;
}

static void spi_set_cs(uint8_t high)
{
	if (high)
		write(gpio_fd, "1", 1);
	else
		write(gpio_fd, "0", 1);
}

#else

void spi_init(void) {}

spi_status_t spi_transfer_buf(uint8_t *tx_buf, size_t tx_len, uint8_t *rx_buf) 
{ return SPI_STATUS_NO_SUPPORT; }

spi_status_t spi_send_buf(uint8_t *tx_buf, size_t tx_len)
{ return SPI_STATUS_NO_SUPPORT; }

spi_status_t spi_transfer_byte(uint8_t tx_byte, uint8_t *rx_byte)
{ return SPI_STATUS_NO_SUPPORT; }

spi_status_t spi_send_byte(uint8_t tx_byte)
{ return SPI_STATUS_NO_SUPPORT; }

#endif


