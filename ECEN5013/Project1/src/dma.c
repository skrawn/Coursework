/**
 * @file dma.c
 * @author Sean Donohue
 * @date 20 Oct 2016
 * @brief DMA for the KL25Z
 */

#ifdef FRDM

#include <stddef.h>

#include "dma.h"

#define DMA_MAX_BYTES	0xFFFFF

const uint8_t DMA_IRQ_NUMS[] = {
	DMA0_IRQn, DMA1_IRQn, DMA2_IRQn, DMA3_IRQn
};

void dma_init(void)
{
	// Enable the clock to the DMA mux
	SIM->SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
	// Enable the clock to DMA
	SIM->SCGC7 |= SIM_SCGC7_DMA_MASK;

	// Set up DMA channel 3 trigger source to nothing (software controlled)
	DMAMUX0->CHCFG[MEM_DMA_CH] = 0x00 | DMAMUX_CHCFG_ENBL(1) |
			DMAMUX_CHCFG_SOURCE(DMAMUX_SRC_DMAMUX3);

	// Configure settings for the general purpose channel that will always be used
	DMA0->DMA[MEM_DMA_CH].DCR = 0x00000000 |
			//DMA_DCR_EINT(1)		|	// Enable interrupts
			DMA_DCR_AA(1)		|	// Auto-alignment of transfers
			DMA_DCR_DSIZE(0)	|	// Destination size = 32-bits
			DMA_DCR_SSIZE(0);		// Source size = 32-bits

	// Enable DMA interrupt for the GP channel
	NVIC_EnableIRQ(DMA_IRQ_NUMS[MEM_DMA_CH]);
}

int8_t dma_mem_transfer(dma_desc_t *desc)
{
	uint32_t mod_transfers;
	uint8_t *src8 = (uint8_t *) desc->src_addr;
	uint8_t *dst8 = (uint8_t *) desc->dst_addr;

	if (desc->dst_addr == 0 || desc->src_addr == 0 || desc->num_bytes == 0)
		return DMA_ERR_DESC;

	// Check num bytes is within the transfer size
	if (desc->num_bytes > DMA_MAX_BYTES)
		return DMA_ERR_SIZE;

	// Check for source and destination overlap
	if ((desc->src_addr + desc->num_bytes) >= desc->dst_addr)
		return DMA_ERR_OVLP;

	// Check that the transfer size is aligned to 4-bytes
	if ((mod_transfers = desc->num_bytes % 4)) {
		// Perform the first however many transfers with the
		// CPU to get the alignment right
		while (mod_transfers > 0) {
			*((uint8_t *) desc->dst_addr) = *((uint8_t *) desc->src_addr);
			if (desc->dst_increment)
				desc->dst_addr++;
			if (desc->src_increment)
				desc->src_addr++;
			mod_transfers--;
		}
		desc->num_bytes -= mod_transfers;
		desc->src_addr = (uint32_t) src8;
		desc->dst_addr = (uint32_t) dst8;
	}

	DMA0->DMA[MEM_DMA_CH].DAR = desc->dst_addr;
	DMA0->DMA[MEM_DMA_CH].SAR = desc->src_addr;
	DMA0->DMA[MEM_DMA_CH].DCR &= ~(DMA_DCR_DINC_MASK | DMA_DCR_SINC_MASK);
	DMA0->DMA[MEM_DMA_CH].DCR |= DMA_DCR_DINC(desc->dst_increment) |
			DMA_DCR_SINC(desc->src_increment);
	DMA0->DMA[MEM_DMA_CH].DSR_BCR |= desc->num_bytes |
			DMA_DSR_BCR_DONE_MASK;	// Clear the DONE flag

	// Start the transfer
	DMA0->DMA[MEM_DMA_CH].DCR |= DMA_DCR_START(1);
	return 0;
}

void DMA3_IRQHandler(void)
{

}

#endif
