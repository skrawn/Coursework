/*******************************************************************************
* File Name: P6_6.c  
* Version 2.10
*
* Description:
*  This file contains API to enable firmware control of a Pins component.
*
* Note:
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "cytypes.h"
#include "P6_6.h"

/* APIs are not generated for P15[7:6] on PSoC 5 */
#if !(CY_PSOC5A &&\
	 P6_6__PORT == 15 && ((P6_6__MASK & 0xC0) != 0))


/*******************************************************************************
* Function Name: P6_6_Write
********************************************************************************
*
* Summary:
*  Assign a new value to the digital port's data output register.  
*
* Parameters:  
*  prtValue:  The value to be assigned to the Digital Port. 
*
* Return: 
*  None
*  
*******************************************************************************/
void P6_6_Write(uint8 value) 
{
    uint8 staticBits = (P6_6_DR & (uint8)(~P6_6_MASK));
    P6_6_DR = staticBits | ((uint8)(value << P6_6_SHIFT) & P6_6_MASK);
}


/*******************************************************************************
* Function Name: P6_6_SetDriveMode
********************************************************************************
*
* Summary:
*  Change the drive mode on the pins of the port.
* 
* Parameters:  
*  mode:  Change the pins to one of the following drive modes.
*
*  P6_6_DM_STRONG     Strong Drive 
*  P6_6_DM_OD_HI      Open Drain, Drives High 
*  P6_6_DM_OD_LO      Open Drain, Drives Low 
*  P6_6_DM_RES_UP     Resistive Pull Up 
*  P6_6_DM_RES_DWN    Resistive Pull Down 
*  P6_6_DM_RES_UPDWN  Resistive Pull Up/Down 
*  P6_6_DM_DIG_HIZ    High Impedance Digital 
*  P6_6_DM_ALG_HIZ    High Impedance Analog 
*
* Return: 
*  None
*
*******************************************************************************/
void P6_6_SetDriveMode(uint8 mode) 
{
	CyPins_SetPinDriveMode(P6_6_0, mode);
}


/*******************************************************************************
* Function Name: P6_6_Read
********************************************************************************
*
* Summary:
*  Read the current value on the pins of the Digital Port in right justified 
*  form.
*
* Parameters:  
*  None
*
* Return: 
*  Returns the current value of the Digital Port as a right justified number
*  
* Note:
*  Macro P6_6_ReadPS calls this function. 
*  
*******************************************************************************/
uint8 P6_6_Read(void) 
{
    return (P6_6_PS & P6_6_MASK) >> P6_6_SHIFT;
}


/*******************************************************************************
* Function Name: P6_6_ReadDataReg
********************************************************************************
*
* Summary:
*  Read the current value assigned to a Digital Port's data output register
*
* Parameters:  
*  None 
*
* Return: 
*  Returns the current value assigned to the Digital Port's data output register
*  
*******************************************************************************/
uint8 P6_6_ReadDataReg(void) 
{
    return (P6_6_DR & P6_6_MASK) >> P6_6_SHIFT;
}


/* If Interrupts Are Enabled for this Pins component */ 
#if defined(P6_6_INTSTAT) 

    /*******************************************************************************
    * Function Name: P6_6_ClearInterrupt
    ********************************************************************************
    * Summary:
    *  Clears any active interrupts attached to port and returns the value of the 
    *  interrupt status register.
    *
    * Parameters:  
    *  None 
    *
    * Return: 
    *  Returns the value of the interrupt status register
    *  
    *******************************************************************************/
    uint8 P6_6_ClearInterrupt(void) 
    {
        return (P6_6_INTSTAT & P6_6_MASK) >> P6_6_SHIFT;
    }

#endif /* If Interrupts Are Enabled for this Pins component */ 

#endif /* CY_PSOC5A... */

    
/* [] END OF FILE */
