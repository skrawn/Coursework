/*******************************************************************************
* File Name: S_H.c
* Version 1.40
*
* Description:
*  This file contains the function prototypes and constants used in
*  the SAMPLE/TRACK AND HOLD component.
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

#if !defined(CY_SAMPLE_HOLD_S_H_H) 
#define CY_SAMPLE_HOLD_S_H_H 

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h"

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component Sample_Hold_v1_40 requires cy_boot v3.0 or later
#endif /* (CY_ PSOC5LP) */


#if(!CY_PSOC5A)
    #if(CYDEV_VARIABLE_VDDA == 1)
        #if (!defined(CY_LIB_SC_BST_CLK_EN))
            #error Component Sample_Hold_v1_40 requires cy_boot v3.30 or later
        #endif /* (!defined(CY_LIB_SC_BST_CLK_EN)) */
    #endif /* CYDEV_VARIABLE_VDDA == 1 */
#endif /* (!CY_PSOC5A) */

/***************************************
*   Data Struct Definition
***************************************/

/* Low power Mode API Support */
typedef struct
{
    uint8   enableState;
    uint8   scCR0Reg;
    uint8   scCR1Reg;
    uint8   scCR2Reg;
}   S_H_backupStruct;

/* compinent init state */
extern uint8 S_H_initVar;


/***************************************
*        Function Prototypes 
***************************************/
    
void S_H_Init(void)                ;
void S_H_Enable(void)              ;
void S_H_Start(void)               ;
void S_H_Stop(void)                ;
void S_H_SetPower(uint8 power)     ;
void S_H_Sleep(void)               ;
void S_H_Wakeup(void)              ;
void S_H_SaveConfig(void)          ;
void S_H_RestoreConfig(void)       ;


/***************************************
*            API Constants        
***************************************/

/* Constants for Mixer Type */
#define S_H_SAMPLEANDHOLD          (0x01u)
#define S_H_TRACKANDHOLD           (0x02u)

/* Power constants for SetPower function */
#define S_H_MINPOWER               (0x00u)
#define S_H_LOWPOWER               (0x01u)
#define S_H_MEDPOWER               (0x02u)
#define S_H_HIGHPOWER              (0x03u)

/* Vref constats */
#define S_H_INTERNAL               (0x01u)
#define S_H_EXTERNAL               (0x02u)

/* Clock edge constats */
#define S_H_EDGE_NEGATIVE          (0x01u)
#define S_H_EDGE_POSITIVENEGATIVE  (0x02u)


/***************************************
*       Initialization Values
***************************************/

#define S_H_INIT_POWER             (2u)
#define S_H_SAMPLE_TRACK_MODE      (1u)
#define S_H_SAMPLE_CLOCK_EDGE      (1u)
#define S_H_VddaVal                (5u)
#define S_H_VREF_TYPE              (2u)


/***************************************
*              Registers        
***************************************/

#define S_H_CR0_REG                (* (reg8 *) S_H_SC__CR0 )
#define S_H_CR0_PTR                (  (reg8 *) S_H_SC__CR0 )
#define S_H_CR1_REG                (* (reg8 *) S_H_SC__CR1 )
#define S_H_CR1_PTR                (  (reg8 *) S_H_SC__CR1 )
#define S_H_CR2_REG                (* (reg8 *) S_H_SC__CR2 )
#define S_H_CR2_PTR                (  (reg8 *) S_H_SC__CR2 )
#define S_H_CLK_REG                (* (reg8 *) S_H_SC__CLK )  /* SC Block clk control */
#define S_H_CLK_PTR                (  (reg8 *) S_H_SC__CLK )
#define S_H_BSTCLK_REG             (* (reg8 *) S_H_SC__BST )
#define S_H_BSTCLK_PTR             (  (reg8 *) S_H_SC__BST )
#define S_H_PM_ACT_CFG_REG         (* (reg8 *) S_H_SC__PM_ACT_CFG )  /* Power manager */
#define S_H_PM_ACT_CFG_PTR         (  (reg8 *) S_H_SC__PM_ACT_CFG )
#define S_H_PM_STBY_CFG_REG        (* (reg8 *) S_H_SC__PM_STBY_CFG )  /* Power manager */
#define S_H_PM_STBY_CFG_PTR        (  (reg8 *) S_H_SC__PM_STBY_CFG )

