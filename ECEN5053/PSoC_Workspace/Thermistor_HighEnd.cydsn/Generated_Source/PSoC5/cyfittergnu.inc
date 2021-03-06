.ifndef INCLUDED_CYFITTERGNU_INC
.set INCLUDED_CYFITTERGNU_INC, 1
.include "cydevicegnu.inc"
.include "cydevicegnu_trm.inc"

/* CapSense_CompCH0_ctComp */
.set CapSense_CompCH0_ctComp__CLK, CYREG_CMP0_CLK
.set CapSense_CompCH0_ctComp__CMP_MASK, 0x01
.set CapSense_CompCH0_ctComp__CMP_NUMBER, 0
.set CapSense_CompCH0_ctComp__CR, CYREG_CMP0_CR
.set CapSense_CompCH0_ctComp__LUT__CR, CYREG_LUT0_CR
.set CapSense_CompCH0_ctComp__LUT__MSK, CYREG_LUT_MSK
.set CapSense_CompCH0_ctComp__LUT__MSK_MASK, 0x01
.set CapSense_CompCH0_ctComp__LUT__MSK_SHIFT, 0
.set CapSense_CompCH0_ctComp__LUT__MX, CYREG_LUT0_MX
.set CapSense_CompCH0_ctComp__LUT__SR, CYREG_LUT_SR
.set CapSense_CompCH0_ctComp__LUT__SR_MASK, 0x01
.set CapSense_CompCH0_ctComp__LUT__SR_SHIFT, 0
.set CapSense_CompCH0_ctComp__PM_ACT_CFG, CYREG_PM_ACT_CFG7
.set CapSense_CompCH0_ctComp__PM_ACT_MSK, 0x01
.set CapSense_CompCH0_ctComp__PM_STBY_CFG, CYREG_PM_STBY_CFG7
.set CapSense_CompCH0_ctComp__PM_STBY_MSK, 0x01
.set CapSense_CompCH0_ctComp__SW0, CYREG_CMP0_SW0
.set CapSense_CompCH0_ctComp__SW2, CYREG_CMP0_SW2
.set CapSense_CompCH0_ctComp__SW3, CYREG_CMP0_SW3
.set CapSense_CompCH0_ctComp__SW4, CYREG_CMP0_SW4
.set CapSense_CompCH0_ctComp__SW6, CYREG_CMP0_SW6
.set CapSense_CompCH0_ctComp__TR0, CYREG_CMP0_TR0
.set CapSense_CompCH0_ctComp__TR1, CYREG_CMP0_TR1
.set CapSense_CompCH0_ctComp__TRIM__TR0, CYREG_FLSHID_MFG_CFG_CMP0_TR0
.set CapSense_CompCH0_ctComp__TRIM__TR0_HS, CYREG_FLSHID_CUST_TABLES_CMP0_TR0_HS
.set CapSense_CompCH0_ctComp__TRIM__TR1, CYREG_FLSHID_MFG_CFG_CMP0_TR1
.set CapSense_CompCH0_ctComp__TRIM__TR1_HS, CYREG_FLSHID_CUST_TABLES_CMP0_TR1_HS
.set CapSense_CompCH0_ctComp__WRK, CYREG_CMP_WRK
.set CapSense_CompCH0_ctComp__WRK_MASK, 0x01
.set CapSense_CompCH0_ctComp__WRK_SHIFT, 0

/* CapSense_IdacCH0_viDAC8 */
.set CapSense_IdacCH0_viDAC8__CR0, CYREG_DAC3_CR0
.set CapSense_IdacCH0_viDAC8__CR1, CYREG_DAC3_CR1
.set CapSense_IdacCH0_viDAC8__D, CYREG_DAC3_D
.set CapSense_IdacCH0_viDAC8__PM_ACT_CFG, CYREG_PM_ACT_CFG8
.set CapSense_IdacCH0_viDAC8__PM_ACT_MSK, 0x08
.set CapSense_IdacCH0_viDAC8__PM_STBY_CFG, CYREG_PM_STBY_CFG8
.set CapSense_IdacCH0_viDAC8__PM_STBY_MSK, 0x08
.set CapSense_IdacCH0_viDAC8__STROBE, CYREG_DAC3_STROBE
.set CapSense_IdacCH0_viDAC8__SW0, CYREG_DAC3_SW0
.set CapSense_IdacCH0_viDAC8__SW2, CYREG_DAC3_SW2
.set CapSense_IdacCH0_viDAC8__SW3, CYREG_DAC3_SW3
.set CapSense_IdacCH0_viDAC8__SW4, CYREG_DAC3_SW4
.set CapSense_IdacCH0_viDAC8__TR, CYREG_DAC3_TR
.set CapSense_IdacCH0_viDAC8__TRIM__M1, CYREG_FLSHID_CUST_TABLES_DAC3_M1
.set CapSense_IdacCH0_viDAC8__TRIM__M2, CYREG_FLSHID_CUST_TABLES_DAC3_M2
.set CapSense_IdacCH0_viDAC8__TRIM__M3, CYREG_FLSHID_CUST_TABLES_DAC3_M3
.set CapSense_IdacCH0_viDAC8__TRIM__M4, CYREG_FLSHID_CUST_TABLES_DAC3_M4
.set CapSense_IdacCH0_viDAC8__TRIM__M5, CYREG_FLSHID_CUST_TABLES_DAC3_M5
.set CapSense_IdacCH0_viDAC8__TRIM__M6, CYREG_FLSHID_CUST_TABLES_DAC3_M6
.set CapSense_IdacCH0_viDAC8__TRIM__M7, CYREG_FLSHID_CUST_TABLES_DAC3_M7
.set CapSense_IdacCH0_viDAC8__TRIM__M8, CYREG_FLSHID_CUST_TABLES_DAC3_M8
.set CapSense_IdacCH0_viDAC8__TST, CYREG_DAC3_TST

/* CapSense_MeasureCH0 */
.set CapSense_MeasureCH0_UDB_Counter_u0__16BIT_A0_REG, CYREG_B0_UDB10_11_A0
.set CapSense_MeasureCH0_UDB_Counter_u0__16BIT_A1_REG, CYREG_B0_UDB10_11_A1
.set CapSense_MeasureCH0_UDB_Counter_u0__16BIT_D0_REG, CYREG_B0_UDB10_11_D0
.set CapSense_MeasureCH0_UDB_Counter_u0__16BIT_D1_REG, CYREG_B0_UDB10_11_D1
.set CapSense_MeasureCH0_UDB_Counter_u0__16BIT_DP_AUX_CTL_REG, CYREG_B0_UDB10_11_ACTL
.set CapSense_MeasureCH0_UDB_Counter_u0__16BIT_F0_REG, CYREG_B0_UDB10_11_F0
.set CapSense_MeasureCH0_UDB_Counter_u0__16BIT_F1_REG, CYREG_B0_UDB10_11_F1
.set CapSense_MeasureCH0_UDB_Counter_u0__A0_A1_REG, CYREG_B0_UDB10_A0_A1
.set CapSense_MeasureCH0_UDB_Counter_u0__A0_REG, CYREG_B0_UDB10_A0
.set CapSense_MeasureCH0_UDB_Counter_u0__A1_REG, CYREG_B0_UDB10_A1
.set CapSense_MeasureCH0_UDB_Counter_u0__D0_D1_REG, CYREG_B0_UDB10_D0_D1
.set CapSense_MeasureCH0_UDB_Counter_u0__D0_REG, CYREG_B0_UDB10_D0
.set CapSense_MeasureCH0_UDB_Counter_u0__D1_REG, CYREG_B0_UDB10_D1
.set CapSense_MeasureCH0_UDB_Counter_u0__DP_AUX_CTL_REG, CYREG_B0_UDB10_ACTL
.set CapSense_MeasureCH0_UDB_Counter_u0__F0_F1_REG, CYREG_B0_UDB10_F0_F1
.set CapSense_MeasureCH0_UDB_Counter_u0__F0_REG, CYREG_B0_UDB10_F0
.set CapSense_MeasureCH0_UDB_Counter_u0__F1_REG, CYREG_B0_UDB10_F1
.set CapSense_MeasureCH0_UDB_Window_u0__16BIT_A0_REG, CYREG_B0_UDB14_15_A0
.set CapSense_MeasureCH0_UDB_Window_u0__16BIT_A1_REG, CYREG_B0_UDB14_15_A1
.set CapSense_MeasureCH0_UDB_Window_u0__16BIT_D0_REG, CYREG_B0_UDB14_15_D0
.set CapSense_MeasureCH0_UDB_Window_u0__16BIT_D1_REG, CYREG_B0_UDB14_15_D1
.set CapSense_MeasureCH0_UDB_Window_u0__16BIT_DP_AUX_CTL_REG, CYREG_B0_UDB14_15_ACTL
.set CapSense_MeasureCH0_UDB_Window_u0__16BIT_F0_REG, CYREG_B0_UDB14_15_F0
.set CapSense_MeasureCH0_UDB_Window_u0__16BIT_F1_REG, CYREG_B0_UDB14_15_F1
.set CapSense_MeasureCH0_UDB_Window_u0__A0_A1_REG, CYREG_B0_UDB14_A0_A1
.set CapSense_MeasureCH0_UDB_Window_u0__A0_REG, CYREG_B0_UDB14_A0
.set CapSense_MeasureCH0_UDB_Window_u0__A1_REG, CYREG_B0_UDB14_A1
.set CapSense_MeasureCH0_UDB_Window_u0__D0_D1_REG, CYREG_B0_UDB14_D0_D1
.set CapSense_MeasureCH0_UDB_Window_u0__D0_REG, CYREG_B0_UDB14_D0
.set CapSense_MeasureCH0_UDB_Window_u0__D1_REG, CYREG_B0_UDB14_D1
.set CapSense_MeasureCH0_UDB_Window_u0__DP_AUX_CTL_REG, CYREG_B0_UDB14_ACTL
.set CapSense_MeasureCH0_UDB_Window_u0__F0_F1_REG, CYREG_B0_UDB14_F0_F1
.set CapSense_MeasureCH0_UDB_Window_u0__F0_REG, CYREG_B0_UDB14_F0
.set CapSense_MeasureCH0_UDB_Window_u0__F1_REG, CYREG_B0_UDB14_F1

