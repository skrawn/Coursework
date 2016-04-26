/**************************************************************************//**
 * @file mma8452q.h
 * @brief Xtrinsic MMA8452Q 3-axis accelerometer driver
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


#ifndef MMA8452Q_H_
#define MMA8452Q_H_

void MMA8452Q_Init(void);
void MMA8452Q_Enable(bool enable);
void MMA8452Q_ReadAll(void);
int16_t MMA8452Q_GetXData(void);
int16_t MMA8452Q_GetYData(void);
int16_t MMA8452Q_GetZData(void);
int16_t MMA8452Q_GetMaxX(void);
int16_t MMA8452Q_GetMaxY(void);
int16_t MMA8452Q_GetMaxZ(void);
void MMA8452Q_SetXAlarm(uint16_t alarm_val);
void MMA8452Q_SetYAlarm(uint16_t alarm_val);
void MMA8452Q_SetZAlarm(uint16_t alarm_val);
void MMA8452Q_ResetMax(void);
uint8_t MMA8452Q_GetPulseIntStatus(void);
void MMA8452Q_Realign(void);

#endif /* MMA8452Q_H_ */
