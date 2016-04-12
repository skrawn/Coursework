/**************************************************************************//**
 * @file mma8452q.cpp
 * @brief Xtrinsic MMA8452Q 3-axis accelerometer driver
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

#include <stdio.h>
#include "em_gpio.h"
#include "flash.h"
#include "i2c_drv.h"
#include "leuart.h"
#include "mma8452q.h"

// GPIO pins
#define MMA8452Q_VCC_Port		gpioPortB
#define MMA8452Q_VCC_Pin		11
#define MMA8452Q_VCC_Mode		gpioModePushPullDrive

#define MMA8452Q_INT1_Port		gpioPortD
#define MMA8452Q_INT1_Pin		2
#define MMA8452Q_INT1_Mode		gpioModeInput

#define MMA8452Q_INT2_Port		gpioPortD
#define MMA8452Q_INT2_Pin		1
#define MMA8452Q_INT2_Mode		gpioModeInput

#define MMA8452Q_SLAVE_ADDR		0x3A // SA = 1

// Register map
#define REG_STATUS				0x00
#define REG_OUT_X_MSB			0x01
#define REG_OUT_X_LSB			0x02
#define REG_OUT_Y_MSB			0x03
#define REG_OUT_Y_LSB			0x04
#define REG_OUT_Z_MSB			0x05
#define REG_OUT_Z_LSB			0x06

#define REG_SYSMOD				0x0B
#define REG_INT_SOURCE			0x0C
#define REG_WHO_AM_I			0x0D
#define REG_XYZ_DATA_CFG		0x0E
#define REG_HP_FILTER_CUTOFF	0x0F

#define REG_PL_STATUS			0x10
#define REG_PL_CFG				0x11
#define REG_PL_COUNT			0x12
#define REG_PL_BF_ZCOMP			0x13
#define REG_P_L_THIS_REG		0x14

#define REG_FF_MT_CFG			0x15
#define REG_FF_MT_SRC			0x16
#define REG_FF_MT_THS			0x17
#define REG_FF_MT_COUNT			0x18

#define REG_TRANSIENT_CFG		0x1D
#define REG_TRANSIENT_SRC		0x1E
#define REG_TRANSIENT_THS		0x1F
#define REG_TRANSIENT_COUNT		0x20

#define REG_PULSE_CFG			0x21
#define REG_PULSE_SRC			0x22
#define REG_PULSE_THSX			0x23
#define REG_PULSE_THSY			0x24
#define REG_PULSE_THSZ			0x25
#define REG_PULSE_TMLT			0x26
#define REG_PULSE_LTCY			0x27
#define REG_PULSE_WIND			0x28

#define REG_ASLP_COUNT			0x29
#define REG_CTRL_REG1			0x2A
#define REG_CTRL_REG2			0x2B
#define REG_CTRL_REG3			0x2C
#define REG_CTRL_REG4			0x2D
#define REG_CTRL_REG5			0x2E

#define REG_OFF_X				0x2F
#define REG_OFF_Y				0x30
#define REG_OFF_Z				0x31

#define Z_OFFSET_1G				500		// 500 * 0.002g = 1.000g
#define PULSE_THS_CONV_NUM		10		// Converts the user data in 0.01g units
#define PULSE_THS_CONV_DEN		63		// to the threshold register units of 0.063g

#define PULSE_SRC_AXZ			0x40
#define PULSE_SRC_AXY			0x20
#define PULSE_SRC_AXX			0x10
#define PULSE_SRC_POLZ			0x04
#define PULSE_SRC_POLY			0x02
#define PULSE_SRC_POLX			0x01

uint8_t offset_x = 0;
uint8_t offset_y = 0;
uint8_t offset_z = 0;

void MMA8452Q_Init(void)
{
	uint8_t reg = 0;

	GPIO_PinModeSet(MMA8452Q_INT1_Port, MMA8452Q_INT1_Pin, MMA8452Q_INT1_Mode, 0);
	GPIO_PinModeSet(MMA8452Q_INT2_Port, MMA8452Q_INT2_Pin, MMA8452Q_INT1_Mode, 0);

	// Enable external interrupt on these pins. Interrupt on rising edge.
	GPIO_IntConfig(MMA8452Q_INT1_Port, MMA8452Q_INT1_Pin, true, false, true);
	GPIO_IntConfig(MMA8452Q_INT2_Port, MMA8452Q_INT2_Pin, true, false, true);

	// Check the device ID first
	I2C_Read_Polling(MMA8452Q_SLAVE_ADDR, REG_WHO_AM_I, 1, &reg, 1);

	// Setup for 12.5Hz ODR, Low Noise = 1, OSR = 4, 4g mode gives a current
	// draw of 8uA (from Freescale AN4075)
	reg = 0x00 |
			(0 << 7) |	// Self-test disabled
			(0 << 6) |	// Reset off
			(1 << 3) |	// Sleep mode power scheme: low noise low power
			(1 << 2) |	// Auto-sleep enabled
			(1);		// Normal mode power scheme: low noise low power
	I2C_Write_Polling(MMA8452Q_SLAVE_ADDR, REG_CTRL_REG2, 1, &reg, 1);

	reg = 0x00 |
			(1 << 6) |	// Transient can wake system
			(0 << 5) |	// Landscape/portrait cannot wake system
			(1 << 4) |	// Pulse can wake system
			(0 << 3) |	// Freefall/motion cannot wake system
			(1 << 1) |	// Active high interrupt polarity
			(0);		// Push-pull
	I2C_Write_Polling(MMA8452Q_SLAVE_ADDR, REG_CTRL_REG3, 1, &reg, 1);

	reg = 0x00 |
			(0 << 7) |	// Sleep/Wake interrupt disabled
			//(1 << 5) |	// Transient interrupt enabled
			(0 << 5) |	// Transient interrupt disabled
			(0 << 4) |	// Landscape/portrait interrupt disabled
			(1 << 3) |	// Pulse interrupt enabled
			//(0 << 3) |	// Pulse interrupt disabled
			(0 << 2) |	// Freefall/motion interrupt disabled
			(0);		// Data ready interrupt disabled
	I2C_Write_Polling(MMA8452Q_SLAVE_ADDR, REG_CTRL_REG4, 1, &reg, 1);

	reg = 0x00 |
			(1 << 5) |	// Transient interrupt goes to INT1
			(0 << 3);	// Pulse interrupt goes to INT2
	I2C_Write_Polling(MMA8452Q_SLAVE_ADDR, REG_CTRL_REG4, 1, &reg, 1);

	reg = 0x00 |
			(0 << 4) |	// High-pass filter disabled
			(1);		// 4g range
	I2C_Write_Polling(MMA8452Q_SLAVE_ADDR, REG_XYZ_DATA_CFG, 1, &reg, 1);

	// Update the offset registers with whatever was stored in EEPROM
	I2C_Write_Polling(MMA8452Q_SLAVE_ADDR, REG_OFF_X, 1, (uint8_t *) &EEPROM_Data.XAxisOffset, 1);
	I2C_Write_Polling(MMA8452Q_SLAVE_ADDR, REG_OFF_Y, 1, (uint8_t *) &EEPROM_Data.YAxisOffset, 1);
	I2C_Write_Polling(MMA8452Q_SLAVE_ADDR, REG_OFF_Z, 1, (uint8_t *) &EEPROM_Data.ZAxisOffset, 1);

	// Configure pulse detection
	// Set the pulse thresholds
	reg = (uint8_t) (EEPROM_Data.XAxisAlarm * PULSE_THS_CONV_NUM / PULSE_THS_CONV_DEN);
	I2C_Write_Polling(MMA8452Q_SLAVE_ADDR, REG_PULSE_THSX, 1, &reg, 1);

	reg = (uint8_t) (EEPROM_Data.YAxisAlarm * PULSE_THS_CONV_NUM / PULSE_THS_CONV_DEN);
	I2C_Write_Polling(MMA8452Q_SLAVE_ADDR, REG_PULSE_THSY, 1, &reg, 1);

	reg = (uint8_t) (EEPROM_Data.ZAxisAlarm * PULSE_THS_CONV_NUM / PULSE_THS_CONV_DEN);
	I2C_Write_Polling(MMA8452Q_SLAVE_ADDR, REG_PULSE_THSZ, 1, &reg, 1);

	reg = 0x00 |
			(0 << 5) |	// Bypass HPF for the pulse processing
			(1 << 4) |	// Low pass filtered enable for pulse processing
			(0);		// High pass filter cutoff = 0.5Hz

	// Pulse window time: in LPLN mode with ODR = 12.5Hz, each count is 80ms. Set the
	// window for 240ms?
	reg = 3;
	I2C_Write_Polling(MMA8452Q_SLAVE_ADDR, REG_PULSE_TMLT, 1, &reg, 1);

	// Set latency to 500ms. Each count is 160ms
	reg = 3;
	I2C_Write_Polling(MMA8452Q_SLAVE_ADDR, REG_PULSE_LTCY, 1, &reg, 1);

	// Set window time to...5 seconds? This is probably not needed for single pulse mode,
	// but configure it anyway. Each tick is 160ms.
	reg = 31;
	I2C_Write_Polling(MMA8452Q_SLAVE_ADDR, REG_PULSE_WIND, 1, &reg, 1);

	reg = 0x00 |
			(0 << 7) | 	// Doulbe pulse detection not aborted
			(1 << 6) | 	// Latch pulse event flags
			(0 << 5) |	// Disable Z axis double pulse detection
			(1 << 4) |	// Enable Z axis single pulse detection
			(0 << 3) |	// Disable Y axis double pulse detection
			(1 << 2) |	// Enable Y axis single pulse detection
			(0 << 1) |	// Disable X axis double pulse detection
			(1); 		// Enable X axis single pulse detection
	I2C_Write_Polling(MMA8452Q_SLAVE_ADDR, REG_PULSE_CFG, 1, &reg, 1);

	// Set the data rate and put the accelerometer into active mode
	reg = 0x00 |
			(1 << 6) |	// Autosleep data rate = 12.5Hz
			(5 << 3) |	// Output data rate = 12.5Hz
			(1 << 2) |	// Low noise mode enabled
			(0 << 1) |	// Normal read mode
			(1);		// Active mode
	I2C_Write_Polling(MMA8452Q_SLAVE_ADDR, REG_CTRL_REG1, 1, &reg, 1);

	// Enable GPIO interrupts
	GPIO_IntEnable(MMA8452Q_INT1_Pin | MMA8452Q_INT2_Pin);

}

void MMA8452Q_Realign(void)
{
	// Used to realign board to 0g positions
	uint8_t xyz_data[6] = {0}, status = 0;
	int16_t x_data = 0, y_data = 0, z_data = 0;
	uint8_t x_offset = 0, y_offset = 0, z_offset = 0;;

	I2C_Read_Polling(MMA8452Q_SLAVE_ADDR, REG_STATUS, 1, &status, 1);
	while ((status & 0x7) != 0x7)
	{
		I2C_Read_Polling(MMA8452Q_SLAVE_ADDR, REG_STATUS, 1, &status, 1);
	}

	// Assume the board is completely static.
	// 1 count from the output data register
	// corresponds to 1 count in the calibration
	// register (in 4g mode)
	I2C_Read_Polling(MMA8452Q_SLAVE_ADDR, REG_OUT_X_MSB, 1, xyz_data, 6);
	x_data = (((int16_t) xyz_data[0]) << 4) | (((int16_t) (xyz_data[1] & 0xF0)) >> 4);
	y_data = (((int16_t) xyz_data[2]) << 4) | (((int16_t) (xyz_data[3] & 0xF0)) >> 4);
	z_data = (((int16_t) xyz_data[4]) << 4) | (((int16_t) (xyz_data[5] & 0xF0)) >> 4);

	// Switch back to standby
	I2C_Read_Polling(MMA8452Q_SLAVE_ADDR, REG_CTRL_REG1, 1, &status, 1);
	status &= (0xFE);
	I2C_Write_Polling(MMA8452Q_SLAVE_ADDR, REG_CTRL_REG1, 1, &status, 1);

	// Write the offset to the offset registers
	x_offset = (uint8_t) (x_data * -1);
	I2C_Write_Polling(MMA8452Q_SLAVE_ADDR, REG_OFF_X, 1, &x_offset, 1);
	y_offset = (uint8_t) (y_data * -1);
	I2C_Write_Polling(MMA8452Q_SLAVE_ADDR, REG_OFF_Y, 1, &y_offset, 1);
	z_offset = (uint8_t) (Z_OFFSET_1G - (z_data * -1));
	I2C_Write_Polling(MMA8452Q_SLAVE_ADDR, REG_OFF_Z, 1, &z_offset, 1);

	// Back to active mode
	status |= (0x1);
	I2C_Write_Polling(MMA8452Q_SLAVE_ADDR, REG_CTRL_REG1, 1, &status, 1);

	// Store the new offsets into EEPROM
	Flash_Update_XAxisOffset(x_offset);
	Flash_Update_YAxisOffset(y_offset);
	Flash_Update_ZAxisOffset(z_offset);
}

void GPIO_IRQHandler(void)
{
	uint32_t intflags = GPIO_IntGet(), tx_size = 0;
	uint8_t reg = 0, tx_buf[40];
	GPIO_IntClear(intflags);

	// Check the source of the interrupt
	if (intflags & MMA8452Q_INT1_Pin)
	{
		// Transient detected
	}

	if (intflags & MMA8452Q_INT2_Pin)
	{
		// Pulse detected
		// Read the pulse source register
		I2C_Read_Polling(MMA8452Q_SLAVE_ADDR, REG_PULSE_SRC, 1, &reg, 1);

		if (reg & PULSE_SRC_AXZ)
		{
			if (reg & PULSE_SRC_POLZ)
				tx_size = sprintf((char *) tx_buf, "Pulse exceeding %d.%02dg detected downwards!",
						EEPROM_Data.XAxisAlarm / 100, EEPROM_Data.XAxisAlarm % 100);
			else
				tx_size = sprintf((char *) tx_buf, "Pulse exceeding %d.%02dg detected upwards!",
						EEPROM_Data.XAxisAlarm / 100, EEPROM_Data.XAxisAlarm % 100);
			LEUART_Put_TX_Buffer(tx_buf, tx_size);
		}

		// Okay, need to get the direction right here. The Z-axis is correct, but on my dev kit,
		// the positive x-axis is to the left and the positive y-axis is toward the back of the
		// board (battery).
		if (reg & PULSE_SRC_AXY)
		{
			if (reg & PULSE_SRC_POLY)
				tx_size = sprintf((char *) tx_buf, "Pulse exceeding %d.%02dg detected backwards!",
						EEPROM_Data.YAxisAlarm / 100, EEPROM_Data.YAxisAlarm % 100);
			else
				tx_size = sprintf((char *) tx_buf, "Pulse exceeding %d.%02dg detected forwards!",
						EEPROM_Data.YAxisAlarm / 100, EEPROM_Data.YAxisAlarm % 100);
			LEUART_Put_TX_Buffer(tx_buf, tx_size);
		}

		if (reg & PULSE_SRC_AXX)
		{
			if (reg & PULSE_SRC_POLX)
				tx_size = sprintf((char *) tx_buf, "Pulse exceeding %d.%02dg detected to the right!",
						EEPROM_Data.XAxisAlarm / 100, EEPROM_Data.XAxisAlarm % 100);
			else
				tx_size = sprintf((char *) tx_buf, "Pulse exceeding %d.%02dg detected to the left!",
						EEPROM_Data.XAxisAlarm / 100, EEPROM_Data.XAxisAlarm % 100);
			LEUART_Put_TX_Buffer(tx_buf, tx_size);
		}

		LEUART_TX_Buffer();
	}
}
