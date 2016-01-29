/*******************************************************************************
* File Name: Thermistor_1.h
* Version 1.0
*
* Description:
*  This header file provides registers and constants associated with the
*  ThermistorCalc component.
*
* Note:
*  None.
*
********************************************************************************
* Copyright 2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_THERMISTOR_CALC_Thermistor_1_H)
#define CY_THERMISTOR_CALC_Thermistor_1_H

#include <cyfitter.h>
#include "CyLib.h"

/***************************************
*   Conditional Compilation Parameters
***************************************/

#define Thermistor_1_IMPLEMENTATION         (0u)
#define Thermistor_1_REF_RESISTOR           (10000u)
#define Thermistor_1_REF_RES_SHIFT          (0u)
#define Thermistor_1_ACCURACY               (10u)
#define Thermistor_1_MIN_TEMP               (-40 * Thermistor_1_SCALE)


/***************************************
*        Function Prototypes
***************************************/

uint32 Thermistor_1_GetResistance(int16 Vreference, int16 VThermistor) 
                                      ;
int16 Thermistor_1_GetTemperature(uint32 ResT) ;


/***************************************
*    Enumerated Types and Parameters
***************************************/

/* Enumerated Types ImplementationType, Used in parameter Implementation */

#define Thermistor_1__LUT 1
#define Thermistor_1__EQUATION 0



/***************************************
*           API Constants
***************************************/

#define Thermistor_1_K2C                    (273.15)
#define Thermistor_1_SCALE                  (100)


#define Thermistor_1_THA               (0.0008613933)
#define Thermistor_1_THB               (0.0002563769)
#define Thermistor_1_THC               (1.680552E-07)


#endif /* CY_THERMISTOR_CALC_Thermistor_1_H */

/* [] END OF FILE */