/* CapSense_ClockGen */
.set CapSense_ClockGen_ScanSpeed__16BIT_CONTROL_AUX_CTL_REG, CYREG_B0_UDB13_14_ACTL
.set CapSense_ClockGen_ScanSpeed__16BIT_CONTROL_CONTROL_REG, CYREG_B0_UDB13_14_CTL
.set CapSense_ClockGen_ScanSpeed__16BIT_CONTROL_COUNT_REG, CYREG_B0_UDB13_14_CTL
.set CapSense_ClockGen_ScanSpeed__16BIT_COUNT_CONTROL_REG, CYREG_B0_UDB13_14_CTL
.set CapSense_ClockGen_ScanSpeed__16BIT_COUNT_COUNT_REG, CYREG_B0_UDB13_14_CTL
.set CapSense_ClockGen_ScanSpeed__16BIT_MASK_MASK_REG, CYREG_B0_UDB13_14_MSK
.set CapSense_ClockGen_ScanSpeed__16BIT_MASK_PERIOD_REG, CYREG_B0_UDB13_14_MSK
.set CapSense_ClockGen_ScanSpeed__16BIT_PERIOD_MASK_REG, CYREG_B0_UDB13_14_MSK
.set CapSense_ClockGen_ScanSpeed__16BIT_PERIOD_PERIOD_REG, CYREG_B0_UDB13_14_MSK
.set CapSense_ClockGen_ScanSpeed__CONTROL_AUX_CTL_REG, CYREG_B0_UDB13_ACTL
.set CapSense_ClockGen_ScanSpeed__CONTROL_REG, CYREG_B0_UDB13_CTL
.set CapSense_ClockGen_ScanSpeed__CONTROL_ST_REG, CYREG_B0_UDB13_ST_CTL
.set CapSense_ClockGen_ScanSpeed__COUNT_REG, CYREG_B0_UDB13_CTL
.set CapSense_ClockGen_ScanSpeed__COUNT_ST_REG, CYREG_B0_UDB13_ST_CTL
.set CapSense_ClockGen_ScanSpeed__MASK_CTL_AUX_CTL_REG, CYREG_B0_UDB13_MSK_ACTL
.set CapSense_ClockGen_ScanSpeed__PERIOD_REG, CYREG_B0_UDB13_MSK
.set CapSense_ClockGen_ScanSpeed__PER_CTL_AUX_CTL_REG, CYREG_B0_UDB13_MSK_ACTL
.set CapSense_ClockGen_SyncCtrl_CtrlReg__0__MASK, 0x01
.set CapSense_ClockGen_SyncCtrl_CtrlReg__0__POS, 0
.set CapSense_ClockGen_SyncCtrl_CtrlReg__16BIT_CONTROL_AUX_CTL_REG, CYREG_B0_UDB12_13_ACTL
.set CapSense_ClockGen_SyncCtrl_CtrlReg__16BIT_CONTROL_CONTROL_REG, CYREG_B0_UDB12_13_CTL
.set CapSense_ClockGen_SyncCtrl_CtrlReg__16BIT_CONTROL_COUNT_REG, CYREG_B0_UDB12_13_CTL
.set CapSense_ClockGen_SyncCtrl_CtrlReg__16BIT_COUNT_CONTROL_REG, CYREG_B0_UDB12_13_CTL
.set CapSense_ClockGen_SyncCtrl_CtrlReg__16BIT_COUNT_COUNT_REG, CYREG_B0_UDB12_13_CTL
.set CapSense_ClockGen_SyncCtrl_CtrlReg__16BIT_MASK_MASK_REG, CYREG_B0_UDB12_13_MSK
.set CapSense_ClockGen_SyncCtrl_CtrlReg__16BIT_MASK_PERIOD_REG, CYREG_B0_UDB12_13_MSK
.set CapSense_ClockGen_SyncCtrl_CtrlReg__16BIT_PERIOD_MASK_REG, CYREG_B0_UDB12_13_MSK
.set CapSense_ClockGen_SyncCtrl_CtrlReg__16BIT_PERIOD_PERIOD_REG, CYREG_B0_UDB12_13_MSK
.set CapSense_ClockGen_SyncCtrl_CtrlReg__1__MASK, 0x02
.set CapSense_ClockGen_SyncCtrl_CtrlReg__1__POS, 1
.set CapSense_ClockGen_SyncCtrl_CtrlReg__2__MASK, 0x04
.set CapSense_ClockGen_SyncCtrl_CtrlReg__2__POS, 2
.set CapSense_ClockGen_SyncCtrl_CtrlReg__4__MASK, 0x10
.set CapSense_ClockGen_SyncCtrl_CtrlReg__4__POS, 4
.set CapSense_ClockGen_SyncCtrl_CtrlReg__CONTROL_AUX_CTL_REG, CYREG_B0_UDB12_ACTL
.set CapSense_ClockGen_SyncCtrl_CtrlReg__CONTROL_REG, CYREG_B0_UDB12_CTL
.set CapSense_ClockGen_SyncCtrl_CtrlReg__CONTROL_ST_REG, CYREG_B0_UDB12_ST_CTL
.set CapSense_ClockGen_SyncCtrl_CtrlReg__COUNT_REG, CYREG_B0_UDB12_CTL
.set CapSense_ClockGen_SyncCtrl_CtrlReg__COUNT_ST_REG, CYREG_B0_UDB12_ST_CTL
.set CapSense_ClockGen_SyncCtrl_CtrlReg__MASK, 0x17
.set CapSense_ClockGen_SyncCtrl_CtrlReg__MASK_CTL_AUX_CTL_REG, CYREG_B0_UDB12_MSK_ACTL
.set CapSense_ClockGen_SyncCtrl_CtrlReg__PERIOD_REG, CYREG_B0_UDB12_MSK
.set CapSense_ClockGen_SyncCtrl_CtrlReg__PER_CTL_AUX_CTL_REG, CYREG_B0_UDB12_MSK_ACTL
.set CapSense_ClockGen_UDB_PrescalerDp_u0__A0_A1_REG, CYREG_B0_UDB15_A0_A1
.set CapSense_ClockGen_UDB_PrescalerDp_u0__A0_REG, CYREG_B0_UDB15_A0
.set CapSense_ClockGen_UDB_PrescalerDp_u0__A1_REG, CYREG_B0_UDB15_A1
.set CapSense_ClockGen_UDB_PrescalerDp_u0__D0_D1_REG, CYREG_B0_UDB15_D0_D1
.set CapSense_ClockGen_UDB_PrescalerDp_u0__D0_REG, CYREG_B0_UDB15_D0
.set CapSense_ClockGen_UDB_PrescalerDp_u0__D1_REG, CYREG_B0_UDB15_D1
.set CapSense_ClockGen_UDB_PrescalerDp_u0__DP_AUX_CTL_REG, CYREG_B0_UDB15_ACTL
.set CapSense_ClockGen_UDB_PrescalerDp_u0__F0_F1_REG, CYREG_B0_UDB15_F0_F1
.set CapSense_ClockGen_UDB_PrescalerDp_u0__F0_REG, CYREG_B0_UDB15_F0
.set CapSense_ClockGen_UDB_PrescalerDp_u0__F1_REG, CYREG_B0_UDB15_F1
.set CapSense_ClockGen_sC16_PRSdp_u0__16BIT_A0_REG, CYREG_B0_UDB12_13_A0
.set CapSense_ClockGen_sC16_PRSdp_u0__16BIT_A1_REG, CYREG_B0_UDB12_13_A1
.set CapSense_ClockGen_sC16_PRSdp_u0__16BIT_D0_REG, CYREG_B0_UDB12_13_D0
.set CapSense_ClockGen_sC16_PRSdp_u0__16BIT_D1_REG, CYREG_B0_UDB12_13_D1
.set CapSense_ClockGen_sC16_PRSdp_u0__16BIT_DP_AUX_CTL_REG, CYREG_B0_UDB12_13_ACTL
.set CapSense_ClockGen_sC16_PRSdp_u0__16BIT_F0_REG, CYREG_B0_UDB12_13_F0
.set CapSense_ClockGen_sC16_PRSdp_u0__16BIT_F1_REG, CYREG_B0_UDB12_13_F1
.set CapSense_ClockGen_sC16_PRSdp_u0__A0_A1_REG, CYREG_B0_UDB12_A0_A1
.set CapSense_ClockGen_sC16_PRSdp_u0__A0_REG, CYREG_B0_UDB12_A0
.set CapSense_ClockGen_sC16_PRSdp_u0__A1_REG, CYREG_B0_UDB12_A1
.set CapSense_ClockGen_sC16_PRSdp_u0__D0_D1_REG, CYREG_B0_UDB12_D0_D1
.set CapSense_ClockGen_sC16_PRSdp_u0__D0_REG, CYREG_B0_UDB12_D0
.set CapSense_ClockGen_sC16_PRSdp_u0__D1_REG, CYREG_B0_UDB12_D1
.set CapSense_ClockGen_sC16_PRSdp_u0__DP_AUX_CTL_REG, CYREG_B0_UDB12_ACTL
.set CapSense_ClockGen_sC16_PRSdp_u0__F0_F1_REG, CYREG_B0_UDB12_F0_F1
.set CapSense_ClockGen_sC16_PRSdp_u0__F0_REG, CYREG_B0_UDB12_F0
.set CapSense_ClockGen_sC16_PRSdp_u0__F1_REG, CYREG_B0_UDB12_F1
.set CapSense_ClockGen_sC16_PRSdp_u0__MSK_DP_AUX_CTL_REG, CYREG_B0_UDB12_MSK_ACTL
.set CapSense_ClockGen_sC16_PRSdp_u0__PER_DP_AUX_CTL_REG, CYREG_B0_UDB12_MSK_ACTL
.set CapSense_ClockGen_sC16_PRSdp_u1__16BIT_A0_REG, CYREG_B0_UDB13_14_A0
.set CapSense_ClockGen_sC16_PRSdp_u1__16BIT_A1_REG, CYREG_B0_UDB13_14_A1
.set CapSense_ClockGen_sC16_PRSdp_u1__16BIT_D0_REG, CYREG_B0_UDB13_14_D0
.set CapSense_ClockGen_sC16_PRSdp_u1__16BIT_D1_REG, CYREG_B0_UDB13_14_D1
.set CapSense_ClockGen_sC16_PRSdp_u1__16BIT_DP_AUX_CTL_REG, CYREG_B0_UDB13_14_ACTL
.set CapSense_ClockGen_sC16_PRSdp_u1__16BIT_F0_REG, CYREG_B0_UDB13_14_F0
.set CapSense_ClockGen_sC16_PRSdp_u1__16BIT_F1_REG, CYREG_B0_UDB13_14_F1
.set CapSense_ClockGen_sC16_PRSdp_u1__A0_A1_REG, CYREG_B0_UDB13_A0_A1
.set CapSense_ClockGen_sC16_PRSdp_u1__A0_REG, CYREG_B0_UDB13_A0
.set CapSense_ClockGen_sC16_PRSdp_u1__A1_REG, CYREG_B0_UDB13_A1
.set CapSense_ClockGen_sC16_PRSdp_u1__D0_D1_REG, CYREG_B0_UDB13_D0_D1
.set CapSense_ClockGen_sC16_PRSdp_u1__D0_REG, CYREG_B0_UDB13_D0
.set CapSense_ClockGen_sC16_PRSdp_u1__D1_REG, CYREG_B0_UDB13_D1
.set CapSense_ClockGen_sC16_PRSdp_u1__DP_AUX_CTL_REG, CYREG_B0_UDB13_ACTL
.set CapSense_ClockGen_sC16_PRSdp_u1__F0_F1_REG, CYREG_B0_UDB13_F0_F1
.set CapSense_ClockGen_sC16_PRSdp_u1__F0_REG, CYREG_B0_UDB13_F0
.set CapSense_ClockGen_sC16_PRSdp_u1__F1_REG, CYREG_B0_UDB13_F1
.set CapSense_ClockGen_sC16_PRSdp_u1__MSK_DP_AUX_CTL_REG, CYREG_B0_UDB13_MSK_ACTL
.set CapSense_ClockGen_sC16_PRSdp_u1__PER_DP_AUX_CTL_REG, CYREG_B0_UDB13_MSK_ACTL

