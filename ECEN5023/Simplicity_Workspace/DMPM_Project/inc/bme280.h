/**************************************************************************//**
 * @file bme280.h
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


#ifndef BME280_H_
#define BME280_H_

#include "mbed.h"

typedef enum {
	BME280_OVS_SKIP = 0,
	BME280_OVS_X1,
	BME280_OVS_X2,
	BME280_OVS_X4,
	BME280_OVS_X8,
	BME280_OVS_X16
} BME280_OVS_t;

typedef enum {
	BME280_MODE_SLEEP = 0,
	BME280_MODE_FORCED,
	BME280_MODE_NORMAL = 3
} BME280_Mode_t;

typedef enum {
	BME280_STANDBY_0_5 = 0,
	BME280_STANDBY_62_5,
	BME280_STANDBY_125,
	BME280_STANDBY_250,
	BME280_STANDBY_500,
	BME280_STANDBY_1000,
	BME280_STANDBY_10,
	BME280_STANDBY_20
} BME280_Standby_t;

typedef enum {
	BME280_Filter_Off = 0,
	BME280_Filter_2,
	BME280_Filter_4,
	BME280_Filter_8,
	BME280_Filter_16,
} BME280_Filter_t;

void BME280_Init(void);
void BME280_Set_Mode(BME280_Mode_t mode);
void BME280_Convert_And_Read_All(void);
void BME280_Read_Temp(void);
void BME280_Read_Pres(void);
void BME280_Read_Humidity(void);
int32_t BME280_Get_Temp(void);
uint32_t BME280_Get_Pres(void);
uint32_t BME280_Get_Humidity(void);
void BME280_Enable(bool enable);


#endif /* BME280_H_ */
