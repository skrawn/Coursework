/**
 * @file dma.h
 * @author Sean Donohue
 * @date 20 Oct 2016
 * @brief DMA for the KL25Z
 */

#ifndef INC_DMA_H_
#define INC_DMA_H_

#include <stdint.h>

#ifdef FRDM
#include "MKL25Z4.h"
#endif

#define DMAMUX_SRC_DMAMUX0		60
#define DMAMUX_SRC_DMAMUX1		61
#define DMAMUX_SRC_DMAMUX2		62
#define DMAMUX_SRC_DMAMUX3		63

#define DMA_ERR_NONE			0
#define DMA_ERR_SIZE			-1	// Transfer too large
#define DMA_ERR_OVLP			-2	// Source and destination overlap
#define DMA_ERR_DESC			-3 	// Invalid values in DMA descriptor

// Handle all general purpose DMA transactions
// on channel 3.
#define MEM_DMA_CH				3

typedef enum {
	DMA_INC_NONE = 0,
	DMA_INC_POST
} dma_inc_t;

typedef struct {
	uint32_t src_addr;
	uint32_t dst_addr;
	uint32_t num_bytes;
	dma_inc_t src_increment;
	dma_inc_t dst_increment;
} dma_desc_t;

/**
 * @brief Initializes DMA
 */
void dma_init(void);

/**
 * @brief Performs a DMA memory transfer
 * @param dma_desc_t Pointer to the DMA descriptor
 * @return Returns 0 if there were no errors.
 */
int8_t dma_mem_transfer(dma_desc_t *desc);

#ifdef FRDM
/**
 * @brief Checks if the specified DMA channel has finished
 * @param dma_ch_num DMA channel number
 * @return Returns 1 if the DMA channel is complete.
 */
static inline uint8_t dma_transfer_done(uint8_t dma_ch_num)
{
	return ((DMA0->DMA[dma_ch_num].DSR_BCR & DMA_DSR_BCR_DONE_MASK) >> DMA_DSR_BCR_DONE_SHIFT);
}
#endif

#endif /* INC_DMA_H_ */