/* CapSense_IntClock */
.set CapSense_IntClock__CFG0, CYREG_CLKDIST_DCFG1_CFG0
.set CapSense_IntClock__CFG1, CYREG_CLKDIST_DCFG1_CFG1
.set CapSense_IntClock__CFG2, CYREG_CLKDIST_DCFG1_CFG2
.set CapSense_IntClock__CFG2_SRC_SEL_MASK, 0x07
.set CapSense_IntClock__INDEX, 0x01
.set CapSense_IntClock__PM_ACT_CFG, CYREG_PM_ACT_CFG2
.set CapSense_IntClock__PM_ACT_MSK, 0x02
.set CapSense_IntClock__PM_STBY_CFG, CYREG_PM_STBY_CFG2
.set CapSense_IntClock__PM_STBY_MSK, 0x02

/* CapSense_CmodCH0 */
.set CapSense_CmodCH0__0__MASK, 0x10
.set CapSense_CmodCH0__0__PC, CYREG_PRT6_PC4
.set CapSense_CmodCH0__0__PORT, 6
.set CapSense_CmodCH0__0__SHIFT, 4
.set CapSense_CmodCH0__AG, CYREG_PRT6_AG
.set CapSense_CmodCH0__AMUX, CYREG_PRT6_AMUX
.set CapSense_CmodCH0__BIE, CYREG_PRT6_BIE
.set CapSense_CmodCH0__BIT_MASK, CYREG_PRT6_BIT_MASK
.set CapSense_CmodCH0__BYP, CYREG_PRT6_BYP
.set CapSense_CmodCH0__CTL, CYREG_PRT6_CTL
.set CapSense_CmodCH0__Cmod_CH0__MASK, 0x10
.set CapSense_CmodCH0__Cmod_CH0__PC, CYREG_PRT6_PC4
.set CapSense_CmodCH0__Cmod_CH0__PORT, 6
.set CapSense_CmodCH0__Cmod_CH0__SHIFT, 4
.set CapSense_CmodCH0__DM0, CYREG_PRT6_DM0
.set CapSense_CmodCH0__DM1, CYREG_PRT6_DM1
.set CapSense_CmodCH0__DM2, CYREG_PRT6_DM2
.set CapSense_CmodCH0__DR, CYREG_PRT6_DR
.set CapSense_CmodCH0__INP_DIS, CYREG_PRT6_INP_DIS
.set CapSense_CmodCH0__LCD_COM_SEG, CYREG_PRT6_LCD_COM_SEG
.set CapSense_CmodCH0__LCD_EN, CYREG_PRT6_LCD_EN
.set CapSense_CmodCH0__MASK, 0x10
.set CapSense_CmodCH0__PORT, 6
.set CapSense_CmodCH0__PRT, CYREG_PRT6_PRT
.set CapSense_CmodCH0__PRTDSI__CAPS_SEL, CYREG_PRT6_CAPS_SEL
.set CapSense_CmodCH0__PRTDSI__DBL_SYNC_IN, CYREG_PRT6_DBL_SYNC_IN
.set CapSense_CmodCH0__PRTDSI__OE_SEL0, CYREG_PRT6_OE_SEL0
.set CapSense_CmodCH0__PRTDSI__OE_SEL1, CYREG_PRT6_OE_SEL1
.set CapSense_CmodCH0__PRTDSI__OUT_SEL0, CYREG_PRT6_OUT_SEL0
.set CapSense_CmodCH0__PRTDSI__OUT_SEL1, CYREG_PRT6_OUT_SEL1
.set CapSense_CmodCH0__PRTDSI__SYNC_OUT, CYREG_PRT6_SYNC_OUT
.set CapSense_CmodCH0__PS, CYREG_PRT6_PS
.set CapSense_CmodCH0__SHIFT, 4
.set CapSense_CmodCH0__SLW, CYREG_PRT6_SLW

