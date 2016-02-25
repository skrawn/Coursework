/*******************************************************************************
* File Name: PhotoDiode_Amp_PM.c
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

#include "PhotoDiode_Amp.h"

static PhotoDiode_Amp_BACKUP_STRUCT  PhotoDiode_Amp_backup;


/*******************************************************************************  
* Function Name: PhotoDiode_Amp_SaveConfig
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
void PhotoDiode_Amp_SaveConfig(void) 
{
    /* Nothing to save as registers are System reset on retention flops */
}


/*******************************************************************************  
* Function Name: PhotoDiode_Amp_RestoreConfig
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
void PhotoDiode_Amp_RestoreConfig(void) 
{
    /* Nothing to restore */
}


/*******************************************************************************   
* Function Name: PhotoDiode_Amp_Sleep
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
*  PhotoDiode_Amp_backup: The structure field 'enableState' is modified 
*  depending on the enable state of the block before entering to sleep mode.
*
*******************************************************************************/
void PhotoDiode_Amp_Sleep(void) 
{
    /* Save OpAmp enable state */
    if((PhotoDiode_Amp_PM_ACT_CFG_REG & PhotoDiode_Amp_ACT_PWR_EN) != 0u)
    {
        /* Component is enabled */
        PhotoDiode_Amp_backup.enableState = 1u;
         /* Stops the component */
         PhotoDiode_Amp_Stop();
    }
    else
    {
        /* Component is disabled */
        PhotoDiode_Amp_backup.enableState = 0u;
    }
    /* Saves the configuration */
    PhotoDiode_Amp_SaveConfig();
}


/*******************************************************************************  
* Function Name: PhotoDiode_Amp_Wakeup
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
*  PhotoDiode_Amp_backup: The structure field 'enableState' is used to 
*  restore the enable state of block after wakeup from sleep mode.
*
*******************************************************************************/
void PhotoDiode_Amp_Wakeup(void) 
{
    /* Restore the user configuration */
    PhotoDiode_Amp_RestoreConfig();

    /* Enables the component operation */
    if(PhotoDiode_Amp_backup.enableState == 1u)
    {
        PhotoDiode_Amp_Enable();
    } /* Do nothing if component was disable before */
}


/* [] END OF FILE */
