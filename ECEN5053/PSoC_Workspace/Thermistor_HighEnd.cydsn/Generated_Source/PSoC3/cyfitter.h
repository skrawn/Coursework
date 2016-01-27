#ifndef INCLUDED_CYFITTER_H
#define INCLUDED_CYFITTER_H
#include <cydevice.h>
#include <cydevice_trm.h>

/* CapSense_CompCH0_ctComp */
#define CapSense_CompCH0_ctComp__CLK CYREG_CMP0_CLK
#define CapSense_CompCH0_ctComp__CMP_MASK 0x01u
#define CapSense_CompCH0_ctComp__CMP_NUMBER 0u
#define CapSense_CompCH0_ctComp__CR CYREG_CMP0_CR
#define CapSense_CompCH0_ctComp__LUT__CR CYREG_LUT0_CR
#define CapSense_CompCH0_ctComp__LUT__MSK CYREG_LUT_MSK
#define CapSense_CompCH0_ctComp__LUT__MSK_MASK 0x01u
#define CapSense_CompCH0_ctComp__LUT__MSK_SHIFT 0
#define CapSense_CompCH0_ctComp__LUT__MX CYREG_LUT0_MX
#define CapSense_CompCH0_ctComp__LUT__SR CYREG_LUT_SR
#define CapSense_CompCH0_ctComp__LUT__SR_MASK 0x01u
#define CapSense_CompCH0_ctComp__LUT__SR_SHIFT 0
#define CapSense_CompCH0_ctComp__PM_ACT_CFG CYREG_PM_ACT_CFG7
#define CapSense_CompCH0_ctComp__PM_ACT_MSK 0x01u
#define CapSense_CompCH0_ctComp__PM_STBY_CFG CYREG_PM_STBY_CFG7
#define CapSense_CompCH0_ctComp__PM_STBY_MSK 0x01u
#define CapSense_CompCH0_ctComp__SW0 CYREG_CMP0_SW0
#define CapSense_CompCH0_ctComp__SW2 CYREG_CMP0_SW2
#define CapSense_CompCH0_ctComp__SW3 CYREG_CMP0_SW3
#define CapSense_CompCH0_ctComp__SW4 CYREG_CMP0_SW4
#define CapSense_CompCH0_ctComp__SW6 CYREG_CMP0_SW6
#define CapSense_CompCH0_ctComp__TR0 CYREG_CMP0_TR0
#define CapSense_CompCH0_ctComp__TR1 CYREG_CMP0_TR1
#define CapSense_CompCH0_ctComp__TRIM__TR0 CYREG_FLSHID_MFG_CFG_CMP0_TR0
#define CapSense_CompCH0_ctComp__TRIM__TR0_HS CYREG_FLSHID_CUST_TABLES_CMP0_TR0_HS
#define CapSense_CompCH0_ctComp__TRIM__TR1 CYREG_FLSHID_MFG_CFG_CMP0_TR1
#define CapSense_CompCH0_ctComp__TRIM__TR1_HS CYREG_FLSHID_CUST_TABLES_CMP0_TR1_HS
#define CapSense_CompCH0_ctComp__WRK CYREG_CMP_WRK
#define CapSense_CompCH0_ctComp__WRK_MASK 0x01u
#define CapSense_CompCH0_ctComp__WRK_SHIFT 0

/* CapSense_IdacCH0_viDAC8 */
#define CapSense_IdacCH0_viDAC8__CR0 CYREG_DAC1_CR0
#define CapSense_IdacCH0_viDAC8__CR1 CYREG_DAC1_CR1
#define CapSense_IdacCH0_viDAC8__D CYREG_DAC1_D
#define CapSense_IdacCH0_viDAC8__PM_ACT_CFG CYREG_PM_ACT_CFG8
#define CapSense_IdacCH0_viDAC8__PM_ACT_MSK 0x02u
#define CapSense_IdacCH0_viDAC8__PM_STBY_CFG CYREG_PM_STBY_CFG8
#define CapSense_IdacCH0_viDAC8__PM_STBY_MSK 0x02u
#define CapSense_IdacCH0_viDAC8__STROBE CYREG_DAC1_STROBE
#define CapSense_IdacCH0_viDAC8__SW0 CYREG_DAC1_SW0
#define CapSense_IdacCH0_viDAC8__SW2 CYREG_DAC1_SW2
#define CapSense_IdacCH0_viDAC8__SW3 CYREG_DAC1_SW3
#define CapSense_IdacCH0_viDAC8__SW4 CYREG_DAC1_SW4
#define CapSense_IdacCH0_viDAC8__TR CYREG_DAC1_TR
#define CapSense_IdacCH0_viDAC8__TRIM__M1 CYREG_FLSHID_CUST_TABLES_DAC1_M1
#define CapSense_IdacCH0_viDAC8__TRIM__M2 CYREG_FLSHID_CUST_TABLES_DAC1_M2
#define CapSense_IdacCH0_viDAC8__TRIM__M3 CYREG_FLSHID_CUST_TABLES_DAC1_M3
#define CapSense_IdacCH0_viDAC8__TRIM__M4 CYREG_FLSHID_CUST_TABLES_DAC1_M4
#define CapSense_IdacCH0_viDAC8__TRIM__M5 CYREG_FLSHID_CUST_TABLES_DAC1_M5
#define CapSense_IdacCH0_viDAC8__TRIM__M6 CYREG_FLSHID_CUST_TABLES_DAC1_M6
#define CapSense_IdacCH0_viDAC8__TRIM__M7 CYREG_FLSHID_CUST_TABLES_DAC1_M7
#define CapSense_IdacCH0_viDAC8__TRIM__M8 CYREG_FLSHID_CUST_TABLES_DAC1_M8
#define CapSense_IdacCH0_viDAC8__TST CYREG_DAC1_TST

/* CapSense_MeasureCH0 */
#define CapSense_MeasureCH0_UDB_Counter_u0__16BIT_A0_REG CYREG_B0_UDB14_15_A0
#define CapSense_MeasureCH0_UDB_Counter_u0__16BIT_A1_REG CYREG_B0_UDB14_15_A1
#define CapSense_MeasureCH0_UDB_Counter_u0__16BIT_D0_REG CYREG_B0_UDB14_15_D0
#define CapSense_MeasureCH0_UDB_Counter_u0__16BIT_D1_REG CYREG_B0_UDB14_15_D1
#define CapSense_MeasureCH0_UDB_Counter_u0__16BIT_DP_AUX_CTL_REG CYREG_B0_UDB14_15_ACTL
#define CapSense_MeasureCH0_UDB_Counter_u0__16BIT_F0_REG CYREG_B0_UDB14_15_F0
#define CapSense_MeasureCH0_UDB_Counter_u0__16BIT_F1_REG CYREG_B0_UDB14_15_F1
#define CapSense_MeasureCH0_UDB_Counter_u0__A0_A1_REG CYREG_B0_UDB14_A0_A1
#define CapSense_MeasureCH0_UDB_Counter_u0__A0_REG CYREG_B0_UDB14_A0
#define CapSense_MeasureCH0_UDB_Counter_u0__A1_REG CYREG_B0_UDB14_A1
#define CapSense_MeasureCH0_UDB_Counter_u0__D0_D1_REG CYREG_B0_UDB14_D0_D1
#define CapSense_MeasureCH0_UDB_Counter_u0__D0_REG CYREG_B0_UDB14_D0
#define CapSense_MeasureCH0_UDB_Counter_u0__D1_REG CYREG_B0_UDB14_D1
#define CapSense_MeasureCH0_UDB_Counter_u0__DP_AUX_CTL_REG CYREG_B0_UDB14_ACTL
#define CapSense_MeasureCH0_UDB_Counter_u0__F0_F1_REG CYREG_B0_UDB14_F0_F1
#define CapSense_MeasureCH0_UDB_Counter_u0__F0_REG CYREG_B0_UDB14_F0
#define CapSense_MeasureCH0_UDB_Counter_u0__F1_REG CYREG_B0_UDB14_F1
#define CapSense_MeasureCH0_UDB_Window_u0__16BIT_A0_REG CYREG_B0_UDB10_11_A0
#define CapSense_MeasureCH0_UDB_Window_u0__16BIT_A1_REG CYREG_B0_UDB10_11_A1
#define CapSense_MeasureCH0_UDB_Window_u0__16BIT_D0_REG CYREG_B0_UDB10_11_D0
#define CapSense_MeasureCH0_UDB_Window_u0__16BIT_D1_REG CYREG_B0_UDB10_11_D1
#define CapSense_MeasureCH0_UDB_Window_u0__16BIT_DP_AUX_CTL_REG CYREG_B0_UDB10_11_ACTL
#define CapSense_MeasureCH0_UDB_Window_u0__16BIT_F0_REG CYREG_B0_UDB10_11_F0
#define CapSense_MeasureCH0_UDB_Window_u0__16BIT_F1_REG CYREG_B0_UDB10_11_F1
#define CapSense_MeasureCH0_UDB_Window_u0__A0_A1_REG CYREG_B0_UDB10_A0_A1
#define CapSense_MeasureCH0_UDB_Window_u0__A0_REG CYREG_B0_UDB10_A0
#define CapSense_MeasureCH0_UDB_Window_u0__A1_REG CYREG_B0_UDB10_A1
#define CapSense_MeasureCH0_UDB_Window_u0__D0_D1_REG CYREG_B0_UDB10_D0_D1
#define CapSense_MeasureCH0_UDB_Window_u0__D0_REG CYREG_B0_UDB10_D0
#define CapSense_MeasureCH0_UDB_Window_u0__D1_REG CYREG_B0_UDB10_D1
#define CapSense_MeasureCH0_UDB_Window_u0__DP_AUX_CTL_REG CYREG_B0_UDB10_ACTL
#define CapSense_MeasureCH0_UDB_Window_u0__F0_F1_REG CYREG_B0_UDB10_F0_F1
#define CapSense_MeasureCH0_UDB_Window_u0__F0_REG CYREG_B0_UDB10_F0
#define CapSense_MeasureCH0_UDB_Window_u0__F1_REG CYREG_B0_UDB10_F1
#define CapSense_MeasureCH0_UDB_Window_u0__MSK_DP_AUX_CTL_REG CYREG_B0_UDB10_MSK_ACTL
#define CapSense_MeasureCH0_UDB_Window_u0__PER_DP_AUX_CTL_REG CYREG_B0_UDB10_MSK_ACTL