/* CapSense_PortCH0 */
.set CapSense_PortCH0__0__MASK, 0x20
.set CapSense_PortCH0__0__PC, CYREG_PRT5_PC5
.set CapSense_PortCH0__0__PORT, 5
.set CapSense_PortCH0__0__SHIFT, 5
.set CapSense_PortCH0__1__MASK, 0x40
.set CapSense_PortCH0__1__PC, CYREG_PRT5_PC6
.set CapSense_PortCH0__1__PORT, 5
.set CapSense_PortCH0__1__SHIFT, 6
.set CapSense_PortCH0__2__MASK, 0x01
.set CapSense_PortCH0__2__PC, CYREG_PRT5_PC0
.set CapSense_PortCH0__2__PORT, 5
.set CapSense_PortCH0__2__SHIFT, 0
.set CapSense_PortCH0__3__MASK, 0x02
.set CapSense_PortCH0__3__PC, CYREG_PRT5_PC1
.set CapSense_PortCH0__3__PORT, 5
.set CapSense_PortCH0__3__SHIFT, 1
.set CapSense_PortCH0__4__MASK, 0x04
.set CapSense_PortCH0__4__PC, CYREG_PRT5_PC2
.set CapSense_PortCH0__4__PORT, 5
.set CapSense_PortCH0__4__SHIFT, 2
.set CapSense_PortCH0__5__MASK, 0x08
.set CapSense_PortCH0__5__PC, CYREG_PRT5_PC3
.set CapSense_PortCH0__5__PORT, 5
.set CapSense_PortCH0__5__SHIFT, 3
.set CapSense_PortCH0__6__MASK, 0x10
.set CapSense_PortCH0__6__PC, CYREG_PRT5_PC4
.set CapSense_PortCH0__6__PORT, 5
.set CapSense_PortCH0__6__SHIFT, 4
.set CapSense_PortCH0__AG, CYREG_PRT5_AG
.set CapSense_PortCH0__AMUX, CYREG_PRT5_AMUX
.set CapSense_PortCH0__BIE, CYREG_PRT5_BIE
.set CapSense_PortCH0__BIT_MASK, CYREG_PRT5_BIT_MASK
.set CapSense_PortCH0__BYP, CYREG_PRT5_BYP
.set CapSense_PortCH0__Button0__BTN__MASK, 0x20
.set CapSense_PortCH0__Button0__BTN__PC, CYREG_PRT5_PC5
.set CapSense_PortCH0__Button0__BTN__PORT, 5
.set CapSense_PortCH0__Button0__BTN__SHIFT, 5
.set CapSense_PortCH0__Button1__BTN__MASK, 0x40
.set CapSense_PortCH0__Button1__BTN__PC, CYREG_PRT5_PC6
.set CapSense_PortCH0__Button1__BTN__PORT, 5
.set CapSense_PortCH0__Button1__BTN__SHIFT, 6
.set CapSense_PortCH0__CTL, CYREG_PRT5_CTL
.set CapSense_PortCH0__DM0, CYREG_PRT5_DM0
.set CapSense_PortCH0__DM1, CYREG_PRT5_DM1
.set CapSense_PortCH0__DM2, CYREG_PRT5_DM2
.set CapSense_PortCH0__DR, CYREG_PRT5_DR
.set CapSense_PortCH0__INP_DIS, CYREG_PRT5_INP_DIS
.set CapSense_PortCH0__LCD_COM_SEG, CYREG_PRT5_LCD_COM_SEG
.set CapSense_PortCH0__LCD_EN, CYREG_PRT5_LCD_EN
.set CapSense_PortCH0__LinearSlider0_e0__LS__MASK, 0x01
.set CapSense_PortCH0__LinearSlider0_e0__LS__PC, CYREG_PRT5_PC0
.set CapSense_PortCH0__LinearSlider0_e0__LS__PORT, 5
.set CapSense_PortCH0__LinearSlider0_e0__LS__SHIFT, 0
.set CapSense_PortCH0__LinearSlider0_e1__LS__MASK, 0x02
.set CapSense_PortCH0__LinearSlider0_e1__LS__PC, CYREG_PRT5_PC1
.set CapSense_PortCH0__LinearSlider0_e1__LS__PORT, 5
.set CapSense_PortCH0__LinearSlider0_e1__LS__SHIFT, 1
.set CapSense_PortCH0__LinearSlider0_e2__LS__MASK, 0x04
.set CapSense_PortCH0__LinearSlider0_e2__LS__PC, CYREG_PRT5_PC2
.set CapSense_PortCH0__LinearSlider0_e2__LS__PORT, 5
.set CapSense_PortCH0__LinearSlider0_e2__LS__SHIFT, 2
.set CapSense_PortCH0__LinearSlider0_e3__LS__MASK, 0x08
.set CapSense_PortCH0__LinearSlider0_e3__LS__PC, CYREG_PRT5_PC3
.set CapSense_PortCH0__LinearSlider0_e3__LS__PORT, 5
.set CapSense_PortCH0__LinearSlider0_e3__LS__SHIFT, 3
.set CapSense_PortCH0__LinearSlider0_e4__LS__MASK, 0x10
.set CapSense_PortCH0__LinearSlider0_e4__LS__PC, CYREG_PRT5_PC4
.set CapSense_PortCH0__LinearSlider0_e4__LS__PORT, 5
.set CapSense_PortCH0__LinearSlider0_e4__LS__SHIFT, 4
.set CapSense_PortCH0__PORT, 5
.set CapSense_PortCH0__PRT, CYREG_PRT5_PRT
.set CapSense_PortCH0__PRTDSI__CAPS_SEL, CYREG_PRT5_CAPS_SEL
.set CapSense_PortCH0__PRTDSI__DBL_SYNC_IN, CYREG_PRT5_DBL_SYNC_IN
.set CapSense_PortCH0__PRTDSI__OE_SEL0, CYREG_PRT5_OE_SEL0
.set CapSense_PortCH0__PRTDSI__OE_SEL1, CYREG_PRT5_OE_SEL1
.set CapSense_PortCH0__PRTDSI__OUT_SEL0, CYREG_PRT5_OUT_SEL0
.set CapSense_PortCH0__PRTDSI__OUT_SEL1, CYREG_PRT5_OUT_SEL1
.set CapSense_PortCH0__PRTDSI__SYNC_OUT, CYREG_PRT5_SYNC_OUT
.set CapSense_PortCH0__PS, CYREG_PRT5_PS
.set CapSense_PortCH0__SLW, CYREG_PRT5_SLW

/* CapSense_BufCH0 */
.set CapSense_BufCH0__CFG0, CYREG_CAPSL_CFG0
.set CapSense_BufCH0__CFG1, CYREG_CAPSL_CFG1
.set CapSense_BufCH0__PM_ACT_CFG, CYREG_PM_ACT_CFG11
.set CapSense_BufCH0__PM_ACT_MSK, 0x10
.set CapSense_BufCH0__PM_STBY_CFG, CYREG_PM_STBY_CFG11
.set CapSense_BufCH0__PM_STBY_MSK, 0x10

