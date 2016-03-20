/*******************************************************************************
* File Name: LED_Follower.c
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

#include "LED_Follower.h"

uint8 LED_Follower_initVar = 0u;


/*******************************************************************************   
* Function Name: LED_Follower_Init
********************************************************************************
*
* Summary:
*  Initialize component's parameters to the parameters set by user in the 
*  customizer of the component placed onto schematic. Usually called in 
*  LED_Follower_Start().
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void LED_Follower_Init(void) 
{
    LED_Follower_SetPower(LED_Follower_DEFAULT_POWER);
}


/*******************************************************************************   
* Function Name: LED_Follower_Enable
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
void LED_Follower_Enable(void) 
{
    /* Enable negative charge pumps in ANIF */
    LED_Follower_PUMP_CR1_REG  |= (LED_Follower_PUMP_CR1_CLKSEL | LED_Follower_PUMP_CR1_FORCE);

    /* Enable power to buffer in active mode */
    LED_Follower_PM_ACT_CFG_REG |= LED_Follower_ACT_PWR_EN;

    /* Enable power to buffer in alternative active mode */
    LED_Follower_PM_STBY_CFG_REG |= LED_Follower_STBY_PWR_EN;
}


/*******************************************************************************
* Function Name:   LED_Follower_Start
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
*  LED_Follower_initVar: Used to check the initial configuration, modified 
*  when this function is called for the first time.
*
*******************************************************************************/
void LED_Follower_Start(void) 
{
    if(LED_Follower_initVar == 0u)
    {
        LED_Follower_initVar = 1u;
        LED_Follower_Init();
    }

    LED_Follower_Enable();
}


/*******************************************************************************
* Function Name: LED_Follower_Stop
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
void LED_Follower_Stop(void) 
{
    /* Disable power to buffer in active mode template */
    LED_Follower_PM_ACT_CFG_REG &= (uint8)(~LED_Follower_ACT_PWR_EN);

    /* Disable power to buffer in alternative active mode template */
    LED_Follower_PM_STBY_CFG_REG &= (uint8)(~LED_Follower_STBY_PWR_EN);
    
    /* Disable negative charge pumps for ANIF only if all ABuf is turned OFF */
    if(LED_Follower_PM_ACT_CFG_REG == 0u)
    {
        LED_Follower_PUMP_CR1_REG &= (uint8)(~(LED_Follower_PUMP_CR1_CLKSEL | LED_Follower_PUMP_CR1_FORCE));
    }
}


/*******************************************************************************
* Function Name: LED_Follower_SetPower
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
void LED_Follower_SetPower(uint8 power) 
{
    #if (CY_PSOC3 || CY_PSOC5LP)
        LED_Follower_CR_REG &= (uint8)(~LED_Follower_PWR_MASK);
        LED_Follower_CR_REG |= power & LED_Follower_PWR_MASK;      /* Set device power */
    #else
        CYASSERT(LED_Follower_HIGHPOWER == power);
    #endif /* CY_PSOC3 || CY_PSOC5LP */
}


/* [] END OF FILE */