/* CapSense_ClockGen */
#define CapSense_ClockGen_ScanSpeed__16BIT_CONTROL_AUX_CTL_REG CYREG_B0_UDB12_13_ACTL
#define CapSense_ClockGen_ScanSpeed__16BIT_CONTROL_CONTROL_REG CYREG_B0_UDB12_13_CTL
#define CapSense_ClockGen_ScanSpeed__16BIT_CONTROL_COUNT_REG CYREG_B0_UDB12_13_CTL
#define CapSense_ClockGen_ScanSpeed__16BIT_COUNT_CONTROL_REG CYREG_B0_UDB12_13_CTL
#define CapSense_ClockGen_ScanSpeed__16BIT_COUNT_COUNT_REG CYREG_B0_UDB12_13_CTL
#define CapSense_ClockGen_ScanSpeed__16BIT_MASK_MASK_REG CYREG_B0_UDB12_13_MSK
#define CapSense_ClockGen_ScanSpeed__16BIT_MASK_PERIOD_REG CYREG_B0_UDB12_13_MSK
#define CapSense_ClockGen_ScanSpeed__16BIT_PERIOD_MASK_REG CYREG_B0_UDB12_13_MSK
#define CapSense_ClockGen_ScanSpeed__16BIT_PERIOD_PERIOD_REG CYREG_B0_UDB12_13_MSK
#define CapSense_ClockGen_ScanSpeed__CONTROL_AUX_CTL_REG CYREG_B0_UDB12_ACTL
#define CapSense_ClockGen_ScanSpeed__CONTROL_REG CYREG_B0_UDB12_CTL
#define CapSense_ClockGen_ScanSpeed__CONTROL_ST_REG CYREG_B0_UDB12_ST_CTL
#define CapSense_ClockGen_ScanSpeed__COUNT_REG CYREG_B0_UDB12_CTL
#define CapSense_ClockGen_ScanSpeed__COUNT_ST_REG CYREG_B0_UDB12_ST_CTL
#define CapSense_ClockGen_ScanSpeed__MASK_CTL_AUX_CTL_REG CYREG_B0_UDB12_MSK_ACTL
#define CapSense_ClockGen_ScanSpeed__PERIOD_REG CYREG_B0_UDB12_MSK
#define CapSense_ClockGen_ScanSpeed__PER_CTL_AUX_CTL_REG CYREG_B0_UDB12_MSK_ACTL
#define CapSense_ClockGen_SyncCtrl_CtrlReg__0__MASK 0x01u
#define CapSense_ClockGen_SyncCtrl_CtrlReg__0__POS 0
#define CapSense_ClockGen_SyncCtrl_CtrlReg__16BIT_CONTROL_AUX_CTL_REG CYREG_B0_UDB10_11_ACTL
#define CapSense_ClockGen_SyncCtrl_CtrlReg__16BIT_CONTROL_CONTROL_REG CYREG_B0_UDB10_11_CTL
#define CapSense_ClockGen_SyncCtrl_CtrlReg__16BIT_CONTROL_COUNT_REG CYREG_B0_UDB10_11_CTL
#define CapSense_ClockGen_SyncCtrl_CtrlReg__16BIT_COUNT_CONTROL_REG CYREG_B0_UDB10_11_CTL
#define CapSense_ClockGen_SyncCtrl_CtrlReg__16BIT_COUNT_COUNT_REG CYREG_B0_UDB10_11_CTL
#define CapSense_ClockGen_SyncCtrl_CtrlReg__16BIT_MASK_MASK_REG CYREG_B0_UDB10_11_MSK
#define CapSense_ClockGen_SyncCtrl_CtrlReg__16BIT_MASK_PERIOD_REG CYREG_B0_UDB10_11_MSK
#define CapSense_ClockGen_SyncCtrl_CtrlReg__16BIT_PERIOD_MASK_REG CYREG_B0_UDB10_11_MSK
#define CapSense_ClockGen_SyncCtrl_CtrlReg__16BIT_PERIOD_PERIOD_REG CYREG_B0_UDB10_11_MSK
#define CapSense_ClockGen_SyncCtrl_CtrlReg__1__MASK 0x02u
#define CapSense_ClockGen_SyncCtrl_CtrlReg__1__POS 1
#define CapSense_ClockGen_SyncCtrl_CtrlReg__2__MASK 0x04u
#define CapSense_ClockGen_SyncCtrl_CtrlReg__2__POS 2
#define CapSense_ClockGen_SyncCtrl_CtrlReg__4__MASK 0x10u
#define CapSense_ClockGen_SyncCtrl_CtrlReg__4__POS 4
#define CapSense_ClockGen_SyncCtrl_CtrlReg__CONTROL_AUX_CTL_REG CYREG_B0_UDB10_ACTL
#define CapSense_ClockGen_SyncCtrl_CtrlReg__CONTROL_REG CYREG_B0_UDB10_CTL
#define CapSense_ClockGen_SyncCtrl_CtrlReg__CONTROL_ST_REG CYREG_B0_UDB10_ST_CTL
#define CapSense_ClockGen_SyncCtrl_CtrlReg__COUNT_REG CYREG_B0_UDB10_CTL
#define CapSense_ClockGen_SyncCtrl_CtrlReg__COUNT_ST_REG CYREG_B0_UDB10_ST_CTL
#define CapSense_ClockGen_SyncCtrl_CtrlReg__MASK 0x17u
#define CapSense_ClockGen_SyncCtrl_CtrlReg__MASK_CTL_AUX_CTL_REG CYREG_B0_UDB10_MSK_ACTL
#define CapSense_ClockGen_SyncCtrl_CtrlReg__PERIOD_REG CYREG_B0_UDB10_MSK
#define CapSense_ClockGen_SyncCtrl_CtrlReg__PER_CTL_AUX_CTL_REG CYREG_B0_UDB10_MSK_ACTL
#define CapSense_ClockGen_UDB_PrescalerDp_u0__A0_A1_REG CYREG_B0_UDB15_A0_A1
#define CapSense_ClockGen_UDB_PrescalerDp_u0__A0_REG CYREG_B0_UDB15_A0
#define CapSense_ClockGen_UDB_PrescalerDp_u0__A1_REG CYREG_B0_UDB15_A1
#define CapSense_ClockGen_UDB_PrescalerDp_u0__D0_D1_REG CYREG_B0_UDB15_D0_D1
#define CapSense_ClockGen_UDB_PrescalerDp_u0__D0_REG CYREG_B0_UDB15_D0
#define CapSense_ClockGen_UDB_PrescalerDp_u0__D1_REG CYREG_B0_UDB15_D1
#define CapSense_ClockGen_UDB_PrescalerDp_u0__DP_AUX_CTL_REG CYREG_B0_UDB15_ACTL
#define CapSense_ClockGen_UDB_PrescalerDp_u0__F0_F1_REG CYREG_B0_UDB15_F0_F1
#define CapSense_ClockGen_UDB_PrescalerDp_u0__F0_REG CYREG_B0_UDB15_F0
#define CapSense_ClockGen_UDB_PrescalerDp_u0__F1_REG CYREG_B0_UDB15_F1
#define CapSense_ClockGen_sC16_PRSdp_u0__16BIT_A0_REG CYREG_B0_UDB12_13_A0
#define CapSense_ClockGen_sC16_PRSdp_u0__16BIT_A1_REG CYREG_B0_UDB12_13_A1
#define CapSense_ClockGen_sC16_PRSdp_u0__16BIT_D0_REG CYREG_B0_UDB12_13_D0
#define CapSense_ClockGen_sC16_PRSdp_u0__16BIT_D1_REG CYREG_B0_UDB12_13_D1
#define CapSense_ClockGen_sC16_PRSdp_u0__16BIT_DP_AUX_CTL_REG CYREG_B0_UDB12_13_ACTL
#define CapSense_ClockGen_sC16_PRSdp_u0__16BIT_F0_REG CYREG_B0_UDB12_13_F0
#define CapSense_ClockGen_sC16_PRSdp_u0__16BIT_F1_REG CYREG_B0_UDB12_13_F1
#define CapSense_ClockGen_sC16_PRSdp_u0__A0_A1_REG CYREG_B0_UDB12_A0_A1
#define CapSense_ClockGen_sC16_PRSdp_u0__A0_REG CYREG_B0_UDB12_A0
#define CapSense_ClockGen_sC16_PRSdp_u0__A1_REG CYREG_B0_UDB12_A1
#define CapSense_ClockGen_sC16_PRSdp_u0__D0_D1_REG CYREG_B0_UDB12_D0_D1
#define CapSense_ClockGen_sC16_PRSdp_u0__D0_REG CYREG_B0_UDB12_D0
#define CapSense_ClockGen_sC16_PRSdp_u0__D1_REG CYREG_B0_UDB12_D1
#define CapSense_ClockGen_sC16_PRSdp_u0__DP_AUX_CTL_REG CYREG_B0_UDB12_ACTL
#define CapSense_ClockGen_sC16_PRSdp_u0__F0_F1_REG CYREG_B0_UDB12_F0_F1
#define CapSense_ClockGen_sC16_PRSdp_u0__F0_REG CYREG_B0_UDB12_F0
#define CapSense_ClockGen_sC16_PRSdp_u0__F1_REG CYREG_B0_UDB12_F1
#define CapSense_ClockGen_sC16_PRSdp_u0__MSK_DP_AUX_CTL_REG CYREG_B0_UDB12_MSK_ACTL
#define CapSense_ClockGen_sC16_PRSdp_u0__PER_DP_AUX_CTL_REG CYREG_B0_UDB12_MSK_ACTL
#define CapSense_ClockGen_sC16_PRSdp_u1__16BIT_A0_REG CYREG_B0_UDB13_14_A0
#define CapSense_ClockGen_sC16_PRSdp_u1__16BIT_A1_REG CYREG_B0_UDB13_14_A1
#define CapSense_ClockGen_sC16_PRSdp_u1__16BIT_D0_REG CYREG_B0_UDB13_14_D0
#define CapSense_ClockGen_sC16_PRSdp_u1__16BIT_D1_REG CYREG_B0_UDB13_14_D1
#define CapSense_ClockGen_sC16_PRSdp_u1__16BIT_DP_AUX_CTL_REG CYREG_B0_UDB13_14_ACTL
#define CapSense_ClockGen_sC16_PRSdp_u1__16BIT_F0_REG CYREG_B0_UDB13_14_F0
#define CapSense_ClockGen_sC16_PRSdp_u1__16BIT_F1_REG CYREG_B0_UDB13_14_F1
#define CapSense_ClockGen_sC16_PRSdp_u1__A0_A1_REG CYREG_B0_UDB13_A0_A1
#define CapSense_ClockGen_sC16_PRSdp_u1__A0_REG CYREG_B0_UDB13_A0
#define CapSense_ClockGen_sC16_PRSdp_u1__A1_REG CYREG_B0_UDB13_A1
#define CapSense_ClockGen_sC16_PRSdp_u1__D0_D1_REG CYREG_B0_UDB13_D0_D1
#define CapSense_ClockGen_sC16_PRSdp_u1__D0_REG CYREG_B0_UDB13_D0
#define CapSense_ClockGen_sC16_PRSdp_u1__D1_REG CYREG_B0_UDB13_D1
#define CapSense_ClockGen_sC16_PRSdp_u1__DP_AUX_CTL_REG CYREG_B0_UDB13_ACTL
#define CapSense_ClockGen_sC16_PRSdp_u1__F0_F1_REG CYREG_B0_UDB13_F0_F1
#define CapSense_ClockGen_sC16_PRSdp_u1__F0_REG CYREG_B0_UDB13_F0
#define CapSense_ClockGen_sC16_PRSdp_u1__F1_REG CYREG_B0_UDB13_F1

