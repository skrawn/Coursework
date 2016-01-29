/*******************************************************************************
* File Name: Vlow.c  
* Version 1.90
*
* Description:
*  This file contains API to enable firmware control of a Pins component.
*
* Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "cytypes.h"
#include "Vlow.h"

/* APIs are not generated for P15[7:6] on PSoC 5 */
#if !(CY_PSOC5A &&\
	 Vlow__PORT == 15 && ((Vlow__MASK & 0xC0) != 0))


/*******************************************************************************
* Function Name: Vlow_Write
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
void Vlow_Write(uint8 value) 
{
    uint8 staticBits = (Vlow_DR & (uint8)(~Vlow_MASK));
    Vlow_DR = staticBits | ((uint8)(value << Vlow_SHIFT) & Vlow_MASK);
}


/*******************************************************************************
* Function Name: Vlow_SetDriveMode
********************************************************************************
*
* Summary:
*  Change the drive mode on the pins of the port.
* 
* Parameters:  
*  mode:  Change the pins to this drive mode.
*
* Return: 
*  None
*
*******************************************************************************/
void Vlow_SetDriveMode(uint8 mode) 
{
	CyPins_SetPinDriveMode(Vlow_0, mode);
}


/*******************************************************************************
* Function Name: Vlow_Read
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
*  Macro Vlow_ReadPS calls this function. 
*  
*******************************************************************************/
uint8 Vlow_Read(void) 
{
    return (Vlow_PS & Vlow_MASK) >> Vlow_SHIFT;
}


/*******************************************************************************
* Function Name: Vlow_ReadDataReg
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
uint8 Vlow_ReadDataReg(void) 
{
    return (Vlow_DR & Vlow_MASK) >> Vlow_SHIFT;
}


/* If Interrupts Are Enabled for this Pins component */ 
#if defined(Vlow_INTSTAT) 

    /*******************************************************************************
    * Function Name: Vlow_ClearInterrupt
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
    uint8 Vlow_ClearInterrupt(void) 
    {
        return (Vlow_INTSTAT & Vlow_MASK) >> Vlow_SHIFT;
    }

#endif /* If Interrupts Are Enabled for this Pins component */ 

#endif /* CY_PSOC5A... */

    
/* [] END OF FILE */
