/*******************************************************************************
* File Name: Photodiode_Source.c  
* Version 1.90
*
* Description:
*  This file provides the source code to the API for the 8-bit Voltage DAC 
*  (VDAC8) User Module.
*
* Note:
*  Any unusual or non-standard behavior should be noted here. Other-
*  wise, this section should remain blank.
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#include "cytypes.h"
#include "Photodiode_Source.h"

#if (CY_PSOC5A)
#include <CyLib.h>
#endif /* CY_PSOC5A */

uint8 Photodiode_Source_initVar = 0u;

#if (CY_PSOC5A)
    static uint8 Photodiode_Source_restoreVal = 0u;
#endif /* CY_PSOC5A */

#if (CY_PSOC5A)
    static Photodiode_Source_backupStruct Photodiode_Source_backup;
#endif /* CY_PSOC5A */


/*******************************************************************************
* Function Name: Photodiode_Source_Init
********************************************************************************
* Summary:
*  Initialize to the schematic state.
* 
* Parameters:
*  void:
*
* Return:
*  void
*
* Theory:
*
* Side Effects:
*
*******************************************************************************/
void Photodiode_Source_Init(void) 
{
    Photodiode_Source_CR0 = (Photodiode_Source_MODE_V );

    /* Set default data source */
    #if(Photodiode_Source_DEFAULT_DATA_SRC != 0 )
        Photodiode_Source_CR1 = (Photodiode_Source_DEFAULT_CNTL | Photodiode_Source_DACBUS_ENABLE) ;
    #else
        Photodiode_Source_CR1 = (Photodiode_Source_DEFAULT_CNTL | Photodiode_Source_DACBUS_DISABLE) ;
    #endif /* (Photodiode_Source_DEFAULT_DATA_SRC != 0 ) */

    /* Set default strobe mode */
    #if(Photodiode_Source_DEFAULT_STRB != 0)
        Photodiode_Source_Strobe |= Photodiode_Source_STRB_EN ;
    #endif/* (Photodiode_Source_DEFAULT_STRB != 0) */

    /* Set default range */
    Photodiode_Source_SetRange(Photodiode_Source_DEFAULT_RANGE); 

    /* Set default speed */
    Photodiode_Source_SetSpeed(Photodiode_Source_DEFAULT_SPEED);
}


/*******************************************************************************
* Function Name: Photodiode_Source_Enable
********************************************************************************
* Summary:
*  Enable the VDAC8
* 
* Parameters:
*  void
*
* Return:
*  void
*
* Theory:
*
* Side Effects:
*
*******************************************************************************/
void Photodiode_Source_Enable(void) 
{
    Photodiode_Source_PWRMGR |= Photodiode_Source_ACT_PWR_EN;
    Photodiode_Source_STBY_PWRMGR |= Photodiode_Source_STBY_PWR_EN;

    /*This is to restore the value of register CR0 ,
    which is modified  in Stop API , this prevents misbehaviour of VDAC */
    #if (CY_PSOC5A)
        if(Photodiode_Source_restoreVal == 1u) 
        {
             Photodiode_Source_CR0 = Photodiode_Source_backup.data_value;
             Photodiode_Source_restoreVal = 0u;
        }
    #endif /* CY_PSOC5A */
}


/*******************************************************************************
* Function Name: Photodiode_Source_Start
********************************************************************************
*
* Summary:
*  The start function initializes the voltage DAC with the default values, 
*  and sets the power to the given level.  A power level of 0, is the same as
*  executing the stop function.
*
* Parameters:
*  Power: Sets power level between off (0) and (3) high power
*
* Return:
*  void 
*
* Global variables:
*  Photodiode_Source_initVar: Is modified when this function is called for the 
*  first time. Is used to ensure that initialization happens only once.
*
*******************************************************************************/
void Photodiode_Source_Start(void)  
{
    /* Hardware initiazation only needs to occure the first time */
    if(Photodiode_Source_initVar == 0u)
    { 
        Photodiode_Source_Init();
        Photodiode_Source_initVar = 1u;
    }

    /* Enable power to DAC */
    Photodiode_Source_Enable();

    /* Set default value */
    Photodiode_Source_SetValue(Photodiode_Source_DEFAULT_DATA); 
}


