/**************************************************************************//**
 * @file i2c_drv.h
 * @brief I2C Driver
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

#ifndef I2C_DRV_H_
#define I2C_DRV_H_

#include "em_i2c.h"
#include "mbed.h"

#define I2C_DESC_BUF_LEN	4

typedef struct {
	I2C_TransferSeq_TypeDef seq;
	uint16_t turn_around_time_ms;
	void (*transfer_done_cb)(void);
} I2C_Desc_t;

void I2C_Initialize(void);
I2C_TransferReturn_TypeDef I2C_Write_Polling(uint8_t slave_addr, uint16_t reg_addr, uint8_t reg_len, uint8_t *tx_data, uint16_t len);
I2C_TransferReturn_TypeDef I2C_Read_Polling(uint8_t slave_addr, uint16_t reg_addr, uint8_t reg_len, uint8_t *rx_data, uint16_t len);
I2C_TransferReturn_TypeDef I2C_Read_Reg_Polling(uint8_t slave_addr, uint8_t *reg_addr, uint8_t reg_len, uint8_t *rx_data, uint16_t len);
void I2C1_Enable(bool enable);

#endif /* I2C_DRV_H_ */
