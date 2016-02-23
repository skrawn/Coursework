/*******************************************************************************
* File Name: OpAmp_PM.c
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

#include "OpAmp.h"

static OpAmp_BACKUP_STRUCT  OpAmp_backup;


/*******************************************************************************  
* Function Name: OpAmp_SaveConfig
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
void OpAmp_SaveConfig(void) 
{
    /* Nothing to save as registers are System reset on retention flops */
}


/*******************************************************************************  
* Function Name: OpAmp_RestoreConfig
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
void OpAmp_RestoreConfig(void) 
{
    /* Nothing to restore */
}


/*******************************************************************************   
* Function Name: OpAmp_Sleep
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
*  OpAmp_backup: The structure field 'enableState' is modified 
*  depending on the enable state of the block before entering to sleep mode.
*
*******************************************************************************/
void OpAmp_Sleep(void) 
{
    /* Save OpAmp enable state */
    if((OpAmp_PM_ACT_CFG_REG & OpAmp_ACT_PWR_EN) != 0u)
    {
        /* Component is enabled */
        OpAmp_backup.enableState = 1u;
         /* Stops the component */
         OpAmp_Stop();
    }
    else
    {
        /* Component is disabled */
        OpAmp_backup.enableState = 0u;
    }
    /* Saves the configuration */
    OpAmp_SaveConfig();
}


/*******************************************************************************  
* Function Name: OpAmp_Wakeup
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
*  OpAmp_backup: The structure field 'enableState' is used to 
*  restore the enable state of block after wakeup from sleep mode.
*
*******************************************************************************/
void OpAmp_Wakeup(void) 
{
    /* Restore the user configuration */
    OpAmp_RestoreConfig();

    /* Enables the component operation */
    if(OpAmp_backup.enableState == 1u)
    {
        OpAmp_Enable();
    } /* Do nothing if component was disable before */
}


/* [] END OF FILE */
