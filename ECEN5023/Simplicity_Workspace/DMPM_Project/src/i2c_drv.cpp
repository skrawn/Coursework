/**************************************************************************//**
 * @file i2c_drv.cpp
 * @brief I2C Driver
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

#include "em_cmu.h"
#include "em_gpio.h"
#include "i2c_drv.h"
#include "sleepmodes.h"

#define I2C1_SDA_Port	gpioPortE
#define I2C1_SDA_Pin	0
//#define I2C1_SDA_Mode	gpioModeWiredAnd
#define I2C1_SDA_Mode	gpioModeWiredAndPullUp

#define I2C1_SCL_Port	gpioPortE
#define I2C1_SCL_Pin	1
//#define I2C1_SCL_Mode	gpioModeWiredAnd
#define I2C1_SCL_Mode	gpioModeWiredAndPullUp

#define I2C1_Bus_Freq	I2C_FREQ_FAST_MAX

I2C_TransferReturn_TypeDef I2C_Status;

/**************************************************************************//**
 * @brief Initializes the I2C1 bus.
 * @verbatim I2C_Initialize(void); @endverbatim
 *****************************************************************************/
void I2C_Initialize(void)
{
	I2C_Init_TypeDef i2cInit = I2C_INIT_DEFAULT;

	CMU_ClockEnable(cmuClock_I2C1, true);

	GPIO_PinModeSet(I2C1_SDA_Port, I2C1_SDA_Pin, I2C1_SDA_Mode, 1);
	GPIO_PinModeSet(I2C1_SCL_Port, I2C1_SCL_Pin, I2C1_SCL_Mode, 1);

	// Temporary: enable all the I2C devices

	// BME280
	GPIO_DriveModeSet(gpioPortC, gpioDriveModeStandard);
	GPIO_PinModeSet(gpioPortC, 1, gpioModePushPullDrive, 1);

	// Accelerometer
	GPIO_DriveModeSet(gpioPortA, gpioDriveModeLowest);
	GPIO_PinModeSet(gpioPortA, 14, gpioModePushPullDrive, 1);

	// Magnetometer
	GPIO_DriveModeSet(gpioPortC, gpioDriveModeLowest);
	GPIO_PinModeSet(gpioPortC, 0, gpioModePushPullDrive, 1);

	// In some situations, the slave device may be left in an unknown state.
	// Send 9 clock pulses just in case.
	for (int i = 0; i < 9; i++)
	{
		GPIO_PinOutClear(I2C1_SCL_Port, I2C1_SCL_Pin);
		GPIO_PinOutSet(I2C1_SCL_Port, I2C1_SCL_Pin);
	}

	// Route pins to position #2
	I2C1->ROUTE = I2C_ROUTE_SDAPEN | I2C_ROUTE_SCLPEN | I2C_ROUTE_LOCATION_LOC2;

	i2cInit.enable = true;
	i2cInit.freq = I2C1_Bus_Freq;
	I2C_Init(I2C1, &i2cInit);

	// Enable interrupts
	/*I2C_IntEnable(I2C1, //I2C_IEN_SSTOP |
			//I2C_IEN_CLTO |
			//I2C_IEN_BITO |
			//I2C_IEN_RXUF |
			//I2C_IEN_TXOF |
			//I2C_IEN_BUSHOLD |
			I2C_IEN_BUSERR |
			I2C_IEN_ARBLOST |
			I2C_IEN_MSTOP |
			I2C_IEN_NACK |
			I2C_IEN_ACK |
			I2C_IEN_RXDATAV
			//I2C_IEN_TXBL |
			//I2C_IEN_TXC |
			//I2C_IEN_ADDR |
			//I2C_IEN_RSTART |
			//I2C_IEN_START
			);
	I2C_IntClear(I2C1, 0x1FFFF);

	NVIC_EnableIRQ(I2C1_IRQn);*/
}

I2C_TransferReturn_TypeDef I2C_Write_Polling(uint8_t slave_addr, uint16_t reg_addr, uint8_t reg_len, uint8_t *tx_data, uint16_t len)
{
	I2C_TransferSeq_TypeDef seq;
	uint8_t *reg_ptr;

	seq.addr = slave_addr;
	seq.flags = I2C_FLAG_WRITE;

	// Make sure the register address is set correctly, if it is a 16-bit or 8-bit register
	reg_ptr = (uint8_t *) &reg_addr;
	if (reg_len == 1)
		reg_ptr += 1;
	seq.buf[0].data = reg_ptr;

	seq.buf[1].data = tx_data;
	seq.buf[1].len = len;

	I2C_Status = I2C_TransferInit(I2C1, &seq);

	blockSleepMode(EM1);
	while (I2C_Status == i2cTransferInProgress) {
		sleep();
	}
	unblockSleepMode(EM1);

	return I2C_Status;
}

I2C_TransferReturn_TypeDef I2C_Read_Polling(uint8_t slave_addr, uint16_t reg_addr, uint8_t reg_len, uint8_t *rx_data, uint16_t len)
{
	I2C_TransferSeq_TypeDef seq;
	uint8_t *reg_ptr;

	seq.addr = slave_addr;
	seq.flags = I2C_FLAG_WRITE_READ;

	// Make sure the register address is set correctly, if it is a 16-bit or 8-bit register
	reg_ptr = (uint8_t *) &reg_addr;
	if (reg_len == 1) {
		reg_ptr += 1;
	}
	seq.buf[0].data = reg_ptr;

	seq.buf[1].data = rx_data;
	seq.buf[1].len = len;

	I2C_Status = I2C_TransferInit(I2C1, &seq);

	blockSleepMode(EM1);
	while (I2C_Status == i2cTransferInProgress) {
		//sleep();
		I2C_Status = I2C_Transfer(I2C1);
	}
	unblockSleepMode(EM1);

	return I2C_Status;
}

void I2C1_IRQHandler(void)
{
	uint32_t active_ints = I2C_IntGet(I2C1);
	I2C_Status = I2C_Transfer(I2C1);
	I2C_IntClear(I2C1, active_ints);
}
