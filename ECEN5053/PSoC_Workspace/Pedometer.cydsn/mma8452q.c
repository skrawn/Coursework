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
#include <math.h>
#include "eeprom_api.h"
#include "fivePtTripleSmooth.h"
#include "hanning.h"
#include "i2c_api.h"
#include "LCD.h"
#include "mma8452q.h"
#include "steps.h"

// I2C driver expects slave addresses in 7-bit format
#define MMA8452Q_SLAVE_ADDR		(0x3A >> 1) // SA = 1

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

#define REG_1G_COUNTS_INT       500
#define Z_OFFSET_1G				500		// 500 * 0.002g = 1.000g
#define PULSE_THS_CONV_NUM		10		// Converts the user data in 0.01g units
#define PULSE_THS_CONV_DEN		63		// to the threshold register units of 0.063g

//#define DETECTION_THRESHOLD     100     // Detection threshold = +/- 0.2g = 0.2/0.002 = 100 
#define DETECTION_THRESHOLD     75     // Detection threshold = +/- 0.15g = 0.15/0.002 = 75 

#define REG_TO_READ_NUM			1		// Converts the 0.002g units to 0.01g units
#define REG_TO_READ_DEN			5

#define PULSE_SRC_AXZ			0x40
#define PULSE_SRC_AXY			0x20
#define PULSE_SRC_AXX			0x10
#define PULSE_SRC_POLZ			0x04
#define PULSE_SRC_POLY			0x02
#define PULSE_SRC_POLX			0x01

//#define PULSE_X_THRESH          8       // ~0.5G
#define PULSE_X_THRESH          3       // ~0.2G
//#define PULSE_Y_THRESH          8       // ~0.5G
#define PULSE_Y_THRESH          3       // ~0.2G
//#define PULSE_Z_THRESH          24      // ~1.5G
#define PULSE_Z_THRESH          19      // ~1.2G

typedef enum {
    X_IS_GRAVITY,
    Y_IS_GRAVITY,
    Z_IS_GRAVITY,
} Orientation_t;

const double REG_1G_COUNTS = 500.0;

uint8_t offset_x = 0;
uint8_t offset_y = 0;
uint8_t offset_z = 0;

int16_t x_axis_data = 0;
int16_t y_axis_data = 0;
int16_t z_axis_data = 0;

int16_t max_X_value = 0;
int16_t max_Y_value = 0;
int16_t max_Z_value = 0;

fivePtTripleSmooth_t x_filter, y_filter, z_filter;
Hanning_t xh_filter, yh_filter, zh_filter;

bool at_rest = true;

