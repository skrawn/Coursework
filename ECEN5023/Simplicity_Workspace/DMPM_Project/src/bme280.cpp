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
#include "wait_api.h"

// Temporary
#include "Timer.h"

// SDO = 0
#define BME280_SLAVE_ADDR	0xEC

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
#define REG_HUM_MSB			0xFD
#define REG_HUM_LSB			0xFE

// Calibration register addresses 0..41
const uint8_t REG_CALIB[42] = {0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F, 0x90, 0x91, 0x92,
	0x93, 0x94, 0x95, 0x96, 0x97, 0x98, 0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E, 0x9F, 0xA0, 0xA1,
	0xE1, 0xE2, 0xE3, 0xE4, 0xE5, 0xE6, 0xE7, 0xE8, 0xE9, 0xEA, 0xEB, 0xEC, 0xED, 0xEE, 0xEF,
	0xF0};

#define BME280_Enable_Port	gpioPortC
#define BME280_Enable_Pin	1
#define BME280_Enable_Mode	gpioModePushPullDrive
#define BME280_Enable_Drive	gpioDriveModeStandard

#define BME280_DRDY_Port	gpioPortD
#define BME280_DRDY_Pin		0
#define BME280_DRDY_Mode	gpioModeInput

// Conversion Factors
#define Q24_8_TO_PA			256
#define Q22_10_TO_RH		1024
#define PA_TO_INHG_NUM		295
#define PA_TO_INHG_DEN		10000

int32_t temp_degC;		// Temperature in 0.01C
uint32_t pres_inHg;		// Pressure in 0.01 inHg
uint32_t rel_humidity;	// Relative humidity in 0.1 %

int32_t t_fine;

// Compenstation parameters
uint16_t dig_T1;
int16_t dig_T2;
int16_t dig_T3;

uint16_t dig_P1;
int16_t dig_P2;
int16_t dig_P3;
int16_t dig_P4;
int16_t dig_P5;
int16_t dig_P6;
int16_t dig_P7;
int16_t dig_P8;
int16_t dig_P9;

uint8_t dig_H1;
int16_t dig_H2;
uint8_t dig_H3;
int16_t dig_H4;
int16_t dig_H5;
int8_t dig_H6;

static int32_t BME280_Compensate_Temp(int32_t adc_val);
static uint32_t BME280_Compensate_Pres(int32_t adc_val);
static uint32_t BME280_Compensate_Humidity(int32_t adc_val);
static int32_t BME280_Swap_Bytes(int32_t adc_val);

/**************************************************************************//**
 * @brief Initializes the BME280
 * @verbatim BME280_Init(void); @endverbatim
 *****************************************************************************/
