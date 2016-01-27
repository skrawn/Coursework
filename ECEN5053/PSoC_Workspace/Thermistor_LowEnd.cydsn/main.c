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
#include <stdio.h>
	
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
	
	/*Start all the hardware components required*/
	ADC_Start();
	AMux_Start();
	LCD_Start();
	VDAC8_Start();
	Opamp_Start();
	
	/* 	Get Display Ready*/
	LCD_Position(0,0);
    LCD_PrintString("Temp(Deg C)");

	for(;;)
    {

    	/* Measure Voltage Across Thermistor*/
    	iVtherm = MeasureResistorVoltage(AMUX_1_VT); 
		
		/* Measure Voltage Across Reference Resistor*/
    	iVref = MeasureResistorVoltage(AMUX_1_VREF);

	   	/* Use the thermistor component API function call to obtain the 
		temperature corresponding to the voltages measured*/	
    	iRes = Thermistor_GetResistance((int16)iVref, (int16)iVtherm);
    	iTemp = Thermistor_GetTemperature(iRes);
        
    	/*Display -- For Debug*/	
    	LCD_Position(1,0);
    	sprintf(displayStr,"%ld    ",(iTemp/100));
    	LCD_Position(1,0);
    	LCD_PrintString(displayStr);
    	CyDelay(20);
    }
}

/* [] END OF FILE */
