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
	UpperPressureAlarm, LowerPressureAlarm, UpperHumidityAlarm, LowerHumidityAlarm, XAxisOffset,
	YAxisOffset, ZAxisOffset;

#define FIRST_INIT_VAL		0xBEEF
#define NUM_EEPROM_PAGES	4	// Uses approximately 2kB of flash

#define DEFAULT_XAXISALARM			50		//0.50g
#define DEFAULT_YAXISALARM			50		//0.50g
#define DEFAULT_ZAXISALARM			50		//0.50g

#define DEFAULT_XAXISOFFSET			0
#define DEFAULT_YAXISOFFSET			0
#define DEFAULT_ZAXISOFFSET			0

#define DEFAULT_UPPERTEMPALARM		500		// 50.0C
#define DEFAULT_LOWERTEMPALARM		-400	// -40.0C

#define DEFAULT_UPPERPRESALARM		3248	// 32.48 inHg
#define DEFAULT_LOWERPRESALARM		886		// 8.86 inHg

#define DEFAULT_UPPERHUMALARM		100		// % RH
#define DEFAULT_LOWERHUMALARM		0		// % RH

EEPROM_Data_t EEPROM_Data;

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
	EE_DeclareVariable(&XAxisOffset);
	EE_DeclareVariable(&YAxisOffset);
	EE_DeclareVariable(&ZAxisOffset);

	// Check if anything has been initialized yet
#if RESET_EEPROM_VALUES
	EE_Read(&FirstInit, &first_init);
	if (first_init == FIRST_INIT_VAL)
	{
		// Restore the old configuration parameters
		EE_Read(&XAxisAlarm, (uint16_t *) &EEPROM_Data.XAxisAlarm);
		EE_Read(&YAxisAlarm, (uint16_t *) &EEPROM_Data.YAxisAlarm);
		EE_Read(&ZAxisAlarm, (uint16_t *) &EEPROM_Data.ZAxisAlarm);

		EE_Read(&UpperTempAlarm, (uint16_t *) &EEPROM_Data.UpperTempAlarm);
		EE_Read(&LowerTempAlarm, (uint16_t *) &EEPROM_Data.LowerTempAlarm);

		EE_Read(&UpperPressureAlarm, &EEPROM_Data.UpperPressureAlarm);
		EE_Read(&LowerPressureAlarm, &EEPROM_Data.LowerPressureAlarm);

		EE_Read(&UpperHumidityAlarm, &EEPROM_Data.UpperHumidityAlarm);
		EE_Read(&LowerHumidityAlarm, &EEPROM_Data.LowerHumidityAlarm);

		EE_Read(&XAxisOffset, (uint16_t *) &EEPROM_Data.XAxisOffset);
		EE_Read(&YAxisOffset, (uint16_t *) &EEPROM_Data.YAxisOffset);
		EE_Read(&ZAxisOffset, (uint16_t *) &EEPROM_Data.ZAxisOffset);
	}
	else
#endif
	{
		EE_Write(&FirstInit, FIRST_INIT_VAL);

		// Write the default values to EEPROM
		EEPROM_Data.XAxisAlarm = DEFAULT_XAXISALARM;
		EEPROM_Data.YAxisAlarm = DEFAULT_YAXISALARM;
		EEPROM_Data.ZAxisAlarm = DEFAULT_ZAXISALARM;

		EEPROM_Data.UpperTempAlarm = DEFAULT_UPPERTEMPALARM;
		EEPROM_Data.LowerTempAlarm = DEFAULT_LOWERTEMPALARM;

		EEPROM_Data.UpperPressureAlarm = DEFAULT_UPPERPRESALARM;
		EEPROM_Data.LowerPressureAlarm = DEFAULT_LOWERPRESALARM;

		EEPROM_Data.UpperHumidityAlarm = DEFAULT_UPPERHUMALARM;
		EEPROM_Data.LowerHumidityAlarm = DEFAULT_LOWERHUMALARM;

		EEPROM_Data.XAxisOffset = DEFAULT_XAXISOFFSET;
		EEPROM_Data.YAxisOffset = DEFAULT_YAXISOFFSET;
		EEPROM_Data.ZAxisOffset = DEFAULT_ZAXISOFFSET;

		EE_Write(&XAxisAlarm, (uint16_t) EEPROM_Data.XAxisAlarm);
		EE_Write(&YAxisAlarm, (uint16_t) EEPROM_Data.YAxisAlarm);
		EE_Write(&ZAxisAlarm, (uint16_t) EEPROM_Data.ZAxisAlarm);

		EE_Write(&UpperTempAlarm, (uint16_t) EEPROM_Data.UpperTempAlarm);
		EE_Write(&LowerTempAlarm, (uint16_t) EEPROM_Data.LowerTempAlarm);

		EE_Write(&UpperPressureAlarm, EEPROM_Data.UpperPressureAlarm);
		EE_Write(&LowerPressureAlarm, EEPROM_Data.LowerPressureAlarm);

		EE_Write(&UpperHumidityAlarm, EEPROM_Data.UpperHumidityAlarm);
		EE_Write(&LowerHumidityAlarm, EEPROM_Data.LowerHumidityAlarm);

		EE_Write(&XAxisOffset, (uint16_t) EEPROM_Data.XAxisOffset);
		EE_Write(&YAxisOffset, (uint16_t) EEPROM_Data.YAxisOffset);
		EE_Write(&ZAxisOffset, (uint16_t) EEPROM_Data.ZAxisOffset);
	}
}