/* CapSense_IntClock */
#define CapSense_IntClock__CFG0 CYREG_CLKDIST_DCFG1_CFG0
#define CapSense_IntClock__CFG1 CYREG_CLKDIST_DCFG1_CFG1
#define CapSense_IntClock__CFG2 CYREG_CLKDIST_DCFG1_CFG2
#define CapSense_IntClock__CFG2_SRC_SEL_MASK 0x07u
#define CapSense_IntClock__INDEX 0x01u
#define CapSense_IntClock__PM_ACT_CFG CYREG_PM_ACT_CFG2
#define CapSense_IntClock__PM_ACT_MSK 0x02u
#define CapSense_IntClock__PM_STBY_CFG CYREG_PM_STBY_CFG2
#define CapSense_IntClock__PM_STBY_MSK 0x02u

/* CapSense_CmodCH0 */
#define CapSense_CmodCH0__0__MASK 0x10u
#define CapSense_CmodCH0__0__PC CYREG_PRT6_PC4
#define CapSense_CmodCH0__0__PORT 6u
#define CapSense_CmodCH0__0__SHIFT 4
#define CapSense_CmodCH0__AG CYREG_PRT6_AG
#define CapSense_CmodCH0__AMUX CYREG_PRT6_AMUX
#define CapSense_CmodCH0__BIE CYREG_PRT6_BIE
#define CapSense_CmodCH0__BIT_MASK CYREG_PRT6_BIT_MASK
#define CapSense_CmodCH0__BYP CYREG_PRT6_BYP
#define CapSense_CmodCH0__CTL CYREG_PRT6_CTL
#define CapSense_CmodCH0__Cmod_CH0__MASK 0x10u
#define CapSense_CmodCH0__Cmod_CH0__PC CYREG_PRT6_PC4
#define CapSense_CmodCH0__Cmod_CH0__PORT 6u
#define CapSense_CmodCH0__Cmod_CH0__SHIFT 4
#define CapSense_CmodCH0__DM0 CYREG_PRT6_DM0
#define CapSense_CmodCH0__DM1 CYREG_PRT6_DM1
#define CapSense_CmodCH0__DM2 CYREG_PRT6_DM2
#define CapSense_CmodCH0__DR CYREG_PRT6_DR
#define CapSense_CmodCH0__INP_DIS CYREG_PRT6_INP_DIS
#define CapSense_CmodCH0__LCD_COM_SEG CYREG_PRT6_LCD_COM_SEG
#define CapSense_CmodCH0__LCD_EN CYREG_PRT6_LCD_EN
#define CapSense_CmodCH0__MASK 0x10u
#define CapSense_CmodCH0__PORT 6u
#define CapSense_CmodCH0__PRT CYREG_PRT6_PRT
#define CapSense_CmodCH0__PRTDSI__CAPS_SEL CYREG_PRT6_CAPS_SEL
#define CapSense_CmodCH0__PRTDSI__DBL_SYNC_IN CYREG_PRT6_DBL_SYNC_IN
#define CapSense_CmodCH0__PRTDSI__OE_SEL0 CYREG_PRT6_OE_SEL0
#define CapSense_CmodCH0__PRTDSI__OE_SEL1 CYREG_PRT6_OE_SEL1
#define CapSense_CmodCH0__PRTDSI__OUT_SEL0 CYREG_PRT6_OUT_SEL0
#define CapSense_CmodCH0__PRTDSI__OUT_SEL1 CYREG_PRT6_OUT_SEL1
#define CapSense_CmodCH0__PRTDSI__SYNC_OUT CYREG_PRT6_SYNC_OUT
#define CapSense_CmodCH0__PS CYREG_PRT6_PS
#define CapSense_CmodCH0__SHIFT 4
#define CapSense_CmodCH0__SLW CYREG_PRT6_SLW

