/*******************************************************************************
* File Name: PhotoDiode_Amp.c
* Version 1.90
*
* Description:
*  This file provides the source code to the API for OpAmp (Analog Buffer) 
*  Component.
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

uint8 PhotoDiode_Amp_initVar = 0u;


/*******************************************************************************   
* Function Name: PhotoDiode_Amp_Init
********************************************************************************
*
* Summary:
*  Initialize component's parameters to the parameters set by user in the 
*  customizer of the component placed onto schematic. Usually called in 
*  PhotoDiode_Amp_Start().
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void PhotoDiode_Amp_Init(void) 
{
    PhotoDiode_Amp_SetPower(PhotoDiode_Amp_DEFAULT_POWER);
}


/*******************************************************************************   
* Function Name: PhotoDiode_Amp_Enable
********************************************************************************
*
* Summary:
*  Enables the OpAmp block operation
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void PhotoDiode_Amp_Enable(void) 
{
    /* Enable negative charge pumps in ANIF */
    PhotoDiode_Amp_PUMP_CR1_REG  |= (PhotoDiode_Amp_PUMP_CR1_CLKSEL | PhotoDiode_Amp_PUMP_CR1_FORCE);

    /* Enable power to buffer in active mode */
    PhotoDiode_Amp_PM_ACT_CFG_REG |= PhotoDiode_Amp_ACT_PWR_EN;

    /* Enable power to buffer in alternative active mode */
    PhotoDiode_Amp_PM_STBY_CFG_REG |= PhotoDiode_Amp_STBY_PWR_EN;
}


/*******************************************************************************
* Function Name:   PhotoDiode_Amp_Start
********************************************************************************
*
* Summary:
*  The start function initializes the Analog Buffer with the default values and 
*  sets the power to the given level. A power level of 0, is same as 
*  executing the stop function.
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  PhotoDiode_Amp_initVar: Used to check the initial configuration, modified 
*  when this function is called for the first time.
*
*******************************************************************************/
void PhotoDiode_Amp_Start(void) 
{
    if(PhotoDiode_Amp_initVar == 0u)
    {
        PhotoDiode_Amp_initVar = 1u;
        PhotoDiode_Amp_Init();
    }

    PhotoDiode_Amp_Enable();
}


/*******************************************************************************
* Function Name: PhotoDiode_Amp_Stop
********************************************************************************
*
* Summary:
*  Powers down amplifier to lowest power state.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void PhotoDiode_Amp_Stop(void) 
{
    /* Disable power to buffer in active mode template */
    PhotoDiode_Amp_PM_ACT_CFG_REG &= (uint8)(~PhotoDiode_Amp_ACT_PWR_EN);

    /* Disable power to buffer in alternative active mode template */
    PhotoDiode_Amp_PM_STBY_CFG_REG &= (uint8)(~PhotoDiode_Amp_STBY_PWR_EN);
    
    /* Disable negative charge pumps for ANIF only if all ABuf is turned OFF */
    if(PhotoDiode_Amp_PM_ACT_CFG_REG == 0u)
    {
        PhotoDiode_Amp_PUMP_CR1_REG &= (uint8)(~(PhotoDiode_Amp_PUMP_CR1_CLKSEL | PhotoDiode_Amp_PUMP_CR1_FORCE));
    }
}


/*******************************************************************************
* Function Name: PhotoDiode_Amp_SetPower
********************************************************************************
*
* Summary:
*  Sets power level of Analog buffer.
*
* Parameters: 
*  power: PSoC3: Sets power level between low (1) and high power (3).
*         PSoC5: Sets power level High (0)
*
* Return:
*  void
*
**********************************************************************************/
void PhotoDiode_Amp_SetPower(uint8 power) 
{
    #if (CY_PSOC3 || CY_PSOC5LP)
        PhotoDiode_Amp_CR_REG &= (uint8)(~PhotoDiode_Amp_PWR_MASK);
        PhotoDiode_Amp_CR_REG |= power & PhotoDiode_Amp_PWR_MASK;      /* Set device power */
    #else
        CYASSERT(PhotoDiode_Amp_HIGHPOWER == power);
    #endif /* CY_PSOC3 || CY_PSOC5LP */
}


/* [] END OF FILE */
