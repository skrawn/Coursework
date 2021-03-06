/*******************************************************************************
* File Name: Thermistor_Calc_1.c
* Version 1.1
*
* Description:
*  This file provides the API source code for the Thermistor component.
*
* Note: none
*
********************************************************************************
* Copyright (2011), Cypress Semiconductor Corporation. All Rights Reserved.
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

#include "Thermistor_Calc_1.h"
#include <device.h>

/* Method of Implementation = Equation*/ 
#if(Thermistor_Calc_1_IMPL == 1) 
#include "math.h"
#endif
/******************************************************************************* 
* Function Name: Thermistor_Calc_1_GetResistance
********************************************************************************
*
* Summary: 
*  In Equation method, the Stainhart-Hart equation is used to obtain the temperature.
*  The digital values of the voltages across the reference resistor and the thermistor are passed to this function as parameters. 
*  These can be considered as the inputs to the component. The function returns (outputs) the temperature, based on the voltage values.
*
*  In LUT method, the look-up-table generated by the component customizer software is used to obtain the temperature.
*  The digital values of the voltages across the reference resistor and the thermistor are passed to this function as parameters. 
*  These can be considered as the inputs to the component. The function returns (outputs) the temperature, based on the voltage values.
*  The resistance is calculated in LUT method using long integer arthematic instead of floating point to optimize the code space. 
*  This will add some rounding errors, but avoid floating point math library. In Equation method, since the floating point library is 
*  being used for other functions, the resistance is also calculated in the floating point math.
*  
*
* Parameters:  
*  Vref (Vreference) is the voltage across the reference resistor. 
*  VT (Vthermistor) is the voltage across the thermistor.
*
* Return: 
*  The return value is the resistance. The return type is 32-bit integer as shown in the function prototype provided above.
*  Breaking the APIs into obtaining resistance and then temperature helps during calibration and if other circuits, 
*  like IDAC method are used to setup thermistor
*
* Reentrance: 
*  No
*
*******************************************************************************/
int32 Thermistor_Calc_1_GetResistance(int32 Vref, int32 VT)
{
	/* Method of Implementation = Equation*/ 
	#if(Thermistor_Calc_1_IMPL == 1) 

		/* local variables */
		uint32 iresT;

		/*Convert measured voltage to resistance*/
		iresT = Thermistor_Calc_1_REF_RES * ((float)(VT)/((float)(Vref)));

		/*Return resistance of thermistor*/
		return(iresT);
	#endif

	/* Method of Implementation = LUT*/ 
	#if (Thermistor_Calc_1_IMPL == 0)
		uint32 res_TR;

		/* Calculate thermistor resistance from the voltages */	
		res_TR = Thermistor_Calc_1_REF_RES * (long)VT;
		res_TR = res_TR + (long)Vref / 2;
		res_TR = res_TR / (long)Vref;
		return(res_TR);
	#endif
}	
/******************************************************************************* 
* Function Name: Thermistor_Calc_1_GetTemperature
********************************************************************************
* Summary: 
*  In Equation method, the Stainhart-Hart equation is used to obtain the temperature.
*  The digital values of the voltages across the reference resistor and the thermistor are passed to this function as parameters. 
*  These can be considered as the inputs to the component. The function returns (outputs) the temperature, based on the voltage values.
*
*  In LUT method, the look-up-table generated by the component customizer software is used to obtain the temperature.
*  The digital values of the voltages across the reference resistor and the thermistor are passed to this function as parameters. 
*  These can be considered as the inputs to the component. The function returns (outputs) the temperature, based on the voltage values.
*
* Parameters:  
*  resT is the resistance of the Thermistor
*
* Return: 
*  The return value is the temperature. The return type is 32-bit integer as shown in the function prototype provided above. 
*  The value returned is the scaled version of temperature. The value is scaled up by 100 to avoid use of floating point math 
*  in the calculations. For example, the return value is 2345, when the actual temperature is 23.45 
*
* Reentrance: 
*  No
*
*******************************************************************************/
int32 Thermistor_Calc_1_GetTemperature(int32 resT)
{
	/* Method of Implementation = Equation*/ 
	#if(Thermistor_Calc_1_IMPL == 1) 

		/* local variables */
		float logrT;
		int32 temp_TR;
		float st_Eqn;

		/* Calculate thermistor resistance from the voltages */		
		logrT = log(resT);

		/* Calculate temperature from the resistance of thermistor using Steinhart-Hart Equation */
		st_Eqn = Thermistor_Calc_1_THA + (Thermistor_Calc_1_THB * logrT) + (Thermistor_Calc_1_THC * pow(logrT,3.0));
		temp_TR = (int32) ((((1 / st_Eqn)) - Thermistor_Calc_1_K2C) * Thermistor_Calc_1_SCALE);

		/* Return temperature */
		return(temp_TR);
	#endif

	/* Method of Implementation = LUT*/ 
	#if (Thermistor_Calc_1_IMPL == 0)	

		/* local variables */
		CYCODE const uint32 Thermistor_Calc_1_LUTARR[Thermistor_Calc_1_LUTSIZE] = Thermistor_Calc_1_LUT;
		uint32 res_TR;
		int32 temp_TR;
		uint16 loop;
		uint8 accuracy;
		res_TR = resT;
		accuracy = Thermistor_Calc_1_ACCURACY * Thermistor_Calc_1_SCALE;

		/* Calculate temperature from the resistance of thermistor by using the LUT */			
		for(loop = 0; loop < Thermistor_Calc_1_LUTSIZE; loop++)
		{
			if(res_TR > Thermistor_Calc_1_LUTARR[loop])
			{
				break;
			}
		}
		temp_TR = Thermistor_Calc_1_MIN_TEMP * Thermistor_Calc_1_SCALE + (loop * accuracy);

		/* Return temperature */
		return(temp_TR);
	#endif
}

/* [] END OF FILE */
