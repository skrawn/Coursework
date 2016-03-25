/**************************************************************************//**
 * @file bme280.cpp
 * @brief Bosch BME280 Humidity and Pressure Sensor Driver
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

#include "bme280.h"
#include "i2c_drv.h"

#define BME280_SLAVE_ADDR	0xAC

#define REG_ID				0xD0
#define REG_RESET			0xE0
#define REG_CTRL_HUM		0xF2
#define REG_STATUS			0xF3
#define REG_CTRL_MEAS		0xF4
#define REG_CONFIG			0xF5
#define REG_PRESS_MSB		0xF7
#define REG_PRESS_LSB		0xF8
#define REG_PRESS_XLSB		0xF9
#define REG_TEMP_MSB		0xFA
#define REG_TEMP_LSB		0xFB
#define REG_TEMP_XLSB		0xFC

// Calibration register addresses 0..41
const uint8_t REG_CALIB[42] = {0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92,
	0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 0xA0, 0xA1,
	0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
	0xF0};

#define BME280_Enable_Port	gpioPortC
#define BME280_Enable_Pin	1
#define BME280_Enable_Mode	gpioModePushPullDrive
#define BME280_Enable_Drive	gpioDriveModeStandard

int32_t temp_degC;		// Temperature in 0.01C
uint32_t pres_Pa;		// Pressure in Q24.8 Pa
uint32_t rel_humidity;	// Relative humidity in Q22.10 %

void BME280_Init(void)
{
	// Setup the pin for VCC
	GPIO_DriveModeSet(BME280_Enable_Port, BME280_Enable_Drive);
	GPIO_PinModeSet(BME280_Enable_Port, BME280_Enable_Pin, BME280_Enable_Mode, 1);

	// Wait 2ms for device to start up
	for (int i = 0; i < 1000000; i++)
	{}

	// Read the device ID.
	uint8_t dev_id;
	uint16_t reg = REG_ID;
	I2C_Read_Polling(BME280_SLAVE_ADDR, reg, 1, &dev_id, 1);

}

void BME280_Enable(bool enable)
{
	if (enable)
		GPIO_PinOutSet(BME280_Enable_Port, BME280_Enable_Pin);
	else
		GPIO_PinOutClear(BME280_Enable_Port, BME280_Enable_Pin);
}
