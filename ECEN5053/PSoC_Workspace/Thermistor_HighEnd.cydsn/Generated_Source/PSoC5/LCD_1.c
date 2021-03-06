/*******************************************************************************
* File Name: LCD_1.c
* Version 1.70
*
* Description:
*  This file provides source code for the Character LCD component's API.
*
* Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "CyLib.h"
#include "LCD_1.h"


void LCD_1_Init(void) ;
void LCD_1_Enable(void) ;


uint8 LCD_1_enableState = 0u;

uint8 LCD_1_initVar = 0u;

char8 const CYCODE LCD_1_hex[16u] = "0123456789ABCDEF";

extern uint8 const CYCODE LCD_1_customFonts[];


/*******************************************************************************
* Function Name: LCD_1_Init
********************************************************************************
*
* Summary:
*  Perform initialization required for components normal work.
*  This function initializes the LCD hardware module as follows:
*        Enable 4-bit interface
*        Clear the display
*        Enable auto cursor increment
*        Resets the cursor to start position
*  Also loads custom character set to LCD if it was defined in the customizer.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void LCD_1_Init(void) 
{
    /* INIT CODE */
    CyDelay(40u);                                                        /* Delay 40 ms */
    LCD_1_WrCntrlNib(LCD_1_DISPLAY_8_BIT_INIT);   /* Selects 8-bit mode */
    CyDelay(5u);                                                         /* Delay 5 ms */
    LCD_1_WrCntrlNib(LCD_1_DISPLAY_8_BIT_INIT);   /* Selects 8-bit mode */
    CyDelay(15u);                                                        /* Delay 15 ms */
    LCD_1_WrCntrlNib(LCD_1_DISPLAY_8_BIT_INIT);   /* Selects 8-bit mode */
    CyDelay(1u);                                                         /* Delay 1 ms */
    LCD_1_WrCntrlNib(LCD_1_DISPLAY_4_BIT_INIT);   /* Selects 4-bit mode */
    CyDelay(5u);                                                         /* Delay 5 ms */

    LCD_1_WriteControl(LCD_1_CURSOR_AUTO_INCR_ON);    /* Incr Cursor After Writes */
    LCD_1_WriteControl(LCD_1_DISPLAY_CURSOR_ON);      /* Turn Display, Cursor ON */
    LCD_1_WriteControl(LCD_1_DISPLAY_2_LINES_5x10);   /* 2 Lines by 5x10 Characters */
    LCD_1_WriteControl(LCD_1_DISPLAY_CURSOR_OFF);     /* Turn Display, Cursor OFF */
    LCD_1_WriteControl(LCD_1_CLEAR_DISPLAY);          /* Clear LCD Screen */
    LCD_1_WriteControl(LCD_1_DISPLAY_ON_CURSOR_OFF);  /* Turn Display ON, Cursor OFF */
    LCD_1_WriteControl(LCD_1_RESET_CURSOR_POSITION);  /* Set Cursor to 0,0 */
    CyDelay(5u);

    #if(LCD_1_CUSTOM_CHAR_SET != LCD_1_NONE)
        LCD_1_LoadCustomFonts(LCD_1_customFonts);
    #endif /* LCD_1_CUSTOM_CHAR_SET != LCD_1_NONE */
}


/*******************************************************************************
* Function Name: LCD_1_Enable
********************************************************************************
*
* Summary:
*  Turns on the display.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Reentrant:
*  No.
*
* Theory:
*  This finction has no effect when it called first time as
*  LCD_1_Init() turns on the LCD.
*
*******************************************************************************/
void LCD_1_Enable(void) 
{
    LCD_1_DisplayOn();
    LCD_1_enableState = 1u;
}


/*******************************************************************************
* Function Name: LCD_1_Start
********************************************************************************
*
* Summary:
*  Perform initialization required for components normal work.
*  This function initializes the LCD hardware module as follows:
*        Enable 4-bit interface
*        Clear the display
*        Enable auto cursor increment
*        Resets the cursor to start position
*  Also loads custom character set to LCD if it was defined in the customizer.
*  If it was not the first call in this project then it just turns on the
*  display
*
*
* Parameters:
*  LCD_1_initVar - global variable.
*
* Return:
*  LCD_1_initVar - global variable.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void LCD_1_Start() 
{
    /* If not initialized then perform initialization */
    if(LCD_1_initVar == 0u)
    {
        LCD_1_Init();
        LCD_1_initVar = 1u;
    }

    /* Turn on the LCD */
    LCD_1_Enable();
}