void MMA8452Q_Init(void)
{
	uint8_t reg = 0;
    
    fivePtTripleSmooth_InitFilter(&x_filter);
    fivePtTripleSmooth_InitFilter(&y_filter);
    fivePtTripleSmooth_InitFilter(&z_filter);
    Hanning_Init(&xh_filter);
    Hanning_Init(&yh_filter);
    Hanning_Init(&zh_filter);

	// Check the device ID first
	I2C_ReadBuffer(MMA8452Q_SLAVE_ADDR, REG_WHO_AM_I, &reg, 1);

	// Not sure why, but sometime I can't talk to the accelerometer on the first
	// try. Just keep trying.
	if (reg != 0x2A) {
		while (reg != 0x2A) {
			I2C_ReadBuffer(MMA8452Q_SLAVE_ADDR, REG_WHO_AM_I, &reg, 1);
		}
	}

	// Place device into standby mode	
    I2C_ReadBuffer(MMA8452Q_SLAVE_ADDR, REG_CTRL_REG1, &reg, 1);
	reg &= ~(0x1);
    I2C_WriteBuffer(MMA8452Q_SLAVE_ADDR, REG_CTRL_REG1, &reg, 1);	

	// Setup for 12.5Hz ODR, Low Noise = 1, OSR = 4, 4g mode gives a current
	// draw of 8uA (from Freescale AN4075)
	reg = 0x00 |
			(0 << 7) |	// Self-test disabled
			(0 << 6) |	// Reset off
			(1 << 3) |	// Sleep mode power scheme: low noise low power
			(1 << 2) |	// Auto-sleep enabled
			(1);		// Normal mode power scheme: low noise low power
	I2C_WriteBuffer(MMA8452Q_SLAVE_ADDR, REG_CTRL_REG2, &reg, 1);

	reg = 0x00 |
			(1 << 6) |	// Transient can wake system
			(0 << 5) |	// Landscape/portrait cannot wake system
			(1 << 4) |	// Pulse can wake system
			(0 << 3) |	// Freefall/motion cannot wake system
			(1 << 1) |	// Active high interrupt polarity
			(0);		// Push-pull
	I2C_WriteBuffer(MMA8452Q_SLAVE_ADDR, REG_CTRL_REG3, &reg, 1);

	reg = 0x00 |
			(0 << 7) |	// Sleep/Wake interrupt disabled
			//(1 << 5) |	// Transient interrupt enabled
			(0 << 5) |	// Transient interrupt disabled
			(1 << 4) |	// Landscape/portrait interrupt enabled
			(0 << 3) |	// Pulse interrupt disabled
			//(0 << 3) |	// Pulse interrupt disabled
			(0 << 2) |	// Freefall/motion interrupt disabled
			(1);		// Data ready interrupt Enabled
	I2C_WriteBuffer(MMA8452Q_SLAVE_ADDR, REG_CTRL_REG4, &reg, 1);

	reg = 0x00 |
            (0 << 4) |      // Landscape/portrait interrupt goes to INT2
            (1);            // Data ready interrupt goes to INT1
			//(1 << 5) |	// Transient interrupt goes to INT1
			//(0 << 3);	// Pulse interrupt goes to INT2
	I2C_WriteBuffer(MMA8452Q_SLAVE_ADDR, REG_CTRL_REG5, &reg, 1);

	reg = 0x00 |
			(0 << 4) |	// High-pass filter disabled
			(1);		// 4g range
	I2C_WriteBuffer(MMA8452Q_SLAVE_ADDR, REG_XYZ_DATA_CFG, &reg, 1);

	// Update the offset registers with whatever was stored in EEPROM
    reg = EEPROM_Get_XAxisOffset();
	I2C_WriteBuffer(MMA8452Q_SLAVE_ADDR, REG_OFF_X, (uint8_t *) &reg, 1);
    reg = EEPROM_Get_YAxisOffset();
	I2C_WriteBuffer(MMA8452Q_SLAVE_ADDR, REG_OFF_Y, (uint8_t *) &reg, 1);
    reg = EEPROM_Get_ZAxisOffset();
	I2C_WriteBuffer(MMA8452Q_SLAVE_ADDR, REG_OFF_Z, (uint8_t *) &reg, 1);

	// Configure pulse detection
	// Set the pulse thresholds
	reg = PULSE_X_THRESH;
	I2C_WriteBuffer(MMA8452Q_SLAVE_ADDR, REG_PULSE_THSX, &reg, 1);

	reg = PULSE_Y_THRESH;
	I2C_WriteBuffer(MMA8452Q_SLAVE_ADDR, REG_PULSE_THSY, &reg, 1);

	reg = PULSE_Z_THRESH;
	I2C_WriteBuffer(MMA8452Q_SLAVE_ADDR, REG_PULSE_THSZ, &reg, 1);

	reg = 0x00 |
			(0 << 5) |	// Bypass HPF for the pulse processing
			//(1 << 4) |	// Low pass filtered enable for pulse processing
			(0 << 4) |	// Low pass filtered enable for pulse processing
			(0);		// High pass filter cutoff = 0.5Hz
	I2C_WriteBuffer(MMA8452Q_SLAVE_ADDR, REG_HP_FILTER_CUTOFF, &reg, 1);

	// Pulse window time: in LPLN mode with ODR = 12.5Hz, each count is 80ms. Set the
	// window for 240ms?
	reg = 3;
    //reg = 1;
	I2C_WriteBuffer(MMA8452Q_SLAVE_ADDR, REG_PULSE_TMLT, &reg, 1);

	// Set latency to 500ms. Each count is 160ms
	//reg = 3;
    reg = 1;
	I2C_WriteBuffer(MMA8452Q_SLAVE_ADDR, REG_PULSE_LTCY, &reg, 1);

	// Set window time to 1 second? aEach tick is 160ms.
	//reg = 7;
    reg = 1;
	I2C_WriteBuffer(MMA8452Q_SLAVE_ADDR, REG_PULSE_WIND, &reg, 1);

	reg = 0x00 |
			(0 << 7) | 	// Doulbe pulse detection not aborted
			(1 << 6) | 	// Latch pulse event flags
			(0 << 5) |	// Disable Z axis double pulse detection
			(1 << 4) |	// Enable Z axis single pulse detection
			(0 << 3) |	// Disable Y axis double pulse detection
			(1 << 2) |	// Enable Y axis single pulse detection
			(0 << 1) |	// Disable X axis double pulse detection
			(1); 		// Enable X axis single pulse detection
	I2C_WriteBuffer(MMA8452Q_SLAVE_ADDR, REG_PULSE_CFG, &reg, 1);

	MMA8452Q_GetPulseIntStatus();
	MMA8452Q_GetInterruptSource();

	// Set the data rate and put the accelerometer into active mode
	reg = 0x00 |
			(1 << 6) |	// Autosleep data rate = 12.5Hz
			//(5 << 3) |	// Output data rate = 12.5Hz
            //(4 << 3) |	// Output data rate = 50Hz
            (3 << 3) |	// Output data rate = 100Hz
			(1 << 2) |	// Low noise mode enabled
			(0 << 1) |	// Normal read mode
			(1);		// Active mode
	I2C_WriteBuffer(MMA8452Q_SLAVE_ADDR, REG_CTRL_REG1, &reg, 1);
    
    // check by reading it back
    reg = 0;
    I2C_ReadBuffer(MMA8452Q_SLAVE_ADDR, REG_CTRL_REG1, &reg, 1);
}

