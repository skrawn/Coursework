/*******************************************************************************
* File Name: S_H.c
* Version 1.40
*
* Description:
*  This file provides the source code to the API for the SAMPLE/TRACK AND HOLD 
*  component.
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

#if (!CY_PSOC5A)
    #if (CYDEV_VARIABLE_VDDA == 1u)
        #include "CyScBoostClk.h"
    #endif /* (CYDEV_VARIABLE_VDDA == 1u) */
#endif /* (!CY_PSOC5A) */

uint8 S_H_initVar = 0u;

/* static S_H_backupStruct  S_H_backup; */
#if (CY_PSOC5A)
    static S_H_backupStruct  S_H_P5backup;
#endif /* CY_PSOC5A */



/*******************************************************************************   
* Function Name: S_H_Init
********************************************************************************
*
* Summary:
*  Initialize component's parameters to the parameters set by user in the 
*  customizer of the component placed onto schematic. Usually called in 
*  S_H_Start().
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void S_H_Init(void) 
{   
    /* Configure SC Block based on selected Sample/Track type */
    #if (S_H_SAMPLE_TRACK_MODE == S_H_SAMPLEANDHOLD) /* Sample and hold mode */
        /* SC Block mode -  - SCxx_CR0[3:1] */
        S_H_CR0_REG = S_H_MODE_SAMPLEANDHOLD;
        
        /* SC Block CR1 */ 
        S_H_CR1_REG = (S_H_COMP_4P35PF  |
                                S_H_GAIN_0DB);
        #if(S_H_SAMPLE_CLOCK_EDGE == S_H_EDGE_POSITIVENEGATIVE)
            S_H_CR1_REG =  S_H_CR1_REG  | S_H_DIV2_DISABLE ;
        #else
            S_H_CR1_REG =  S_H_CR1_REG  | S_H_DIV2_ENABLE ;
        #endif
         
        #if(S_H_VREF_TYPE == S_H_EXTERNAL)
            S_H_CR2_REG = S_H_BIAS_LOW |
                                       S_H_REDC_00 | 
                                       S_H_GNDVREF_DI;
        #else
            S_H_CR2_REG = S_H_BIAS_LOW |
                                       S_H_REDC_00 | 
                                       S_H_GNDVREF_E;
        #endif
    #else
        /* Track and Hold Mode */
        /* SC Block mode -  - SCxx_CR0[3:1] */
        S_H_CR0_REG = S_H_MODE_TRACKANDHOLD; 
        
        /* SC Block CR1 */ 
        S_H_CR1_REG = S_H_COMP_4P35PF  |
                                   S_H_DIV2_ENABLE |
                                   S_H_GAIN_0DB;
                                
        /* SC Block CR2 */
        S_H_CR2_REG = S_H_BIAS_LOW |
                                   S_H_REDC_00 |
                                   S_H_GNDVREF_E;
    #endif /* end if - Sample/Track Type */
    
    /* Enable SC Block clocking */
    S_H_CLK_REG |= S_H_CLK_EN;
    
    /* Set default power */
    S_H_SetPower(S_H_INIT_POWER);
}


