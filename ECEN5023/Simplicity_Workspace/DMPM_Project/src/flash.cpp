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

#include "em_ebi.h"
#include "em_gpio.h"
#include "flash.h"
#include "nandflash.h"

// Define all of the pins for the flash device
#define NAND_IO_Port	gpioPortE
#define NAND_IO0_Pin	8
#define NAND_IO1_Pin	9
#define NAND_IO2_Pin	10
#define NAND_IO3_Pin	11
#define NAND_IO4_Pin	12
#define NAND_IO5_Pin	13
#define NAND_IO6_Pin	14
#define NAND_IO7_Pin	15

#define NAND_PWR_EN_Port	gpioPortB
#define NAND_PWR_EN_Pin		15

#define NAND_RnB_Port	gpioPortD
#define NAND_RnB_Pin		15

#define NAND_nCE_Port	gpioPortD
#define NAND_nCE_Pin		14

#define NAND_nWP_Port	gpioPortD
#define NAND_nWP_Pin		13

#define NAND_nRE_Port	gpioPortF
#define NAND_nRE_Pin		9

#define NAND_nWE_Port	gpioPortF
#define NAND_nWE_Pin		8

#define NAND_ALE_Port	gpioPortC
#define NAND_ALE_Pin	1

#define NAND_CLE_Port	gpioPortC
#define NAND_CLE_Pin	2

#define NAND_N_Pages	62500
#define NAND_N_Blocks
#define NAND_Block_Size	512	//bytes

#define NAND_ALE_BIT	24
#define NAND_CLE_BIT	25

#define NAND_FLASH_SIZE				32000000
#define BLOCK_SIZE					(16 * 1024)
#define PAGE_SIZE					512
#define DEFAULT_NUMBER_OF_PAGES		32
#define MAX_NUMBER_OF_PAGES			(NAND_FLASH_SIZE/PAGE_SIZE)

#define EBI_REGION0_BASE			0x80000000

#define MAX_ACTIVE_VARIABLES 		(PAGE_SIZE / SIZE_OF_VARIABLE)

static bool EE_validateIfErased(EE_Page_TypeDef *page);
static bool EE_WriteToPage(EE_Page_TypeDef *page, uint16_t virtualAddress, uint16_t writeData);
static EE_Status_TypeDef EE_TransferPage(EE_Variable_TypeDef *var, uint16_t writeData);

uint8_t activePageBuffer[PAGE_SIZE];

__STATIC_INLINE void waitReady(void);

