/******************************************************************************
* File Name: initialize.c
* Version 2.0
*
* Description:
* This file contains the code to calibrate the Thermistor. There are two types 
* of calibration. The first one calibrates the reference resistor. This is done
* by applying a known stable resistor ~10k to the RTD inputs. The UI will ask
* you to specificy the exact resistance of this ~10k resistor. The proejct will
* then measure the resistor and based off the measurement and what you entered
* it will calculate the correct value for the reference resistor
*
* The second calibration calibrates out any temperature offset in the thermistor
* Again the UI will ask you for the precise temperature, you will enter this 
* temperature in. The project will then measure the system and determine the 
* temperature offset.
*
* Both the temperature offset and the real reference resistor values are stored 
* in EEPROM. They are then retrieved and used in the main routine.
*
* Note:
*
********************************************************************************
* Copyright (2013), Cypress Semiconductor Corporation. All Rights Reserved.
********************************************************************************
* This software is owned by Cypress Semiconductor Corporation (Cypress)
* and is protected by and subject to worldwide patent protection (United
* States and foreign), United States copyright laws and international treaty
* provisions. Cypress hereby grants to licensee a personal, non-exclusive,
* non-transferable license to copy, use, modify, create derivative works of,
* and compile the Cypress Source Code and derivative works for the sole
* purpose of creating custom software in support of licensee product to be
* used only in conjunction with a Cypress integrated circuit as specified in
* the applicable agreement. Any reproduction, modification, translation,
* compilation, or representation of this software except as specified above 
* is prohibited without the express written permission of Cypress.
*
* Disclaimer: CYPRESS MAKES NO WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, WITH 
* REGARD TO THIS MATERIAL, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
* Cypress reserves the right to make changes without further notice to the 
* materials described herein. Cypress does not assume any liability arising out 
* of the application or use of any product or circuit described herein. Cypress 
* does not authorize its products for use as critical components in life-support 
* systems where a malfunction or failure may reasonably be expected to result in 
* significant injury to the user. The inclusion of Cypress' product in a life-
* support systems application implies that the manufacturer assumes all risk of 
* such use and in doing so indemnifies Cypress against all charges. 
*
* Use of this Software may be limited by and subject to the applicable Cypress
* software license agreement. 
*******************************************************************************/

#include "calibration.h"
#include "device.h"
#include "measurement.h"
#include "removeOffsetNoise.h"
#include <stdio.h>

/* Flag to indicated when SW2 has been pressed*/
extern volatile uint8 CalSelectFlag;

/* Calibration Data in EEPROM*/
extern uint32 eepromRefRes;
extern int32 eepromTempOffset;


/* Variable used to store the user input */
static float UISetting;

/* flag to indicate if display needs to be updated with new instructions*/
static uint8 displayChangeFlag = 0;

/* Variable to store coarse UI setting */
static int16 UICoarseSetting = 0;

