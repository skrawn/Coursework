/**************************************************************************//**
 * @file config.h
 * @brief Configuration file.
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

#ifndef CONFIG_H_
#define CONFIG_H_

#include "mbed.h"
#include "em_dma.h"

#define Debug				false

#define BLE_Program			false
#define BLE_Factory_Reset	false

#define ULFRCO_FREQ					850		// Hz
#define LFXO_FREQ					32768	// Hz

#define UPPER_TEMP_LIMIT			30		// C
#define LOWER_TEMP_LIMIT			15		// C
#define DEG_C_TO_TENTHS_C			10

#define SAMPLE_TIMER_PERIOD			4000	// ms

#define LOW_POWER_MODE_SLEEP_TIME	4	// s

#define N_DMA_CH_IN_USE		3

float convertToCelsius(int16_t adcSample);
void wait_ms(uint32_t time_ms);

#endif /* CONFIG_H_ */