/* CapSense_PortCH0 */
#define CapSense_PortCH0__0__MASK 0x20u
#define CapSense_PortCH0__0__PC CYREG_PRT5_PC5
#define CapSense_PortCH0__0__PORT 5u
#define CapSense_PortCH0__0__SHIFT 5
#define CapSense_PortCH0__1__MASK 0x40u
#define CapSense_PortCH0__1__PC CYREG_PRT5_PC6
#define CapSense_PortCH0__1__PORT 5u
#define CapSense_PortCH0__1__SHIFT 6
#define CapSense_PortCH0__2__MASK 0x01u
#define CapSense_PortCH0__2__PC CYREG_PRT5_PC0
#define CapSense_PortCH0__2__PORT 5u
#define CapSense_PortCH0__2__SHIFT 0
#define CapSense_PortCH0__3__MASK 0x02u
#define CapSense_PortCH0__3__PC CYREG_PRT5_PC1
#define CapSense_PortCH0__3__PORT 5u
#define CapSense_PortCH0__3__SHIFT 1
#define CapSense_PortCH0__4__MASK 0x04u
#define CapSense_PortCH0__4__PC CYREG_PRT5_PC2
#define CapSense_PortCH0__4__PORT 5u
#define CapSense_PortCH0__4__SHIFT 2
#define CapSense_PortCH0__5__MASK 0x08u
#define CapSense_PortCH0__5__PC CYREG_PRT5_PC3
#define CapSense_PortCH0__5__PORT 5u
#define CapSense_PortCH0__5__SHIFT 3
#define CapSense_PortCH0__6__MASK 0x10u
#define CapSense_PortCH0__6__PC CYREG_PRT5_PC4
#define CapSense_PortCH0__6__PORT 5u
#define CapSense_PortCH0__6__SHIFT 4
#define CapSense_PortCH0__AG CYREG_PRT5_AG
#define CapSense_PortCH0__AMUX CYREG_PRT5_AMUX
#define CapSense_PortCH0__BIE CYREG_PRT5_BIE
#define CapSense_PortCH0__BIT_MASK CYREG_PRT5_BIT_MASK
#define CapSense_PortCH0__BYP CYREG_PRT5_BYP
#define CapSense_PortCH0__Button0__BTN__MASK 0x20u
#define CapSense_PortCH0__Button0__BTN__PC CYREG_PRT5_PC5
#define CapSense_PortCH0__Button0__BTN__PORT 5u
#define CapSense_PortCH0__Button0__BTN__SHIFT 5
#define CapSense_PortCH0__Button1__BTN__MASK 0x40u
#define CapSense_PortCH0__Button1__BTN__PC CYREG_PRT5_PC6
#define CapSense_PortCH0__Button1__BTN__PORT 5u
#define CapSense_PortCH0__Button1__BTN__SHIFT 6
#define CapSense_PortCH0__CTL CYREG_PRT5_CTL
#define CapSense_PortCH0__DM0 CYREG_PRT5_DM0
#define CapSense_PortCH0__DM1 CYREG_PRT5_DM1
#define CapSense_PortCH0__DM2 CYREG_PRT5_DM2
#define CapSense_PortCH0__DR CYREG_PRT5_DR
#define CapSense_PortCH0__INP_DIS CYREG_PRT5_INP_DIS
#define CapSense_PortCH0__LCD_COM_SEG CYREG_PRT5_LCD_COM_SEG
#define CapSense_PortCH0__LCD_EN CYREG_PRT5_LCD_EN
#define CapSense_PortCH0__LinearSlider0_e0__LS__MASK 0x01u
#define CapSense_PortCH0__LinearSlider0_e0__LS__PC CYREG_PRT5_PC0
#define CapSense_PortCH0__LinearSlider0_e0__LS__PORT 5u
#define CapSense_PortCH0__LinearSlider0_e0__LS__SHIFT 0
#define CapSense_PortCH0__LinearSlider0_e1__LS__MASK 0x02u
#define CapSense_PortCH0__LinearSlider0_e1__LS__PC CYREG_PRT5_PC1
#define CapSense_PortCH0__LinearSlider0_e1__LS__PORT 5u
#define CapSense_PortCH0__LinearSlider0_e1__LS__SHIFT 1
#define CapSense_PortCH0__LinearSlider0_e2__LS__MASK 0x04u
#define CapSense_PortCH0__LinearSlider0_e2__LS__PC CYREG_PRT5_PC2
#define CapSense_PortCH0__LinearSlider0_e2__LS__PORT 5u
#define CapSense_PortCH0__LinearSlider0_e2__LS__SHIFT 2
#define CapSense_PortCH0__LinearSlider0_e3__LS__MASK 0x08u
#define CapSense_PortCH0__LinearSlider0_e3__LS__PC CYREG_PRT5_PC3
#define CapSense_PortCH0__LinearSlider0_e3__LS__PORT 5u
#define CapSense_PortCH0__LinearSlider0_e3__LS__SHIFT 3
#define CapSense_PortCH0__LinearSlider0_e4__LS__MASK 0x10u
#define CapSense_PortCH0__LinearSlider0_e4__LS__PC CYREG_PRT5_PC4
#define CapSense_PortCH0__LinearSlider0_e4__LS__PORT 5u
#define CapSense_PortCH0__LinearSlider0_e4__LS__SHIFT 4
#define CapSense_PortCH0__PORT 5u
#define CapSense_PortCH0__PRT CYREG_PRT5_PRT
#define CapSense_PortCH0__PRTDSI__CAPS_SEL CYREG_PRT5_CAPS_SEL
#define CapSense_PortCH0__PRTDSI__DBL_SYNC_IN CYREG_PRT5_DBL_SYNC_IN
#define CapSense_PortCH0__PRTDSI__OE_SEL0 CYREG_PRT5_OE_SEL0
#define CapSense_PortCH0__PRTDSI__OE_SEL1 CYREG_PRT5_OE_SEL1
#define CapSense_PortCH0__PRTDSI__OUT_SEL0 CYREG_PRT5_OUT_SEL0
#define CapSense_PortCH0__PRTDSI__OUT_SEL1 CYREG_PRT5_OUT_SEL1
#define CapSense_PortCH0__PRTDSI__SYNC_OUT CYREG_PRT5_SYNC_OUT
#define CapSense_PortCH0__PS CYREG_PRT5_PS
#define CapSense_PortCH0__SLW CYREG_PRT5_SLW

/* CapSense_BufCH0 */
#define CapSense_BufCH0__CFG0 CYREG_CAPSL_CFG0
#define CapSense_BufCH0__CFG1 CYREG_CAPSL_CFG1
#define CapSense_BufCH0__PM_ACT_CFG CYREG_PM_ACT_CFG11
#define CapSense_BufCH0__PM_ACT_MSK 0x10u
#define CapSense_BufCH0__PM_STBY_CFG CYREG_PM_STBY_CFG11
#define CapSense_BufCH0__PM_STBY_MSK 0x10u