void Flash_Init(void)
{
	EBI_Init_TypeDef ebiInit;

	// Configure GPIOs
	GPIO_PinModeSet(NAND_PWR_EN_Port, NAND_PWR_EN_Pin, gpioModePushPull, 1);

	// Read/Busy - input from NAND
	GPIO_PinModeSet(NAND_RnB_Port, NAND_RnB_Pin, gpioModeInput, 0);

	// Adress Latch Enable, Command Latch Enable, Write Protect, Chip Enable,
	// Read Enable - outputs to NAND
	GPIO_PinModeSet(NAND_nCE_Port, NAND_nCE_Pin, gpioModePushPull, 1);
	GPIO_PinModeSet(NAND_nWP_Port, NAND_nWP_Pin, gpioModePushPull, 1);
	GPIO_PinModeSet(NAND_ALE_Port, NAND_ALE_Pin, gpioModePushPull, 0);
	GPIO_PinModeSet(NAND_CLE_Port, NAND_CLE_Pin, gpioModePushPull, 0);
	GPIO_PinModeSet(NAND_nRE_Port, NAND_nRE_Pin, gpioModePushPull, 1);
	GPIO_PinModeSet(NAND_nWE_Port, NAND_nWE_Pin, gpioModePushPull, 1);

	// EBI IO pins
	GPIO_PinModeSet(NAND_IO_Port, NAND_IO0_Pin, gpioModePushPull, 0);
	GPIO_PinModeSet(NAND_IO_Port, NAND_IO1_Pin, gpioModePushPull, 0);
	GPIO_PinModeSet(NAND_IO_Port, NAND_IO2_Pin, gpioModePushPull, 0);
	GPIO_PinModeSet(NAND_IO_Port, NAND_IO3_Pin, gpioModePushPull, 0);
	GPIO_PinModeSet(NAND_IO_Port, NAND_IO4_Pin, gpioModePushPull, 0);
	GPIO_PinModeSet(NAND_IO_Port, NAND_IO5_Pin, gpioModePushPull, 0);
	GPIO_PinModeSet(NAND_IO_Port, NAND_IO6_Pin, gpioModePushPull, 0);
	GPIO_PinModeSet(NAND_IO_Port, NAND_IO7_Pin, gpioModePushPull, 0);

	CMU_ClockEnable(cmuClock_EBI, true);

	ebiInit.mode = ebiModeD8A8;
	ebiInit.ardyPolarity = ebiActiveLow;
	ebiInit.alePolarity = ebiActiveLow;
	ebiInit.wePolarity = ebiActiveLow;
	ebiInit.csPolarity = ebiActiveLow;
	ebiInit.blPolarity = ebiActiveLow;
	ebiInit.blEnable = false;
	ebiInit.noIdle = true;
	ebiInit.ardyEnable = false;
	ebiInit.ardyDisableTimeout = true;
	ebiInit.banks = EBI_BANK0;
	ebiInit.csLines = 0;
	ebiInit.addrSetupCycles = 0;
	ebiInit.addrHoldCycles = 0;
	ebiInit.addrHalfALE = false;
	ebiInit.readSetupCycles = 0;
	ebiInit.readStrobeCycles = 2;
	ebiInit.readHoldCycles = 1;
	ebiInit.readPageMode = false;
	ebiInit.readPrefetch = false;
	ebiInit.readHalfRE = false;
	ebiInit.writeSetupCycles = 0;
	ebiInit.writeStrobeCycles = 2;
	ebiInit.writeHoldCycles = 1;
	ebiInit.writeBufferDisable = false;
	ebiInit.writeHalfWE = false;
	ebiInit.aLow = ebiALowA24;
	ebiInit.aHigh = ebiAHighA26;
	ebiInit.location = ebiLocation1;
	ebiInit.enable = true;

	EBI_Init(&ebiInit);
	EBI->NANDCTRL |= EBI_NANDCTRL_BANKSEL_BANK0 | EBI_NANDCTRL_EN;

	// Initialize the NAND flash driver using DMA
	NANDFLASH_Init(NANDFLASH_DMA_CH);

}

/**************************************************************************//**
 * @brief Enables or disables the external NAND flash.
 * @verbatim Flash_Enable(bool); @endverbatim
 *****************************************************************************/
void Flash_Enable(bool enabled)
{
	if (enabled)
		GPIO_PinOutSet(NAND_PWR_EN_Port, NAND_PWR_EN_Pin);
	else
		GPIO_PinOutClear(NAND_PWR_EN_Port, NAND_PWR_EN_Pin);
}

/**************************************************************************//**
 * @brief Enables or disables the external NAND flash write protect. When true,
 * the NAND flash will protected and cannot be written to.
 * @verbatim Flash_Enable(bool); @endverbatim
 *****************************************************************************/
void Flash_Write_Protect(bool protect)
{
	if (protect)
		GPIO_PinOutClear(NAND_nWP_Port, NAND_nWP_Pin);
	else
		GPIO_PinOutSet(NAND_nWP_Port, NAND_nWP_Pin);
}

// The following code is from Silicon Labs AN0019
/*******************************************************************************
 **************************   GLOBAL VARIABLES   *******************************
 ******************************************************************************/

/* Variables to keep track of what pages are active and receiving. */
static int  activePageNumber    = -1;
static int  receivingPageNumber = -1;

static bool initialized = false;

/* Array of all pages allocated to the eeprom */
static EE_Page_TypeDef pages[DEFAULT_NUMBER_OF_PAGES];

static int16_t         numberOfVariablesDeclared = 0;
static int16_t         numberOfActiveVariables = 0;
static int16_t         numberOfPagesAllocated;
static int16_t 		   numberOfBlocks = NAND256W3A_NUMBLOCKS;


/*******************************************************************************
 **************************   LOCAL FUNCTIONS   ********************************
 ******************************************************************************/

/***************************************************************************//**
* @brief
*   Checks if all the bits in the page are 1's.
*
* @param[in]
*   Pointer to the page that is to be validated.
*
* @return
*   Returns the result of the check.
*
* @verbatim
*   true - All bits in the page are 1's.
*   false - One or more bits in the page are 0's.
* @endverbatim
*******************************************************************************/
static bool EE_validateIfErased(EE_Page_TypeDef *page)
{
	uint8_t pageBuffer[NAND256W3A_PAGESIZE];
	uint32_t i = 0;

	NANDFLASH_ReadPage(*page->startAddress, pageBuffer);

	while (i < NAND256W3A_PAGESIZE) {
		if (pageBuffer[i] != 0xFF)
			return false;
		else
			i++;
	}

	return true;
}


