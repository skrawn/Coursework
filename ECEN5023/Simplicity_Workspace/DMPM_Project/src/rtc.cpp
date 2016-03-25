/**************************************************************************//**
 * @file rtc.cpp
 * @brief Real-Time Clock Driver
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
#include "em_cmu.h"
#include "em_rtc.h"
#include "rtc.h"
#include "rtc_api_HAL.h"

#define RTC_TICK_VAL	305		// 30.5us
#define RTC_CLK_FREQ	32768	// Hz

void RTC_Comp0_Handler(void);

void RTC_Initialize(void)
{
	RTC_Init_TypeDef rtc_init;

	CMU_ClockEnable(cmuClock_RTC, true);

	rtc_init.comp0Top = true;
	rtc_init.debugRun = true;
	rtc_init.enable = false;

	RTC_Init(&rtc_init);

	// Setup comp0 interrupt
	rtc_set_comp0_handler((uint32_t) (&RTC_Comp0_Handler));

}

// Called by RTC_IRQHandler
void RTC_Comp0_Handler(void)
{

}
