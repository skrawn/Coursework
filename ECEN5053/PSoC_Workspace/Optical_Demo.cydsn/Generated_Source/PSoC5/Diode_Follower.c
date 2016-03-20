/*******************************************************************************
* File Name: Diode_Follower.c
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

#include "Diode_Follower.h"

uint8 Diode_Follower_initVar = 0u;


/*******************************************************************************   
* Function Name: Diode_Follower_Init
********************************************************************************
*
* Summary:
*  Initialize component's parameters to the parameters set by user in the 
*  customizer of the component placed onto schematic. Usually called in 
*  Diode_Follower_Start().
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void Diode_Follower_Init(void) 
{
    Diode_Follower_SetPower(Diode_Follower_DEFAULT_POWER);
}


/*******************************************************************************   
* Function Name: Diode_Follower_Enable
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
void Diode_Follower_Enable(void) 
{
    /* Enable negative charge pumps in ANIF */
    Diode_Follower_PUMP_CR1_REG  |= (Diode_Follower_PUMP_CR1_CLKSEL | Diode_Follower_PUMP_CR1_FORCE);

    /* Enable power to buffer in active mode */
    Diode_Follower_PM_ACT_CFG_REG |= Diode_Follower_ACT_PWR_EN;

    /* Enable power to buffer in alternative active mode */
    Diode_Follower_PM_STBY_CFG_REG |= Diode_Follower_STBY_PWR_EN;
}


/*******************************************************************************
* Function Name:   Diode_Follower_Start
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
*  Diode_Follower_initVar: Used to check the initial configuration, modified 
*  when this function is called for the first time.
*
*******************************************************************************/
void Diode_Follower_Start(void) 
{
    if(Diode_Follower_initVar == 0u)
    {
        Diode_Follower_initVar = 1u;
        Diode_Follower_Init();
    }

    Diode_Follower_Enable();
}


/*******************************************************************************
* Function Name: Diode_Follower_Stop
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
void Diode_Follower_Stop(void) 
{
    /* Disable power to buffer in active mode template */
    Diode_Follower_PM_ACT_CFG_REG &= (uint8)(~Diode_Follower_ACT_PWR_EN);

    /* Disable power to buffer in alternative active mode template */
    Diode_Follower_PM_STBY_CFG_REG &= (uint8)(~Diode_Follower_STBY_PWR_EN);
    
    /* Disable negative charge pumps for ANIF only if all ABuf is turned OFF */
    if(Diode_Follower_PM_ACT_CFG_REG == 0u)
    {
        Diode_Follower_PUMP_CR1_REG &= (uint8)(~(Diode_Follower_PUMP_CR1_CLKSEL | Diode_Follower_PUMP_CR1_FORCE));
    }
}


/*******************************************************************************
* Function Name: Diode_Follower_SetPower
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
void Diode_Follower_SetPower(uint8 power) 
{
    #if (CY_PSOC3 || CY_PSOC5LP)
        Diode_Follower_CR_REG &= (uint8)(~Diode_Follower_PWR_MASK);
        Diode_Follower_CR_REG |= power & Diode_Follower_PWR_MASK;      /* Set device power */
    #else
        CYASSERT(Diode_Follower_HIGHPOWER == power);
    #endif /* CY_PSOC3 || CY_PSOC5LP */
}


/* [] END OF FILE */