/*******************************************************************************
* Function Name: LCD_1_Stop
********************************************************************************
*
* Summary:
*  Turns off the display of the LCD screen.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void LCD_1_Stop() 
{
    /* Calls LCD Off Macro */
    LCD_1_DisplayOff();
    LCD_1_enableState = 0u;
}


/*******************************************************************************
*  Function Name: LCD_1_Position
********************************************************************************
*
* Summary:
*  Moves active cursor location to a point specified by the input arguments
*
* Parameters:
*  row:     Specific row of LCD module to be written
*  column:  Column of LCD module to be written
*
* Return:
*  None.
*
* Note:
*  This only applies for LCD displays which use the 2X40 address mode.
*  This results in Row 2 offset from row one by 0x28.
*  When there are more than 2 rows, each row must be fewer than 20 characters.
*
*******************************************************************************/
void LCD_1_Position(uint8 row, uint8 column) 
{
    switch (row)
    {
        case 0:
            LCD_1_WriteControl(LCD_1_ROW_0_START + column);
            break;
        case 1:
            LCD_1_WriteControl(LCD_1_ROW_1_START + column);
            break;
        case 2:
            LCD_1_WriteControl(LCD_1_ROW_2_START + column);
            break;
        case 3:
            LCD_1_WriteControl(LCD_1_ROW_3_START + column);
            break;
        default:
            /* if default case is hit, invalid row argument was passed.*/
            break;
    }
}


/*******************************************************************************
* Function Name: LCD_1_PrintString
********************************************************************************
*
* Summary:
*  Writes a zero terminated string to the LCD.
*
* Parameters:
*  string:  pointer to head of char8 array to be written to the LCD module
*
* Return:
*  None.
*
*******************************************************************************/
void LCD_1_PrintString(char8 * string) 
{
    uint8 indexU8 = 1u;
    char8 current = *string;

    /* Until null is reached, print next character */
    while(current != (char8) '\0')
    {
        LCD_1_WriteData(current);
        current = *(string + indexU8);
        indexU8++;
    }
}


/*******************************************************************************
*  Function Name: LCD_1_PutChar
********************************************************************************
*
* Summary:
*  Writes a single character to the current cursor position of the LCD module.
*  Custom character names (_CUSTOM_0 through
*  _CUSTOM_7) are acceptable as inputs.
*
* Parameters:
*  character:  character to be written to the LCD
*
* Return:
*  None.
*
*******************************************************************************/
void LCD_1_PutChar(char8 character) 
{
    LCD_1_WriteData(character);
}


/*******************************************************************************
*  Function Name: LCD_1_WriteData
********************************************************************************
*
* Summary:
*  Writes a data byte to the LCD module's Data Display RAM.
*
* Parameters:
*  dByte:  byte to be written to LCD module.
*
* Return:
*  None.
*
*******************************************************************************/
void LCD_1_WriteData(uint8 dByte) 
{
    uint8 nibble;

    LCD_1_IsReady();
    nibble = dByte >> LCD_1_NIBBLE_SHIFT;

    /* Write high nibble */
    LCD_1_WrDatNib(nibble);

    nibble = dByte & LCD_1_NIBBLE_MASK;
    /* Write low nibble */
    LCD_1_WrDatNib(nibble);
}


/*******************************************************************************
*  Function Name: LCD_1_WriteControl
********************************************************************************
*
* Summary:
*  Writes a command byte to the LCD module.
*
* Parameters:
*  cByte:   byte to be written to LCD module.
*
* Return:
*  None.
*
*******************************************************************************/
void LCD_1_WriteControl(uint8 cByte) 
{
    uint8 nibble;

    nibble = cByte >> LCD_1_NIBBLE_SHIFT;

    LCD_1_IsReady();
    nibble &= LCD_1_NIBBLE_MASK;

    /* WrCntrlNib(High Nibble) */
    LCD_1_WrCntrlNib(nibble);
    nibble = cByte & LCD_1_NIBBLE_MASK;

    /* WrCntrlNib(Low Nibble) */
    LCD_1_WrCntrlNib(nibble);
}