/* CapSense_IsrCH0 */
#define CapSense_IsrCH0__ES2_PATCH 0u
#define CapSense_IsrCH0__INTC_CLR_EN_REG CYREG_INTC_CLR_EN0
#define CapSense_IsrCH0__INTC_CLR_PD_REG CYREG_INTC_CLR_PD0
#define CapSense_IsrCH0__INTC_MASK 0x01u
#define CapSense_IsrCH0__INTC_NUMBER 0u
#define CapSense_IsrCH0__INTC_PRIOR_NUM 7u
#define CapSense_IsrCH0__INTC_PRIOR_REG CYREG_INTC_PRIOR0
#define CapSense_IsrCH0__INTC_SET_EN_REG CYREG_INTC_SET_EN0
#define CapSense_IsrCH0__INTC_SET_PD_REG CYREG_INTC_SET_PD0
#define CapSense_IsrCH0__INTC_VECT (CYREG_INTC_VECT_MBASE+0x00u)

/* isr_Calibration */
#define isr_Calibration__ES2_PATCH 0u
#define isr_Calibration__INTC_CLR_EN_REG CYREG_INTC_CLR_EN0
#define isr_Calibration__INTC_CLR_PD_REG CYREG_INTC_CLR_PD0
#define isr_Calibration__INTC_MASK 0x02u
#define isr_Calibration__INTC_NUMBER 1u
#define isr_Calibration__INTC_PRIOR_NUM 7u
#define isr_Calibration__INTC_PRIOR_REG CYREG_INTC_PRIOR1
#define isr_Calibration__INTC_SET_EN_REG CYREG_INTC_SET_EN0
#define isr_Calibration__INTC_SET_PD_REG CYREG_INTC_SET_PD0
#define isr_Calibration__INTC_VECT (CYREG_INTC_VECT_MBASE+0x02u)

/* ADC_Ext_CP_Clk */
#define ADC_Ext_CP_Clk__CFG0 CYREG_CLKDIST_DCFG0_CFG0
#define ADC_Ext_CP_Clk__CFG1 CYREG_CLKDIST_DCFG0_CFG1
#define ADC_Ext_CP_Clk__CFG2 CYREG_CLKDIST_DCFG0_CFG2
#define ADC_Ext_CP_Clk__CFG2_SRC_SEL_MASK 0x07u
#define ADC_Ext_CP_Clk__INDEX 0x00u
#define ADC_Ext_CP_Clk__PM_ACT_CFG CYREG_PM_ACT_CFG2
#define ADC_Ext_CP_Clk__PM_ACT_MSK 0x01u
#define ADC_Ext_CP_Clk__PM_STBY_CFG CYREG_PM_STBY_CFG2
#define ADC_Ext_CP_Clk__PM_STBY_MSK 0x01u

/* VDAC8_viDAC8 */
#define VDAC8_viDAC8__CR0 CYREG_DAC0_CR0
#define VDAC8_viDAC8__CR1 CYREG_DAC0_CR1
#define VDAC8_viDAC8__D CYREG_DAC0_D
#define VDAC8_viDAC8__PM_ACT_CFG CYREG_PM_ACT_CFG8
#define VDAC8_viDAC8__PM_ACT_MSK 0x01u
#define VDAC8_viDAC8__PM_STBY_CFG CYREG_PM_STBY_CFG8
#define VDAC8_viDAC8__PM_STBY_MSK 0x01u
#define VDAC8_viDAC8__STROBE CYREG_DAC0_STROBE
#define VDAC8_viDAC8__SW0 CYREG_DAC0_SW0
#define VDAC8_viDAC8__SW2 CYREG_DAC0_SW2
#define VDAC8_viDAC8__SW3 CYREG_DAC0_SW3
#define VDAC8_viDAC8__SW4 CYREG_DAC0_SW4
#define VDAC8_viDAC8__TR CYREG_DAC0_TR
#define VDAC8_viDAC8__TRIM__M1 CYREG_FLSHID_CUST_TABLES_DAC0_M1
#define VDAC8_viDAC8__TRIM__M2 CYREG_FLSHID_CUST_TABLES_DAC0_M2
#define VDAC8_viDAC8__TRIM__M3 CYREG_FLSHID_CUST_TABLES_DAC0_M3
#define VDAC8_viDAC8__TRIM__M4 CYREG_FLSHID_CUST_TABLES_DAC0_M4
#define VDAC8_viDAC8__TRIM__M5 CYREG_FLSHID_CUST_TABLES_DAC0_M5
#define VDAC8_viDAC8__TRIM__M6 CYREG_FLSHID_CUST_TABLES_DAC0_M6
#define VDAC8_viDAC8__TRIM__M7 CYREG_FLSHID_CUST_TABLES_DAC0_M7
#define VDAC8_viDAC8__TRIM__M8 CYREG_FLSHID_CUST_TABLES_DAC0_M8
#define VDAC8_viDAC8__TST CYREG_DAC0_TST

/* ADC_theACLK */
#define ADC_theACLK__CFG0 CYREG_CLKDIST_ACFG0_CFG0
#define ADC_theACLK__CFG1 CYREG_CLKDIST_ACFG0_CFG1
#define ADC_theACLK__CFG2 CYREG_CLKDIST_ACFG0_CFG2
#define ADC_theACLK__CFG2_SRC_SEL_MASK 0x07u
#define ADC_theACLK__CFG3 CYREG_CLKDIST_ACFG0_CFG3
#define ADC_theACLK__CFG3_PHASE_DLY_MASK 0x0Fu
#define ADC_theACLK__INDEX 0x00u
#define ADC_theACLK__PM_ACT_CFG CYREG_PM_ACT_CFG1
#define ADC_theACLK__PM_ACT_MSK 0x01u
#define ADC_theACLK__PM_STBY_CFG CYREG_PM_STBY_CFG1
#define ADC_theACLK__PM_STBY_MSK 0x01u

/* LCD_LCDPort */
#define LCD_LCDPort__0__MASK 0x01u
#define LCD_LCDPort__0__PC CYREG_PRT2_PC0
#define LCD_LCDPort__0__PORT 2u
#define LCD_LCDPort__0__SHIFT 0
#define LCD_LCDPort__1__MASK 0x02u
#define LCD_LCDPort__1__PC CYREG_PRT2_PC1
#define LCD_LCDPort__1__PORT 2u
#define LCD_LCDPort__1__SHIFT 1
#define LCD_LCDPort__2__MASK 0x04u
#define LCD_LCDPort__2__PC CYREG_PRT2_PC2
#define LCD_LCDPort__2__PORT 2u
#define LCD_LCDPort__2__SHIFT 2
#define LCD_LCDPort__3__MASK 0x08u
#define LCD_LCDPort__3__PC CYREG_PRT2_PC3
#define LCD_LCDPort__3__PORT 2u
#define LCD_LCDPort__3__SHIFT 3
#define LCD_LCDPort__4__MASK 0x10u
#define LCD_LCDPort__4__PC CYREG_PRT2_PC4
#define LCD_LCDPort__4__PORT 2u
#define LCD_LCDPort__4__SHIFT 4
#define LCD_LCDPort__5__MASK 0x20u
#define LCD_LCDPort__5__PC CYREG_PRT2_PC5
#define LCD_LCDPort__5__PORT 2u
#define LCD_LCDPort__5__SHIFT 5
#define LCD_LCDPort__6__MASK 0x40u
#define LCD_LCDPort__6__PC CYREG_PRT2_PC6
#define LCD_LCDPort__6__PORT 2u
#define LCD_LCDPort__6__SHIFT 6
#define LCD_LCDPort__AG CYREG_PRT2_AG
#define LCD_LCDPort__AMUX CYREG_PRT2_AMUX
#define LCD_LCDPort__BIE CYREG_PRT2_BIE
#define LCD_LCDPort__BIT_MASK CYREG_PRT2_BIT_MASK
#define LCD_LCDPort__BYP CYREG_PRT2_BYP
#define LCD_LCDPort__CTL CYREG_PRT2_CTL
#define LCD_LCDPort__DM0 CYREG_PRT2_DM0
#define LCD_LCDPort__DM1 CYREG_PRT2_DM1
#define LCD_LCDPort__DM2 CYREG_PRT2_DM2
#define LCD_LCDPort__DR CYREG_PRT2_DR
#define LCD_LCDPort__INP_DIS CYREG_PRT2_INP_DIS
#define LCD_LCDPort__LCD_COM_SEG CYREG_PRT2_LCD_COM_SEG
#define LCD_LCDPort__LCD_EN CYREG_PRT2_LCD_EN
#define LCD_LCDPort__MASK 0x7Fu
#define LCD_LCDPort__PORT 2u
#define LCD_LCDPort__PRT CYREG_PRT2_PRT
#define LCD_LCDPort__PRTDSI__CAPS_SEL CYREG_PRT2_CAPS_SEL
#define LCD_LCDPort__PRTDSI__DBL_SYNC_IN CYREG_PRT2_DBL_SYNC_IN
#define LCD_LCDPort__PRTDSI__OE_SEL0 CYREG_PRT2_OE_SEL0
#define LCD_LCDPort__PRTDSI__OE_SEL1 CYREG_PRT2_OE_SEL1
#define LCD_LCDPort__PRTDSI__OUT_SEL0 CYREG_PRT2_OUT_SEL0
#define LCD_LCDPort__PRTDSI__OUT_SEL1 CYREG_PRT2_OUT_SEL1
#define LCD_LCDPort__PRTDSI__SYNC_OUT CYREG_PRT2_SYNC_OUT
#define LCD_LCDPort__PS CYREG_PRT2_PS
#define LCD_LCDPort__SHIFT 0
#define LCD_LCDPort__SLW CYREG_PRT2_SLW

