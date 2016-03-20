/*******************************************************************************
* File Name: LED_DAC_PM.c  
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

#include "LED_DAC.h"

static LED_DAC_backupStruct LED_DAC_backup;


/*******************************************************************************
* Function Name: LED_DAC_SaveConfig
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
void LED_DAC_SaveConfig(void) 
{
    if (!((LED_DAC_CR1 & LED_DAC_SRC_MASK) == LED_DAC_SRC_UDB))
    {
        LED_DAC_backup.data_value = LED_DAC_Data;
    }
}


/*******************************************************************************
* Function Name: LED_DAC_RestoreConfig
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
void LED_DAC_RestoreConfig(void) 
{
    if (!((LED_DAC_CR1 & LED_DAC_SRC_MASK) == LED_DAC_SRC_UDB))
    {
        if((LED_DAC_Strobe & LED_DAC_STRB_MASK) == LED_DAC_STRB_EN)
        {
            LED_DAC_Strobe &= (uint8)(~LED_DAC_STRB_MASK);
            LED_DAC_Data = LED_DAC_backup.data_value;
            LED_DAC_Strobe |= LED_DAC_STRB_EN;
        }
        else
        {
            LED_DAC_Data = LED_DAC_backup.data_value;
        }
    }
}


/*******************************************************************************
* Function Name: LED_DAC_Sleep
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
*  LED_DAC_backup.enableState:  Is modified depending on the enable 
*  state  of the block before entering sleep mode.
*
*******************************************************************************/
void LED_DAC_Sleep(void) 
{
    /* Save VDAC8's enable state */    
    if(LED_DAC_ACT_PWR_EN == (LED_DAC_PWRMGR & LED_DAC_ACT_PWR_EN))
    {
        /* VDAC8 is enabled */
        LED_DAC_backup.enableState = 1u;
    }
    else
    {
        /* VDAC8 is disabled */
        LED_DAC_backup.enableState = 0u;
    }
    
    LED_DAC_Stop();
    LED_DAC_SaveConfig();
}


/*******************************************************************************
* Function Name: LED_DAC_Wakeup
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
*  LED_DAC_backup.enableState:  Is used to restore the enable state of 
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void LED_DAC_Wakeup(void) 
{
    LED_DAC_RestoreConfig();
    
    if(LED_DAC_backup.enableState == 1u)
    {
        /* Enable VDAC8's operation */
        LED_DAC_Enable();

        /* Restore the data register */
        LED_DAC_SetValue(LED_DAC_Data);
    } /* Do nothing if VDAC8 was disabled before */    
}


/* [] END OF FILE */
