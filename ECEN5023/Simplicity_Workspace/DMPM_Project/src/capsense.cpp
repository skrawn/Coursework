/**************************************************************************//**
 * @file capsense.cpp
 * @brief LESENSE Capacitive Touch Sense Driver
 * @author Sean Donohue
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2016 Sean Donohue</b>
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
 * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 *
 *******************************************************************************/


#include "capsense.h"
#include "em_acmp.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_lesense.h"
#include "em_int.h"

#define Cap_Button_Port		gpioPortC
#define Cap_Button_Mode 	gpioModeDisabled
#define Cap_Button_0_Pin	8
#define Cap_Button_1_Pin	9
#define Cap_Button_2_Pin	10
#define Cap_Button_3_Pin	11

#define CAPSENSE_SENSITIVITY_OFFSET 	1
#define CAPSENSE_ACMP_VDD_SCALE			0x37

uint16_t capsenseCalValues[4] = {0};
LESENSE_ChAll_TypeDef lesenseCh;
LESENSE_ChDesc_TypeDef defaultDesc;

void Capsense_Init(void)
{
	ACMP_CapsenseInit_TypeDef initACMP;
	LESENSE_Init_TypeDef initLESENSE;
	uint8_t i;

	// Configure the GPIO pins that correspond to the cap sense buttons
	GPIO_PinModeSet(Cap_Button_Port, Cap_Button_0_Pin, Cap_Button_Mode, 0);
	GPIO_PinModeSet(Cap_Button_Port, Cap_Button_1_Pin, Cap_Button_Mode, 0);
	GPIO_PinModeSet(Cap_Button_Port, Cap_Button_2_Pin, Cap_Button_Mode, 0);
	GPIO_PinModeSet(Cap_Button_Port, Cap_Button_3_Pin, Cap_Button_Mode, 0);

	// Enable clock to LESENSE, ACMP
	CMU_ClockEnable(cmuClock_ACMP0, 1);
	CMU_ClockEnable(cmuClock_ACMP1, 1);
	CMU_ClockEnable(cmuClock_LESENSE, 1);

	CMU_ClockDivSet(cmuClock_LESENSE, cmuClkDiv_1);

	initACMP.fullBias = false;
	initACMP.halfBias = false;
	initACMP.biasProg = 0x7;
	initACMP.warmTime = acmpWarmTime512;
	initACMP.hysteresisLevel = acmpHysteresisLevel7;
	initACMP.resistor = acmpResistor0;
	initACMP.lowPowerReferenceEnabled = false;
	initACMP.vddLevel = 0x3d;
	initACMP.enable = false;

	// Configure ACMP locations, ACMP output pin disabled
	ACMP_GPIOSetup(ACMP0, 0, false, false);
	ACMP_GPIOSetup(ACMP1, 0, false, false);

	// Initialize ACMP in capacitive sense mode
	ACMP_CapsenseInit(ACMP0, &initACMP);
	ACMP_CapsenseInit(ACMP1, &initACMP);

	initLESENSE.coreCtrl.scanStart = lesenseScanStartPeriodic;
	initLESENSE.coreCtrl.prsSel = lesensePRSCh0;
	initLESENSE.coreCtrl.scanConfSel = lesenseScanConfDirMap;
	initLESENSE.coreCtrl.invACMP0 = false;
	initLESENSE.coreCtrl.invACMP1 = false;
	initLESENSE.coreCtrl.dualSample = false;
	initLESENSE.coreCtrl.storeScanRes = false;
	initLESENSE.coreCtrl.bufOverWr = true;
	initLESENSE.coreCtrl.bufTrigLevel = lesenseBufTrigHalf;
	initLESENSE.coreCtrl.wakeupOnDMA = lesenseDMAWakeUpDisable;
	initLESENSE.coreCtrl.biasMode = lesenseBiasModeDutyCycle;
	initLESENSE.coreCtrl.debugRun = false;

	initLESENSE.timeCtrl.startDelay = 0;

	initLESENSE.perCtrl.dacCh0Data = lesenseDACIfData;
	initLESENSE.perCtrl.dacCh0ConvMode = lesenseDACConvModeDisable;
	initLESENSE.perCtrl.dacCh0OutMode = lesenseDACOutModeDisable;
	initLESENSE.perCtrl.dacCh1Data = lesenseDACIfData;
	initLESENSE.perCtrl.dacCh1ConvMode = lesenseDACConvModeDisable;
	initLESENSE.perCtrl.dacCh1OutMode = lesenseDACOutModeDisable;
	initLESENSE.perCtrl.dacPresc = 0;
	initLESENSE.perCtrl.dacRef = lesenseDACRefBandGap;
	initLESENSE.perCtrl.acmp0Mode = lesenseACMPModeMuxThres;
	initLESENSE.perCtrl.acmp1Mode = lesenseACMPModeMuxThres;
	initLESENSE.perCtrl.warmupMode = lesenseWarmupModeNormal;

	initLESENSE.decCtrl.decInput = lesenseDecInputSensorSt;
	initLESENSE.decCtrl.chkState = false;
	initLESENSE.decCtrl.intMap = true;
	initLESENSE.decCtrl.hystPRS0 = false;
	initLESENSE.decCtrl.hystPRS1 = false;
	initLESENSE.decCtrl.hystPRS2 = false;
	initLESENSE.decCtrl.hystIRQ = false;
	initLESENSE.decCtrl.prsCount = true;
	initLESENSE.decCtrl.prsChSel0 = lesensePRSCh0;
	initLESENSE.decCtrl.prsChSel1 = lesensePRSCh1;
	initLESENSE.decCtrl.prsChSel2 = lesensePRSCh2;
	initLESENSE.decCtrl.prsChSel3 = lesensePRSCh3;

	LESENSE_Init(&initLESENSE, true);

	// Set the scan frequency to 10Hz
	LESENSE_ScanFreqSet(0, 10);

	// Default scan descriptor
	defaultDesc.enaScanCh = true;
	defaultDesc.enaPin = true;
	defaultDesc.enaInt = false;
	defaultDesc.chPinExMode = lesenseChPinExDis;
	defaultDesc.chPinIdleMode = lesenseChPinIdleDis;
	defaultDesc.useAltEx = false;
	defaultDesc.shiftRes = false;
	defaultDesc.invRes = false;
	defaultDesc.storeCntRes = true;
	defaultDesc.exClk = lesenseClkLF;
	defaultDesc.sampleClk = lesenseClkLF;
	defaultDesc.exTime = 0;
	defaultDesc.sampleDelay = 0xF;
	defaultDesc.measDelay = 0;
	defaultDesc.acmpThres = CAPSENSE_ACMP_VDD_SCALE;
	defaultDesc.sampleMode = lesenseSampleModeCounter;
	defaultDesc.intMode = lesenseSetIntLevel;
	defaultDesc.cntThres = 0;
	defaultDesc.compMode = lesenseCompModeLess;

	lesenseCh.Ch[Cap_Button_0_Pin] = defaultDesc;
	lesenseCh.Ch[Cap_Button_1_Pin] = defaultDesc;
	lesenseCh.Ch[Cap_Button_2_Pin] = defaultDesc;
	lesenseCh.Ch[Cap_Button_3_Pin] = defaultDesc;

	// Configure the scan channels
	LESENSE_ChannelAllConfig(&lesenseCh);

	LESENSE_ScanStart();

	// Start scanning of LESENSE channels
	while (!(LESENSE->STATUS & LESENSE_STATUS_BUFHALFFULL));

	for (i = 0; i < 4; i++)
	{
		capsenseCalValues[i] = LESENSE_ScanResultDataBufferGet(i) - CAPSENSE_SENSITIVITY_OFFSET;
	}

	// Set the calibration values
	LESENSE_ChannelThresSet(Cap_Button_0_Pin, CAPSENSE_ACMP_VDD_SCALE, capsenseCalValues[0]);
	LESENSE_ChannelThresSet(Cap_Button_1_Pin, CAPSENSE_ACMP_VDD_SCALE, capsenseCalValues[1]);
	LESENSE_ChannelThresSet(Cap_Button_2_Pin, CAPSENSE_ACMP_VDD_SCALE, capsenseCalValues[2]);
	LESENSE_ChannelThresSet(Cap_Button_3_Pin, CAPSENSE_ACMP_VDD_SCALE, capsenseCalValues[3]);

}

void LESENSE_IRQHandler(void)
{

}
