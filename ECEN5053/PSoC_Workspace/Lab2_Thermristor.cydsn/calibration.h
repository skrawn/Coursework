/******************************************************************************
* File Name:calibration.h
*  Version 1.0
*
* Description:
*  This file contains defines and function prototype for calibration.c file
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
#if !defined(calibration_H)
#define calibration_H

#include "device.h"

/* Time out timer for UI */
#if(CY_PSOC3)
#define TIMEOUT								500000
#elif(CY_PSOC5A || CY_PSOC5LP)
#define TIMEOUT								4000000
#endif

/* Define Scaling Multipliers */
#define RESISTANCE_SCALING              10

/*Define Value of Reference Resistor*/
#define REFERENCE_RESISTOR 				10000

/*EEPROM ROW defines*/
#define ROW_ZERO 	0x00
#define ROW_ONE  	0x10
#define ROW_TWO  	0x20
#define ROW_THREE	0x30

/* Boolean Defines*/
#define FALSE		0x00
#define TRUE        0x01

/* Coefficients for calculating RTD resistance at a given temperature */
#define RTD_R0 100.0
#define RTD_COEFF_A 3.9083E-3
#define RTD_COEFF_B -5.775E-7
#define RTD_COEFF_C -4.183E-12

/* Function prototype for initial gain calibration */
void Calibrate(void);
/* Function prototype to read gain calibration data out of EEPROM*/
void ReadCalibrationData(void);
/* Function prototype for selecting input temperature using CapSense*/
void UserInput(void);

#endif

//[] END OF FILE
