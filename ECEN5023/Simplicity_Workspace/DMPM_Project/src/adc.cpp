/**************************************************************************//**
 * @file adc.cpp
 * @brief ADC Driver
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

#include <stdio.h>
#include "adc.h"
#include "em_gpio.h"
#include "em_adc.h"
#include "leuart.h"
#include "sleepmodes.h"
#include "config.h"

// Global Variables
DMA_CB_TypeDef ADC_Done_CB;
int16_t current_temp; // 0.1C units

// Buffers MUST be aligned on a 0x100 (256 byte) boundary to work properly
uint16_t ADC_Result_Buf[ADC_DMA_BUF_SIZE] __attribute__((aligned(256)));

void ADC_DMA_Done_CB(unsigned int channel, bool primary, void *user);

/**************************************************************************//**
 * @brief Initializes ADC
 * @verbatim ADC_Initialize(void); @endverbatim
 *****************************************************************************/
void ADC_Initialize(void)
{
	ADC_Init_TypeDef ADC_InitValues = ADC_INIT_DEFAULT;
	ADC_InitSingle_TypeDef ADC_Single = ADC_INITSINGLE_DEFAULT;

	CMU_ClockEnable(cmuClock_ADC0, true);

	// Set the ADC clock to HFRCO/4 = 1.75MHz
	ADC_InitValues.prescale = ADC_PrescaleCalc(1750000, 7000000);
	ADC_InitValues.timebase = ADC_TimebaseCalc(0);
	ADC_InitValues.lpfMode = adcLPFilterBypass;
	ADC_InitValues.warmUpMode = adcWarmupKeepADCWarm;
	ADC_Init(ADC0, &ADC_InitValues);

	// Configure the ADC for single channel scan of the temperature sensor
	// T_conv = (16+12)*(1/1.75M) = 16us
	// f_conv = (1/16us) = 62.5kHz
	ADC_Single.acqTime = adcAcqTime16;
	ADC_Single.reference = adcRef1V25;
	ADC_Single.input = adcSingleInpTemp;
	ADC_Single.rep = true;
	ADC_InitSingle(ADC0, &ADC_Single);

	// Initialize DMA for the ADC
	ADC_DMA_Init();
}

/**************************************************************************//**
 * @brief Initializes DMA for ADC
 * @verbatim ADC_DMA_Init(void); @endverbatim
 *****************************************************************************/
void ADC_DMA_Init(void)
{
	DMA_CfgChannel_TypeDef ADC_CfgChannel;
	DMA_CfgDescr_TypeDef ADC_Cfg_Descr;

	// Assign the call back and select the request source as ADC0 Single
	ADC_Done_CB.cbFunc = ADC_DMA_Done_CB;
	ADC_Done_CB.userPtr = NULL;
	ADC_CfgChannel.cb = &ADC_Done_CB;
	ADC_CfgChannel.select = DMA_CH_CTRL_SOURCESEL_ADC0 | DMA_CH_CTRL_SIGSEL_ADC0SINGLE;
	ADC_CfgChannel.enableInt = true;
	ADC_CfgChannel.highPri = ADC_DMA_PRI;
	DMA_CfgChannel(ADC_DMA_CH, &ADC_CfgChannel);

	// Configure the descriptor for word sized transfers
	ADC_Cfg_Descr.arbRate = ADC_DMA_ARB;
	ADC_Cfg_Descr.size = dmaDataSize2;
	ADC_Cfg_Descr.dstInc = dmaDataInc2;
	ADC_Cfg_Descr.srcInc = dmaDataIncNone;
	ADC_Cfg_Descr.hprot = 0;

	DMA_CfgDescr(ADC_DMA_CH, true, &ADC_Cfg_Descr);

	// Enable the ADC Channel interrupt
	DMA->IEN |= (1 << ADC_DMA_CH);
}

/**************************************************************************//**
 * @brief Restarts the ADC and DMA channel for the next samples.
 * @verbatim ADC_Restart(void); @endverbatim
 *****************************************************************************/
void ADC_Restart(void)
{
	ADC0->CTRL |= adcWarmupKeepADCWarm;
	DMA_ActivateBasic(ADC_DMA_CH, true, false, ADC_Result_Buf, (void *) &ADC0->SINGLEDATA, ADC_DMA_N_XFERS);
	ADC_Start(ADC0, adcStartSingle);
}

/**************************************************************************//**
 * @brief ADC DMA done callback
 * @verbatim ADC_DMA_Done_CB(unsigned int channel, bool primary, void *user);
 * @endverbatim
 *****************************************************************************/
void ADC_DMA_Done_CB(unsigned int channel, bool primary, void *user)
{
	uint32_t temp_sum = 0, transfer_size;
	uint8_t temp_buf[50];
	float float_temp;

	// Stop conversions
	ADC0->CMD = ADC_CMD_SINGLESTOP;
	ADC0->CTRL &= (~_ADC_CTRL_WARMUPMODE_MASK);

	// Average the samples
	for (int i = 0; i < ADC_DMA_BUF_SIZE; i++)
		temp_sum += ADC_Result_Buf[i];
	temp_sum /= ADC_DMA_BUF_SIZE;
	float_temp = convertToCelsius(temp_sum);
	current_temp = (int16_t) (float_temp * DEG_C_TO_TENTHS_C);

	// Determine if the temperature is within range
	if (current_temp > UPPER_TEMP_LIMIT*DEG_C_TO_TENTHS_C) {
		transfer_size = sprintf((char *) temp_buf, "Temperature ABOVE set minimum = %d.%dC\n\r",
				current_temp / DEG_C_TO_TENTHS_C, abs(current_temp % DEG_C_TO_TENTHS_C));
		LEUART_Put_TX_Buffer(temp_buf, transfer_size);
		temp_buf[transfer_size] = '\0';
		LEUART_TX_Buffer();
 	}
	else if (current_temp < LOWER_TEMP_LIMIT*DEG_C_TO_TENTHS_C) {
		transfer_size = sprintf((char *) temp_buf, "Temperature BELOW set minimum = %d.%dC\n\r",
				current_temp / DEG_C_TO_TENTHS_C, abs(current_temp % DEG_C_TO_TENTHS_C));
		temp_buf[transfer_size] = '\0';
		LEUART_Put_TX_Buffer(temp_buf, transfer_size);
		LEUART_TX_Buffer();
	}

	// Go back to sleep
	unblockSleepMode(EM1);
}
