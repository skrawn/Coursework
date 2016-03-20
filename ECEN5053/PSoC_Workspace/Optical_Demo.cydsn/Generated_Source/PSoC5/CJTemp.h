/*******************************************************************************
* File Name: CJTemp.h  
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

#if !defined(CY_PINS_CJTemp_H) /* Pins CJTemp_H */
#define CY_PINS_CJTemp_H

#include "cytypes.h"
#include "cyfitter.h"
#include "cypins.h"
#include "CJTemp_aliases.h"

/* Check to see if required defines such as CY_PSOC5A are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5A)
    #error Component cy_pins_v1_90 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5A) */

/* APIs are not generated for P15[7:6] */
#if !(CY_PSOC5A &&\
	 CJTemp__PORT == 15 && ((CJTemp__MASK & 0xC0) != 0))


/***************************************
*        Function Prototypes             
***************************************/    

void    CJTemp_Write(uint8 value) ;
void    CJTemp_SetDriveMode(uint8 mode) ;
uint8   CJTemp_ReadDataReg(void) ;
uint8   CJTemp_Read(void) ;
uint8   CJTemp_ClearInterrupt(void) ;


/***************************************
*           API Constants        
***************************************/

/* Drive Modes */
#define CJTemp_DM_ALG_HIZ         PIN_DM_ALG_HIZ
#define CJTemp_DM_DIG_HIZ         PIN_DM_DIG_HIZ
#define CJTemp_DM_RES_UP          PIN_DM_RES_UP
#define CJTemp_DM_RES_DWN         PIN_DM_RES_DWN
#define CJTemp_DM_OD_LO           PIN_DM_OD_LO
#define CJTemp_DM_OD_HI           PIN_DM_OD_HI
#define CJTemp_DM_STRONG          PIN_DM_STRONG
#define CJTemp_DM_RES_UPDWN       PIN_DM_RES_UPDWN

/* Digital Port Constants */
#define CJTemp_MASK               CJTemp__MASK
#define CJTemp_SHIFT              CJTemp__SHIFT
#define CJTemp_WIDTH              1u


/***************************************
*             Registers        
***************************************/

/* Main Port Registers */
/* Pin State */
#define CJTemp_PS                     (* (reg8 *) CJTemp__PS)
/* Data Register */
#define CJTemp_DR                     (* (reg8 *) CJTemp__DR)
/* Port Number */
#define CJTemp_PRT_NUM                (* (reg8 *) CJTemp__PRT) 
/* Connect to Analog Globals */                                                  
#define CJTemp_AG                     (* (reg8 *) CJTemp__AG)                       
/* Analog MUX bux enable */
#define CJTemp_AMUX                   (* (reg8 *) CJTemp__AMUX) 
/* Bidirectional Enable */                                                        
#define CJTemp_BIE                    (* (reg8 *) CJTemp__BIE)
/* Bit-mask for Aliased Register Access */
#define CJTemp_BIT_MASK               (* (reg8 *) CJTemp__BIT_MASK)
/* Bypass Enable */
#define CJTemp_BYP                    (* (reg8 *) CJTemp__BYP)
/* Port wide control signals */                                                   
#define CJTemp_CTL                    (* (reg8 *) CJTemp__CTL)
/* Drive Modes */
#define CJTemp_DM0                    (* (reg8 *) CJTemp__DM0) 
#define CJTemp_DM1                    (* (reg8 *) CJTemp__DM1)
#define CJTemp_DM2                    (* (reg8 *) CJTemp__DM2) 
/* Input Buffer Disable Override */
#define CJTemp_INP_DIS                (* (reg8 *) CJTemp__INP_DIS)
/* LCD Common or Segment Drive */
#define CJTemp_LCD_COM_SEG            (* (reg8 *) CJTemp__LCD_COM_SEG)
/* Enable Segment LCD */
#define CJTemp_LCD_EN                 (* (reg8 *) CJTemp__LCD_EN)
/* Slew Rate Control */
#define CJTemp_SLW                    (* (reg8 *) CJTemp__SLW)

/* DSI Port Registers */
/* Global DSI Select Register */
#define CJTemp_PRTDSI__CAPS_SEL       (* (reg8 *) CJTemp__PRTDSI__CAPS_SEL) 
/* Double Sync Enable */
#define CJTemp_PRTDSI__DBL_SYNC_IN    (* (reg8 *) CJTemp__PRTDSI__DBL_SYNC_IN) 
/* Output Enable Select Drive Strength */
#define CJTemp_PRTDSI__OE_SEL0        (* (reg8 *) CJTemp__PRTDSI__OE_SEL0) 
#define CJTemp_PRTDSI__OE_SEL1        (* (reg8 *) CJTemp__PRTDSI__OE_SEL1) 
/* Port Pin Output Select Registers */
#define CJTemp_PRTDSI__OUT_SEL0       (* (reg8 *) CJTemp__PRTDSI__OUT_SEL0) 
#define CJTemp_PRTDSI__OUT_SEL1       (* (reg8 *) CJTemp__PRTDSI__OUT_SEL1) 
/* Sync Output Enable Registers */
#define CJTemp_PRTDSI__SYNC_OUT       (* (reg8 *) CJTemp__PRTDSI__SYNC_OUT) 


#if defined(CJTemp__INTSTAT)  /* Interrupt Registers */

    #define CJTemp_INTSTAT                (* (reg8 *) CJTemp__INTSTAT)
    #define CJTemp_SNAP                   (* (reg8 *) CJTemp__SNAP)

#endif /* Interrupt Registers */

#endif /* CY_PSOC5A... */

#endif /*  CY_PINS_CJTemp_H */


/* [] END OF FILE */