/***************************************************************************//**
 * @brief
 *   Writes the desired data to the specified page.
 *
 * @param[in] page
 *   Pointer to the page to write variable to.
 *
 * @param[in] virtualAddress
 *   The virtual address of the variable to be written.
 *
 * @param[in] writeData
 *   The data to be associated with the given virtual address.
 *
 * @return
 *   Returns whether the write has been a success or not. In normal operation
 *   a failed write indicates that the currently active page is full.
 ******************************************************************************/
static bool EE_WriteToPage(EE_Page_TypeDef *page, uint16_t virtualAddress, uint16_t writeData)
{
  /* Start at the second word. The fist one is reserved for status and erase count. */
  uint32_t *address = page->startAddress + 1;
  uint32_t virtualAddressAndData;

  /* Iterate through the page from the beginning, and stop at the fist empty word. */
  while (address <= page->endAddress)
  {
    /* Empty word found. */
    if (*address == 0xFFFFFFFF)
    {
      /* The virtual address and data is combined to a full word. */
      virtualAddressAndData = ((uint32_t)(virtualAddress << 16) & 0xFFFF0000) | (uint32_t)(writeData);

      /* Make sure that the write to flash is a success. */
      if (MSC_WriteWord(address, &virtualAddressAndData, SIZE_OF_VARIABLE) != eeReturnOk)
      {
        /* Write failed. Halt for debug trace, if enabled. */
        EFM_ASSERT(0);
        return false;
      }
      /* Data written successfully. */
      return true;
    }
    else
    {
      address++;
    }
  }
  /* Reached the end of the page without finding any empty words. */
  return false;
}

/***************************************************************************//**
 * @brief
 *   Erase all blocks in the NAND flash and starts over from page 0.
 *
 * @return
 *   Returns true if the format was successful.
 ******************************************************************************/
bool EE_Format(void)
{
  int i, j = 0;
  bool blockEmpty = true;
  NANDFLASH_Status_TypeDef retStatus;
  EE_Page_TypeDef page;

  /* Erase all pages allocated to the eeprom emulator*/
  for (i = 0; i < numberOfBlocks; i++)
  {
	  page.startAddress = i * NAND256W3A_BLOCKSIZE;
	  page.endAddress = (i + 1) * NAND256W3A_BLOCKSIZE;

	  // Check if the block is already erased
	  while (j++ < (NAND256W3A_BLOCKSIZE / NAND256W3A_PAGESIZE) && blockEmpty) {
		  blockEmpty |= EE_validateIfErased(&page);
	  }

	  if (!blockEmpty)
		  NANDFLASH_EraseBlock(i);

	  blockEmpty = true;
	  j = 0;
  }

  /* Page 0 is the active page. */
  activePageNumber = 0;

  /* There should be no receiving page. */
  receivingPageNumber = -1;

  /* Set page status active to page 0. */
  retStatus = EE_setPageStatusActive(&pages[activePageNumber]);

  /** Successfully formatted pages */
  return true;
}


/***************************************************************************//**
 * @brief
 *   Transfers the most recently written value of each variable, from the active
 *   to a new receiving page.
 *
 * @param[in] var
 *   Pointer to a variable whose data will be written to the first free word of
 *   the receiving page. If var is a null pointer, this operation will be
 *   skipped.
 *
 * @param[in] writeData
 *   Data to be associated with var's virtual address, if var is not a null
 *   pointer.
 *
 * @return
 *   Returns the status of the last flash operation.
 ******************************************************************************/
