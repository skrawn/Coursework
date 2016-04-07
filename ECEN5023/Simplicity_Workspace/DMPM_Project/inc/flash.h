/**************************************************************************//**
 * @file flash.cpp
 * @brief Flash and EEPROM emulation driver
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

#ifndef FLASH_H_
#define FLASH_H_

#include "mbed.h"
#include "em_msc.h"

#define SIZE_OF_DATA				4		// bytes
#define SIZE_OF_VIRTUAL_ADDRESS		2		// bytes
#define SIZE_OF_VARIABLE			(SIZE_OF_DATA + SIZE_OF_VIRTUAL_ADDRESS)

#define PAGE_STATUS_ERASED			0xFF
#define PAGE_STATUS_RECEIVING		0xAA
#define PAGE_STATUS_ACTIVE			0x00

static uint32_t pageStatusActiveValue = ((uint32_t) PAGE_STATUS_ACTIVE << 24) | 0x00FFFFFF;
static uint32_t pageStatusReceivingValue = ((uint32_t) PAGE_STATUS_RECEIVING << 24) | 0x00FFFFFF;

/*******************************************************************************
 *******************************   ENUMS   *************************************
 ******************************************************************************/
typedef enum {
	eePageStatusErased = PAGE_STATUS_ERASED,
	eePageStatusReceiving = PAGE_STATUS_RECEIVING,
	eePageStatusActive = PAGE_STATUS_ACTIVE
} EE_PageStatus_TypeDef;

typedef enum
{
  eeReturnOk          = 0,  /**< Flash write/erase successful. */
  eeReturnInvalidAddr = -1, /**< Invalid address. Write to an address that is not flash. */
  eeReturnLocked      = -2, /**< Flash address is locked. */
  eeReturnTimeOut     = -3, /**< Timeout while writing to flash. */
  eeReturnUnaligned   = -4  /**< Unaligned access to flash. */
} EE_Status_TypeDef;

/*******************************************************************************
 ******************************   STRUCTS   ************************************
 ******************************************************************************/
typedef struct
{
  /* Each variable is assigned a unique virtual address automatically when first
   * written to, or when using the declare function. */
  uint16_t virtualAddress;
} EE_Variable_TypeDef;

typedef struct
{
  uint32_t *startAddress;
  uint32_t *endAddress;
} EE_Page_TypeDef;

void Flash_Init(void);
void Flash_Enable(bool enabled);
void Flash_ChipEnable(bool enable);
bool EE_Init(uint32_t);
bool EE_Format(uint32_t);
bool EE_Read(EE_Variable_TypeDef *var, uint32_t *readData);
void EE_Write(EE_Variable_TypeDef *var, uint32_t writeData);
bool EE_DeclareVariable(EE_Variable_TypeDef *var);
void EE_DeleteVariable(EE_Variable_TypeDef *var);
uint32_t EE_GetEraseCount(void);

/***************************************************************************//**
 * @brief
 *   Returns the page status of the given page.
 *
 * @param[in] page
 *   Pointer to the page whose status to be returned.
 *
 * @return
 *   Returns the status of the given page.
 ******************************************************************************/
__STATIC_INLINE EE_PageStatus_TypeDef EE_getPageStatus(EE_Page_TypeDef *page)
{
  return (EE_PageStatus_TypeDef)((*(page->startAddress) >> 24) & 0xFF);
}

/***************************************************************************//**
 * @brief
 *   Sets status of the given page to active.
 *
 * @param[in] page
 *   Pointer to the page whose status to be altered.
 *
 * @return
 *   Returns the status of the flash operation.
 ******************************************************************************/
__STATIC_INLINE EE_Status_TypeDef EE_setPageStatusActive(EE_Page_TypeDef *page)
{
  //eturn MSC_WriteWord(page->startAddress, &pageStatusActiveValue, SIZE_OF_VARIABLE);
	return eeReturnOk;
}

/***************************************************************************//**
 * @brief
 *   Sets status of the given page to receiving.
 *
 * @param[in] page
 *   Pointer to the page whose status to be altered.
 *
 * @return
 *   Returns the status of the flash operation.
 ******************************************************************************/
__STATIC_INLINE EE_Status_TypeDef EE_setPageStatusReceiving(EE_Page_TypeDef *page)
{
  //return MSC_WriteWord(page->startAddress, &pageStatusReceivingValue, SIZE_OF_VARIABLE);
	return eeReturnOk;
}

#endif /* FLASH_H_ */