/* Opamp_ABuf */
#define Opamp_ABuf__CR CYREG_OPAMP2_CR
#define Opamp_ABuf__MX CYREG_OPAMP2_MX
#define Opamp_ABuf__NPUMP_OPAMP_TR0 CYREG_NPUMP_OPAMP_TR0
#define Opamp_ABuf__PM_ACT_CFG CYREG_PM_ACT_CFG4
#define Opamp_ABuf__PM_ACT_MSK 0x04u
#define Opamp_ABuf__PM_STBY_CFG CYREG_PM_STBY_CFG4
#define Opamp_ABuf__PM_STBY_MSK 0x04u
#define Opamp_ABuf__RSVD CYREG_OPAMP2_RSVD
#define Opamp_ABuf__SW CYREG_OPAMP2_SW
#define Opamp_ABuf__TR0 CYREG_OPAMP2_TR0
#define Opamp_ABuf__TR1 CYREG_OPAMP2_TR1

/* ADC_DEC */
#define ADC_DEC__COHER CYREG_DEC_COHER
#define ADC_DEC__CR CYREG_DEC_CR
#define ADC_DEC__DR1 CYREG_DEC_DR1
#define ADC_DEC__DR2 CYREG_DEC_DR2
#define ADC_DEC__DR2H CYREG_DEC_DR2H
#define ADC_DEC__GCOR CYREG_DEC_GCOR
#define ADC_DEC__GCORH CYREG_DEC_GCORH
#define ADC_DEC__GVAL CYREG_DEC_GVAL
#define ADC_DEC__OCOR CYREG_DEC_OCOR
#define ADC_DEC__OCORH CYREG_DEC_OCORH
#define ADC_DEC__OCORM CYREG_DEC_OCORM
#define ADC_DEC__OUTSAMP CYREG_DEC_OUTSAMP
#define ADC_DEC__OUTSAMPH CYREG_DEC_OUTSAMPH
#define ADC_DEC__OUTSAMPM CYREG_DEC_OUTSAMPM
#define ADC_DEC__OUTSAMPS CYREG_DEC_OUTSAMPS
#define ADC_DEC__PM_ACT_CFG CYREG_PM_ACT_CFG10
#define ADC_DEC__PM_ACT_MSK 0x01u
#define ADC_DEC__PM_STBY_CFG CYREG_PM_STBY_CFG10
#define ADC_DEC__PM_STBY_MSK 0x01u
#define ADC_DEC__SHIFT1 CYREG_DEC_SHIFT1
#define ADC_DEC__SHIFT2 CYREG_DEC_SHIFT2
#define ADC_DEC__SR CYREG_DEC_SR
#define ADC_DEC__TRIM__M1 CYREG_FLSHID_CUST_TABLES_DEC_M1
#define ADC_DEC__TRIM__M2 CYREG_FLSHID_CUST_TABLES_DEC_M2
#define ADC_DEC__TRIM__M3 CYREG_FLSHID_CUST_TABLES_DEC_M3
#define ADC_DEC__TRIM__M4 CYREG_FLSHID_CUST_TABLES_DEC_M4
#define ADC_DEC__TRIM__M5 CYREG_FLSHID_CUST_TABLES_DEC_M5
#define ADC_DEC__TRIM__M6 CYREG_FLSHID_CUST_TABLES_DEC_M6
#define ADC_DEC__TRIM__M7 CYREG_FLSHID_CUST_TABLES_DEC_M7
#define ADC_DEC__TRIM__M8 CYREG_FLSHID_CUST_TABLES_DEC_M8

/* ADC_DSM */
#define ADC_DSM__BUF0 CYREG_DSM0_BUF0
#define ADC_DSM__BUF1 CYREG_DSM0_BUF1
#define ADC_DSM__BUF2 CYREG_DSM0_BUF2
#define ADC_DSM__BUF3 CYREG_DSM0_BUF3
#define ADC_DSM__CLK CYREG_DSM0_CLK
#define ADC_DSM__CR0 CYREG_DSM0_CR0
#define ADC_DSM__CR1 CYREG_DSM0_CR1
#define ADC_DSM__CR10 CYREG_DSM0_CR10
#define ADC_DSM__CR11 CYREG_DSM0_CR11
#define ADC_DSM__CR12 CYREG_DSM0_CR12
#define ADC_DSM__CR13 CYREG_DSM0_CR13
#define ADC_DSM__CR14 CYREG_DSM0_CR14
#define ADC_DSM__CR15 CYREG_DSM0_CR15
#define ADC_DSM__CR16 CYREG_DSM0_CR16
#define ADC_DSM__CR17 CYREG_DSM0_CR17
#define ADC_DSM__CR2 CYREG_DSM0_CR2
#define ADC_DSM__CR3 CYREG_DSM0_CR3
#define ADC_DSM__CR4 CYREG_DSM0_CR4
#define ADC_DSM__CR5 CYREG_DSM0_CR5
#define ADC_DSM__CR6 CYREG_DSM0_CR6
#define ADC_DSM__CR7 CYREG_DSM0_CR7
#define ADC_DSM__CR8 CYREG_DSM0_CR8
#define ADC_DSM__CR9 CYREG_DSM0_CR9
#define ADC_DSM__DEM0 CYREG_DSM0_DEM0
#define ADC_DSM__DEM1 CYREG_DSM0_DEM1
#define ADC_DSM__MISC CYREG_DSM0_MISC
#define ADC_DSM__OUT0 CYREG_DSM0_OUT0
#define ADC_DSM__OUT1 CYREG_DSM0_OUT1
#define ADC_DSM__REF0 CYREG_DSM0_REF0
#define ADC_DSM__REF1 CYREG_DSM0_REF1
#define ADC_DSM__REF2 CYREG_DSM0_REF2
#define ADC_DSM__REF3 CYREG_DSM0_REF3
#define ADC_DSM__RSVD1 CYREG_DSM0_RSVD1
#define ADC_DSM__SW0 CYREG_DSM0_SW0
#define ADC_DSM__SW2 CYREG_DSM0_SW2
#define ADC_DSM__SW3 CYREG_DSM0_SW3
#define ADC_DSM__SW4 CYREG_DSM0_SW4
#define ADC_DSM__SW6 CYREG_DSM0_SW6
#define ADC_DSM__TR0 CYREG_NPUMP_DSM_TR0
#define ADC_DSM__TST0 CYREG_DSM0_TST0
#define ADC_DSM__TST1 CYREG_DSM0_TST1

/* ADC_IRQ */
#define ADC_IRQ__ES2_PATCH 0u
#define ADC_IRQ__INTC_CLR_EN_REG CYREG_INTC_CLR_EN3
#define ADC_IRQ__INTC_CLR_PD_REG CYREG_INTC_CLR_PD3
#define ADC_IRQ__INTC_MASK 0x20u
#define ADC_IRQ__INTC_NUMBER 29u
#define ADC_IRQ__INTC_PRIOR_NUM 7u
#define ADC_IRQ__INTC_PRIOR_REG CYREG_INTC_PRIOR29
#define ADC_IRQ__INTC_SET_EN_REG CYREG_INTC_SET_EN3
#define ADC_IRQ__INTC_SET_PD_REG CYREG_INTC_SET_PD3
#define ADC_IRQ__INTC_VECT (CYREG_INTC_VECT_MBASE+0x3Au)

