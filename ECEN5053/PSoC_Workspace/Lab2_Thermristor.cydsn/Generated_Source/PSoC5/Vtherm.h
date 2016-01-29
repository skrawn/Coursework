/*******************************************************************************
* File Name: Vtherm.h  
* Version 1.90
*
* Description:
*  This file containts Control Register function prototypes and register defines
*
* Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_PINS_Vtherm_H) /* Pins Vtherm_H */
#define CY_PINS_Vtherm_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Vtherm_aliases.h"

/* Check to see if required defines such as CY_PSOC5A are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5A)
    #error Component cy_pins_v1_90 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5A) */

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Vtherm__PORT == 15 && ((Vtherm__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

void    Vtherm_Write(uint8 value) ;
void    Vtherm_SetDriveMode(uint8 mode) ;
uint8   Vtherm_ReadDataReg(void) ;
uint8   Vtherm_Read(void) ;
uint8   Vtherm_ClearInterrupt(void) ;


/***************************************
*           API Constants        
***************************************/

/* Drive Modes */
#define Vtherm_DM_ALG_HIZ         PIN_DM_ALG_HIZ
#define Vtherm_DM_DIG_HIZ         PIN_DM_DIG_HIZ
#define Vtherm_DM_RES_UP          PIN_DM_RES_UP
#define Vtherm_DM_RES_DWN         PIN_DM_RES_DWN
#define Vtherm_DM_OD_LO           PIN_DM_OD_LO
#define Vtherm_DM_OD_HI           PIN_DM_OD_HI
#define Vtherm_DM_STRONG          PIN_DM_STRONG
#define Vtherm_DM_RES_UPDWN       PIN_DM_RES_UPDWN

/* Digital Port Constants */
#define Vtherm_MASK               Vtherm__MASK
#define Vtherm_SHIFT              Vtherm__SHIFT
#define Vtherm_WIDTH              1u


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Vtherm_PS                     (* (reg8 *) Vtherm__PS)
/* Data Register */
#define Vtherm_DR                     (* (reg8 *) Vtherm__DR)
/* Port Number */
#define Vtherm_PRT_NUM                (* (reg8 *) Vtherm__PRT) 
/* Connect to Analog Globals */                                                  
#define Vtherm_AG                     (* (reg8 *) Vtherm__AG)                       
/* Analog MUX bux enable */
#define Vtherm_AMUX                   (* (reg8 *) Vtherm__AMUX) 
/* Bidirectional Enable */                                                        
#define Vtherm_BIE                    (* (reg8 *) Vtherm__BIE)
/* Bit-mask for Aliased Register Access */
#define Vtherm_BIT_MASK               (* (reg8 *) Vtherm__BIT_MASK)
/* Bypass Enable */
#define Vtherm_BYP                    (* (reg8 *) Vtherm__BYP)
/* Port wide control signals */                                                   
#define Vtherm_CTL                    (* (reg8 *) Vtherm__CTL)
/* Drive Modes */
#define Vtherm_DM0                    (* (reg8 *) Vtherm__DM0) 
#define Vtherm_DM1                    (* (reg8 *) Vtherm__DM1)
#define Vtherm_DM2                    (* (reg8 *) Vtherm__DM2) 
/* Input Buffer Disable Override */
#define Vtherm_INP_DIS                (* (reg8 *) Vtherm__INP_DIS)
/* LCD Common or Segment Drive */
#define Vtherm_LCD_COM_SEG            (* (reg8 *) Vtherm__LCD_COM_SEG)
/* Enable Segment LCD */
#define Vtherm_LCD_EN                 (* (reg8 *) Vtherm__LCD_EN)
/* Slew Rate Control */
#define Vtherm_SLW                    (* (reg8 *) Vtherm__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Vtherm_PRTDSI__CAPS_SEL       (* (reg8 *) Vtherm__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Vtherm_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Vtherm__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Vtherm_PRTDSI__OE_SEL0        (* (reg8 *) Vtherm__PRTDSI__OE_SEL0) 
#define Vtherm_PRTDSI__OE_SEL1        (* (reg8 *) Vtherm__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Vtherm_PRTDSI__OUT_SEL0       (* (reg8 *) Vtherm__PRTDSI__OUT_SEL0) 
#define Vtherm_PRTDSI__OUT_SEL1       (* (reg8 *) Vtherm__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Vtherm_PRTDSI__SYNC_OUT       (* (reg8 *) Vtherm__PRTDSI__SYNC_OUT) 


#if defined(Vtherm__INTSTAT)  /* Interrupt Registers */

    #define Vtherm_INTSTAT                (* (reg8 *) Vtherm__INTSTAT)
    #define Vtherm_SNAP                   (* (reg8 *) Vtherm__SNAP)

#endif /* Interrupt Registers */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Vtherm_H */


/* [] END OF FILE */
