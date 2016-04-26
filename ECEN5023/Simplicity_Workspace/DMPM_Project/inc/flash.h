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

#ifndef FLASH_H_
#define FLASH_H_

#include "mbed.h"

typedef struct {
	int16_t XAxisAlarm;
	int16_t YAxisAlarm;
	int16_t ZAxisAlarm;

	int16_t UpperTempAlarm;
	int16_t LowerTempAlarm;

	uint16_t UpperPressureAlarm;
	uint16_t LowerPressureAlarm;

	uint16_t UpperHumidityAlarm;
	uint16_t LowerHumidityAlarm;

	int8_t XAxisOffset;
	int8_t YAxisOffset;
	int8_t ZAxisOffset;
} EEPROM_Data_t;

extern EEPROM_Data_t EEPROM_Data;

void Flash_Init(void);
void Flash_Update_XAxisAlarm(int16_t new_val);
void Flash_Update_YAxisAlarm(int16_t new_val);
void Flash_Update_ZAxisAlarm(int16_t new_val);
void Flash_Update_UpperTempAlarm(int16_t new_val);
void Flash_Update_LowerTempAlarm(int16_t new_val);
void Flash_Update_UpperPresAlarm(uint16_t new_val);
void Flash_Update_LowerPresAlarm(uint16_t new_val);
void Flash_Update_UpperHumAlarm(uint16_t new_val);
void Flash_Update_LowerHumAlarm(uint16_t new_val);
void Flash_Update_XAxisOffset(int8_t new_val);
void Flash_Update_YAxisOffset(int8_t new_val);
void Flash_Update_ZAxisOffset(int8_t new_val);

__STATIC_INLINE int16_t Flash_Get_XAxisAlarm(void)
{
	return EEPROM_Data.XAxisAlarm;
}

__STATIC_INLINE int16_t Flash_Get_YAxisAlarm(void)
{
	return EEPROM_Data.YAxisAlarm;
}

__STATIC_INLINE int16_t Flash_Get_ZAxisAlarm(void)
{
	return EEPROM_Data.ZAxisAlarm;
}

__STATIC_INLINE int16_t Flash_Get_UpperTempAlarm(void)
{
	return EEPROM_Data.UpperTempAlarm;
}

__STATIC_INLINE int16_t Flash_Get_LowerTempAlarm(void)
{
	return EEPROM_Data.LowerTempAlarm;
}

__STATIC_INLINE uint16_t Flash_Get_UpperPressureAlarm(void)
{
	return EEPROM_Data.UpperPressureAlarm;
}

__STATIC_INLINE uint16_t Flash_Get_LowerPressureAlarm(void)
{
	return EEPROM_Data.LowerPressureAlarm;
}

__STATIC_INLINE uint16_t Flash_Get_UpperHumidityAlarm(void)
{
	return EEPROM_Data.UpperHumidityAlarm;
}

__STATIC_INLINE uint16_t Flash_Get_LowerHumidityAlarm(void)
{
	return EEPROM_Data.LowerHumidityAlarm;
}

#endif /* FLASH_H_ */