void BME280_Init(void)
{
	uint8_t osr;
	uint16_t temp;

	// Setup the pin for VCC
	GPIO_DriveModeSet(BME280_Enable_Port, BME280_Enable_Drive);
	GPIO_PinModeSet(BME280_Enable_Port, BME280_Enable_Pin, BME280_Enable_Mode, 1);

	// Data ready input
	GPIO_PinModeSet(BME280_DRDY_Port, BME280_DRDY_Pin, BME280_DRDY_Mode, 0);

	// Wait 2ms for device to start up
	wait_ms(2);

	// Read the compensation parameters
	I2C_Read_Polling(BME280_SLAVE_ADDR, REG_CALIB[0], 1, (uint8_t *) &dig_T1, 2);
	I2C_Read_Polling(BME280_SLAVE_ADDR, REG_CALIB[2], 1, (uint8_t *) &dig_T2, 2);
	I2C_Read_Polling(BME280_SLAVE_ADDR, REG_CALIB[4], 1, (uint8_t *) &dig_T3, 2);

	I2C_Read_Polling(BME280_SLAVE_ADDR, REG_CALIB[6], 1, (uint8_t *) &dig_P1, 2);
	I2C_Read_Polling(BME280_SLAVE_ADDR, REG_CALIB[8], 1, (uint8_t *) &dig_P2, 2);
	I2C_Read_Polling(BME280_SLAVE_ADDR, REG_CALIB[10], 1, (uint8_t *) &dig_P3, 2);
	I2C_Read_Polling(BME280_SLAVE_ADDR, REG_CALIB[12], 1, (uint8_t *) &dig_P4, 2);
	I2C_Read_Polling(BME280_SLAVE_ADDR, REG_CALIB[14], 1, (uint8_t *) &dig_P5, 2);
	I2C_Read_Polling(BME280_SLAVE_ADDR, REG_CALIB[16], 1, (uint8_t *) &dig_P6, 2);
	I2C_Read_Polling(BME280_SLAVE_ADDR, REG_CALIB[18], 1, (uint8_t *) &dig_P7, 2);
	I2C_Read_Polling(BME280_SLAVE_ADDR, REG_CALIB[20], 1, (uint8_t *) &dig_P8, 2);
	I2C_Read_Polling(BME280_SLAVE_ADDR, REG_CALIB[22], 1, (uint8_t *) &dig_P9, 2);

	I2C_Read_Polling(BME280_SLAVE_ADDR, REG_CALIB[25], 1, &dig_H1, 1);
	I2C_Read_Polling(BME280_SLAVE_ADDR, REG_CALIB[26], 1, (uint8_t *) &dig_H2, 2);
	I2C_Read_Polling(BME280_SLAVE_ADDR, REG_CALIB[28], 1, &dig_H3, 1);

	// H4 and H5 are in 12 bit formats
	I2C_Read_Polling(BME280_SLAVE_ADDR, REG_CALIB[29], 1, (uint8_t *) &dig_H4, 2);
	I2C_Read_Polling(BME280_SLAVE_ADDR, REG_CALIB[30], 1, (uint8_t *) &dig_H5, 2);

	temp = (dig_H4 & 0xFF) << 4;
	dig_H4 = (dig_H4 & 0xF00) >> 8;
	dig_H4 |= temp;

	temp = (dig_H5 & 0xFF00) >> 4;
	dig_H5 = (dig_H5 & 0xF0) >> 4;
	dig_H5 |= temp;

	I2C_Read_Polling(BME280_SLAVE_ADDR, REG_CALIB[32], 1, (uint8_t *) &dig_H6, 1);

	// Set the oversampling ratio to 1 for each sensor meaning all results are in
	// 16-bit format
	osr = BME280_OVS_X1;
	I2C_Write_Polling(BME280_SLAVE_ADDR, REG_CTRL_HUM, 1, &osr, 1);

	osr = (BME280_OVS_X1 << 5) | (BME280_OVS_X1 << 2);
	I2C_Write_Polling(BME280_SLAVE_ADDR, REG_CTRL_MEAS, 1, &osr, 1);
}

/**************************************************************************//**
 * @brief Sets the BME280 mode
 * @verbatim BME280_Set_Mode(BME280_Mode_t); @endverbatim
 *****************************************************************************/
void BME280_Set_Mode(BME280_Mode_t mode)
{
	uint8_t ctrl_meas_reg;

	// Get the current value of the ctrl_meas register
	I2C_Read_Polling(BME280_SLAVE_ADDR, REG_CTRL_MEAS, 1, &ctrl_meas_reg, 1);

	// Set the new control mode
	ctrl_meas_reg = (ctrl_meas_reg & 0xFC) | mode;
	I2C_Write_Polling(BME280_SLAVE_ADDR, REG_CTRL_MEAS, 1, &ctrl_meas_reg, 1);
}

void BME280_Convert_And_Read_All(void)
{
	uint8_t status;
	int32_t pres_raw = 0, hum_raw = 0, temp_raw = 0;

	// Force a conversion
	BME280_Set_Mode(BME280_MODE_FORCED);

	I2C_Read_Polling(BME280_SLAVE_ADDR, REG_CTRL_MEAS, 1, &status, 1);
	while (status & 0x3) {
		I2C_Read_Polling(BME280_SLAVE_ADDR, REG_CTRL_MEAS, 1, &status, 1);
	}

	// Read back the measured values
	I2C_Read_Polling(BME280_SLAVE_ADDR, REG_PRESS_MSB, 1, (uint8_t *) &pres_raw, 2);
	I2C_Read_Polling(BME280_SLAVE_ADDR, REG_TEMP_MSB, 1, (uint8_t *) &temp_raw, 2);
	I2C_Read_Polling(BME280_SLAVE_ADDR, REG_HUM_MSB, 1, (uint8_t *) &hum_raw, 2);

	// Bytes are read MSB->LSB but stored LSB->MSB. Perform a byte swap
	temp_raw = BME280_Swap_Bytes(temp_raw);
	pres_raw = BME280_Swap_Bytes(pres_raw);
	hum_raw = BME280_Swap_Bytes(hum_raw);

	// Pressure and temperature are expected in 20-bit format
	temp_raw = temp_raw << 4;
	pres_raw = pres_raw << 4;

	// Compensate the ADC values
	temp_degC = BME280_Compensate_Temp(temp_raw);
	pres_inHg = (BME280_Compensate_Pres(pres_raw) / Q24_8_TO_PA) * PA_TO_INHG_NUM / PA_TO_INHG_DEN;
	rel_humidity = BME280_Compensate_Humidity(hum_raw);
}