static EE_Status_TypeDef EE_TransferPage(EE_Variable_TypeDef *var, uint16_t writeData)
{
  EE_Status_TypeDef retStatus;
  uint32_t           *activeAddress;
  uint32_t           *receivingAddress;
  bool               newVariable;
  uint32_t           eraseCount;

  /* If there is no receiving page predefined, set it to cycle through all allocated pages. */
  if (receivingPageNumber == -1)
  {
    receivingPageNumber = activePageNumber + 1;

    if (receivingPageNumber >= numberOfPagesAllocated) {
      receivingPageNumber = 0;
    }

    /* Check if the new receiving page really is erased. */
    if (!EE_validateIfErased(&pages[receivingPageNumber]))
    {
      /* If this page is not truly erased, it means that it has been written to
       * from outside this API, this could be an address conflict. */
      EFM_ASSERT(0);
      NANDFLASH_EraseBlock(*pages[receivingPageNumber].startAddress / NAND256W3A_BLOCKSIZE);
    }
  }

  /* Set the status of the receiving page */
  EE_setPageStatusReceiving(&pages[receivingPageNumber]);

  /* If a variable was specified, write it to the receiving page */
  if (var != NULL)
  {
    EE_WriteToPage(&pages[receivingPageNumber], var->virtualAddress, writeData);
  }

  /* Start at the last word. */
  activeAddress = pages[activePageNumber].endAddress;

  /* Iterate through all words in the active page. Each time a new virtual
   * address is found, write it and it's data to the receiving page */
  while (activeAddress > pages[activePageNumber].startAddress)
  {
    /* 0x0000 and 0xFFFF are not valid virtual addresses. */
    if ((uint16_t)(*activeAddress >> 16) == 0x0000 || (uint16_t)(*activeAddress >> 16) == 0xFFFF)
    {
      newVariable = false;
    }
    /* Omit when transfer is initiated from inside the EE_Init() function. */
    else if (var != NULL && (uint16_t)(*activeAddress >> 16) > numberOfVariablesDeclared)
    {
      /* A virtual address outside the virtual address space, defined by the
       * number of variables declared, are considered garbage. */
      newVariable = false;
    }

    else
    {
      receivingAddress = pages[receivingPageNumber].startAddress + 1;

      /* Start at the beginning of the receiving page. Check if the variable is
       * already transfered. */
      while (receivingAddress <= pages[receivingPageNumber].endAddress)
      {
        /* Variable found, and is therefore already transferred. */
        if ((uint16_t)(*activeAddress >> 16) == (uint16_t)(*receivingAddress >> 16))
        {
          newVariable = false;
          break;
        }
        /* Empty word found. All transferred variables are checked.  */
        else if (*receivingAddress == 0xFFFFFFFF)
        {
          newVariable = true;
          break;
        }
        receivingAddress++;
      }
    }

    if (newVariable)
    {
      /* Write the new variable to the receiving page. */
      EE_WriteToPage(&pages[receivingPageNumber], (uint16_t)(*activeAddress >> 16), (uint16_t)(*activeAddress));
    }
    activeAddress--;
  }

  /* Update erase count */
  eraseCount = EE_GetEraseCount();

  /* If a new page cycle is started, increment the erase count. */
  if (receivingPageNumber == 0)
    eraseCount++;

  /* Set the first byte, in this way the page status is not altered when the erase count is written. */
  eraseCount = eraseCount | 0xFF000000;

  /* Write the erase count obtained to the active page head. */
  //retStatus = MSC_WriteWord(pages[receivingPageNumber].startAddress, &eraseCount, 4);
  if (retStatus != eeReturnOk) {
    return retStatus;
  }

  /* Erase the old active page. */
  //retStatus = MSC_ErasePage(pages[activePageNumber].startAddress);
  if (retStatus != eeReturnOk) {
    return retStatus;
  }

  /* Set the receiving page to be the new active page. */
  retStatus = EE_setPageStatusActive(&pages[receivingPageNumber]);
  if (retStatus != eeReturnOk) {
    return retStatus;
  }

  activePageNumber    = receivingPageNumber;
  receivingPageNumber = -1;

  return eeReturnOk;
}


/*******************************************************************************
 **************************   GLOBAL FUNCTIONS   *******************************
 ******************************************************************************/

/***************************************************************************//**
 * @brief
 *    Should be run once before any other eeprom emulator functions are called.
 *    It restores the pages to a known good state in case of page status
 *    corruption after a power loss or another unwanted system reset.
 *
 * @param[in] numberOfPages
 *   Number of pages to be allocated to the eeprom emulator for the entire
 *   lifetime of the application. Must be 2 or greater.
 *
 * @note
 *   When choosing the number of pages to assign to the eeprom emulator, it is
 *   highly recommended to have a good insight into how much data that will be
 *   written to the eeprom emulator during the application lifetime. Using an
 *   estimate of this data, and the maximum number of rewrite cycles the flash
 *   is guaranteed to endure, it should be easy to get a good idea of how many
 *   pages that are needed to keep flash wear within the recommended level
 *   throughout the application lifetime.
 *   It should also be noted that it is critical to have control over what
 *   areas of the flash that is in use by other parts of the application, to
 *   avoid possible conflicts and hard-to-find bugs.
 *
 * @return
 *   Returns true if the initialization was successful.
 ******************************************************************************/
