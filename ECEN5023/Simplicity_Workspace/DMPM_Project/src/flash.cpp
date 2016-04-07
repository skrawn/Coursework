/**************************************************************************//**
 * @file flash.cpp
 * @brief Flash and EEPROM emulation driver
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

#include "em_ebi.h"
#include "em_gpio.h"
#include "flash.h"
#include "eeprom_emulation.h"

EE_Variable_TypeDef FirstInit, XAxisAlarm, YAxisAlarm, ZAxisAlarm, UpperTempAlarm, LowerTempAlarm,
	UpperPressureAlarm, LowerPressureAlarm, UpperHumidityAlarm, LowerHumidityAlarm,
	CurrentMode;

#define FIRST_INIT_VAL		0xBEEF
#define NUM_EEPROM_PAGES	4	// Uses approximately 2kB of flash


void Flash_Init(void)
{
	uint16_t first_init;

	EE_Init(NUM_EEPROM_PAGES);

	// Declare all of the EEPROM variables. Any new variables that are added
	// must not be added before existing variables!
	EE_DeclareVariable(&FirstInit);
	EE_DeclareVariable(&XAxisAlarm);
	EE_DeclareVariable(&YAxisAlarm);
	EE_DeclareVariable(&ZAxisAlarm);
	EE_DeclareVariable(&UpperTempAlarm);
	EE_DeclareVariable(&LowerTempAlarm);
	EE_DeclareVariable(&UpperPressureAlarm);
	EE_DeclareVariable(&LowerPressureAlarm);
	EE_DeclareVariable(&UpperHumidityAlarm);
	EE_DeclareVariable(&LowerHumidityAlarm);
	EE_DeclareVariable(&CurrentMode);

	// Check if anything has been initialized yet
	EE_Read(&FirstInit, &first_init);
	if (first_init == FIRST_INIT_VAL)
	{

	}
	else
	{
		EE_Write(&FirstInit, FIRST_INIT_VAL);

		// Write the default values to EEPROM

	}
}