/* CapSense_IsrCH0 */
.set CapSense_IsrCH0__INTC_CLR_EN_REG, CYREG_NVIC_CLRENA0
.set CapSense_IsrCH0__INTC_CLR_PD_REG, CYREG_NVIC_CLRPEND0
.set CapSense_IsrCH0__INTC_MASK, 0x01
.set CapSense_IsrCH0__INTC_NUMBER, 0
.set CapSense_IsrCH0__INTC_PRIOR_NUM, 7
.set CapSense_IsrCH0__INTC_PRIOR_REG, CYREG_NVIC_PRI_0
.set CapSense_IsrCH0__INTC_SET_EN_REG, CYREG_NVIC_SETENA0
.set CapSense_IsrCH0__INTC_SET_PD_REG, CYREG_NVIC_SETPEND0

/* isr_Calibration */
.set isr_Calibration__INTC_CLR_EN_REG, CYREG_NVIC_CLRENA0
.set isr_Calibration__INTC_CLR_PD_REG, CYREG_NVIC_CLRPEND0
.set isr_Calibration__INTC_MASK, 0x02
.set isr_Calibration__INTC_NUMBER, 1
.set isr_Calibration__INTC_PRIOR_NUM, 7
.set isr_Calibration__INTC_PRIOR_REG, CYREG_NVIC_PRI_1
.set isr_Calibration__INTC_SET_EN_REG, CYREG_NVIC_SETENA0
.set isr_Calibration__INTC_SET_PD_REG, CYREG_NVIC_SETPEND0

/* ADC_Ext_CP_Clk */
.set ADC_Ext_CP_Clk__CFG0, CYREG_CLKDIST_DCFG0_CFG0
.set ADC_Ext_CP_Clk__CFG1, CYREG_CLKDIST_DCFG0_CFG1
.set ADC_Ext_CP_Clk__CFG2, CYREG_CLKDIST_DCFG0_CFG2
.set ADC_Ext_CP_Clk__CFG2_SRC_SEL_MASK, 0x07
.set ADC_Ext_CP_Clk__INDEX, 0x00
.set ADC_Ext_CP_Clk__PM_ACT_CFG, CYREG_PM_ACT_CFG2
.set ADC_Ext_CP_Clk__PM_ACT_MSK, 0x01
.set ADC_Ext_CP_Clk__PM_STBY_CFG, CYREG_PM_STBY_CFG2
.set ADC_Ext_CP_Clk__PM_STBY_MSK, 0x01

/* VDAC8_viDAC8 */
.set VDAC8_viDAC8__CR0, CYREG_DAC0_CR0
.set VDAC8_viDAC8__CR1, CYREG_DAC0_CR1
.set VDAC8_viDAC8__D, CYREG_DAC0_D
.set VDAC8_viDAC8__PM_ACT_CFG, CYREG_PM_ACT_CFG8
.set VDAC8_viDAC8__PM_ACT_MSK, 0x01
.set VDAC8_viDAC8__PM_STBY_CFG, CYREG_PM_STBY_CFG8
.set VDAC8_viDAC8__PM_STBY_MSK, 0x01
.set VDAC8_viDAC8__STROBE, CYREG_DAC0_STROBE
.set VDAC8_viDAC8__SW0, CYREG_DAC0_SW0
.set VDAC8_viDAC8__SW2, CYREG_DAC0_SW2
.set VDAC8_viDAC8__SW3, CYREG_DAC0_SW3
.set VDAC8_viDAC8__SW4, CYREG_DAC0_SW4
.set VDAC8_viDAC8__TR, CYREG_DAC0_TR
.set VDAC8_viDAC8__TRIM__M1, CYREG_FLSHID_CUST_TABLES_DAC0_M1
.set VDAC8_viDAC8__TRIM__M2, CYREG_FLSHID_CUST_TABLES_DAC0_M2
.set VDAC8_viDAC8__TRIM__M3, CYREG_FLSHID_CUST_TABLES_DAC0_M3
.set VDAC8_viDAC8__TRIM__M4, CYREG_FLSHID_CUST_TABLES_DAC0_M4
.set VDAC8_viDAC8__TRIM__M5, CYREG_FLSHID_CUST_TABLES_DAC0_M5
.set VDAC8_viDAC8__TRIM__M6, CYREG_FLSHID_CUST_TABLES_DAC0_M6
.set VDAC8_viDAC8__TRIM__M7, CYREG_FLSHID_CUST_TABLES_DAC0_M7
.set VDAC8_viDAC8__TRIM__M8, CYREG_FLSHID_CUST_TABLES_DAC0_M8
.set VDAC8_viDAC8__TST, CYREG_DAC0_TST

/* ADC_theACLK */
.set ADC_theACLK__CFG0, CYREG_CLKDIST_ACFG0_CFG0
.set ADC_theACLK__CFG1, CYREG_CLKDIST_ACFG0_CFG1
.set ADC_theACLK__CFG2, CYREG_CLKDIST_ACFG0_CFG2
.set ADC_theACLK__CFG2_SRC_SEL_MASK, 0x07
.set ADC_theACLK__CFG3, CYREG_CLKDIST_ACFG0_CFG3
.set ADC_theACLK__CFG3_PHASE_DLY_MASK, 0x0F
.set ADC_theACLK__INDEX, 0x00
.set ADC_theACLK__PM_ACT_CFG, CYREG_PM_ACT_CFG1
.set ADC_theACLK__PM_ACT_MSK, 0x01
.set ADC_theACLK__PM_STBY_CFG, CYREG_PM_STBY_CFG1
.set ADC_theACLK__PM_STBY_MSK, 0x01

/* LCD_LCDPort */
.set LCD_LCDPort__0__MASK, 0x01
.set LCD_LCDPort__0__PC, CYREG_PRT2_PC0
.set LCD_LCDPort__0__PORT, 2
.set LCD_LCDPort__0__SHIFT, 0
.set LCD_LCDPort__1__MASK, 0x02
.set LCD_LCDPort__1__PC, CYREG_PRT2_PC1
.set LCD_LCDPort__1__PORT, 2
.set LCD_LCDPort__1__SHIFT, 1
.set LCD_LCDPort__2__MASK, 0x04
.set LCD_LCDPort__2__PC, CYREG_PRT2_PC2
.set LCD_LCDPort__2__PORT, 2
.set LCD_LCDPort__2__SHIFT, 2
.set LCD_LCDPort__3__MASK, 0x08
.set LCD_LCDPort__3__PC, CYREG_PRT2_PC3
.set LCD_LCDPort__3__PORT, 2
.set LCD_LCDPort__3__SHIFT, 3
.set LCD_LCDPort__4__MASK, 0x10
.set LCD_LCDPort__4__PC, CYREG_PRT2_PC4
.set LCD_LCDPort__4__PORT, 2
.set LCD_LCDPort__4__SHIFT, 4
.set LCD_LCDPort__5__MASK, 0x20
.set LCD_LCDPort__5__PC, CYREG_PRT2_PC5
.set LCD_LCDPort__5__PORT, 2
.set LCD_LCDPort__5__SHIFT, 5
.set LCD_LCDPort__6__MASK, 0x40
.set LCD_LCDPort__6__PC, CYREG_PRT2_PC6
.set LCD_LCDPort__6__PORT, 2
.set LCD_LCDPort__6__SHIFT, 6
.set LCD_LCDPort__AG, CYREG_PRT2_AG
.set LCD_LCDPort__AMUX, CYREG_PRT2_AMUX
.set LCD_LCDPort__BIE, CYREG_PRT2_BIE
.set LCD_LCDPort__BIT_MASK, CYREG_PRT2_BIT_MASK
.set LCD_LCDPort__BYP, CYREG_PRT2_BYP
.set LCD_LCDPort__CTL, CYREG_PRT2_CTL
.set LCD_LCDPort__DM0, CYREG_PRT2_DM0
.set LCD_LCDPort__DM1, CYREG_PRT2_DM1
.set LCD_LCDPort__DM2, CYREG_PRT2_DM2
.set LCD_LCDPort__DR, CYREG_PRT2_DR
.set LCD_LCDPort__INP_DIS, CYREG_PRT2_INP_DIS
.set LCD_LCDPort__LCD_COM_SEG, CYREG_PRT2_LCD_COM_SEG
.set LCD_LCDPort__LCD_EN, CYREG_PRT2_LCD_EN
.set LCD_LCDPort__MASK, 0x7F
.set LCD_LCDPort__PORT, 2
.set LCD_LCDPort__PRT, CYREG_PRT2_PRT
.set LCD_LCDPort__PRTDSI__CAPS_SEL, CYREG_PRT2_CAPS_SEL
.set LCD_LCDPort__PRTDSI__DBL_SYNC_IN, CYREG_PRT2_DBL_SYNC_IN
.set LCD_LCDPort__PRTDSI__OE_SEL0, CYREG_PRT2_OE_SEL0
.set LCD_LCDPort__PRTDSI__OE_SEL1, CYREG_PRT2_OE_SEL1
.set LCD_LCDPort__PRTDSI__OUT_SEL0, CYREG_PRT2_OUT_SEL0
.set LCD_LCDPort__PRTDSI__OUT_SEL1, CYREG_PRT2_OUT_SEL1
.set LCD_LCDPort__PRTDSI__SYNC_OUT, CYREG_PRT2_SYNC_OUT
.set LCD_LCDPort__PS, CYREG_PRT2_PS
.set LCD_LCDPort__SHIFT, 0
.set LCD_LCDPort__SLW, CYREG_PRT2_SLW

