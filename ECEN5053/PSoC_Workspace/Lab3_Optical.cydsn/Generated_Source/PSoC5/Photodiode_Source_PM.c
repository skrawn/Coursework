/*******************************************************************************
* File Name: Photodiode_Source_PM.c  
* Version 1.90
*
* Description:
*  This file provides the power management source code to API for the
*  VDAC8.  
*
* Note:
*  None
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "Photodiode_Source.h"

static Photodiode_Source_backupStruct Photodiode_Source_backup;


/*******************************************************************************
* Function Name: Photodiode_Source_SaveConfig
********************************************************************************
* Summary:
*  Save the current user configuration
*
* Parameters:  
*  void  
*
* Return: 
*  void
*
*******************************************************************************/
void Photodiode_Source_SaveConfig(void) 
{
    if (!((Photodiode_Source_CR1 & Photodiode_Source_SRC_MASK) == Photodiode_Source_SRC_UDB))
    {
        Photodiode_Source_backup.data_value = Photodiode_Source_Data;
    }
}


/*******************************************************************************
* Function Name: Photodiode_Source_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:  
*  void
*
* Return: 
*  void
*
*******************************************************************************/
void Photodiode_Source_RestoreConfig(void) 
{
    if (!((Photodiode_Source_CR1 & Photodiode_Source_SRC_MASK) == Photodiode_Source_SRC_UDB))
    {
        if((Photodiode_Source_Strobe & Photodiode_Source_STRB_MASK) == Photodiode_Source_STRB_EN)
        {
            Photodiode_Source_Strobe &= (uint8)(~Photodiode_Source_STRB_MASK);
            Photodiode_Source_Data = Photodiode_Source_backup.data_value;
            Photodiode_Source_Strobe |= Photodiode_Source_STRB_EN;
        }
        else
        {
            Photodiode_Source_Data = Photodiode_Source_backup.data_value;
        }
    }
}


/*******************************************************************************
* Function Name: Photodiode_Source_Sleep
********************************************************************************
* Summary:
*  Stop and Save the user configuration
*
* Parameters:  
*  void:  
*
* Return: 
*  void
*
* Global variables:
*  Photodiode_Source_backup.enableState:  Is modified depending on the enable 
*  state  of the block before entering sleep mode.
*
*******************************************************************************/
void Photodiode_Source_Sleep(void) 
{
    /* Save VDAC8's enable state */    
    if(Photodiode_Source_ACT_PWR_EN == (Photodiode_Source_PWRMGR & Photodiode_Source_ACT_PWR_EN))
    {
        /* VDAC8 is enabled */
        Photodiode_Source_backup.enableState = 1u;
    }
    else
    {
        /* VDAC8 is disabled */
        Photodiode_Source_backup.enableState = 0u;
    }
    
    Photodiode_Source_Stop();
    Photodiode_Source_SaveConfig();
}


/*******************************************************************************
* Function Name: Photodiode_Source_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*  
* Parameters:  
*  void
*
* Return: 
*  void
*
* Global variables:
*  Photodiode_Source_backup.enableState:  Is used to restore the enable state of 
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Photodiode_Source_Wakeup(void) 
{
    Photodiode_Source_RestoreConfig();
    
    if(Photodiode_Source_backup.enableState == 1u)
    {
        /* Enable VDAC8's operation */
        Photodiode_Source_Enable();

        /* Restore the data register */
        Photodiode_Source_SetValue(Photodiode_Source_Data);
    } /* Do nothing if VDAC8 was disabled before */    
}


/* [] END OF FILE */
