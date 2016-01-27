/*****************************************************************************
* File Name: main.c
*
* Version: 2.0
*
* Description:
*   The main C file for the Temperature measurement with Thermistor project. 
* 
* Note:
* 	The main project includes the ADC and other components required for the
*   temperature measurement. The Thermistor component is a complete firmware component
*   as decribed in the component datahseet and application note
******************************************************************************
* Copyright (C) 2013, Cypress Semiconductor Corporation.
******************************************************************************
* This software is owned by Cypress Semiconductor Corporation (Cypress) and is
* protected by and subject to worldwide patent protection (United States and
* foreign), United States copyright laws and international treaty provisions.
* Cypress hereby grants to licensee a personal, non-exclusive, non-transferable
* license to copy, use, modify, create derivative works of, and compile the
* Cypress Source Code and derivative works for the sole purpose of creating
* custom software in support of licensee product to be used only in conjunction
* with a Cypress integrated circuit as specified in the applicable agreement.
* Any reproduction, modification, translation, compilation, or representation of
* this software except as specified above is prohibited without the express
* written permission of Cypress.
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
* such use and in doing so indemnifies Cypress against all charges. Use may be
* limited by and subject to the applicable Cypress software license agreement.
*****************************************************************************/
#include <device.h>
#include "removeOffsetNoise.h"
#include "measurement.h"
#include "calibration.h"
#include <stdio.h>

/*Holds calibration data*/
uint32 eepromRefRes = 10000;
int32 eepromTempOffset = 0;

/* Indicates status of SW2*/
uint8 volatile CalSelectFlag = 0;
	
void main()
{
	/* Voltages across reference resistor and thermistor*/
	int32 iVref, iVtherm;
	
	/* Resistance of Thermistor*/
	uint32 iRes;
	
	/* Temperature value in 100ths of a degree C*/
	int32 iTemp; 
	
	/* LCD display variable */
	char displayStr[16]={'\0'};

	/*Enable global interrupts*/
	CYGlobalIntEnable; 
	
	/*Start LCD before Capsense*/
	LCD_Start();
	LCD_Position(0,0);
    LCD_PrintString((char8 *)"AN66477");
	
	/* Start Capsense*/
	CapSense_Start();
    CapSense_InitializeAllBaselines();

	/* Wait until CapSense Finishes */
	while (CapSense_IsBusy());
	
	/*Start all the hardware components required*/
	isr_Calibration_Start();
	ADC_Start();
	AMux_Start();
	VDAC8_Start();
	Opamp_Start();
	EEPROM_Start();
	
	/* Run Calibration Routine */
	Calibrate();
	
	/* Read the gain and offset calibration coefficients from EEPROM */
	ReadCalibrationData();
	
	/* 	Get Display Ready*/
	LCD_Position(0,0);
    LCD_PrintString("Temp(Deg C)");

	for(;;)
    {

    	/* Measure Voltage Across Thermistor*/
    	iVtherm = MeasureResistorVoltage(THERMISTOR); 
		
		/* Measure Voltage Across Reference Resistor*/
    	iVref = MeasureResistorVoltage(REF_RES);
        
		/*Calculate the resistance of the Thermistor*/
		iRes = ((float)iVtherm / iVref) * eepromRefRes;
		/*Calculate the temperature from the Resistance*/
    	iTemp = Thermistor_GetTemperature(iRes) - eepromTempOffset ;
        
    	/*Display -- For Debug*/	
    	LCD_Position(1,0);
    	sprintf(displayStr,"%0.2f  ",(float)(iTemp/100.0));
    	LCD_Position(1,0);
    	LCD_PrintString(displayStr);
    	CyDelay(20);
    }
}

/* [] END OF FILE */
