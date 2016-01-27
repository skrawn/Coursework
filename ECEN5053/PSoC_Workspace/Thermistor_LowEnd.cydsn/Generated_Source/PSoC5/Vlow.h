/*******************************************************************************
* File Name: Vlow.h  
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

#if !defined(CY_PINS_Vlow_H) /* Pins Vlow_H */
#define CY_PINS_Vlow_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "Vlow_aliases.h"

/* Check to see if required defines such as CY_PSOC5A are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5A)
    #error Component cy_pins_v1_90 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5A) */

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 Vlow__PORT == 15 && ((Vlow__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

void    Vlow_Write(uint8 value) ;
void    Vlow_SetDriveMode(uint8 mode) ;
uint8   Vlow_ReadDataReg(void) ;
uint8   Vlow_Read(void) ;
uint8   Vlow_ClearInterrupt(void) ;


/***************************************
*           API Constants        
***************************************/

/* Drive Modes */
#define Vlow_DM_ALG_HIZ         PIN_DM_ALG_HIZ
#define Vlow_DM_DIG_HIZ         PIN_DM_DIG_HIZ
#define Vlow_DM_RES_UP          PIN_DM_RES_UP
#define Vlow_DM_RES_DWN         PIN_DM_RES_DWN
#define Vlow_DM_OD_LO           PIN_DM_OD_LO
#define Vlow_DM_OD_HI           PIN_DM_OD_HI
#define Vlow_DM_STRONG          PIN_DM_STRONG
#define Vlow_DM_RES_UPDWN       PIN_DM_RES_UPDWN

/* Digital Port Constants */
#define Vlow_MASK               Vlow__MASK
#define Vlow_SHIFT              Vlow__SHIFT
#define Vlow_WIDTH              1u


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define Vlow_PS                     (* (reg8 *) Vlow__PS)
/* Data Register */
#define Vlow_DR                     (* (reg8 *) Vlow__DR)
/* Port Number */
#define Vlow_PRT_NUM                (* (reg8 *) Vlow__PRT) 
/* Connect to Analog Globals */                                                  
#define Vlow_AG                     (* (reg8 *) Vlow__AG)                       
/* Analog MUX bux enable */
#define Vlow_AMUX                   (* (reg8 *) Vlow__AMUX) 
/* Bidirectional Enable */                                                        
#define Vlow_BIE                    (* (reg8 *) Vlow__BIE)
/* Bit-mask for Aliased Register Access */
#define Vlow_BIT_MASK               (* (reg8 *) Vlow__BIT_MASK)
/* Bypass Enable */
#define Vlow_BYP                    (* (reg8 *) Vlow__BYP)
/* Port wide control signals */                                                   
#define Vlow_CTL                    (* (reg8 *) Vlow__CTL)
/* Drive Modes */
#define Vlow_DM0                    (* (reg8 *) Vlow__DM0) 
#define Vlow_DM1                    (* (reg8 *) Vlow__DM1)
#define Vlow_DM2                    (* (reg8 *) Vlow__DM2) 
/* Input Buffer Disable Override */
#define Vlow_INP_DIS                (* (reg8 *) Vlow__INP_DIS)
/* LCD Common or Segment Drive */
#define Vlow_LCD_COM_SEG            (* (reg8 *) Vlow__LCD_COM_SEG)
/* Enable Segment LCD */
#define Vlow_LCD_EN                 (* (reg8 *) Vlow__LCD_EN)
/* Slew Rate Control */
#define Vlow_SLW                    (* (reg8 *) Vlow__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define Vlow_PRTDSI__CAPS_SEL       (* (reg8 *) Vlow__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define Vlow_PRTDSI__DBL_SYNC_IN    (* (reg8 *) Vlow__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define Vlow_PRTDSI__OE_SEL0        (* (reg8 *) Vlow__PRTDSI__OE_SEL0) 
#define Vlow_PRTDSI__OE_SEL1        (* (reg8 *) Vlow__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define Vlow_PRTDSI__OUT_SEL0       (* (reg8 *) Vlow__PRTDSI__OUT_SEL0) 
#define Vlow_PRTDSI__OUT_SEL1       (* (reg8 *) Vlow__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define Vlow_PRTDSI__SYNC_OUT       (* (reg8 *) Vlow__PRTDSI__SYNC_OUT) 


#if defined(Vlow__INTSTAT)  /* Interrupt Registers */

    #define Vlow_INTSTAT                (* (reg8 *) Vlow__INTSTAT)
    #define Vlow_SNAP                   (* (reg8 *) Vlow__SNAP)

#endif /* Interrupt Registers */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_Vlow_H */


/* [] END OF FILE */
