/**************************************************************************//**
 * @file leuart.h
 * @brief LEUART Driver
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
#ifndef LEUART_H_
#define LEUART_H_

#define LEUART_RX_DMA_CH		0
#define LEUART_TX_DMA_CH		1

#define LEUART_TX_DMA_PRI		0
#define LEUART_TX_DMA_ARB		dmaArbitrate1
#define LEUART_TX_DMA_BUF_SIZE	64
#define LEUART_TX_DMA_N_XFERS	LEUART_TX_DMA_BUF_SIZE - 1

#define LEUART_RX_DMA_PRI		1
#define LEUART_RX_DMA_ARB		dmaArbitrate1
#define LEUART_RX_DMA_BUF_SIZE	512
#define LEUART_RX_DMA_N_XFERS	LEUART_RX_DMA_BUF_SIZE - 1 - END_OF_CMD_DELIM_SIZE

#define END_OF_CMD_DELIMITER	'!'
#define END_OF_CMD_DELIM_SIZE	1

void LEUART_Initialize(void);
void LEUART_DMA_Init(void);
void LEUART_TX_DMA_Done_CB(unsigned int channel, bool primary, void *user);
void LEUART_TX_Buffer(void);
void LEUART_TX_Wait(uint16_t tx_length);
void LEUART_Put_TX_Buffer(uint8_t *data, uint32_t length);
void LEUART_Reset_RX_Buffer(void);
void LEUART_RX_DMA_Done_CB(unsigned int channel, bool primary, void *user);

#endif /* LEUART_H_ */