void Flash_Update_XAxisAlarm(int16_t new_val)
{
	if (new_val != EEPROM_Data.XAxisAlarm)
	{
		EEPROM_Data.XAxisAlarm = new_val;
		EE_Write(&XAxisAlarm, (uint16_t) EEPROM_Data.XAxisAlarm);
	}
}

void Flash_Update_YAxisAlarm(int16_t new_val)
{
	if (new_val != EEPROM_Data.YAxisAlarm)
	{
		EEPROM_Data.YAxisAlarm = new_val;
		EE_Write(&YAxisAlarm, (uint16_t) EEPROM_Data.YAxisAlarm);
	}
}

void Flash_Update_ZAxisAlarm(int16_t new_val)
{
	if (new_val != EEPROM_Data.ZAxisAlarm)
	{
		EEPROM_Data.ZAxisAlarm = new_val;
		EE_Write(&ZAxisAlarm, (uint16_t) EEPROM_Data.ZAxisAlarm);
	}
}

void Flash_Update_UpperTempAlarm(int16_t new_val)
{
	if (new_val != EEPROM_Data.UpperTempAlarm)
	{
		EEPROM_Data.UpperTempAlarm = new_val;
		EE_Write(&UpperTempAlarm, (uint16_t) EEPROM_Data.UpperTempAlarm);
	}
}

void Flash_Update_LowerTempAlarm(int16_t new_val)
{
	if (new_val != EEPROM_Data.LowerTempAlarm)
	{
		EEPROM_Data.LowerTempAlarm = new_val;
		EE_Write(&LowerTempAlarm, (uint16_t) EEPROM_Data.LowerTempAlarm);
	}
}

void Flash_Update_UpperPresAlarm(uint16_t new_val)
{
	if (new_val != EEPROM_Data.UpperPressureAlarm)
	{
		EEPROM_Data.UpperPressureAlarm = new_val;
		EE_Write(&UpperPressureAlarm, (uint16_t) EEPROM_Data.UpperPressureAlarm);
	}
}

void Flash_Update_LowerPresAlarm(uint16_t new_val)
{
	if (new_val != EEPROM_Data.LowerPressureAlarm)
	{
		EEPROM_Data.LowerPressureAlarm = new_val;
		EE_Write(&LowerPressureAlarm, (uint16_t) EEPROM_Data.LowerPressureAlarm);
	}
}

void Flash_Update_UpperHumAlarm(uint16_t new_val)
{
	if (new_val != EEPROM_Data.UpperHumidityAlarm)
	{
		EEPROM_Data.UpperHumidityAlarm = new_val;
		EE_Write(&UpperHumidityAlarm, (uint16_t) EEPROM_Data.UpperHumidityAlarm);
	}
}

void Flash_Update_LowerHumAlarm(uint16_t new_val)
{
	if (new_val != EEPROM_Data.LowerHumidityAlarm)
	{
		EEPROM_Data.LowerHumidityAlarm = new_val;
		EE_Write(&LowerHumidityAlarm, (uint16_t) EEPROM_Data.LowerHumidityAlarm);
	}
}

void Flash_Update_XAxisOffset(int8_t new_val)
{
	if (new_val != EEPROM_Data.XAxisOffset)
	{
		EEPROM_Data.XAxisOffset = new_val;
		EE_Write(&XAxisOffset, (uint16_t) EEPROM_Data.XAxisOffset);
	}
}

void Flash_Update_YAxisOffset(int8_t new_val)
{
	if (new_val != EEPROM_Data.YAxisOffset)
	{
		EEPROM_Data.YAxisOffset = new_val;
		EE_Write(&YAxisOffset, (uint16_t) EEPROM_Data.YAxisOffset);
	}
}

void Flash_Update_ZAxisOffset(int8_t new_val)
{
	if (new_val != EEPROM_Data.ZAxisOffset)
	{
		EEPROM_Data.ZAxisOffset = new_val;
		EE_Write(&ZAxisOffset, (uint16_t) EEPROM_Data.ZAxisOffset);
	}
}