/*******************************************************************************
* Function Name: LCD_1_IsReady
********************************************************************************
*
* Summary:
*  Polls LCD until the ready bit is set.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Note:
*  Changes pins to High-Z.
*
*******************************************************************************/
void LCD_1_IsReady() 
{
    uint8 value = 0u;

    /* Clear the LCD port*/
    LCD_1_PORT_DR_REG &= ~LCD_1_PORT_MASK ;

    /* Change Port to High-Z Status on data pins */

    /* Mask off data pins to clear old values out */
    value = LCD_1_PORT_DM0_REG & ~LCD_1_DATA_MASK;
    /* Load in high Z values for data pins, others unchanged */
    LCD_1_PORT_DM0_REG = value | (LCD_1_HIGH_Z_DM0 & LCD_1_DATA_MASK);

    /* Mask off data pins to clear old values out */
    value = LCD_1_PORT_DM1_REG & ~LCD_1_DATA_MASK;
    /* Load in high Z values for data pins, others unchanged */
    LCD_1_PORT_DM1_REG = value | (LCD_1_HIGH_Z_DM1 & LCD_1_DATA_MASK);

    /* Mask off data pins to clear old values out */
    value = LCD_1_PORT_DM2_REG & ~LCD_1_DATA_MASK;
    /* Load in high Z values for data pins, others unchanged */
    LCD_1_PORT_DM2_REG = value | (LCD_1_HIGH_Z_DM2 & LCD_1_DATA_MASK);

    /* Make sure RS is low */
    LCD_1_PORT_DR_REG &= ~LCD_1_RS;

    /* Set R/W high to read */
    LCD_1_PORT_DR_REG |= LCD_1_RW;

    do
    {
        /* 40 ns delay required before rising Enable and 500ns between neighbour Enables */
        CyDelayUs(0u);

        /* Set E high */
        LCD_1_PORT_DR_REG |= LCD_1_E;

        /* 360 ns delay the setup time for data pins */
        CyDelayUs(1u);

        /* Get port state */
        value = LCD_1_PORT_PS_REG;

        /* Set enable low */
        LCD_1_PORT_DR_REG &= ~LCD_1_E;

        /* Allow time for the enable signal to transition */
        CyDelayUs(0u);
        
        /* Extract ready bit */
        value &= LCD_1_READY_BIT;

        /* Set E high as we in 4-bit interface we need extra oparation */
        LCD_1_PORT_DR_REG |= LCD_1_E;

        /* 360 ns delay the setup time for data pins */
        CyDelayUs(1u);

        /* Set enable low */
        LCD_1_PORT_DR_REG &= ~LCD_1_E;

        /* Repeat until bit 4 is not zero. */

    } while (value != 0u);

    /* Set R/W low to write */
    LCD_1_PORT_DR_REG &= ~LCD_1_RW;

    /* Clear the LCD port*/
    LCD_1_PORT_DR_REG &= ~LCD_1_PORT_MASK ;

    /* Change Port to Output (Strong) on data pins */

    /* Mask off data pins to clear high z values out */
    value = LCD_1_PORT_DM0_REG & (~LCD_1_DATA_MASK);
    /* Load in high Z values for data pins, others unchanged */
    LCD_1_PORT_DM0_REG = value | (LCD_1_STRONG_DM0 & LCD_1_DATA_MASK);

    /* Mask off data pins to clear high z values out */
    value = LCD_1_PORT_DM1_REG & (~LCD_1_DATA_MASK);
    /* Load in high Z values for data pins, others unchanged */
    LCD_1_PORT_DM1_REG = value | (LCD_1_STRONG_DM1 & LCD_1_DATA_MASK);

    /* Mask off data pins to clear high z values out */
    value = LCD_1_PORT_DM2_REG & (~LCD_1_DATA_MASK);
    /* Load in high Z values for data pins, others unchanged */
    LCD_1_PORT_DM2_REG = value | (LCD_1_STRONG_DM2 & LCD_1_DATA_MASK);
}