/*******************************************************************************
* Function Name:  Calibrate
********************************************************************************
* Summary:
* This function contains the code to calibrate the Thermistor. There are two types 
* of calibration. The first one calibrates the reference resistor. This is done
* by applying a known stable resistor ~10k to the RTD inputs. The UI will ask
* you to specificy the exact resistance of this ~10k resistor. The proejct will
* then measure the resistor and based off the measurement and what you entered
* it will calculate the correct value for the reference resistor
*
* The second calibration calibrates out any temperature offset in the thermistor
* Again the UI will ask you for the precise temperature, you will enter this 
* temperature in. The project will then measure the system and determine the 
* temperature offset.
*
* Parameters:  
* void
* 
* Return: 
* void
*
* Theory: 
* None
* 
* Side Effects:
* None 
*
*******************************************************************************/
void Calibrate(void)
{
	/* Variables for Thermistor resistance and Reference Resistor Voltage*/
	int32 thermRes, vRef, vTherm, refRes, thermTemp, thermOffset;
	
	/* Gain correction scale factor*/
	float thermScaleFactor;
	
	/* Counter variable for 'for loop' */ 
	uint8 count;
	
	/* Timeout timer*/
	uint32 timeout = 0;
	
	/* Flag is set if calibration data is written properly to EEPROM*/
	uint8 eepromFlag = 0;
	
	/* Returns the status of EEPROM writes */
	cystatus dataWriteStatus,flagWriteStatus;	
	
	/* LCD display string */
	char displayStr[16] = {'\0'};
	
	/* Directions for user to start calibration */
	LCD_Position(0,0);
	LCD_PrintString((char8 *)"Cal RefRes?     ");
	
	LCD_Position(1,0);
	LCD_PrintString((char8 *)"Y-> SW2, N->Wait");
	
	
	/* Give the user time to press SW2 If the user doesn't press SW2 
	 within TIMEOUT, skip Ref Resistor calibration */
	while ((CalSelectFlag == 0) && (timeout < TIMEOUT))
	{
		timeout++;
	}
	timeout = 0;
	
	/* CalSelectFlag flag is initially set to zero. It turns 1 when the user 
	presses SW2 */
	if(CalSelectFlag == 1)
	{
		/* Ask the user the value of the resistance they have applied*/
		LCD_Position(0,0);
		LCD_PrintString((char8 *)"Set Res:  ");
		LCD_Position(1,0);
		LCD_PrintString((char8 *)"Use Capsense    ");
		CalSelectFlag = 0;
		
		/* initally measure the resistance to help the user out*/
		/* Measure Voltage Across Thermistor*/
    	vTherm = MeasureResistorVoltage(AMUX_1_VT); 
		/* Measure Voltage Across Reference Resistor*/
    	vRef = MeasureResistorVoltage(AMUX_1_VREF);
		/*Calculate the resistance of the Thermistor*/
		thermRes = ((float)vTherm / vRef) * REFERENCE_RESISTOR;
		
		/*Use UI menu to select resistance*/
    	UICoarseSetting = thermRes;

		/* Blocking statement to wait until user selects resistance */
		/* User selects resistance using Capsense Slider and Buttons*/
		while(CalSelectFlag == 0)
		{
			/* If user has used CapSense tell them how to move on*/
			if (displayChangeFlag)
			{
				LCD_Position(1,0);
				LCD_PrintString((char8 *)"SW2 ->Next     ");
			}
			
			/* Call the routine to read CapSense Slider and Buttons*/
			UserInput();
			
			/* Display new resistance value*/
			sprintf(displayStr, "%4.1f  ", UISetting);
			LCD_Position(0u, 10);
			LCD_PrintString(displayStr);
		}
		
		/* Resistance value has been entered. Now measure the Therm resistance*/
		LCD_Position(0u, 0u);
		LCD_PrintString((char8 *)"Calibrating ... ");
		
		LCD_Position(1u, 0u);
		LCD_PrintString((char8 *)"Wait for few sec");
		
		/* Take 64 readings to get get best filtered result*/
		for(count = 0; count < 64; count++)
		{
			/* Measure Voltage Across Thermistor*/
    		vTherm = MeasureResistorVoltage(AMUX_1_VT); 
			/* Measure Voltage Across Reference Resistor*/
    		vRef = MeasureResistorVoltage(AMUX_1_VREF);
		}
		
		/*Calculate Resistance of thermistor*/
		thermRes = ((float)vTherm / vRef) * REFERENCE_RESISTOR * RESISTANCE_SCALING;
		
		/* Calculate ratio of measured resistance to actual resistance*/
		thermScaleFactor = ( UISetting * RESISTANCE_SCALING / thermRes );
		
		/*Based on this ratio calcuate the real value of the ref resistor */
		refRes = thermScaleFactor * REFERENCE_RESISTOR;
		
		/* Display refResisor value*/
		LCD_Position(0u, 0u);
		LCD_PrintString((char8 *)"RefRes =  ");
		sprintf(displayStr, "%ld   ", refRes);
	    LCD_Position(0,9);
		LCD_PrintString(displayStr);

		/* Tell user to press SW2 to save refRes value to EEPROM*/
		LCD_Position(1,0);
		LCD_PrintString((char8 *)"EERPOM -> SW2   ");
		CalSelectFlag = 0;
		
		/* Blocking statement to wait until the user sees the calib constants */
		while(CalSelectFlag == 0);
		dataWriteStatus = CYRET_BAD_PARAM;
		/* Write refRes value to EERPROM*/
		dataWriteStatus = EEPROM_Write((uint8 *)&refRes, 0);
		flagWriteStatus = CYRET_BAD_PARAM;
		
		/* If refRes is written into EEPROM successfully, set eepromFlag to 1 */
		if (dataWriteStatus == CYRET_SUCCESS)
		{
			eepromFlag = 1;
			/* set flag to indciated refRes value is valid*/
			flagWriteStatus = EEPROM_Write((uint8 *)&eepromFlag, 1);
		}
		
		/*if flag was written succesfully to EEPROM tell user*/
		if (flagWriteStatus == CYRET_SUCCESS)
		{
			LCD_Position(0,0);
			LCD_PrintString((char8 *)"EEPROM Success  ");
			LCD_Position(1,0);
			LCD_PrintString((char8 *)"Continue -> SW2 ");
			CalSelectFlag = 0;
		}
		
		/*if Flag was not written succesfully, or calibration not wirtten 
			sucessfully tell user*/
		else
		{
			LCD_Position(0,0);
			LCD_PrintString((char8 *)"EEPROM Failure  ");
			LCD_Position(1,0);
			LCD_PrintString((char8 *)"Continue -> SW2 ");
			CalSelectFlag = 0;
		}
		/* Blocking statement to wait until the user chooses to continue */
		while(CalSelectFlag == 0);
		
	}

	/* Directions for user to start temperature calibration */
	LCD_Position(0,0);
	LCD_PrintString((char8 *)"Cal Temp?       ");
	
	LCD_Position(1,0);
	LCD_PrintString((char8 *)"Y-> SW2, N->Wait");
	CalSelectFlag = 0;
	
	/* Give the user time to press SW2 If the user doesn't press calibrate 
	button within TIMEOUT, skip calibration */
	while ((CalSelectFlag == 0) && (timeout < TIMEOUT))
	{
		timeout++;
	}
	timeout = 0;
	
	if(CalSelectFlag == 1)
	{

		/* Ask the user the value of the temperature they have applied*/
		LCD_Position(0,0);
		LCD_PrintString((char8 *)"Set Temp:  ");
		LCD_Position(1,0);
		LCD_PrintString((char8 *)"Use Capsense    ");
		CalSelectFlag = 0;
		
		/*Check Calibration data to see if there is a refResistor cal*/
		ReadCalibrationData();
		
		/* initally measure the temperature to help the user out*/
		/* Measure Voltage Across Thermistor*/
    	vTherm = MeasureResistorVoltage(AMUX_1_VT); 
		/* Measure Voltage Across Reference Resistor*/
    	vRef = MeasureResistorVoltage(AMUX_1_VREF);
		/*Calculate the resistance of the Thermistor*/
		thermRes = ((float)vTherm / vRef) * eepromRefRes;
		
		/*Use Temperatrue Selection menu to select resistance*/
    	UICoarseSetting = Thermistor_GetTemperature(thermRes)/100;
		
		displayChangeFlag = FALSE;

		/* Blocking statement to wait until user selects temperature */
		/* User selects temperature using Capsense Slider and Buttons*/
		while(CalSelectFlag == 0)
		{
			/* If user has used CapSense tell them how to move on*/
			if (displayChangeFlag)
			{
				LCD_Position(1,0);
				LCD_PrintString((char8 *)"SW2 ->Next      ");
			}
			
			/* Call the routine to read CapSense Slider and Buttons*/
			UserInput();
			
			/* Display new temperature value*/
			sprintf(displayStr, "%4.1f  ", UISetting);
			LCD_Position(0u, 12);
			LCD_PrintString(displayStr);
		}
		
		/* Temperature value has been entered. Now measure the Therm temp*/
		LCD_Position(0u, 0u);
		LCD_PrintString((char8 *)"Calibrating ... ");
		
		LCD_Position(1u, 0u);
		LCD_PrintString((char8 *)"Wait for few sec");
		
		/* Take 64 readings to get get best filtered result*/
		for(count = 0; count < 64; count++)
		{
			/* Measure Voltage Across Thermistor*/
    		vTherm = MeasureResistorVoltage(AMUX_1_VT); 
			/* Measure Voltage Across Reference Resistor*/
    		vRef = MeasureResistorVoltage(AMUX_1_VREF);
		}
		
		/*Calculate Resistance of thermistor*/
		thermRes = ((float)vTherm / vRef) * eepromRefRes;
		
		/* Calculate Temperature of thermistor*/
		thermTemp = Thermistor_GetTemperature(thermRes);
		
		/*Calculate the temperature offset */
		/*Scale User input upt to 100ths of a degree C*/
		thermOffset = thermTemp - (UISetting * 100);
		
		/* Display Temperature offset*/
		LCD_Position(0u, 0u);
		LCD_PrintString((char8 *)"TempOffset=");
		sprintf(displayStr, "%2.2f   ", thermOffset/100.0);
	    LCD_Position(0,11);
		LCD_PrintString(displayStr);

		/* Tell user to press SW2 to save refRes value to EEPROM*/
		LCD_Position(1,0);
		LCD_PrintString((char8 *)"EERPOM -> SW2   ");
		CalSelectFlag = 0;
		
		/* Blocking statement to wait until the user sees the calib constants */
		while(CalSelectFlag == 0);
		dataWriteStatus = CYRET_BAD_PARAM;
		/* Write refRes value to EERPROM*/
		dataWriteStatus = EEPROM_Write((uint8 *)&thermOffset, 2);
		flagWriteStatus = CYRET_BAD_PARAM;
		
		/* If refRes is written into EEPROM successfully, set eepromFlag to 1 */
		if (dataWriteStatus == CYRET_SUCCESS)
		{
			eepromFlag = 1;
			/* set flag to indciated refRes value is valid*/
			flagWriteStatus = EEPROM_Write((uint8 *)&eepromFlag, 3);
		}
		
		/*if flag was written succesfully to EEPROM tell user*/
		if (flagWriteStatus == CYRET_SUCCESS)
		{
			LCD_Position(0,0);
			LCD_PrintString((char8 *)"EEPROM Success  ");
			LCD_Position(1,0);
			LCD_PrintString((char8 *)"Continue -> SW2 ");
			CalSelectFlag = 0;
		}
		
		/*if Flag was not written succesfully, or calibration not wirtten 
			sucessfully tell user*/
		else
		{
			LCD_Position(0,0);
			LCD_PrintString((char8 *)"EEPROM Failure  ");
			LCD_Position(1,0);
			LCD_PrintString((char8 *)"Continue -> SW2 ");
			CalSelectFlag = 0;
		}
		/* Blocking statement to wait until the user chooses to continue */
		while(CalSelectFlag == 0);
		CalSelectFlag = 0;
	}
	
	/* Get the LCD display ready to display Therm temperature */
	LCD_ClearDisplay();
}