/* Pump clock selectin register */
#define S_H_PUMP_CR1_REG           (* (reg8 *) CYDEV_ANAIF_CFG_PUMP_CR1)   
#define S_H_PUMP_CR1_PTR           (  (reg8 *) CYDEV_ANAIF_CFG_PUMP_CR1)

/* Pump Register for SC block */
#define S_H_SC_MISC_REG            (* (reg8 *) CYDEV_ANAIF_RT_SC_MISC)
#define S_H_SC_MISC_PTR            (  (reg8 *) CYDEV_ANAIF_RT_SC_MISC)

/* Power manager */
/* Active mode power enable register */
#define S_H_ACLK_PM_ACT_CFG_REG    (* (reg8 *) S_H_PM_ACT_CFG ) 
#define S_H_ACLK_PM_ACT_CFG_PTR    (  (reg8 *) S_H_PM_ACT_CFG ) 

/* Power manager */
/* Alternative Active mode power enable register */
#define S_H_ACLK_PM_STBY_CFG_REG   (* (reg8 *) S_H_PM_STBY_CFG )
#define S_H_ACLK_PM_STBY_CFG_PTR   (  (reg8 *) S_H_PM_STBY_CFG ) 

/* PM_ACT_CFG (Active Power Mode CFG Register)mask */ 
#define S_H_ACT_PWR_EN             S_H_SC__PM_ACT_MSK /* Power enable mask */

/* PM_STBY_CFG (Alternate Active Power Mode CFG Register)mask */ 
#define S_H_STBY_PWR_EN            S_H_SC__PM_STBY_MSK /* Power enable mask */


/***************************************
*            Register Constants        
***************************************/

/* ANIF.PUMP.CR1 Constants */
#define S_H_PUMP_CR1_SC_CLKSEL     (0x80u)

/* SC_MISC constants */
#define S_H_PUMP_FORCE             (0x20u)
#define S_H_PUMP_AUTO              (0x10u)
#define S_H_DIFF_PGA_1_3           (0x02u)
#define S_H_DIFF_PGA_0_2           (0x01u)

/* CR0 SC/CT Control Register 0 definitions */

/* Bit Field SC_MODE_ENUM - SCxx_CR0[3:1] */
#define S_H_MODE_SAMPLEANDHOLD     ((uint8)(0x03u << 1u))
#define S_H_MODE_TRACKANDHOLD      ((uint8)(0x07u << 1u))

/* CR1 SC/CT Control Register 1 definitions */

/* Bit Field  SC_DRIVE_ENUM - SCxx_CR1[1:0] */
#define S_H_DRIVE_MASK             (0x03u)
#define S_H_DRIVE_280UA            (0x00u)
#define S_H_DRIVE_420UA            (0x01u)
#define S_H_DRIVE_530UA            (0x02u)
#define S_H_DRIVE_650UA            (0x03u)

/* Bit Field  SC_COMP_ENUM - SCxx_CR1[3:2] */
#define S_H_COMP_MASK              ((uint8)(0x03u << 2u))
#define S_H_COMP_3P0PF             ((uint8)(0x00u << 2u))
#define S_H_COMP_3P6PF             ((uint8)(0x01u << 2u))
#define S_H_COMP_4P35PF            ((uint8)(0x02u << 2u))
#define S_H_COMP_5P1PF             ((uint8)(0x03u << 2u))

/* Bit Field  SC_DIV2_ENUM - SCxx_CR1[4] */
#define S_H_DIV2                   ((uint8)(0x01u << 4u))
#define S_H_DIV2_DISABLE           ((uint8)(0x00u << 4u))
#define S_H_DIV2_ENABLE            ((uint8)(0x01u << 4u))

