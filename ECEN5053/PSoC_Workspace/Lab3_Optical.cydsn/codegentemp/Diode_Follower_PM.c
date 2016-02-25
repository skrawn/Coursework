/*******************************************************************************
* File Name: Diode_Follower_PM.c
* Version 1.90
*
* Description:
*  This file provides the power management source code to the API for the 
*  OpAmp (Analog Buffer) component.
*
* Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "Diode_Follower.h"

static Diode_Follower_BACKUP_STRUCT  Diode_Follower_backup;


/*******************************************************************************  
* Function Name: Diode_Follower_SaveConfig
********************************************************************************
*
* Summary:
*  Saves the current user configuration registers.
* 
* Parameters:
*  void
* 
* Return:
*  void
*
*******************************************************************************/
void Diode_Follower_SaveConfig(void) 
{
    /* Nothing to save as registers are System reset on retention flops */
}


/*******************************************************************************  
* Function Name: Diode_Follower_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration registers.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void Diode_Follower_RestoreConfig(void) 
{
    /* Nothing to restore */
}


/*******************************************************************************   
* Function Name: Diode_Follower_Sleep
********************************************************************************
*
* Summary:
*  Disables block's operation and saves its configuration. Should be called 
*  just prior to entering sleep.
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Diode_Follower_backup: The structure field 'enableState' is modified 
*  depending on the enable state of the block before entering to sleep mode.
*
*******************************************************************************/
void Diode_Follower_Sleep(void) 
{
    /* Save OpAmp enable state */
    if((Diode_Follower_PM_ACT_CFG_REG & Diode_Follower_ACT_PWR_EN) != 0u)
    {
        /* Component is enabled */
        Diode_Follower_backup.enableState = 1u;
         /* Stops the component */
         Diode_Follower_Stop();
    }
    else
    {
        /* Component is disabled */
        Diode_Follower_backup.enableState = 0u;
    }
    /* Saves the configuration */
    Diode_Follower_SaveConfig();
}


/*******************************************************************************  
* Function Name: Diode_Follower_Wakeup
********************************************************************************
*
* Summary:
*  Enables block's operation and restores its configuration. Should be called
*  just after awaking from sleep.
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  Diode_Follower_backup: The structure field 'enableState' is used to 
*  restore the enable state of block after wakeup from sleep mode.
*
*******************************************************************************/
void Diode_Follower_Wakeup(void) 
{
    /* Restore the user configuration */
    Diode_Follower_RestoreConfig();

    /* Enables the component operation */
    if(Diode_Follower_backup.enableState == 1u)
    {
        Diode_Follower_Enable();
    } /* Do nothing if component was disable before */
}


/* [] END OF FILE */
