/*******************************************************************************
* File Name: isr_Calibration.h
* Version 1.70
*
*  Description:
*   Provides the function definitions for the Interrupt Controller.
*
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_ISR_isr_Calibration_H)
#define CY_ISR_isr_Calibration_H

#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void isr_Calibration_Start(void) ;
void isr_Calibration_StartEx(cyisraddress address) ;
void isr_Calibration_Stop(void) ;

CY_ISR_PROTO(isr_Calibration_Interrupt);

void isr_Calibration_SetVector(cyisraddress address) ;
cyisraddress isr_Calibration_GetVector(void) ;

void isr_Calibration_SetPriority(uint8 priority) ;
uint8 isr_Calibration_GetPriority(void) ;

void isr_Calibration_Enable(void) ;
uint8 isr_Calibration_GetState(void) ;
void isr_Calibration_Disable(void) ;

void isr_Calibration_SetPending(void) ;
void isr_Calibration_ClearPending(void) ;


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the isr_Calibration ISR. */
#define isr_Calibration_INTC_VECTOR            ((reg16 *) isr_Calibration__INTC_VECT)

/* Address of the isr_Calibration ISR priority. */
#define isr_Calibration_INTC_PRIOR             ((reg8 *) isr_Calibration__INTC_PRIOR_REG)

/* Priority of the isr_Calibration interrupt. */
#define isr_Calibration_INTC_PRIOR_NUMBER      isr_Calibration__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable isr_Calibration interrupt. */
#define isr_Calibration_INTC_SET_EN            ((reg8 *) isr_Calibration__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the isr_Calibration interrupt. */
#define isr_Calibration_INTC_CLR_EN            ((reg8 *) isr_Calibration__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the isr_Calibration interrupt state to pending. */
#define isr_Calibration_INTC_SET_PD            ((reg8 *) isr_Calibration__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the isr_Calibration interrupt. */
#define isr_Calibration_INTC_CLR_PD            ((reg8 *) isr_Calibration__INTC_CLR_PD_REG)



#endif /* CY_ISR_isr_Calibration_H */


/* [] END OF FILE */
