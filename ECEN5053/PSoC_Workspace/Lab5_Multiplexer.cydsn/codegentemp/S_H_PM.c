/*******************************************************************************
* File Name: S_H.c
* Version 1.40
*
* Description:
*  This file provides the power manager source code to the API for 
*  the SAMPLE/TRACK AND HOLD component.
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

#include "S_H.h"

static S_H_backupStruct  S_H_backup;


/*******************************************************************************
* Function Name: S_H_SaveConfig
********************************************************************************
*
* Summary:
*  Saves the current user configuration.
*  
* Parameters:  
*  void.
*
* Return: 
*  void.
*
*******************************************************************************/
void S_H_SaveConfig(void) 
{
    /* Nothing to save before entering into sleep mode as all the registers used 
       here are retention registers. */
}


/*******************************************************************************
* Function Name: S_H_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:  
*  void.
*
* Return: 
*  void
*
*******************************************************************************/
void S_H_RestoreConfig(void) 
{
    /* Nothing to restore */
}


/*******************************************************************************
* Function Name: S_H_Sleep
********************************************************************************
*
* Summary:
*  Disables block's operation and saves its configuration. Should be called 
*  just prior to entering sleep.
*  
* Parameters:  
*  None
*
* Return: 
*  None
*
* Global variables:
*  S_H_backup:  The structure field 'enableState' is modified 
*  depending on the enable state of the block before entering to sleep mode.
*
*******************************************************************************/
void S_H_Sleep(void) 
{
    /* Save TIA enable state */
    if((S_H_PM_ACT_CFG_REG & S_H_ACT_PWR_EN) != 0u)
    {
        /* Component is enabled */
        S_H_backup.enableState = 1u;
        /* Stop the configuration */
        S_H_Stop();
    }
    else
    {
        /* Component is disabled */
        S_H_backup.enableState = 0u;
    }
    /* Saves the user configuration */
    S_H_SaveConfig();
}


/*******************************************************************************
* Function Name: S_H_Wakeup
********************************************************************************
*
* Summary:
*  Enables block's operation and restores its configuration. Should be called
*  just after awaking from sleep.
*  
* Parameters:  
*  None
*
* Return: 
*  void
*
* Global variables:
*  S_H_backup:  The structure field 'enableState' is used to 
*  restore the enable state of block after wakeup from sleep mode.
*
*******************************************************************************/
void S_H_Wakeup(void) 
{
    /* Restore the user configuration */
    S_H_RestoreConfig();
    
    /* Enable's the component operation */
    if(S_H_backup.enableState == 1u)
    {
        S_H_Enable();
    } /* Do nothing if component was disable before */
}


/* [] END OF FILE */