bool EE_Init(uint32_t numberOfPages)
{
  /* Make sure that the eeprom emulator is only initialized once. More that one
   * initialization may result in undefined behavior. */
  EFM_ASSERT(!initialized);

  initialized = true;

  /* Number of pages must be at least 2. */
  if (numberOfPages < 2) {
    numberOfPages = DEFAULT_NUMBER_OF_PAGES;
  }

  /* Make the number of pages allocated accessible throughout the file. */
  numberOfPagesAllocated = numberOfPages;

  /* Initialize the address of each page */
  uint32_t i;
  for (i = 0; i < numberOfPages; i++)
  {
    pages[i].startAddress = (uint32_t *)(NAND_FLASH_SIZE - i * PAGE_SIZE - PAGE_SIZE);
    pages[i].endAddress   = (uint32_t *)(NAND_FLASH_SIZE - i * PAGE_SIZE - 4);
  }

  /* Check status of each page */
  for (i = 0; i < numberOfPages; i++)
  {
    switch (EE_getPageStatus(&pages[i]))
    {
    case eePageStatusActive:
      if (activePageNumber == -1)
      {
        activePageNumber = i;
      }
      else
      {
        /* More than one active page found. This is an invalid system state. */
        return false;
      }
      break;
    case eePageStatusReceiving:
      if (receivingPageNumber == -1)
      {
        receivingPageNumber = i;
      }
      else
      {
        /* More than one receiving page foudn. This is an invalid system state. */
        return false;
      }
      break;
    case eePageStatusErased:
      /* Validate if the page is really erased, and erase it if not. */
      if (!EE_validateIfErased(&pages[i]))
      {
        MSC_ErasePage(pages[i].startAddress);
      }
      break;
    default:
      /* Undefined page status, erase page. */
      MSC_ErasePage(pages[i].startAddress);
      break;
    }
  }

  /* No receiving or active page found. This is an invalid system state. */
  if (receivingPageNumber == -1 && activePageNumber == -1)
  {
    return false;
  }

  /* One active page only. All good. */
  if (receivingPageNumber == -1)
  {
    return true;
  }

  /* One receiving page only. */
  else if (activePageNumber == -1)
  {
    /* Set current receiving page as active. */
    activePageNumber    = receivingPageNumber;
    receivingPageNumber = -1;
    EE_setPageStatusActive(&pages[receivingPageNumber]);
  }
  /* Found exactly one active and one receiving page. */
  else
  {
    /* Transfer variables from active to receiving page. */
    EE_TransferPage(NULL, NULL);
  }

  /* Initialization completed successfully */
  return true;
}


/***************************************************************************//**
 * @brief
 *   Read the latest data associated with the given variable.
 *
 * @note
 *   If attempting to read from an undeclared variable, or a variable that has
 *   no valid value written to it, a null value will be returned.
 *
 * @param[in] var
 *   Pointer to the variable with the virtual address to look for.
 *
 * @param[out] readData
 *   Pointer to the memory area to write the read data to.
 *
 * @return
 *   Returns whether the variable given was found or not.
 ******************************************************************************/
bool EE_Read(EE_Variable_TypeDef *var, uint32_t *readData)
{
  /* Make sure that the eeprom emulator is initialized. */
  EFM_ASSERT(initialized);

  uint32_t *address;

  address = (pages[activePageNumber].endAddress);

  /* 0x0000 and 0xFFFF are illegal addresses. */
  if (var->virtualAddress != 0x0000 && var->virtualAddress != 0xFFFF)
  {
    /* Iterate through the active page, starting from the end. */
    while (address > pages[activePageNumber].startAddress)
    {
      /* Check if the stored virtual address matches the one wanted. */
      if ((uint32_t)(*address >> 16) == var->virtualAddress)
      {
        /* Correct virtual address found, return the corresponding data. */
        *readData = (uint32_t)(*address);
        return true;
      }
      address--;
    }
  }
  /* Variable not found, return null value. */
  *readData = 0x0000;
  return false;
}