/* Opamp_ABuf */
.set Opamp_ABuf__CR, CYREG_OPAMP2_CR
.set Opamp_ABuf__MX, CYREG_OPAMP2_MX
.set Opamp_ABuf__NPUMP_OPAMP_TR0, CYREG_NPUMP_OPAMP_TR0
.set Opamp_ABuf__PM_ACT_CFG, CYREG_PM_ACT_CFG4
.set Opamp_ABuf__PM_ACT_MSK, 0x04
.set Opamp_ABuf__PM_STBY_CFG, CYREG_PM_STBY_CFG4
.set Opamp_ABuf__PM_STBY_MSK, 0x04
.set Opamp_ABuf__RSVD, CYREG_OPAMP2_RSVD
.set Opamp_ABuf__SW, CYREG_OPAMP2_SW
.set Opamp_ABuf__TR0, CYREG_OPAMP2_TR0
.set Opamp_ABuf__TR1, CYREG_OPAMP2_TR1

/* ADC_DEC */
.set ADC_DEC__COHER, CYREG_DEC_COHER
.set ADC_DEC__CR, CYREG_DEC_CR
.set ADC_DEC__DR1, CYREG_DEC_DR1
.set ADC_DEC__DR2, CYREG_DEC_DR2
.set ADC_DEC__DR2H, CYREG_DEC_DR2H
.set ADC_DEC__GCOR, CYREG_DEC_GCOR
.set ADC_DEC__GCORH, CYREG_DEC_GCORH
.set ADC_DEC__GVAL, CYREG_DEC_GVAL
.set ADC_DEC__OCOR, CYREG_DEC_OCOR
.set ADC_DEC__OCORH, CYREG_DEC_OCORH
.set ADC_DEC__OCORM, CYREG_DEC_OCORM
.set ADC_DEC__OUTSAMP, CYREG_DEC_OUTSAMP
.set ADC_DEC__OUTSAMPH, CYREG_DEC_OUTSAMPH
.set ADC_DEC__OUTSAMPM, CYREG_DEC_OUTSAMPM
.set ADC_DEC__OUTSAMPS, CYREG_DEC_OUTSAMPS
.set ADC_DEC__PM_ACT_CFG, CYREG_PM_ACT_CFG10
.set ADC_DEC__PM_ACT_MSK, 0x01
.set ADC_DEC__PM_STBY_CFG, CYREG_PM_STBY_CFG10
.set ADC_DEC__PM_STBY_MSK, 0x01
.set ADC_DEC__SHIFT1, CYREG_DEC_SHIFT1
.set ADC_DEC__SHIFT2, CYREG_DEC_SHIFT2
.set ADC_DEC__SR, CYREG_DEC_SR
.set ADC_DEC__TRIM__M1, CYREG_FLSHID_CUST_TABLES_DEC_M1
.set ADC_DEC__TRIM__M2, CYREG_FLSHID_CUST_TABLES_DEC_M2
.set ADC_DEC__TRIM__M3, CYREG_FLSHID_CUST_TABLES_DEC_M3
.set ADC_DEC__TRIM__M4, CYREG_FLSHID_CUST_TABLES_DEC_M4
.set ADC_DEC__TRIM__M5, CYREG_FLSHID_CUST_TABLES_DEC_M5
.set ADC_DEC__TRIM__M6, CYREG_FLSHID_CUST_TABLES_DEC_M6
.set ADC_DEC__TRIM__M7, CYREG_FLSHID_CUST_TABLES_DEC_M7
.set ADC_DEC__TRIM__M8, CYREG_FLSHID_CUST_TABLES_DEC_M8

/* ADC_DSM */
.set ADC_DSM__BUF0, CYREG_DSM0_BUF0
.set ADC_DSM__BUF1, CYREG_DSM0_BUF1
.set ADC_DSM__BUF2, CYREG_DSM0_BUF2
.set ADC_DSM__BUF3, CYREG_DSM0_BUF3
.set ADC_DSM__CLK, CYREG_DSM0_CLK
.set ADC_DSM__CR0, CYREG_DSM0_CR0
.set ADC_DSM__CR1, CYREG_DSM0_CR1
.set ADC_DSM__CR10, CYREG_DSM0_CR10
.set ADC_DSM__CR11, CYREG_DSM0_CR11
.set ADC_DSM__CR12, CYREG_DSM0_CR12
.set ADC_DSM__CR13, CYREG_DSM0_CR13
.set ADC_DSM__CR14, CYREG_DSM0_CR14
.set ADC_DSM__CR15, CYREG_DSM0_CR15
.set ADC_DSM__CR16, CYREG_DSM0_CR16
.set ADC_DSM__CR17, CYREG_DSM0_CR17
.set ADC_DSM__CR2, CYREG_DSM0_CR2
.set ADC_DSM__CR3, CYREG_DSM0_CR3
.set ADC_DSM__CR4, CYREG_DSM0_CR4
.set ADC_DSM__CR5, CYREG_DSM0_CR5
.set ADC_DSM__CR6, CYREG_DSM0_CR6
.set ADC_DSM__CR7, CYREG_DSM0_CR7
.set ADC_DSM__CR8, CYREG_DSM0_CR8
.set ADC_DSM__CR9, CYREG_DSM0_CR9
.set ADC_DSM__DEM0, CYREG_DSM0_DEM0
.set ADC_DSM__DEM1, CYREG_DSM0_DEM1
.set ADC_DSM__MISC, CYREG_DSM0_MISC
.set ADC_DSM__OUT0, CYREG_DSM0_OUT0
.set ADC_DSM__OUT1, CYREG_DSM0_OUT1
.set ADC_DSM__REF0, CYREG_DSM0_REF0
.set ADC_DSM__REF1, CYREG_DSM0_REF1
.set ADC_DSM__REF2, CYREG_DSM0_REF2
.set ADC_DSM__REF3, CYREG_DSM0_REF3
.set ADC_DSM__RSVD1, CYREG_DSM0_RSVD1
.set ADC_DSM__SW0, CYREG_DSM0_SW0
.set ADC_DSM__SW2, CYREG_DSM0_SW2
.set ADC_DSM__SW3, CYREG_DSM0_SW3
.set ADC_DSM__SW4, CYREG_DSM0_SW4
.set ADC_DSM__SW6, CYREG_DSM0_SW6
.set ADC_DSM__TR0, CYREG_NPUMP_DSM_TR0
.set ADC_DSM__TST0, CYREG_DSM0_TST0
.set ADC_DSM__TST1, CYREG_DSM0_TST1

/* ADC_IRQ */
.set ADC_IRQ__INTC_CLR_EN_REG, CYREG_NVIC_CLRENA0
.set ADC_IRQ__INTC_CLR_PD_REG, CYREG_NVIC_CLRPEND0
.set ADC_IRQ__INTC_MASK, 0x20000000
.set ADC_IRQ__INTC_NUMBER, 29
.set ADC_IRQ__INTC_PRIOR_NUM, 7
.set ADC_IRQ__INTC_PRIOR_REG, CYREG_NVIC_PRI_29
.set ADC_IRQ__INTC_SET_EN_REG, CYREG_NVIC_SETENA0
.set ADC_IRQ__INTC_SET_PD_REG, CYREG_NVIC_SETPEND0