void MMA8452Q_ReadAll(void)
{
	uint8_t xyz_data[6] = {0};
	int16_t x_data = 0, y_data = 0, z_data = 0;

	I2C_ReadBuffer(MMA8452Q_SLAVE_ADDR, REG_OUT_X_MSB, xyz_data, 6);
	x_data = (((int16_t) xyz_data[0]) << 4) | (((int16_t) (xyz_data[1] & 0xF0)) >> 4);
	y_data = (((int16_t) xyz_data[2]) << 4) | (((int16_t) (xyz_data[3] & 0xF0)) >> 4);
	z_data = (((int16_t) xyz_data[4]) << 4) | (((int16_t) (xyz_data[5] & 0xF0)) >> 4);

	// Get the sign right
	if (xyz_data[0] > 0x7F)
		x_data -= 0x1000;

	if (xyz_data[2] > 0x7F)
		y_data -= 0x1000;

	if (xyz_data[4] > 0x7F)
		z_data -= 0x1000;

	// Adjust data for the project
	//x_data = x_data * REG_TO_READ_NUM / REG_TO_READ_DEN;
	//y_data = y_data * REG_TO_READ_NUM / REG_TO_READ_DEN;
	//z_data = z_data * REG_TO_READ_NUM / REG_TO_READ_DEN;

	// Positive board x-axis = x-axis
	// Positive board y-axis = y-axis
    // Positive board z-axis = z-axis
	x_axis_data = x_data;
	y_axis_data = y_data;
	z_axis_data = z_data;

	// Update maximums
	if (abs(x_axis_data) > max_X_value)
		max_X_value = x_axis_data;

	if (abs(y_axis_data) > max_Y_value)
		max_Y_value = y_axis_data;

	if (abs(z_axis_data) > max_Z_value)
		max_Z_value = z_axis_data;
}

int16_t MMA8452Q_GetXData(void)
{
	return x_axis_data;
}

int16_t MMA8452Q_GetYData(void)
{
	return y_axis_data;
}

int16_t MMA8452Q_GetZData(void)
{
	return z_axis_data;
}

int16_t MMA8452Q_GetMaxX(void)
{
	return max_X_value;
}