/***************************************************************************//**
 * @brief
 *   Writes the desired data, together with the given variable's virtual address
 *   to the emulated eeprom memory.
 *
 * @param[in] var
 *   Pointer to the variable to associate data with.
 *
 * @param[in] writeData
 *   The 16 bit data to write to the flash memory. Any 16 bit data can be sent,
 *   as long as it is casted to a uint16_t type.
 ******************************************************************************/
void EE_Write(EE_Variable_TypeDef *var, uint32_t writeData)
{
  /* Make sure that the eeprom emulator is initialized. */
  EFM_ASSERT(initialized);

  /* Make sure that the virtual address is declared and valid. */
  EFM_ASSERT(var->virtualAddress <= numberOfVariablesDeclared);

  uint32_t readData;

  /* Check whether the variable already has a value associated to it. */
  if (EE_Read(var, &readData))
  {
    /* Do not write if data is duplicate. */
    if (readData == writeData)
    {
      return;
    }
  }
  /* Write to flash. */
  if (!EE_WriteToPage(&pages[activePageNumber], var->virtualAddress, writeData))
  {
    /* The write was not successful, which indicates that the active page is full. */
    EE_TransferPage(var, writeData);
  }
}


/***************************************************************************//**
* @brief
*   If the variable no longer is needed, it can be deleted to free flash space.
*
* @note
*   The function writes 0x0000 to the virtual address field of all previous
*   versions of the input variable. All stored data with virtual address 0x0000
*   is marked as garbage, which will be collected on the next page transfer.
*
* @param[in] var
*   Pointer to the variable to be deleted from the emulated eeprom.
*******************************************************************************/
void EE_DeleteVariable(EE_Variable_TypeDef *var)
{
  /* If the eeprom emulator is not initialized, this function has no meaning. */
  if (!initialized)
  {
    /* Halt for debug trace. */
    EFM_ASSERT(0);
    return;
  }

  uint32_t deleteData = 0x0000FFFF;

  uint32_t *address = (pages[activePageNumber].endAddress);

  /* Keep track if we actually removed a variable */
  bool varDeleted = false;

  /* Iterate through the active page from the end. */
  while (address > pages[activePageNumber].startAddress)
  {
    /* Write the virtual address 0x0000 to all instances of the chosen variable.
     * Since 0x0000 is not a valid virtual address, the variable will not
     * transferred to a new page on the next page transfer. */
    if ((uint16_t)(*address >> 16) == var->virtualAddress)
    {
      MSC_WriteWord(address, &deleteData, sizeof deleteData);
      varDeleted = true;
    }
    address--;
  }

  if (varDeleted) {
    numberOfActiveVariables--;
  }
}


/***************************************************************************//**
* @brief
*   Assign a virtual address to a new variable.
*
* @note
*   All variables that is to be used in an application, should be declared
*   prior to any write operations to ensure that all variables are transfered
*   correctly whenever a page is full. The virtual addresses are assigned to
*   the variables according to the order of declaration. This means that in
*   case of a system reset, all variables must be declared in the same order on
*   each startup to retain its previous virtual address.
*
* @param[in] var
*   Pointer to variable to be assigned a virtual address.
* @return
*   Returns whether the declaration was successful.
*******************************************************************************/
bool EE_DeclareVariable(EE_Variable_TypeDef *var)
{
  if ( numberOfActiveVariables < MAX_ACTIVE_VARIABLES ) {

    /* The virtual addresses are assigned according to the order of declaration. */
    var->virtualAddress = ++numberOfVariablesDeclared;

    numberOfActiveVariables++;

    return true;
  } else {
    return false;
  }
}


/***************************************************************************//**
 * @brief
 *   Returns the number of times all pages has been erased.
 *
 * @note
 *   The erase count is the number of cycles where all the pages has been
 *   erased. The value is always written to the 24 LSB of the first word on the
 *   active page. The value is incremented each time data is transferred to
 *   page 0.
 *
 * @return
 *   Returns the number of complete erase cycles endured.
 ******************************************************************************/
uint32_t EE_GetEraseCount(void)
{
  /* Make sure that there is an active page */
  EFM_ASSERT(activePageNumber != -1);

  uint32_t eraseCount;

  /* The number of erase cycles is the 24 LSB of the first word of the active page. */
  eraseCount = (*(pages[activePageNumber].startAddress) & 0x00FFFFFF);

  /* if the page has never been erased, return 0. */
  if (eraseCount == 0xFFFFFF) {
    return 0;
  }

  return eraseCount;
}
