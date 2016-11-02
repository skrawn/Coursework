/**
 * @file project_3.c
 * @author Sean Donohue
 * @date 23 Oct 2016
 * @brief Output for project 3
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "data.h"
#include "dma.h"
#include "log.h"
#include "memory.h"
#include "nRF24L01.h"
#include "profiler.h"
#include "project_3.h"
#include "uart.h"

uint8_t *buffer_5000;
uint8_t *dest_arr;

static void memmove_profiling(void);
static void memzero_profiling(void);

void project_3_report(void)
{
	nrf_pwr_t pwr = NRF_POWER_0;
	uint8_t tx_addr[5] = {0xDE, 0xAD, 0xBE, 0xEF, 0xAA};
	char tx_addr_str[11] = {0};
	nrf_config_t config;
	uint8_t rx_buf[6] = {0};
	nrf_t *nrf;

	buffer_5000 = malloc(5000);
	dest_arr = malloc(5000);

	log_0((uint8_t *) "\nProject 3\n", sizeof("\nProject 3\n"));

#if defined(FRDM) && defined(USE_DMA)
	log_0((uint8_t *) "FRDM DMA Performance", sizeof("FRDM DMA Performance"));

	memmove_profiling();
	memzero_profiling();
#endif

	free(buffer_5000);
	free(dest_arr);

	log_0((uint8_t *) "\nnRF240L01 functions\n", sizeof("\nnRF240L01 functions\n"));
	// Write config register, write TX_ADDR, write RF_SETUP all
	// handled in the nrf_init function
	config.reg = 0;
	config.bits.MASK_RX_DR = 1;		// Mask RX_DR interrupt
	config.bits.MASK_TX_DS = 1;		// Mask TX_DS interrupt
	config.bits.MASK_MAX_RT = 1;	// Mask MAX_RT interrupt
	config.bits.EN_CRC = 1;			// CRC enabled
	config.bits.CRCO = 1;			// 2 byte CRC
	config.bits.PWR_UP = 1;			// Power up
	config.bits.PRIM_RX = 1;		// Go into RX mode

	itoa_hexstring(tx_addr, 5, (uint8_t *) tx_addr_str);
	nrf = nrf_init(&config, pwr, tx_addr, sizeof(tx_addr));

	// Read CONFIG register
	log_1((uint8_t *) "CONFIG Set Value: ", sizeof("CONFIG Set Value: "), (void *) &config.reg,
			log_uint8_t);
	nrf_read_register(nrf, NRF24L01_REG_CONFIG, rx_buf, 2);
	log_1((uint8_t *) "CONFIG Get Value: ", sizeof("CONFIG Set Value: "), (void *) &rx_buf[1],
				log_uint8_t);

	// Read STATUS register
	// Status register should be in rx_buf[0]
	log_1((uint8_t *) "\nSTATUS Get Value: ", sizeof("\nSTATUS Get Value: "), (void *) &rx_buf[0],
			log_uint8_t);

	// Read TX_ADDR register
	log_1((uint8_t *) "\nTX_ADDR Set Value: ", sizeof("\nTX_ADDR Set Value: "),
			(void *) tx_addr_str, log_string_t);
	nrf_read_register(nrf, NRF24L01_REG_TX_ADDR, rx_buf, 6);
	itoa_hexstring(&rx_buf[1], 5, (uint8_t *) tx_addr_str);
	log_1((uint8_t *) "TX_ADDR Get Value: ", sizeof("TX_ADDR Get Value: "),
				(void *) tx_addr_str, log_string_t);

	// Read RF_SETUP register
	pwr <<= 1; // The transmitter power is shifted by 1 in the RF_SETUP register
	log_1((uint8_t *) "\nRF_SETUP Set Value: ", sizeof("\nRF_SETUP Set Value: "), (void *) &pwr,
			log_uint8_t);
	nrf_read_register(nrf, NRF24L01_REG_RF_SETUP, rx_buf, 2);
	log_1((uint8_t *) "RF_SETUP Get Value: ", sizeof("RF_SETUP Get Value: "), (void *) &rx_buf[1],
			log_uint8_t);

	// Read FIFO_STATUS register
	nrf_read_register(nrf, NRF24L01_REG_FIFO_STATUS, rx_buf, 2);
	log_1((uint8_t *) "\nFIFO_STATUS Get Value: ", sizeof("\nFIFO_STATUS Get Value: "),
			(void *) &rx_buf[1], log_uint8_t);

}

static void memmove_profiling(void)
{
	uint32_t time_diff;

#if defined(FRDM) && defined(USE_DMA)
	log_0((uint8_t *) "\nmemmove\n", sizeof("\nmemmove\n"));

	uart_wait_tx_buf_empty();

	profiler_start();
	memmove(buffer_5000, dest_arr, 10);
	profiler_stop();
	time_diff = profiler_get_result();
	log_1((uint8_t *) "memmove 10 bytes: ", sizeof("memmove 10 bytes: "), (void *) &time_diff, log_uint32_t);

	uart_wait_tx_buf_empty();
	profiler_start();
	memmove(buffer_5000, dest_arr, 100);
	profiler_stop();
	time_diff = profiler_get_result();
	log_1((uint8_t *) "memmove 100 bytes: ", sizeof("memmove 100 bytes: "), (void *) &time_diff, log_uint32_t);

	uart_wait_tx_buf_empty();
	profiler_start();
	memmove(buffer_5000, dest_arr, 1000);
	profiler_stop();
	time_diff = profiler_get_result();
	log_1((uint8_t *) "memmove 1000 bytes: ", sizeof("memmove 1000 bytes: "), (void *) &time_diff, log_uint32_t);

	uart_wait_tx_buf_empty();
	profiler_start();
	memmove(buffer_5000, dest_arr, 5000);
	profiler_stop();
	time_diff = profiler_get_result();
	log_1((uint8_t *) "memmove 5000 bytes: ", sizeof("memmove 5000 bytes: "), (void *) &time_diff, log_uint32_t);

	profiler_start();
	my_memmove(buffer_5000, dest_arr, 10);
	profiler_stop();
	time_diff = profiler_get_result();
	log_1((uint8_t *) "my_memmove 10 bytes: ", sizeof("my_memmove 10 bytes: "), (void *) &time_diff, log_uint32_t);

	uart_wait_tx_buf_empty();
	profiler_start();
	my_memmove(buffer_5000, dest_arr, 100);
	profiler_stop();
	time_diff = profiler_get_result();
	log_1((uint8_t *) "my_memmove 100 bytes: ", sizeof("my_memmove 100 bytes: "), (void *) &time_diff, log_uint32_t);

	uart_wait_tx_buf_empty();
	profiler_start();
	my_memmove(buffer_5000, dest_arr, 1000);
	profiler_stop();
	time_diff = profiler_get_result();
	log_1((uint8_t *) "my_memmove 1000 bytes: ", sizeof("my_memmove 1000 bytes: "), (void *) &time_diff, log_uint32_t);

	uart_wait_tx_buf_empty();
	profiler_start();
	my_memmove(buffer_5000, dest_arr, 5000);
	profiler_stop();
	time_diff = profiler_get_result();
	log_1((uint8_t *) "my_memmove 5000 bytes: ", sizeof("my_memmove 5000 bytes: "), (void *) &time_diff, log_uint32_t);

	profiler_start();
	my_memmove_dma(buffer_5000, dest_arr, 10);
	while (!dma_transfer_done(MEM_DMA_CH)) {}
	profiler_stop();
	time_diff = profiler_get_result();
	log_1((uint8_t *) "my_memmove_dma 10 bytes: ", sizeof("my_memmove_dma 10 bytes: "), (void *) &time_diff, log_uint32_t);

	uart_wait_tx_buf_empty();
	profiler_start();
	my_memmove_dma(buffer_5000, dest_arr, 100);
	while (!dma_transfer_done(MEM_DMA_CH)) {}
	profiler_stop();
	time_diff = profiler_get_result();
	log_1((uint8_t *) "my_memmove_dma 100 bytes: ", sizeof("my_memmove_dma 100 bytes: "), (void *) &time_diff, log_uint32_t);

	uart_wait_tx_buf_empty();
	profiler_start();
	my_memmove_dma(buffer_5000, dest_arr, 1000);
	while (!dma_transfer_done(MEM_DMA_CH)) {}
	profiler_stop();
	time_diff = profiler_get_result();
	log_1((uint8_t *) "my_memmove_dma 1000 bytes: ", sizeof("my_memmove_dma 1000 bytes: "), (void *) &time_diff, log_uint32_t);

	uart_wait_tx_buf_empty();
	profiler_start();
	my_memmove_dma(buffer_5000, dest_arr, 5000);
	while (!dma_transfer_done(MEM_DMA_CH)) {}
	profiler_stop();
	time_diff = profiler_get_result();
	log_1((uint8_t *) "my_memmove_dma 5000 bytes: ", sizeof("my_memmove_dma 5000 bytes: "), (void *) &time_diff, log_uint32_t);
#endif
}

static void memzero_profiling(void)
{
	uint32_t time_diff;

#if defined(FRDM) && defined(USE_DMA)
	log_0((uint8_t *) "\nmemset\n", sizeof("\nmemset\n"));

	uart_wait_tx_buf_empty();

	profiler_start();
	memset(buffer_5000, 0, 10);
	profiler_stop();
	time_diff = profiler_get_result();
	log_1((uint8_t *) "memset 10 bytes: ", sizeof("memset 10 bytes: "), (void *) &time_diff, log_uint32_t);

	uart_wait_tx_buf_empty();
	profiler_start();
	memset(buffer_5000, 0, 100);
	profiler_stop();
	time_diff = profiler_get_result();
	log_1((uint8_t *) "memset 100 bytes: ", sizeof("memset 100 bytes: "), (void *) &time_diff, log_uint32_t);

	uart_wait_tx_buf_empty();
	profiler_start();
	memset(buffer_5000, 0, 1000);
	profiler_stop();
	time_diff = profiler_get_result();
	log_1((uint8_t *) "memset 1000 bytes: ", sizeof("memset 1000 bytes: "), (void *) &time_diff, log_uint32_t);

	uart_wait_tx_buf_empty();
	profiler_start();
	memset(buffer_5000, 0, 5000);
	profiler_stop();
	time_diff = profiler_get_result();
	log_1((uint8_t *) "memset 5000 bytes: ", sizeof("memset 5000 bytes: "), (void *) &time_diff, log_uint32_t);

	profiler_start();
	my_memzero(buffer_5000, 10);
	profiler_stop();
	time_diff = profiler_get_result();
	log_1((uint8_t *) "my_memzero 10 bytes: ", sizeof("my_memzero 10 bytes: "), (void *) &time_diff, log_uint32_t);

	uart_wait_tx_buf_empty();
	profiler_start();
	my_memzero(buffer_5000, 100);
	profiler_stop();
	time_diff = profiler_get_result();
	log_1((uint8_t *) "my_memzero 100 bytes: ", sizeof("my_memzero 100 bytes: "), (void *) &time_diff, log_uint32_t);

	uart_wait_tx_buf_empty();
	profiler_start();
	my_memzero(buffer_5000, 1000);
	profiler_stop();
	time_diff = profiler_get_result();
	log_1((uint8_t *) "my_memzero 1000 bytes: ", sizeof("my_memzero 1000 bytes: "), (void *) &time_diff, log_uint32_t);

	uart_wait_tx_buf_empty();
	profiler_start();
	my_memzero(buffer_5000, 5000);
	profiler_stop();
	time_diff = profiler_get_result();
	log_1((uint8_t *) "my_memzero 5000 bytes: ", sizeof("my_memzero 5000 bytes: "), (void *) &time_diff, log_uint32_t);

	profiler_start();
	my_memzero_dma(buffer_5000, 10);
	while (!dma_transfer_done(MEM_DMA_CH)) {}
	profiler_stop();
	time_diff = profiler_get_result();
	log_1((uint8_t *) "my_memzero_dma 10 bytes: ", sizeof("my_memzero_dma 10 bytes: "), (void *) &time_diff, log_uint32_t);

	uart_wait_tx_buf_empty();
	profiler_start();
	my_memzero_dma(buffer_5000, 100);
	while (!dma_transfer_done(MEM_DMA_CH)) {}
	profiler_stop();
	time_diff = profiler_get_result();
	log_1((uint8_t *) "my_memzero_dma 100 bytes: ", sizeof("my_memzero_dma 100 bytes: "), (void *) &time_diff, log_uint32_t);

	uart_wait_tx_buf_empty();
	profiler_start();
	my_memzero_dma(buffer_5000, 1000);
	while (!dma_transfer_done(MEM_DMA_CH)) {}
	profiler_stop();
	time_diff = profiler_get_result();
	log_1((uint8_t *) "my_memzero_dma 1000 bytes: ", sizeof("my_memzero_dma 1000 bytes: "), (void *) &time_diff, log_uint32_t);

	uart_wait_tx_buf_empty();
	profiler_start();
	my_memzero_dma(buffer_5000, 5000);
	while (!dma_transfer_done(MEM_DMA_CH)) {}
	profiler_stop();
	time_diff = profiler_get_result();
	log_1((uint8_t *) "my_memzero_dma 5000 bytes: ", sizeof("my_memzero_dma 5000 bytes: "), (void *) &time_diff, log_uint32_t);

#endif
}