int16_t MMA8452Q_GetMaxY(void)
{
	return max_Y_value;
}

int16_t MMA8452Q_GetMaxZ(void)
{
	return max_Z_value;
}

void MMA8452Q_SetXAlarm(uint16_t alarm_val)
{
	uint8_t reg;

	reg = (uint8_t) (alarm_val * PULSE_THS_CONV_NUM / PULSE_THS_CONV_DEN);
	I2C_WriteBuffer(MMA8452Q_SLAVE_ADDR, REG_PULSE_THSX, &reg, 1);
}

void MMA8452Q_SetYAlarm(uint16_t alarm_val)
{
	uint8_t reg;

	reg = (uint8_t) (alarm_val * PULSE_THS_CONV_NUM / PULSE_THS_CONV_DEN);
	I2C_WriteBuffer(MMA8452Q_SLAVE_ADDR, REG_PULSE_THSY, &reg, 1);
}

void MMA8452Q_SetZAlarm(uint16_t alarm_val)
{
	uint8_t reg;

	reg = (uint8_t) (alarm_val * PULSE_THS_CONV_NUM / PULSE_THS_CONV_DEN);
	I2C_WriteBuffer(MMA8452Q_SLAVE_ADDR, REG_PULSE_THSZ, &reg, 1);
}

void MMA8452Q_ResetMax(void)
{
	max_X_value = 0;
	max_Y_value = 0;
	max_Z_value = 0;
}

uint8_t MMA8452Q_GetPulseIntStatus(void)
{
	uint8_t reg;

	I2C_ReadBuffer(MMA8452Q_SLAVE_ADDR, REG_PULSE_SRC, &reg, 1);

	return reg;
}

uint8_t MMA8452Q_GetInterruptSource(void)
{
	uint8_t reg;

	I2C_ReadBuffer(MMA8452Q_SLAVE_ADDR, REG_INT_SOURCE, &reg, 1);

	return reg;
}

