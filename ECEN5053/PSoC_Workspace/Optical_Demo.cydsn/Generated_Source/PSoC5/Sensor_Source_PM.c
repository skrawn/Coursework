/*******************************************************************************
* File Name: Sensor_Source_PM.c  
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

#include "Sensor_Source.h"

static Sensor_Source_backupStruct Sensor_Source_backup;


/*******************************************************************************
* Function Name: Sensor_Source_SaveConfig
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
void Sensor_Source_SaveConfig(void) 
{
    if (!((Sensor_Source_CR1 & Sensor_Source_SRC_MASK) == Sensor_Source_SRC_UDB))
    {
        Sensor_Source_backup.data_value = Sensor_Source_Data;
    }
}


/*******************************************************************************
* Function Name: Sensor_Source_RestoreConfig
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
void Sensor_Source_RestoreConfig(void) 
{
    if (!((Sensor_Source_CR1 & Sensor_Source_SRC_MASK) == Sensor_Source_SRC_UDB))
    {
        if((Sensor_Source_Strobe & Sensor_Source_STRB_MASK) == Sensor_Source_STRB_EN)
        {
            Sensor_Source_Strobe &= (uint8)(~Sensor_Source_STRB_MASK);
            Sensor_Source_Data = Sensor_Source_backup.data_value;
            Sensor_Source_Strobe |= Sensor_Source_STRB_EN;
        }
        else
        {
            Sensor_Source_Data = Sensor_Source_backup.data_value;
        }
    }
}


/*******************************************************************************
* Function Name: Sensor_Source_Sleep
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
*  Sensor_Source_backup.enableState:  Is modified depending on the enable 
*  state  of the block before entering sleep mode.
*
*******************************************************************************/
void Sensor_Source_Sleep(void) 
{
    /* Save VDAC8's enable state */    
    if(Sensor_Source_ACT_PWR_EN == (Sensor_Source_PWRMGR & Sensor_Source_ACT_PWR_EN))
    {
        /* VDAC8 is enabled */
        Sensor_Source_backup.enableState = 1u;
    }
    else
    {
        /* VDAC8 is disabled */
        Sensor_Source_backup.enableState = 0u;
    }
    
    Sensor_Source_Stop();
    Sensor_Source_SaveConfig();
}


/*******************************************************************************
* Function Name: Sensor_Source_Wakeup
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
*  Sensor_Source_backup.enableState:  Is used to restore the enable state of 
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void Sensor_Source_Wakeup(void) 
{
    Sensor_Source_RestoreConfig();
    
    if(Sensor_Source_backup.enableState == 1u)
    {
        /* Enable VDAC8's operation */
        Sensor_Source_Enable();

        /* Restore the data register */
        Sensor_Source_SetValue(Sensor_Source_Data);
    } /* Do nothing if VDAC8 was disabled before */    
}


/* [] END OF FILE */