/* Clock_1 */
.set Clock_1__CFG0, CYREG_CLKDIST_DCFG2_CFG0
.set Clock_1__CFG1, CYREG_CLKDIST_DCFG2_CFG1
.set Clock_1__CFG2, CYREG_CLKDIST_DCFG2_CFG2
.set Clock_1__CFG2_SRC_SEL_MASK, 0x07
.set Clock_1__INDEX, 0x02
.set Clock_1__PM_ACT_CFG, CYREG_PM_ACT_CFG2
.set Clock_1__PM_ACT_MSK, 0x04
.set Clock_1__PM_STBY_CFG, CYREG_PM_STBY_CFG2
.set Clock_1__PM_STBY_MSK, 0x04

/* CJTemp */
.set CJTemp__0__MASK, 0x02
.set CJTemp__0__PC, CYREG_PRT6_PC1
.set CJTemp__0__PORT, 6
.set CJTemp__0__SHIFT, 1
.set CJTemp__AG, CYREG_PRT6_AG
.set CJTemp__AMUX, CYREG_PRT6_AMUX
.set CJTemp__BIE, CYREG_PRT6_BIE
.set CJTemp__BIT_MASK, CYREG_PRT6_BIT_MASK
.set CJTemp__BYP, CYREG_PRT6_BYP
.set CJTemp__CTL, CYREG_PRT6_CTL
.set CJTemp__DM0, CYREG_PRT6_DM0
.set CJTemp__DM1, CYREG_PRT6_DM1
.set CJTemp__DM2, CYREG_PRT6_DM2
.set CJTemp__DR, CYREG_PRT6_DR
.set CJTemp__INP_DIS, CYREG_PRT6_INP_DIS
.set CJTemp__LCD_COM_SEG, CYREG_PRT6_LCD_COM_SEG
.set CJTemp__LCD_EN, CYREG_PRT6_LCD_EN
.set CJTemp__MASK, 0x02
.set CJTemp__PORT, 6
.set CJTemp__PRT, CYREG_PRT6_PRT
.set CJTemp__PRTDSI__CAPS_SEL, CYREG_PRT6_CAPS_SEL
.set CJTemp__PRTDSI__DBL_SYNC_IN, CYREG_PRT6_DBL_SYNC_IN
.set CJTemp__PRTDSI__OE_SEL0, CYREG_PRT6_OE_SEL0
.set CJTemp__PRTDSI__OE_SEL1, CYREG_PRT6_OE_SEL1
.set CJTemp__PRTDSI__OUT_SEL0, CYREG_PRT6_OUT_SEL0
.set CJTemp__PRTDSI__OUT_SEL1, CYREG_PRT6_OUT_SEL1
.set CJTemp__PRTDSI__SYNC_OUT, CYREG_PRT6_SYNC_OUT
.set CJTemp__PS, CYREG_PRT6_PS
.set CJTemp__SHIFT, 1
.set CJTemp__SLW, CYREG_PRT6_SLW

/* Vtherm */
.set Vtherm__0__MASK, 0x02
.set Vtherm__0__PC, CYREG_PRT0_PC1
.set Vtherm__0__PORT, 0
.set Vtherm__0__SHIFT, 1
.set Vtherm__AG, CYREG_PRT0_AG
.set Vtherm__AMUX, CYREG_PRT0_AMUX
.set Vtherm__BIE, CYREG_PRT0_BIE
.set Vtherm__BIT_MASK, CYREG_PRT0_BIT_MASK
.set Vtherm__BYP, CYREG_PRT0_BYP
.set Vtherm__CTL, CYREG_PRT0_CTL
.set Vtherm__DM0, CYREG_PRT0_DM0
.set Vtherm__DM1, CYREG_PRT0_DM1
.set Vtherm__DM2, CYREG_PRT0_DM2
.set Vtherm__DR, CYREG_PRT0_DR
.set Vtherm__INP_DIS, CYREG_PRT0_INP_DIS
.set Vtherm__LCD_COM_SEG, CYREG_PRT0_LCD_COM_SEG
.set Vtherm__LCD_EN, CYREG_PRT0_LCD_EN
.set Vtherm__MASK, 0x02
.set Vtherm__PORT, 0
.set Vtherm__PRT, CYREG_PRT0_PRT
.set Vtherm__PRTDSI__CAPS_SEL, CYREG_PRT0_CAPS_SEL
.set Vtherm__PRTDSI__DBL_SYNC_IN, CYREG_PRT0_DBL_SYNC_IN
.set Vtherm__PRTDSI__OE_SEL0, CYREG_PRT0_OE_SEL0
.set Vtherm__PRTDSI__OE_SEL1, CYREG_PRT0_OE_SEL1
.set Vtherm__PRTDSI__OUT_SEL0, CYREG_PRT0_OUT_SEL0
.set Vtherm__PRTDSI__OUT_SEL1, CYREG_PRT0_OUT_SEL1
.set Vtherm__PRTDSI__SYNC_OUT, CYREG_PRT0_SYNC_OUT
.set Vtherm__PS, CYREG_PRT0_PS
.set Vtherm__SHIFT, 1
.set Vtherm__SLW, CYREG_PRT0_SLW

/* Vlow */
.set Vlow__0__MASK, 0x04
.set Vlow__0__PC, CYREG_PRT0_PC2
.set Vlow__0__PORT, 0
.set Vlow__0__SHIFT, 2
.set Vlow__AG, CYREG_PRT0_AG
.set Vlow__AMUX, CYREG_PRT0_AMUX
.set Vlow__BIE, CYREG_PRT0_BIE
.set Vlow__BIT_MASK, CYREG_PRT0_BIT_MASK
.set Vlow__BYP, CYREG_PRT0_BYP
.set Vlow__CTL, CYREG_PRT0_CTL
.set Vlow__DM0, CYREG_PRT0_DM0
.set Vlow__DM1, CYREG_PRT0_DM1
.set Vlow__DM2, CYREG_PRT0_DM2
.set Vlow__DR, CYREG_PRT0_DR
.set Vlow__INP_DIS, CYREG_PRT0_INP_DIS
.set Vlow__LCD_COM_SEG, CYREG_PRT0_LCD_COM_SEG
.set Vlow__LCD_EN, CYREG_PRT0_LCD_EN
.set Vlow__MASK, 0x04
.set Vlow__PORT, 0
.set Vlow__PRT, CYREG_PRT0_PRT
.set Vlow__PRTDSI__CAPS_SEL, CYREG_PRT0_CAPS_SEL
.set Vlow__PRTDSI__DBL_SYNC_IN, CYREG_PRT0_DBL_SYNC_IN
.set Vlow__PRTDSI__OE_SEL0, CYREG_PRT0_OE_SEL0
.set Vlow__PRTDSI__OE_SEL1, CYREG_PRT0_OE_SEL1
.set Vlow__PRTDSI__OUT_SEL0, CYREG_PRT0_OUT_SEL0
.set Vlow__PRTDSI__OUT_SEL1, CYREG_PRT0_OUT_SEL1
.set Vlow__PRTDSI__SYNC_OUT, CYREG_PRT0_SYNC_OUT
.set Vlow__PS, CYREG_PRT0_PS
.set Vlow__SHIFT, 2
.set Vlow__SLW, CYREG_PRT0_SLW