/* Clock_1 */
#define Clock_1__CFG0 CYREG_CLKDIST_DCFG2_CFG0
#define Clock_1__CFG1 CYREG_CLKDIST_DCFG2_CFG1
#define Clock_1__CFG2 CYREG_CLKDIST_DCFG2_CFG2
#define Clock_1__CFG2_SRC_SEL_MASK 0x07u
#define Clock_1__INDEX 0x02u
#define Clock_1__PM_ACT_CFG CYREG_PM_ACT_CFG2
#define Clock_1__PM_ACT_MSK 0x04u
#define Clock_1__PM_STBY_CFG CYREG_PM_STBY_CFG2
#define Clock_1__PM_STBY_MSK 0x04u

/* CJTemp */
#define CJTemp__0__MASK 0x02u
#define CJTemp__0__PC CYREG_PRT6_PC1
#define CJTemp__0__PORT 6u
#define CJTemp__0__SHIFT 1
#define CJTemp__AG CYREG_PRT6_AG
#define CJTemp__AMUX CYREG_PRT6_AMUX
#define CJTemp__BIE CYREG_PRT6_BIE
#define CJTemp__BIT_MASK CYREG_PRT6_BIT_MASK
#define CJTemp__BYP CYREG_PRT6_BYP
#define CJTemp__CTL CYREG_PRT6_CTL
#define CJTemp__DM0 CYREG_PRT6_DM0
#define CJTemp__DM1 CYREG_PRT6_DM1
#define CJTemp__DM2 CYREG_PRT6_DM2
#define CJTemp__DR CYREG_PRT6_DR
#define CJTemp__INP_DIS CYREG_PRT6_INP_DIS
#define CJTemp__LCD_COM_SEG CYREG_PRT6_LCD_COM_SEG
#define CJTemp__LCD_EN CYREG_PRT6_LCD_EN
#define CJTemp__MASK 0x02u
#define CJTemp__PORT 6u
#define CJTemp__PRT CYREG_PRT6_PRT
#define CJTemp__PRTDSI__CAPS_SEL CYREG_PRT6_CAPS_SEL
#define CJTemp__PRTDSI__DBL_SYNC_IN CYREG_PRT6_DBL_SYNC_IN
#define CJTemp__PRTDSI__OE_SEL0 CYREG_PRT6_OE_SEL0
#define CJTemp__PRTDSI__OE_SEL1 CYREG_PRT6_OE_SEL1
#define CJTemp__PRTDSI__OUT_SEL0 CYREG_PRT6_OUT_SEL0
#define CJTemp__PRTDSI__OUT_SEL1 CYREG_PRT6_OUT_SEL1
#define CJTemp__PRTDSI__SYNC_OUT CYREG_PRT6_SYNC_OUT
#define CJTemp__PS CYREG_PRT6_PS
#define CJTemp__SHIFT 1
#define CJTemp__SLW CYREG_PRT6_SLW

/* Vtherm */
#define Vtherm__0__MASK 0x02u
#define Vtherm__0__PC CYREG_PRT0_PC1
#define Vtherm__0__PORT 0u
#define Vtherm__0__SHIFT 1
#define Vtherm__AG CYREG_PRT0_AG
#define Vtherm__AMUX CYREG_PRT0_AMUX
#define Vtherm__BIE CYREG_PRT0_BIE
#define Vtherm__BIT_MASK CYREG_PRT0_BIT_MASK
#define Vtherm__BYP CYREG_PRT0_BYP
#define Vtherm__CTL CYREG_PRT0_CTL
#define Vtherm__DM0 CYREG_PRT0_DM0
#define Vtherm__DM1 CYREG_PRT0_DM1
#define Vtherm__DM2 CYREG_PRT0_DM2
#define Vtherm__DR CYREG_PRT0_DR
#define Vtherm__INP_DIS CYREG_PRT0_INP_DIS
#define Vtherm__LCD_COM_SEG CYREG_PRT0_LCD_COM_SEG
#define Vtherm__LCD_EN CYREG_PRT0_LCD_EN
#define Vtherm__MASK 0x02u
#define Vtherm__PORT 0u
#define Vtherm__PRT CYREG_PRT0_PRT
#define Vtherm__PRTDSI__CAPS_SEL CYREG_PRT0_CAPS_SEL
#define Vtherm__PRTDSI__DBL_SYNC_IN CYREG_PRT0_DBL_SYNC_IN
#define Vtherm__PRTDSI__OE_SEL0 CYREG_PRT0_OE_SEL0
#define Vtherm__PRTDSI__OE_SEL1 CYREG_PRT0_OE_SEL1
#define Vtherm__PRTDSI__OUT_SEL0 CYREG_PRT0_OUT_SEL0
#define Vtherm__PRTDSI__OUT_SEL1 CYREG_PRT0_OUT_SEL1
#define Vtherm__PRTDSI__SYNC_OUT CYREG_PRT0_SYNC_OUT
#define Vtherm__PS CYREG_PRT0_PS
#define Vtherm__SHIFT 1
#define Vtherm__SLW CYREG_PRT0_SLW

/* Vlow */
#define Vlow__0__MASK 0x04u
#define Vlow__0__PC CYREG_PRT0_PC2
#define Vlow__0__PORT 0u
#define Vlow__0__SHIFT 2
#define Vlow__AG CYREG_PRT0_AG
#define Vlow__AMUX CYREG_PRT0_AMUX
#define Vlow__BIE CYREG_PRT0_BIE
#define Vlow__BIT_MASK CYREG_PRT0_BIT_MASK
#define Vlow__BYP CYREG_PRT0_BYP
#define Vlow__CTL CYREG_PRT0_CTL
#define Vlow__DM0 CYREG_PRT0_DM0
#define Vlow__DM1 CYREG_PRT0_DM1
#define Vlow__DM2 CYREG_PRT0_DM2
#define Vlow__DR CYREG_PRT0_DR
#define Vlow__INP_DIS CYREG_PRT0_INP_DIS
#define Vlow__LCD_COM_SEG CYREG_PRT0_LCD_COM_SEG
#define Vlow__LCD_EN CYREG_PRT0_LCD_EN
#define Vlow__MASK 0x04u
#define Vlow__PORT 0u
#define Vlow__PRT CYREG_PRT0_PRT
#define Vlow__PRTDSI__CAPS_SEL CYREG_PRT0_CAPS_SEL
#define Vlow__PRTDSI__DBL_SYNC_IN CYREG_PRT0_DBL_SYNC_IN
#define Vlow__PRTDSI__OE_SEL0 CYREG_PRT0_OE_SEL0
#define Vlow__PRTDSI__OE_SEL1 CYREG_PRT0_OE_SEL1
#define Vlow__PRTDSI__OUT_SEL0 CYREG_PRT0_OUT_SEL0
#define Vlow__PRTDSI__OUT_SEL1 CYREG_PRT0_OUT_SEL1
#define Vlow__PRTDSI__SYNC_OUT CYREG_PRT0_SYNC_OUT
#define Vlow__PS CYREG_PRT0_PS
#define Vlow__SHIFT 2
#define Vlow__SLW CYREG_PRT0_SLW

/* Vhi */
#define Vhi__0__MASK 0x01u
#define Vhi__0__PC CYREG_PRT0_PC0
#define Vhi__0__PORT 0u
#define Vhi__0__SHIFT 0
#define Vhi__AG CYREG_PRT0_AG
#define Vhi__AMUX CYREG_PRT0_AMUX
#define Vhi__BIE CYREG_PRT0_BIE
#define Vhi__BIT_MASK CYREG_PRT0_BIT_MASK
#define Vhi__BYP CYREG_PRT0_BYP
#define Vhi__CTL CYREG_PRT0_CTL
#define Vhi__DM0 CYREG_PRT0_DM0
#define Vhi__DM1 CYREG_PRT0_DM1
#define Vhi__DM2 CYREG_PRT0_DM2
#define Vhi__DR CYREG_PRT0_DR
#define Vhi__INP_DIS CYREG_PRT0_INP_DIS
#define Vhi__LCD_COM_SEG CYREG_PRT0_LCD_COM_SEG
#define Vhi__LCD_EN CYREG_PRT0_LCD_EN
#define Vhi__MASK 0x01u
#define Vhi__PORT 0u
#define Vhi__PRT CYREG_PRT0_PRT
#define Vhi__PRTDSI__CAPS_SEL CYREG_PRT0_CAPS_SEL
#define Vhi__PRTDSI__DBL_SYNC_IN CYREG_PRT0_DBL_SYNC_IN
#define Vhi__PRTDSI__OE_SEL0 CYREG_PRT0_OE_SEL0
#define Vhi__PRTDSI__OE_SEL1 CYREG_PRT0_OE_SEL1
#define Vhi__PRTDSI__OUT_SEL0 CYREG_PRT0_OUT_SEL0
#define Vhi__PRTDSI__OUT_SEL1 CYREG_PRT0_OUT_SEL1
#define Vhi__PRTDSI__SYNC_OUT CYREG_PRT0_SYNC_OUT
#define Vhi__PS CYREG_PRT0_PS
#define Vhi__SHIFT 0
#define Vhi__SLW CYREG_PRT0_SLW