/*******************************************************************************
* Function Name:  ReadCalibrationData
********************************************************************************
* Summary:
* Read out Calibration constants
*
* Parameters:  
* void
* 
* Return: 
* void 
*
* Theory: 
* None
* 
* Side Effects:
* None 
*
*******************************************************************************/
void ReadCalibrationData(void)
{
	reg32 * refResPointer;
	reg32 * tempOffsetPointer;
	
	uint8 refResCalFlag;
	uint8 tempOffsetCalFlag;

	refResPointer = 	(reg32 *)(CYDEV_EE_BASE + ROW_ZERO);
	tempOffsetPointer = 	(reg32 *)(CYDEV_EE_BASE + ROW_TWO);
	
	/*Check to see if calibration data in EERPROM is valid*/
	refResCalFlag = CY_GET_REG8(CYDEV_EE_BASE + ROW_ONE);
	
	/* if reference resistor cal is valid, grab it*/
	if (refResCalFlag == 1)
	{
		eepromRefRes = *((int32*)(refResPointer));
	}
	
	/*Check to see if calibration data in EERPROM is valid*/
	tempOffsetCalFlag = CY_GET_REG8(CYDEV_EE_BASE + ROW_THREE);
	if (tempOffsetCalFlag == 1)
	{
		eepromTempOffset = *((int32*)(tempOffsetPointer));
	}
	
	
}
/*******************************************************************************
* Function Name: UserInput
********************************************************************************
* Summary:
*  Function sets a value based on the current button and slider positions
*  Capsense slider centroid is used for settings in steps of 1 
*  and capsense buttons is used for a fine setting in steps of 0.1 
*  The input is limited from 0 to 120000.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void UserInput(void)
{
    /* variable for fine adjustments based on capsense buttons 
	 * This variable will be incremented/decremented in steps of 0.1 on each button press*/ 
	static float UISettingFine = 0;
	
	/* Variable to store old capsense slider positions for temperature setting */
	static int16 oldPos = 0;
	
	/* Variable to keep track of button last state */
    static uint8 prevUpBtnState = 0;
    static uint8 prevDownBtnState = 0;
    
	/* Variables for button state */
	static uint8 upBtnState;
	static uint8 downBtnState;
	
	static uint8 sliderActive = FALSE;
	/* Variable to store the current slider location */
	uint16 currentSliderVal;
	
    /* Wait until scanning is in progress then update the baseline */
    while(CapSense_IsBusy());
    
    /* Update baseline for all the sensors */
    CapSense_UpdateEnabledBaselines();
	
	/* Start scanning CapSense buttons */
    CapSense_ScanEnabledWidgets();

	/* Find Slider Position */
    currentSliderVal = CapSense_GetCentroidPos(CapSense_LINEARSLIDER0__LS);    
    
    /* If the slider is not active reset the capsense position values 
	 * Reset the sliderActive flag */
    if(currentSliderVal == 0xFFFFu)
    {
		currentSliderVal = 0;
		oldPos = 0;
		sliderActive = FALSE;
    }
	/* If slider is active for the first time 
	 * Set store the current slider value in oldPos variable and 
	 * set the flag to indicate that the slider is active */
	else if (sliderActive == FALSE)
	{
		oldPos = currentSliderVal;
		sliderActive = TRUE;
		
		/* Reset the fine setting  if the slider is active */
		UISettingFine = 0;
		
		/* Reset the statemachine changed flag so that display messages are changed */
		displayChangeFlag = TRUE;

	}
    /* If slider is active and the position has changed */
    else
    {
		/* Add 10% of slider position difference onto the UICoarseSetting
		 * This is to get better resolution for coarse  setting
		 */
        UICoarseSetting += (((int16)currentSliderVal - (int16)oldPos) * 0.1);

		/* If the movement of slider has resulted in a change in coarse setting
		 * store the current slider value as oldPos variable */
		if(UICoarseSetting != (UISetting - UISettingFine))
		{
			oldPos = currentSliderVal;
		}
    }
	
	/* Check if button widget is active */
    upBtnState = CapSense_CheckIsWidgetActive(CapSense_BUTTON0__BTN);
    downBtnState = CapSense_CheckIsWidgetActive(CapSense_BUTTON1__BTN);

    /* If both the buttons are active, then no change*/
    if(upBtnState && downBtnState)
    {
        /* No Action */
    }    
	/* If up button is active increment value 0.1°C*/
	else if(upBtnState && (prevUpBtnState == FALSE))
	{
		UISettingFine += 0.1;
		displayChangeFlag = TRUE;
	}
	/* If up button is active decrement value by 0.1°C*/
	else if(downBtnState && (prevDownBtnState == 0))
	{
		UISettingFine -= 0.1;
		displayChangeFlag = TRUE;
	}
	else
	{
		/* Do nothing */
	}
	
	/* UISetting is calculated as the sum of coarse and fine temp settings */
	UISetting = UISettingFine + UICoarseSetting;
	
	/* Limit the value to 0 to 120000 range */
	if(UISetting > 12000)
	{
		UISetting = 12000;
		UICoarseSetting = 12000;
		UISettingFine = 0.0;

	}
	else if(UISetting < 0)
	{
		UISetting = 0;
		UICoarseSetting = 0;
		UISettingFine = 0.0;
	}
	
	/* Store the button state */
    prevUpBtnState = upBtnState;
    prevDownBtnState = downBtnState;
	
}
/* [] END OF FILE */
