/**
 * @file project_3.c
 * @author Sean Donohue
 * @date 23 Oct 2016
 * @brief Output for project 3
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dma.h"
#include "log.h"
#include "memory.h"
#include "profiler.h"
#include "project_3.h"
#include "uart.h"

uint8_t *buffer_5000;
uint8_t *dest_arr;

static void memmove_profiling(void);
static void memzero_profiling(void);

void project_3_report(void)
{
	buffer_5000 = malloc(5000);
	dest_arr = malloc(5000);

	log_0((uint8_t *) "\nProject 3\n", sizeof("\nProject 3\n"));
	log_0((uint8_t *) "FRDM DMA Performance\n", sizeof("FRDM DMA Performance\n"));

	memmove_profiling();
	memzero_profiling();

	free(buffer_5000);
	free(dest_arr);
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