/* Bit Field  SC_GAIN_ENUM - SCxx_CR1[5] */
#define S_H_GAIN                   ((uint8)(0x01u << 5u))
#define S_H_GAIN_0DB               ((uint8)(0x00u << 5u))
#define S_H_GAIN_6DB               ((uint8)(0x01u << 5u))

/* CR2 SC Block Control Register 2 definitions */

/* Bit Field  SC_BIAS_CONTROL_ENUM - SCxx_CR2[0] */
#define S_H_BIAS                   (0x01u)
#define S_H_BIAS_NORMAL            (0x00u)
#define S_H_BIAS_LOW               (0x01u)

/* Bit Field  SC_R20_40B_ENUM - SCxx_CR2[1] */
#define S_H_R20_40B                ((uint8)(0x01u << 1u))
#define S_H_R20_40B_40K            ((uint8)(0x00u << 1u))
#define S_H_R20_40B_20K            ((uint8)(0x01u << 1u))

/* Bit Field  SC_REDC_ENUM  - SCxx_CR2[3:2] */
#define S_H_REDC_MASK              ((uint8)(0x03u << 2u))
#define S_H_REDC_00                ((uint8)(0x00u << 2u))
#define S_H_REDC_01                ((uint8)(0x01u << 2u))
#define S_H_REDC_10                ((uint8)(0x02u << 2u))
#define S_H_REDC_11                ((uint8)(0x03u << 2u))

/* Bit Field  SC_RVAL_ENUM  - SCxx_CR2[6:4] */
#define S_H_RVAL_MASK              ((uint8)(0x07u << 4u))
#define S_H_RVAL_20K               ((uint8)(0x00u << 4u))
#define S_H_RVAL_30K               ((uint8)(0x01u << 4u))
#define S_H_RVAL_40K               ((uint8)(0x02u << 4u))
#define S_H_RVAL_80K               ((uint8)(0x03u << 4u))
#define S_H_RVAL_120K              ((uint8)(0x04u << 4u))
#define S_H_RVAL_250K              ((uint8)(0x05u << 4u))
#define S_H_RVAL_500K              ((uint8)(0x06u << 4u))
#define S_H_RVAL_1000K             ((uint8)(0x07u << 4u))

/* Bit Field  SC_PGA_GNDVREF_ENUM - SCxx_CR2[7] */
#define S_H_GNDVREF                ((uint8)(0x01u << 7u))
#define S_H_GNDVREF_DI             ((uint8)(0x00u << 7u))
#define S_H_GNDVREF_E              ((uint8)(0x01u << 7u)) 

/* SC Block Clock Control SCx.CLk */
#define S_H_DYN_CNTL_EN            ((uint8)(0x01u << 5u))
#define S_H_BYPASS_SYNC            ((uint8)(0x01u << 4u))
#define S_H_CLK_EN                 ((uint8)(0x01u << 3u))

/* SC Block Boost Clock Selection Register - Boost Clock Enable  SCxx_BST[3]  */
#define S_H_BST_CLK_EN             ((uint8)(0x01u << 3u))
#define S_H_BST_CLK_INDEX_MASK     (0x07u)
#define S_H_PM_ACT_CFG_MASK        (0x0Fu)

/* Constant for VDDA Threshold */
#define S_H_CYDEV_VDDA_MV               (CYDEV_VDDA_MV)
#define S_H_MINIMUM_VDDA_THRESHOLD_MV   (2700u)


/*******************************************************************************
* Following code are OBSOLETE and must not be used starting from SampleHold 1.40
*******************************************************************************/
/* Component's enable/disable state */
#define S_H_ENABLED             (0x01u)
#define S_H_DISABLED            (0x00u)
#define S_H_VDDA_THRESHOLD_MV   (3500u)
#if (CY_PSOC5A)
    /* For stop API changes mask to make the SC registers CR1 and CR2 null  */
    #define S_H_REG_NULL        (0x00u)
#endif /* CY_PSOC5A */

#endif /* CY_SAMPLE_HOLD_S_H_H */


/* [] END OF FILE */
