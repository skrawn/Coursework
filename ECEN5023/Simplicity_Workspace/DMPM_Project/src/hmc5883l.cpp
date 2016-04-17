/**************************************************************************//**
 * @file hmc5883l.cpp
 * @brief Honeywell 3-axis Digital Compass Driver
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

#include <math.h>

#include "em_gpio.h"
#include "i2c_drv.h"
#include "hmc5883l.h"

// GPIO Pins
#define HMC5883L_VCC_Port	gpioPortC
#define HMC5883L_VCC_Pin	0
#define HMC5883L_VCC_Mode	gpioModePushPullDrive

#define HMC5883L_DRDY_Port	gpioPortD
#define HMC5883L_DRDY_Pin	0
#define HMC5883L_DRDY_Mode	gpioModeInputPull

#define HMC5883L_SLAVE_ADDR	0x3C

#define REG_CONFIG_A		0x00
#define REG_CONFIG_B		0x01
#define REG_MODE			0x02
#define REG_DATA_X_MSB		0x03
#define REG_DATA_X_LSB		0x04
#define REG_DATA_Z_MSB		0x05
#define REG_DATA_Z_LSB		0x06
#define REG_DATA_Y_MSB		0x07
#define REG_DATA_Y_LSB		0x08
#define REG_DATA_STATUS		0x09
#define REG_IDENT_A			0x0A
#define REG_IDENT_B			0x0B
#define REG_IDENT_C			0x0C

#define CONV_MG_NUM			92
#define CONV_MG_DEN			100

// Magnetic declination for Boulder, CO is 66'29E (positive)
const float declinationAngle = (66.0 + (29.0 / 60.0)) / (180 / M_PI);

uint16_t compass_heading = 0;


int16_t x_compass_data, y_compass_data, z_compass_data = 0;

void HMC5883L_Init(void)
{
	uint8_t reg, ident_a = 0, ident_b = 0, ident_c = 0;

	GPIO_PinModeSet(HMC5883L_DRDY_Port, HMC5883L_DRDY_Pin, HMC5883L_DRDY_Mode, 0);

	// Read the identification registers first
	while (ident_a != 0x48 && ident_b != 0x34 && ident_c != 0x33)
	{
		I2C_Read_Polling(HMC5883L_SLAVE_ADDR, REG_IDENT_A, 1, &ident_a, 1);
		I2C_Read_Polling(HMC5883L_SLAVE_ADDR, REG_IDENT_B, 1, &ident_b, 1);
		I2C_Read_Polling(HMC5883L_SLAVE_ADDR, REG_IDENT_C, 1, &ident_c, 1);
	}

	HMC5883L_SelfTest();

	reg = 0x00 |
			//(0 << 5) |	// 1 samples averaged per measurement output
			(3 << 5) |	// 8 samples averaged per measurement output
			//(4 << 2) |	// 15 Hz output data rate
			(5 << 2) |	// 30 Hz output data rate
			(0);		// Normal measurement mode
	I2C_Write_Polling(HMC5883L_SLAVE_ADDR, REG_CONFIG_A, 1, &reg, 1);

	reg = 0x00 |
			(1 << 5);	// Gain = 1090 LSB/Gauss - Resolution = 0.92 mG/LSB
	I2C_Write_Polling(HMC5883L_SLAVE_ADDR, REG_CONFIG_B, 1, &reg, 1);

	reg = 0x00 |
			(0 << 7) | 	// High Speed mode (3.4MHz) disabled
			(0);		// Continuous Measurement Mode
	I2C_Write_Polling(HMC5883L_SLAVE_ADDR, REG_MODE, 1, &reg, 1);

	// Wait for DRDY to go high
	while (!GPIO_PinInGet(HMC5883L_DRDY_Port, HMC5883L_DRDY_Pin))
	{}
}

void HMC5883L_ReadAll(void)
{
	uint8_t xyz_data[6] = {0};
	int16_t x_data = 0, y_data = 0, z_data = 0;

	I2C_Read_Polling(HMC5883L_SLAVE_ADDR, REG_DATA_X_MSB, 1, xyz_data, 6);
	//I2C_Read_Reg_Polling(HMC5883L_SLAVE_ADDR, xyz_data, 6, xyz_data, 0);
	//I2C_Read_Reg_Polling(HMC5883L_SLAVE_ADDR, xyz_data, 6, xyz_data, 0);
	//I2C_Read_Polling(HMC5883L_SLAVE_ADDR, xyz_data, 6, xyz_data, 0);
	x_data = (((int16_t) xyz_data[0]) << 8) | ((int16_t) xyz_data[1]);
	z_data = (((int16_t) xyz_data[2]) << 4) | ((int16_t) xyz_data[3]);
	y_data = (((int16_t) xyz_data[4]) << 4) | ((int16_t) xyz_data[5]);
	//I2C_Write_Polling(HMC5883L_SLAVE_ADDR, REG_DATA_X_MSB, 1, xyz_data, 0);

	// Get the sign right
	if (xyz_data[0] > 0x7F)
		x_data -= 0x1000;

	if (xyz_data[2] > 0x7F)
		z_data -= 0x1000;

	if (xyz_data[4] > 0x7F)
		y_data -= 0x1000;

	x_compass_data = x_data;
	y_compass_data = y_data;
	z_compass_data = z_data;
	//x_compass_data = x_data * CONV_MG_NUM / CONV_MG_DEN;
	//y_compass_data = y_data * CONV_MG_NUM / CONV_MG_DEN;
	//z_compass_data = z_data * CONV_MG_NUM / CONV_MG_DEN;
}

int16_t HMC5883L_GetXData(void)
{
	return x_compass_data;
}

int16_t HMC5883L_GetYData(void)
{
	return y_compass_data;
}

int16_t HMC5883L_GetZData(void)
{
	return z_compass_data;
}

bool HMC5883L_SelfTest(void)
{
	uint8_t reg, i, reg_old_cfg_A, reg_old_cfg_B, reg_old_mode;
	bool self_test_result = false;

	// Read the original configuration so it can be restored after self test
	I2C_Read_Polling(HMC5883L_SLAVE_ADDR, REG_CONFIG_A, 1, &reg_old_cfg_A, 1);
	I2C_Read_Polling(HMC5883L_SLAVE_ADDR, REG_CONFIG_B, 1, &reg_old_cfg_B, 1);
	I2C_Read_Polling(HMC5883L_SLAVE_ADDR, REG_MODE, 1, &reg_old_mode, 1);

	reg = 0x00 |
			(3 << 5) |	// 8 samples averaged per measurement output
			(4 << 2) |	// 15 Hz output data rate
			(1);		// Positive self test measurement
	I2C_Write_Polling(HMC5883L_SLAVE_ADDR, REG_CONFIG_A, 1, &reg, 1);

	reg = 0x00 |
			(5 << 5);	// Gain = 390 LSB/Gauss - Resolution = 2.56 mG/LSB
			//(1 << 5);	// Gain = 1090 LSB/Gauss - Resolution = 0.92 mG/LSB
	I2C_Write_Polling(HMC5883L_SLAVE_ADDR, REG_CONFIG_B, 1, &reg, 1);

	reg = 0x00 |
			(0 << 7) | 	// High Speed mode (3.4MHz) disabled
			(0);		// Continuous Measurement Mode
			//(1);		// Single Measurement Mode
	I2C_Write_Polling(HMC5883L_SLAVE_ADDR, REG_MODE, 1, &reg, 1);

	// Loop 8 times to get the averaged result.
	for (i = 0; i < 8; i++)
	{
		// Wait for DRDY to go low
		while (HMC5883L_DataReady())
		{}
		// Wait for DRDY to go high
		while (!HMC5883L_DataReady())
		{}

		HMC5883L_ReadAll();
	}

	// Result for each register should be between 679 and 1607 when the gain is 1.
	if (x_compass_data > 1607 || x_compass_data < 679 || y_compass_data > 1607 || y_compass_data < 679 ||
			z_compass_data > 1607 || z_compass_data < 679)
		self_test_result = false;
	else
		self_test_result = true;

	// Go back the the original configuration
	I2C_Write_Polling(HMC5883L_SLAVE_ADDR, REG_CONFIG_A, 1, &reg_old_cfg_A, 1);
	I2C_Write_Polling(HMC5883L_SLAVE_ADDR, REG_CONFIG_B, 1, &reg_old_cfg_B, 1);
	I2C_Write_Polling(HMC5883L_SLAVE_ADDR, REG_MODE, 1, &reg_old_mode, 1);

	return self_test_result;
}

bool HMC5883L_DataReady(void)
{
	uint8_t reg;

	//I2C_Read_Polling(HMC5883L_SLAVE_ADDR, REG_DATA_STATUS, 1, &reg, 1);

	return (bool) GPIO_PinInGet(HMC5883L_DRDY_Port, HMC5883L_DRDY_Pin);
	//return (bool) (reg & 0x1);
}