void MMA8452Q_Realign(void)
{
	// Used to realign board to 0g positions
	uint8_t xyz_data[6] = {0}, status = 0, config_reg5, config_reg4, reg;
	int16_t x_data = 0, y_data = 0, z_data = 0;
	int8_t x_offset = 0, y_offset = 0, z_offset = 0;   
    
    // Go into standby    
	I2C_ReadBuffer(MMA8452Q_SLAVE_ADDR, REG_CTRL_REG1, &status, 1);
	status &= (0xFE);
	I2C_WriteBuffer(MMA8452Q_SLAVE_ADDR, REG_CTRL_REG1, &status, 1);
    
    // Disable interrupts, clear interrupt source registers
    I2C_ReadBuffer(MMA8452Q_SLAVE_ADDR, REG_CTRL_REG4, &config_reg4, 1);
    reg = 0;
    I2C_WriteBuffer(MMA8452Q_SLAVE_ADDR, REG_CTRL_REG4, &reg, 1);
    MMA8452Q_GetInterruptSource();
    
    // Go back into active mode
    status |= (0x1);
	I2C_WriteBuffer(MMA8452Q_SLAVE_ADDR, REG_CTRL_REG1, &status, 1);
    
    // Wait for a conversion to complete
	I2C_ReadBuffer(MMA8452Q_SLAVE_ADDR, REG_STATUS, &status, 1);
	while ((status & 0x7) != 0x7)
	{
		I2C_ReadBuffer(MMA8452Q_SLAVE_ADDR, REG_STATUS, &status, 1);
	}

	// Assume the board is completely static.
	// 1 count from the output data register
	// corresponds to 1 count in the calibration
	// register (in 4g mode)
	I2C_ReadBuffer(MMA8452Q_SLAVE_ADDR, REG_OUT_X_MSB, xyz_data, 6);
	x_data = (((int16_t) xyz_data[0]) << 4) | (((int16_t) (xyz_data[1] & 0xF0)) >> 4);
	y_data = (((int16_t) xyz_data[2]) << 4) | (((int16_t) (xyz_data[3] & 0xF0)) >> 4);
	z_data = (((int16_t) xyz_data[4]) << 4) | (((int16_t) (xyz_data[5] & 0xF0)) >> 4);

	// Get the sign right
	if (xyz_data[0] > 0x7F)
		x_data -= 0x1000;

	if (xyz_data[2] > 0x7F)
		y_data -= 0x1000;

	if (xyz_data[4] > 0x7F)
		z_data -= 0x1000;

	// Switch back to standby
	I2C_ReadBuffer(MMA8452Q_SLAVE_ADDR, REG_CTRL_REG1, &status, 1);
	status &= (0xFE);
	I2C_WriteBuffer(MMA8452Q_SLAVE_ADDR, REG_CTRL_REG1, &status, 1);
   
    // Read the current offsets
    I2C_ReadBuffer(MMA8452Q_SLAVE_ADDR, REG_OFF_X, (uint8_t *) &x_offset, 1);
    I2C_ReadBuffer(MMA8452Q_SLAVE_ADDR, REG_OFF_Y, (uint8_t *) &y_offset, 1);    
    I2C_ReadBuffer(MMA8452Q_SLAVE_ADDR, REG_OFF_Z, (uint8_t *) &z_offset, 1);
    
    // Add the new offsets
    x_offset += (x_data * -1);
    y_offset += (y_data * -1);
    z_offset += (Z_OFFSET_1G - z_data);
    
	// Write the offset to the offset registers	
	I2C_WriteBuffer(MMA8452Q_SLAVE_ADDR, REG_OFF_X, (uint8_t *) &x_offset, 1);	
	I2C_WriteBuffer(MMA8452Q_SLAVE_ADDR, REG_OFF_Y, (uint8_t *) &y_offset, 1);	
	I2C_WriteBuffer(MMA8452Q_SLAVE_ADDR, REG_OFF_Z, (uint8_t *) &z_offset, 1);
    
    // Restore interrupts
    I2C_WriteBuffer(MMA8452Q_SLAVE_ADDR, REG_CTRL_REG4, &config_reg4, 1);

	// Back to active mode
	status |= (0x1);
	I2C_WriteBuffer(MMA8452Q_SLAVE_ADDR, REG_CTRL_REG1, &status, 1);

	// Store the new offsets into EEPROM
	EEPROM_Write_XAxisOffset(x_offset);
	EEPROM_Write_YAxisOffset(y_offset);
	EEPROM_Write_ZAxisOffset(z_offset);
}

void MMA8452Q_Enable_Interrupts(bool enable)
{
	if (enable)
	{
		// Clear any pending interrupts first
		MMA8452Q_GetInterruptSource();
		MMA8452Q_GetPulseIntStatus();

		isr_INT1_Enable();
        isr_INT2_Enable();
	}
	else
	{
		isr_INT1_Disable();
        isr_INT2_Disable();		
	}
}