/* Vhi */
.set Vhi__0__MASK, 0x01
.set Vhi__0__PC, CYREG_PRT0_PC0
.set Vhi__0__PORT, 0
.set Vhi__0__SHIFT, 0
.set Vhi__AG, CYREG_PRT0_AG
.set Vhi__AMUX, CYREG_PRT0_AMUX
.set Vhi__BIE, CYREG_PRT0_BIE
.set Vhi__BIT_MASK, CYREG_PRT0_BIT_MASK
.set Vhi__BYP, CYREG_PRT0_BYP
.set Vhi__CTL, CYREG_PRT0_CTL
.set Vhi__DM0, CYREG_PRT0_DM0
.set Vhi__DM1, CYREG_PRT0_DM1
.set Vhi__DM2, CYREG_PRT0_DM2
.set Vhi__DR, CYREG_PRT0_DR
.set Vhi__INP_DIS, CYREG_PRT0_INP_DIS
.set Vhi__LCD_COM_SEG, CYREG_PRT0_LCD_COM_SEG
.set Vhi__LCD_EN, CYREG_PRT0_LCD_EN
.set Vhi__MASK, 0x01
.set Vhi__PORT, 0
.set Vhi__PRT, CYREG_PRT0_PRT
.set Vhi__PRTDSI__CAPS_SEL, CYREG_PRT0_CAPS_SEL
.set Vhi__PRTDSI__DBL_SYNC_IN, CYREG_PRT0_DBL_SYNC_IN
.set Vhi__PRTDSI__OE_SEL0, CYREG_PRT0_OE_SEL0
.set Vhi__PRTDSI__OE_SEL1, CYREG_PRT0_OE_SEL1
.set Vhi__PRTDSI__OUT_SEL0, CYREG_PRT0_OUT_SEL0
.set Vhi__PRTDSI__OUT_SEL1, CYREG_PRT0_OUT_SEL1
.set Vhi__PRTDSI__SYNC_OUT, CYREG_PRT0_SYNC_OUT
.set Vhi__PS, CYREG_PRT0_PS
.set Vhi__SHIFT, 0
.set Vhi__SLW, CYREG_PRT0_SLW

/* Miscellaneous */
/* -- WARNING: define names containing LEOPARD or PANTHER are deprecated and will be removed in a future release */
.set CYDEV_DEBUGGING_DPS_SWD_SWV, 6
.set CYDEV_CONFIG_UNUSED_IO_Disallowed, 2
.set CYDEV_CONFIGURATION_MODE_DMA, 2
.set CYDEV_CONFIG_FASTBOOT_ENABLED, 0
.set CYDEV_CHIP_REV_PSOC5LP_PRODUCTION, 0
.set CYDEV_CHIP_REVISION_5B_PRODUCTION, 0
.set CYDEV_CHIP_MEMBER_5B, 4
.set CYDEV_CHIP_FAMILY_PSOC5, 3
.set CYDEV_CHIP_DIE_PSOC5LP, 4
.set CYDEV_CHIP_DIE_EXPECT, CYDEV_CHIP_DIE_PSOC5LP
.set BCLK__BUS_CLK__HZ, 24000000
.set BCLK__BUS_CLK__KHZ, 24000
.set BCLK__BUS_CLK__MHZ, 24
.set CYDEV_CHIP_DIE_ACTUAL, CYDEV_CHIP_DIE_EXPECT
.set CYDEV_CHIP_DIE_LEOPARD, 1
.set CYDEV_CHIP_DIE_PANTHER, 3
.set CYDEV_CHIP_DIE_PSOC4A, 2
.set CYDEV_CHIP_DIE_UNKNOWN, 0
.set CYDEV_CHIP_FAMILY_PSOC3, 1
.set CYDEV_CHIP_FAMILY_PSOC4, 2
.set CYDEV_CHIP_FAMILY_UNKNOWN, 0
.set CYDEV_CHIP_FAMILY_USED, CYDEV_CHIP_FAMILY_PSOC5
.set CYDEV_CHIP_JTAG_ID, 0x2E123069
.set CYDEV_CHIP_MEMBER_3A, 1
.set CYDEV_CHIP_MEMBER_4A, 2
.set CYDEV_CHIP_MEMBER_5A, 3
.set CYDEV_CHIP_MEMBER_UNKNOWN, 0
.set CYDEV_CHIP_MEMBER_USED, CYDEV_CHIP_MEMBER_5B
.set CYDEV_CHIP_REVISION_3A_ES1, 0
.set CYDEV_CHIP_REVISION_3A_ES2, 1
.set CYDEV_CHIP_REVISION_3A_ES3, 3
.set CYDEV_CHIP_REVISION_3A_PRODUCTION, 3
.set CYDEV_CHIP_REVISION_4A_ES0, 17
.set CYDEV_CHIP_REVISION_4A_PRODUCTION, 17
.set CYDEV_CHIP_REVISION_5A_ES0, 0
.set CYDEV_CHIP_REVISION_5A_ES1, 1
.set CYDEV_CHIP_REVISION_5A_PRODUCTION, 1
.set CYDEV_CHIP_REVISION_5B_ES0, 0
.set CYDEV_CHIP_REVISION_USED, CYDEV_CHIP_REVISION_5B_PRODUCTION
.set CYDEV_CHIP_REV_EXPECT, CYDEV_CHIP_REV_PSOC5LP_PRODUCTION
.set CYDEV_CHIP_REV_LEOPARD_ES1, 0
.set CYDEV_CHIP_REV_LEOPARD_ES2, 1
.set CYDEV_CHIP_REV_LEOPARD_ES3, 3
.set CYDEV_CHIP_REV_LEOPARD_PRODUCTION, 3
.set CYDEV_CHIP_REV_PANTHER_ES0, 0
.set CYDEV_CHIP_REV_PANTHER_ES1, 1
.set CYDEV_CHIP_REV_PANTHER_PRODUCTION, 1
.set CYDEV_CHIP_REV_PSOC4A_ES0, 17
.set CYDEV_CHIP_REV_PSOC4A_PRODUCTION, 17
.set CYDEV_CHIP_REV_PSOC5LP_ES0, 0
.set CYDEV_CONFIGURATION_COMPRESSED, 0
.set CYDEV_CONFIGURATION_DMA, 1
.set CYDEV_CONFIGURATION_ECC, 1
.set CYDEV_CONFIGURATION_IMOENABLED, CYDEV_CONFIG_FASTBOOT_ENABLED
.set CYDEV_CONFIGURATION_MODE, CYDEV_CONFIGURATION_MODE_DMA
.set CYDEV_CONFIGURATION_MODE_COMPRESSED, 0
.set CYDEV_CONFIGURATION_MODE_UNCOMPRESSED, 1
.set CYDEV_CONFIG_UNUSED_IO, CYDEV_CONFIG_UNUSED_IO_Disallowed
.set CYDEV_CONFIG_UNUSED_IO_AllowButWarn, 0
.set CYDEV_CONFIG_UNUSED_IO_AllowWithInfo, 1
.set CYDEV_DEBUGGING_DPS, CYDEV_DEBUGGING_DPS_SWD_SWV
.set CYDEV_DEBUGGING_DPS_Disable, 3
.set CYDEV_DEBUGGING_DPS_JTAG_4, 1
.set CYDEV_DEBUGGING_DPS_JTAG_5, 0
.set CYDEV_DEBUGGING_DPS_SWD, 2
.set CYDEV_DEBUGGING_ENABLE, 1
.set CYDEV_DEBUGGING_XRES, 0
.set CYDEV_DEBUG_ENABLE_MASK, 0x20
.set CYDEV_DEBUG_ENABLE_REGISTER, CYREG_MLOGIC_DEBUG
.set CYDEV_DMA_CHANNELS_AVAILABLE, 24
.set CYDEV_ECC_ENABLE, 0
.set CYDEV_HEAP_SIZE, 0x1000
.set CYDEV_INSTRUCT_CACHE_ENABLED, 1
.set CYDEV_INTR_RISING, 0x00000003
.set CYDEV_PROJ_TYPE, 0
.set CYDEV_PROJ_TYPE_BOOTLOADER, 1
.set CYDEV_PROJ_TYPE_LOADABLE, 2
.set CYDEV_PROJ_TYPE_MULTIAPPBOOTLOADER, 3
.set CYDEV_PROJ_TYPE_STANDARD, 0
.set CYDEV_PROTECTION_ENABLE, 0
.set CYDEV_STACK_SIZE, 0x4000
.set CYDEV_USE_BUNDLED_CMSIS, 1
.set CYDEV_VARIABLE_VDDA, 0
.set CYDEV_VDDA_MV, 5000
.set CYDEV_VDDD_MV, 5000
.set CYDEV_VDDIO0_MV, 5000
.set CYDEV_VDDIO1_MV, 5000
.set CYDEV_VDDIO2_MV, 5000
.set CYDEV_VDDIO3_MV, 5000
.set CYDEV_VIO0, 5
.set CYDEV_VIO0_MV, 5000
.set CYDEV_VIO1, 5
.set CYDEV_VIO1_MV, 5000
.set CYDEV_VIO2, 5
.set CYDEV_VIO2_MV, 5000
.set CYDEV_VIO3, 5
.set CYDEV_VIO3_MV, 5000
.set DMA_CHANNELS_USED__MASK0, 0x00000000
.set CYDEV_BOOTLOADER_ENABLE, 0
.endif