/* Miscellaneous */
/* -- WARNING: define names containing LEOPARD or PANTHER are deprecated and will be removed in a future release */
#define CYDEV_DEBUGGING_DPS_SWD_SWV 6
#define CYDEV_CONFIG_UNUSED_IO_Disallowed 2
#define CYDEV_CONFIGURATION_MODE_DMA 2
#define CYDEV_CONFIG_FASTBOOT_ENABLED 0
#define CYDEV_CHIP_REV_LEOPARD_PRODUCTION 3u
#define CYDEV_CHIP_REVISION_3A_PRODUCTION 3u
#define CYDEV_CHIP_MEMBER_3A 1u
#define CYDEV_CHIP_FAMILY_PSOC3 1u
#define CYDEV_CHIP_DIE_LEOPARD 1u
#define CYDEV_CHIP_DIE_EXPECT CYDEV_CHIP_DIE_LEOPARD
#define BCLK__BUS_CLK__HZ 24000000U
#define BCLK__BUS_CLK__KHZ 24000U
#define BCLK__BUS_CLK__MHZ 24U
#define CYDEV_CHIP_DIE_ACTUAL CYDEV_CHIP_DIE_EXPECT
#define CYDEV_CHIP_DIE_PANTHER 3u
#define CYDEV_CHIP_DIE_PSOC4A 2u
#define CYDEV_CHIP_DIE_PSOC5LP 4u
#define CYDEV_CHIP_DIE_UNKNOWN 0u
#define CYDEV_CHIP_FAMILY_PSOC4 2u
#define CYDEV_CHIP_FAMILY_PSOC5 3u
#define CYDEV_CHIP_FAMILY_UNKNOWN 0u
#define CYDEV_CHIP_FAMILY_USED CYDEV_CHIP_FAMILY_PSOC3
#define CYDEV_CHIP_JTAG_ID 0x1E028069u
#define CYDEV_CHIP_MEMBER_4A 2u
#define CYDEV_CHIP_MEMBER_5A 3u
#define CYDEV_CHIP_MEMBER_5B 4u
#define CYDEV_CHIP_MEMBER_UNKNOWN 0u
#define CYDEV_CHIP_MEMBER_USED CYDEV_CHIP_MEMBER_3A
#define CYDEV_CHIP_REVISION_3A_ES1 0u
#define CYDEV_CHIP_REVISION_3A_ES2 1u
#define CYDEV_CHIP_REVISION_3A_ES3 3u
#define CYDEV_CHIP_REVISION_4A_ES0 17u
#define CYDEV_CHIP_REVISION_4A_PRODUCTION 17u
#define CYDEV_CHIP_REVISION_5A_ES0 0u
#define CYDEV_CHIP_REVISION_5A_ES1 1u
#define CYDEV_CHIP_REVISION_5A_PRODUCTION 1u
#define CYDEV_CHIP_REVISION_5B_ES0 0u
#define CYDEV_CHIP_REVISION_5B_PRODUCTION 0u
#define CYDEV_CHIP_REVISION_USED CYDEV_CHIP_REVISION_3A_PRODUCTION
#define CYDEV_CHIP_REV_EXPECT CYDEV_CHIP_REV_LEOPARD_PRODUCTION
#define CYDEV_CHIP_REV_LEOPARD_ES1 0u
#define CYDEV_CHIP_REV_LEOPARD_ES2 1u
#define CYDEV_CHIP_REV_LEOPARD_ES3 3u
#define CYDEV_CHIP_REV_PANTHER_ES0 0u
#define CYDEV_CHIP_REV_PANTHER_ES1 1u
#define CYDEV_CHIP_REV_PANTHER_PRODUCTION 1u
#define CYDEV_CHIP_REV_PSOC4A_ES0 17u
#define CYDEV_CHIP_REV_PSOC4A_PRODUCTION 17u
#define CYDEV_CHIP_REV_PSOC5LP_ES0 0u
#define CYDEV_CHIP_REV_PSOC5LP_PRODUCTION 0u
#define CYDEV_CONFIGURATION_CLEAR_SRAM 1
#define CYDEV_CONFIGURATION_COMPRESSED 0
#define CYDEV_CONFIGURATION_DMA 1
#define CYDEV_CONFIGURATION_ECC 1
#define CYDEV_CONFIGURATION_IMOENABLED CYDEV_CONFIG_FASTBOOT_ENABLED
#define CYDEV_CONFIGURATION_MODE CYDEV_CONFIGURATION_MODE_DMA
#define CYDEV_CONFIGURATION_MODE_COMPRESSED 0
#define CYDEV_CONFIGURATION_MODE_UNCOMPRESSED 1
#define CYDEV_CONFIG_UNUSED_IO CYDEV_CONFIG_UNUSED_IO_Disallowed
#define CYDEV_CONFIG_UNUSED_IO_AllowButWarn 0
#define CYDEV_CONFIG_UNUSED_IO_AllowWithInfo 1
#define CYDEV_DEBUGGING_DPS CYDEV_DEBUGGING_DPS_SWD_SWV
#define CYDEV_DEBUGGING_DPS_Disable 3
#define CYDEV_DEBUGGING_DPS_JTAG_4 1
#define CYDEV_DEBUGGING_DPS_JTAG_5 0
#define CYDEV_DEBUGGING_DPS_SWD 2
#define CYDEV_DEBUGGING_ENABLE 1
#define CYDEV_DEBUGGING_XRES 0
#define CYDEV_DEBUG_ENABLE_MASK 0x01u
#define CYDEV_DEBUG_ENABLE_REGISTER CYREG_MLOGIC_DEBUG
#define CYDEV_DMA_CHANNELS_AVAILABLE 24u
#define CYDEV_ECC_ENABLE 0
#define CYDEV_INSTRUCT_CACHE_ENABLED 1
#define CYDEV_INTR_RISING 0x00000003u
#define CYDEV_PROJ_TYPE 0
#define CYDEV_PROJ_TYPE_BOOTLOADER 1
#define CYDEV_PROJ_TYPE_LOADABLE 2
#define CYDEV_PROJ_TYPE_MULTIAPPBOOTLOADER 3
#define CYDEV_PROJ_TYPE_STANDARD 0
#define CYDEV_PROTECTION_ENABLE 0
#define CYDEV_VARIABLE_VDDA 0
#define CYDEV_VDDA 5.0
#define CYDEV_VDDA_MV 5000
#define CYDEV_VDDD 5.0
#define CYDEV_VDDD_MV 5000
#define CYDEV_VDDIO0 5.0
#define CYDEV_VDDIO0_MV 5000
#define CYDEV_VDDIO1 5.0
#define CYDEV_VDDIO1_MV 5000
#define CYDEV_VDDIO2 5.0
#define CYDEV_VDDIO2_MV 5000
#define CYDEV_VDDIO3 5.0
#define CYDEV_VDDIO3_MV 5000
#define CYDEV_VIO0 5
#define CYDEV_VIO0_MV 5000
#define CYDEV_VIO1 5
#define CYDEV_VIO1_MV 5000
#define CYDEV_VIO2 5
#define CYDEV_VIO2_MV 5000
#define CYDEV_VIO3 5
#define CYDEV_VIO3_MV 5000
#define DMA_CHANNELS_USED__MASK0 0x00000000u
#define CYDEV_BOOTLOADER_ENABLE 0

#endif /* INCLUDED_CYFITTER_H */