/*******************************************************************************   
* Function Name: S_H_Enable
********************************************************************************
*
* Summary:
*  Enables the Sample/Track and Hold block operation
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void S_H_Enable(void) 
{
    /* This is to restore the value of register CR1 and CR2 which is saved 
      in prior to the modifications in stop() API */
    #if (CY_PSOC5A)
        if(S_H_P5backup.enableState == 1u)
        {
            S_H_CR1_REG = S_H_P5backup.scCR1Reg;
            S_H_CR2_REG = S_H_P5backup.scCR2Reg;
            S_H_P5backup.enableState = 0u;
        }
    #endif /* CY_PSOC5A */
    
    /* Enable power to SC block in active mode */
    S_H_PM_ACT_CFG_REG |= S_H_ACT_PWR_EN;
    
    /* Enable power to SC block in Alternative active mode */
    S_H_PM_STBY_CFG_REG |= S_H_STBY_PWR_EN;

    S_H_PUMP_CR1_REG |= S_H_PUMP_CR1_SC_CLKSEL;
    
    #if (!CY_PSOC5A)
        #if (CYDEV_VARIABLE_VDDA == 1u)
            if(CyScPumpEnabled == 1u)
            {
                S_H_BSTCLK_REG &= (uint8)(~S_H_BST_CLK_INDEX_MASK);
                S_H_BSTCLK_REG |= S_H_BST_CLK_EN | CyScBoostClk__INDEX;
                S_H_SC_MISC_REG |= S_H_PUMP_FORCE;
                CyScBoostClk_Start();
            }
            else
            {
                S_H_BSTCLK_REG &= (uint8)(~S_H_BST_CLK_EN);
                S_H_SC_MISC_REG &= (uint8)(~S_H_PUMP_FORCE);
            }
        #endif /* (CYDEV_VARIABLE_VDDA == 1u) */
    #endif /* (!CY_PSOC5A) */
}


/*******************************************************************************
* Function Name: S_H_Start
********************************************************************************
*
* Summary:
*  The start function initializes the Sample and Hold with the default values, 
*  and sets the power to the given level.  A power level of 0, is the same as 
*  executing the stop function.
*
* Parameters:
*  void
*
* Return:
*  void
*
*******************************************************************************/
void S_H_Start(void) 
{
    /* If not Initialized then initialize all required hardware and software */
    if(S_H_initVar == 0u)
    {
        S_H_Init();
        S_H_initVar = 1u;
    }
    S_H_Enable();
}


/*******************************************************************************
* Function Name: S_H_Stop
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
void S_H_Stop(void) 
{   
    /* Disble power to the Amp in Active mode template */
    S_H_PM_ACT_CFG_REG &= (uint8)(~S_H_ACT_PWR_EN);

    /* Disble power to the Amp in Alternative Active mode template */
    S_H_PM_STBY_CFG_REG &= (uint8)(~S_H_STBY_PWR_EN);
    
    #if (!CY_PSOC5A)
        #if (CYDEV_VARIABLE_VDDA == 1u)
            S_H_BSTCLK_REG &= (uint8)(~S_H_BST_CLK_EN);
            /* Disable pumps only if there aren't any SC block in use */
            if ((S_H_PM_ACT_CFG_REG & S_H_PM_ACT_CFG_MASK) == 0u)
            {
                S_H_SC_MISC_REG &= (uint8)(~S_H_PUMP_FORCE);
                S_H_PUMP_CR1_REG &= (uint8)(~S_H_PUMP_CR1_SC_CLKSEL);
                CyScBoostClk_Stop();
            }
        #endif /* CYDEV_VARIABLE_VDDA == 1u */
    #endif /* (CY_PSOC3 || CY_PSOC5LP) */
 
    /* This sets Sample and hold in zero current mode and output routes are valid */
    #if (CY_PSOC5A)
        S_H_P5backup.scCR1Reg = S_H_CR1_REG;
        S_H_P5backup.scCR2Reg = S_H_CR2_REG;
        S_H_CR1_REG = 0x00u;
        S_H_CR2_REG = 0x00u;
        S_H_P5backup.enableState = 1u;
    #endif /* CY_PSOC5A */
}


/*******************************************************************************
* Function Name: S_H_SetPower
********************************************************************************
*
* Summary:
*  Set the power of the Sample/Track and Hold.
*
* Parameters:
*  power: Sets power level between (0) and (3) high power
*
* Return:
*  void
*
*******************************************************************************/
void S_H_SetPower(uint8 power) 
{
    uint8 tmpCR;

    /* Sets drive bits in SC Block Control Register 1:  SCxx_CR[1:0] */
    tmpCR = S_H_CR1_REG & (uint8)(~S_H_DRIVE_MASK);
    tmpCR |= (power & S_H_DRIVE_MASK);
    S_H_CR1_REG = tmpCR;
}


/* [] END OF FILE */
