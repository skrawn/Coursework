/**************************************************************************//**
 * @file dma.cpp
 * @brief DMA Driver
 * @author Sean Donohue
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2016 Sean Donohue</b>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 *******************************************************************************/

#include "config.h"
#include "dma.h"
#include "em_dma.h"
#include "dma_api_HAL.h"

/**************************************************************************//**
 * @brief Initializes DMA
 * @verbatim DMA_Initialize(void); @endverbatim
 *****************************************************************************/
void DMA_Initialize(void)
{
	DMA_Init_TypeDef DMA_InitVal;

	// Assign the control block and initialize DMA
	DMA_InitVal.controlBlock = dmaControlBlock;
	DMA_InitVal.hprot = 0;
	DMA_Init(&DMA_InitVal);

	NVIC_EnableIRQ(DMA_IRQn);
}

/**************************************************************************//**
 * @brief Returns the number of transfers - 1 remaining for the specified DMA
 * channel.
 * @verbatim DMA_Get_Transfers_Remaining(uint8_t); @endverbatim
 *****************************************************************************/
uint16_t DMA_Get_Transfers_Remaining(uint8_t ch)
{
	return (uint16_t )((dmaControlBlock[ch].CTRL & 0x3FF0) >> 4);
}