/*******************************************************************************
*  Function Name: LCD_1_WrDatNib
********************************************************************************
*
* Summary:
*  Writes a data nibble to the LCD module.
*
* Parameters:
*  nibble:  byte containing nibble in least significant nibble to be written
*           to LCD module.
*
* Return:
*  None.
*
*******************************************************************************/
void LCD_1_WrDatNib(uint8 nibble) 
{
    LCD_1_IsReady();

    /* RS shoul be low to select data register */
    LCD_1_PORT_DR_REG |= LCD_1_RS;
    /* Reset RW for write operation */
    LCD_1_PORT_DR_REG &= ~LCD_1_RW;

    /* Two following lines of code will provide us with 40ns delay */
    /* Clear data pins */
    LCD_1_PORT_DR_REG &= ~LCD_1_DATA_MASK;

    /* Write in data, bring E high*/
    LCD_1_PORT_DR_REG |= (LCD_1_E | (nibble << LCD_1_PORT_SHIFT));

    /* Minimum of 230 ns delay */
    CyDelayUs(1u);

    LCD_1_PORT_DR_REG &= ~LCD_1_E;
}


/*******************************************************************************
*  Function Name: LCD_1_WrCntrlNib
********************************************************************************
*
* Summary:
*  Writes a control nibble to the LCD module.
*
* Parameters:
*  nibble:  byte containing nibble in least significant nibble to be written
*           to LCD module.
*
* Return:
*  None.
*
*******************************************************************************/
void LCD_1_WrCntrlNib(uint8 nibble) 
{
    /* RS and RW shoul be low to select instruction register and  write operation respectively */
    LCD_1_PORT_DR_REG &= ~(LCD_1_RS | LCD_1_RW);

    /* Two following lines of code will give provide ua with 40ns delay */
    /* Clear data pins */
    LCD_1_PORT_DR_REG &= ~LCD_1_DATA_MASK;

    /* Write control data and set enable signal */
    LCD_1_PORT_DR_REG |= (LCD_1_E | (nibble << LCD_1_PORT_SHIFT));

    /* Minimum of 230 ns delay */
    CyDelayUs(1u);

    LCD_1_PORT_DR_REG &= ~LCD_1_E;
}


#if(LCD_1_CONVERSION_ROUTINES == 1u)

    /*******************************************************************************
    *  Function Name: LCD_1_PrintHexUint8
    ********************************************************************************
    *
    * Summary:
    *  Print a byte as two ASCII characters.
    *
    * Parameters:
    *  value:  The byte to be printed out as ASCII characters.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void LCD_1_PrintHexUint8(uint8 value) 
    {
        LCD_1_PutChar((char8) LCD_1_hex[value >> LCD_1_BYTE_UPPER_NIBBLE_SHIFT]);
        LCD_1_PutChar((char8) LCD_1_hex[value & LCD_1_BYTE_LOWER_NIBBLE_MASK]);
    }


    /*******************************************************************************
    *  Function Name: LCD_1_PrintHexUint16
    ********************************************************************************
    *
    * Summary:
    *  Print a uint16 as four ASCII characters.
    *
    * Parameters:
    *  value:   The uint16 to be printed out as ASCII characters.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void LCD_1_PrintHexUint16(uint16 value) 
    {
        LCD_1_PrintHexUint8(value >> LCD_1_U16_UPPER_BYTE_SHIFT);
        LCD_1_PrintHexUint8(value & LCD_1_U16_LOWER_BYTE_MASK);
    }


    /*******************************************************************************
    *  Function Name: LCD_1_PrintDecUint16
    ********************************************************************************
    *
    * Summary:
    *  Print an uint32 value as a left-justified decimal value.
    *
    * Parameters:
    *  value:  The byte to be printed out as ASCII characters.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void LCD_1_PrintDecUint16(uint16 value) 
    {

        char8 number[LCD_1_NUMBER_OF_REMAINDERS];
        char8 temp[LCD_1_NUMBER_OF_REMAINDERS];

        uint8 index = 0u;
        uint8 numDigits = 0u;


        /* Load these in reverse order */
        while(value >= LCD_1_TEN)
        {
            temp[index] = (value % LCD_1_TEN) + '0';
            value /= LCD_1_TEN;
            index++;
        }

        temp[index] = (value % LCD_1_TEN) + '0';
        numDigits = index;

        /* While index is greater than or equal to zero */
        while (index != 0xFFu)
        {
            number[numDigits - index] = temp[index];
            index--;
        }

        /* Null Termination */
        number[numDigits + 1u] = (char8) 0;

        /* Print out number */
        LCD_1_PrintString(number);
    }

#endif /* LCD_1_CONVERSION_ROUTINES == 1u */


/* [] END OF FILE */