/*******************************************************************************
* Function Name: Photodiode_Source_Stop
********************************************************************************
*
* Summary:
*  Powers down DAC to lowest power state.
*
* Parameters:
*  void
*
* Return:
*  void
*
* Theory:
*
* Side Effects:
*
*******************************************************************************/
void Photodiode_Source_Stop(void) 
{
    /* Disble power to DAC */
    Photodiode_Source_PWRMGR &= (uint8)(~Photodiode_Source_ACT_PWR_EN);
    Photodiode_Source_STBY_PWRMGR &= (uint8)(~Photodiode_Source_STBY_PWR_EN);

    /* This is a work around for PSoC5A  ,
    this sets VDAC to current mode with output off */
    #if (CY_PSOC5A)
        Photodiode_Source_backup.data_value = Photodiode_Source_CR0;
        Photodiode_Source_CR0 = Photodiode_Source_CUR_MODE_OUT_OFF;
        Photodiode_Source_restoreVal = 1u;
    #endif /* CY_PSOC5A */
}


/*******************************************************************************
* Function Name: Photodiode_Source_SetSpeed
********************************************************************************
*
* Summary:
*  Set DAC speed
*
* Parameters:
*  power: Sets speed value
*
* Return:
*  void
*
* Theory:
*
* Side Effects:
*
*******************************************************************************/
void Photodiode_Source_SetSpeed(uint8 speed) 
{
    /* Clear power mask then write in new value */
    Photodiode_Source_CR0 &= (uint8)(~Photodiode_Source_HS_MASK);
    Photodiode_Source_CR0 |=  (speed & Photodiode_Source_HS_MASK);
}


/*******************************************************************************
* Function Name: Photodiode_Source_SetRange
********************************************************************************
*
* Summary:
*  Set one of three current ranges.
*
* Parameters:
*  Range: Sets one of Three valid ranges.
*
* Return:
*  void 
*
* Theory:
*
* Side Effects:
*
*******************************************************************************/
void Photodiode_Source_SetRange(uint8 range) 
{
    Photodiode_Source_CR0 &= (uint8)(~Photodiode_Source_RANGE_MASK);      /* Clear existing mode */
    Photodiode_Source_CR0 |= (range & Photodiode_Source_RANGE_MASK);      /*  Set Range  */
    Photodiode_Source_DacTrim();
}


/*******************************************************************************
* Function Name: Photodiode_Source_SetValue
********************************************************************************
*
* Summary:
*  Set 8-bit DAC value
*
* Parameters:  
*  value:  Sets DAC value between 0 and 255.
*
* Return: 
*  void 
*
* Theory: 
*
* Side Effects:
*
*******************************************************************************/
void Photodiode_Source_SetValue(uint8 value) 
{
    #if (CY_PSOC5A)
        uint8 Photodiode_Source_intrStatus = CyEnterCriticalSection();
    #endif /* CY_PSOC5A */

    Photodiode_Source_Data = value;                /*  Set Value  */

    /* PSOC5A requires a double write */
    /* Exit Critical Section */
    #if (CY_PSOC5A)
        Photodiode_Source_Data = value;
        CyExitCriticalSection(Photodiode_Source_intrStatus);
    #endif /* CY_PSOC5A */
}


/*******************************************************************************
* Function Name: Photodiode_Source_DacTrim
********************************************************************************
*
* Summary:
*  Set the trim value for the given range.
*
* Parameters:
*  range:  1V or 4V range.  See constants.
*
* Return:
*  void
*
* Theory: 
*
* Side Effects:
*
*******************************************************************************/
void Photodiode_Source_DacTrim(void) 
{
    uint8 mode;

    mode = (uint8)((Photodiode_Source_CR0 & Photodiode_Source_RANGE_MASK) >> 2) + Photodiode_Source_TRIM_M7_1V_RNG_OFFSET;
    Photodiode_Source_TR = CY_GET_XTND_REG8((uint8 *)(Photodiode_Source_DAC_TRIM_BASE + mode));
}


/* [] END OF FILE */