int32_t BME280_Get_Temp(void)
{
	return temp_degC;
}

uint32_t BME280_Get_Pres(void)
{
	return pres_inHg;
}

uint32_t BME280_Get_Humidity(void)
{
	return rel_humidity;
}

/**************************************************************************//**
 * @brief Switches the BME280 on or off
 * @verbatim BME280_Enable(void); @endverbatim
 *****************************************************************************/
void BME280_Enable(bool enable)
{
	if (enable)
		GPIO_PinOutSet(BME280_Enable_Port, BME280_Enable_Pin);
	else
		GPIO_PinOutClear(BME280_Enable_Port, BME280_Enable_Pin);
}

/**************************************************************************//**
 * @brief Returns the temperature in 0.01C units from an ADC value. Taken from
 * the BME280 datasheet
 * @verbatim int32_t BME280_Compensate_Temp(int32_t); @endverbatim
 *****************************************************************************/
static int32_t BME280_Compensate_Temp(int32_t adc_val)
{
	int32_t var1, var2, T;
	var1 = ((((adc_val >> 3) - ((int32_t) dig_T1 << 1))) * ((int32_t) dig_T2)) >> 11;
	var2 = (((((adc_val >> 4) - ((int32_t) dig_T1)) * ((adc_val >> 4) - ((int32_t) dig_T1))) >> 12) *
			((int32_t) dig_T3)) >> 14;
	t_fine = var1 + var2;
	T = (t_fine * 5 + 128) >> 8;
	return T;
}

/**************************************************************************//**
 * @brief Returns the pressure in Pa in Q24.8 format from an ADC value. Taken from
 * the BME280 datasheet.
 * @verbatim uint32_t BME280_Compensate_Pres(int32_t); @endverbatim
 *****************************************************************************/
static uint32_t BME280_Compensate_Pres(int32_t adc_val)
{
	int64_t var1, var2, p;
	var1 = ((int64_t) t_fine) - 128000;
	var2 = var1 * var1 * (int64_t) dig_P6;
	var2 = var2 + ((var1 * (int64_t) dig_P5) << 17);
	var2 = var2 + (((int64_t) dig_P4) << 35);
	var1 = ((var1 * var1 * (int64_t) dig_P3) >> 8) + ((var1 + (int64_t) dig_P2) << 12);
	var1 = (((((int64_t) 1) << 47) + var1)) * ((int64_t) dig_P1) >> 33;

	// Avoid exception caused by division by zero
	if (var1 == 0)
		return 0;

	p = 1048576 - adc_val;
	p = (((p << 31) - var2) * 3125) / var1;
	var1 = (((int64_t) dig_P9) * (p >> 13) * (p >> 13)) >> 25;
	var2 = (((int64_t) dig_P8) * p) >> 19;
	p = ((p + var1 + var2) >> 8) + (((int64_t) dig_P7) << 4);
	return (uint32_t) p;
}

/**************************************************************************//**
 * @brief Returns humidity in % in Q22.10 format from an ADC value. Taken from
 * the BME280 datasheet.
 * @verbatim uint32_t BME280_Compensate_Humidity(int32_t); @endverbatim
 *****************************************************************************/
static uint32_t BME280_Compensate_Humidity(int32_t adc_val)
{
	int32_t v_x1_u32r;

	v_x1_u32r = (t_fine - ((int32_t) 76800));
	v_x1_u32r = (((((adc_val << 14) - (((int32_t) dig_H4) << 20) - (((int32_t) dig_H5) * v_x1_u32r)) +
			((int32_t) 16384)) >> 15) * (((((((v_x1_u32r * ((int32_t) dig_H6)) >> 10) * (((v_x1_u32r *
			((int32_t) dig_H3)) >> 11) + ((int32_t) 32768))) >> 10) + ((int32_t) 2097152)) *
			((int32_t) dig_H2) + 8192) >> 14));
	v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t) dig_H1)) >> 4));
	v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
	v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
	return (uint32_t) (v_x1_u32r >> 12);
}

static int32_t BME280_Swap_Bytes(int32_t adc_val)
{
	int32_t temp = (adc_val & 0xFF) << 8;
	adc_val = (adc_val & 0xFF00) >> 8;
	adc_val |= temp;
	return adc_val;
}