void MMA8452Q_INT1_Handler(void)
{
    int16_t x_data_filt, y_data_filt, z_data_filt, x_data, y_data, z_data, 
        x_gd_accel, y_gd_accel, z_gd_accel, gd_max_accel;    
    uint8_t status;
    char display_str[16] = {0};
    double x_inc_angle, y_inc_angle, z_inc_angle;
    
    static uint32_t step_detect_time;
    static int16_t x_rest_val, y_rest_val, z_rest_val;
    static bool step_detected = false;
    static Orientation_t orientation;
    
    status = MMA8452Q_GetInterruptSource();
    
    MMA8452Q_ReadAll();
    
    // Pass the results through the five point triple smooth filter
    x_data_filt = fivePtTripleSmooth_Process(&x_filter, x_axis_data);
    y_data_filt = fivePtTripleSmooth_Process(&y_filter, y_axis_data);
    z_data_filt = fivePtTripleSmooth_Process(&z_filter, z_axis_data);    
    //x_data_filt = Hanning_Process(&xh_filter, x_axis_data);
    //y_data_filt = Hanning_Process(&yh_filter, y_axis_data);
    //z_data_filt = Hanning_Process(&zh_filter, z_axis_data);
    //x_data_filt = x_axis_data;
    //y_data_filt = y_axis_data;
    //z_data_filt = z_axis_data;
    
    if (at_rest)
    {
        x_rest_val = x_data_filt;
        y_rest_val = y_data_filt;
        z_rest_val = z_data_filt;                
    }       
    
    // Calculate the gravity-direction accleration
    x_gd_accel = x_data_filt * x_rest_val / REG_1G_COUNTS_INT;
    y_gd_accel = y_data_filt * y_rest_val / REG_1G_COUNTS_INT;
    z_gd_accel = z_data_filt * z_rest_val / REG_1G_COUNTS_INT;
    
    if (at_rest)
    {
        // The largest of the gravity-directions determines the orientation of the
        // device
        gd_max_accel = x_gd_accel;
        orientation = X_IS_GRAVITY;
        if (gd_max_accel < y_gd_accel)
        {
            gd_max_accel = y_gd_accel;   
            orientation = Y_IS_GRAVITY;
        }
        if (gd_max_accel < z_gd_accel)
        {
            gd_max_accel = z_gd_accel;   
            orientation = Z_IS_GRAVITY;
        }
        
        if (abs(gd_max_accel - REG_1G_COUNTS_INT) > DETECTION_THRESHOLD)
        {            
            at_rest = false;   
            step_detect_time = Tick_Get_Ticks();
        }
    }
    else
    {
        if ((orientation == X_IS_GRAVITY && 
            abs(gd_max_accel - x_gd_accel) > DETECTION_THRESHOLD) ||
            (orientation == Y_IS_GRAVITY && 
            abs(gd_max_accel - y_gd_accel) > DETECTION_THRESHOLD) ||
            (orientation == Z_IS_GRAVITY && 
            abs(gd_max_accel - z_gd_accel) > DETECTION_THRESHOLD))
        {
            Increment_Step_Count();
            //at_rest = true;      
            
            // Calculate new gravity directions
            gd_max_accel = x_gd_accel;
            orientation = X_IS_GRAVITY;
            if (gd_max_accel < y_gd_accel)
            {
                gd_max_accel = y_gd_accel;   
                orientation = Y_IS_GRAVITY;
            }
            if (gd_max_accel < z_gd_accel)
            {
                gd_max_accel = z_gd_accel;   
                orientation = Z_IS_GRAVITY;
            }
        }
        else
        {
            // No second step was detected. If no motion is detected after 500ms,
            // the device is at reset
            if (Tick_Get_Time_Diff(step_detect_time) > 500)
            {
                at_rest = true;               
            }
        }
    }
    
    // Adjust the data to readable values
    x_data = x_data_filt * REG_TO_READ_NUM / REG_TO_READ_DEN;
    y_data = y_data_filt * REG_TO_READ_NUM / REG_TO_READ_DEN;
    z_data = z_data_filt * REG_TO_READ_NUM / REG_TO_READ_DEN;
    
    memset(display_str, ' ', 16);
    LCD_Position(0,0);
    LCD_PrintString(display_str);
    sprintf(display_str, "X:%d.%02d Y:%d.%02d", x_data / 100, 
        abs(x_data % 100), y_data / 100, abs(y_data % 100));
    LCD_Position(0,0);
    LCD_PrintString(display_str);
    
    memset(display_str, ' ', 16);
    LCD_Position(1,0);
    LCD_PrintString(display_str);
    sprintf(display_str, "Z:%d.%02d STEP: %04d", z_data / 100, 
        abs(z_data % 100), Get_Step_Count());
    LCD_Position(1,0);
    LCD_PrintString(display_str);
}

void MMA8452Q_INT2_Handler(void)
{
	uint8_t reg = 0, tx_buf[40];
	uint32_t tx_size;

	// Pulse detected
	// Read (and clear) the pulse source register
	/*reg = MMA8452Q_GetPulseIntStatus();

	if (reg & PULSE_SRC_AXZ)
	{
		Increment_Step_Count();
	}

	if (reg & PULSE_SRC_AXY)
	{
		Increment_Step_Count();
	}

	if (reg & PULSE_SRC_AXX)
	{
		Increment_Step_Count();
	}*/

//	LEUART_TX_Buffer();
}

