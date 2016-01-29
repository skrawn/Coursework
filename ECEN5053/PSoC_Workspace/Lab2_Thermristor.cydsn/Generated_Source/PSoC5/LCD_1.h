/*******************************************************************************
* File Name: LCD_1.h
* Version 1.70
*
* Description:
*  This header file contains registers and constants associated with the
*  Character LCD component.
*
* Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CHARLCD_LCD_1_H)
#define CY_CHARLCD_LCD_1_H

#include "cytypes.h"
#include "cyfitter.h"


/***************************************
*   Conditional Compilation Parameters
***************************************/

#define LCD_1_CONVERSION_ROUTINES     (1u)
#define LCD_1_CUSTOM_CHAR_SET         (0u)

/* Custom character set types */
#define LCD_1_NONE                     (0u)    /* No Custom Fonts      */
#define LCD_1_HORIZONTAL_BG            (1u)    /* Horizontal Bar Graph */
#define LCD_1_VERTICAL_BG              (2u)    /* Vertical Bar Graph   */
#define LCD_1_USER_DEFINED             (3u)    /* User Defined Fonts   */


/***************************************
*     Data Struct Definitions
***************************************/

/* Sleep Mode API Support */
typedef struct _LCD_1_backupStruct
{
    uint8 enableState;
} LCD_1_BACKUP_STRUCT;


/***************************************
*        Function Prototypes
***************************************/

void LCD_1_Start(void) ;
void LCD_1_Stop(void) ;
void LCD_1_WriteControl(uint8 cByte) ;
void LCD_1_WriteData(uint8 dByte) ;
void LCD_1_PrintString(char8 *string) ;
void LCD_1_Position(uint8 row, uint8 column) ;
void LCD_1_PutChar(char8 character) ;
void LCD_1_IsReady(void) ;
void LCD_1_WrDatNib(uint8 nibble) ;
void LCD_1_WrCntrlNib(uint8 nibble) ;
void LCD_1_Sleep(void) ;
void LCD_1_Wakeup(void) ;

#if((LCD_1_CUSTOM_CHAR_SET == LCD_1_VERTICAL_BG) || \
                (LCD_1_CUSTOM_CHAR_SET == LCD_1_HORIZONTAL_BG))

    void  LCD_1_LoadCustomFonts(const uint8 * customData)
                        ;

    void  LCD_1_DrawHorizontalBG(uint8 row, uint8 column, uint8 maxCharacters, uint8 value)
                         ;

    void LCD_1_DrawVerticalBG(uint8 row, uint8 column, uint8 maxCharacters, uint8 value)
                        ;

#endif /* ((LCD_1_CUSTOM_CHAR_SET == LCD_1_VERTICAL_BG) */

#if(LCD_1_CUSTOM_CHAR_SET == LCD_1_USER_DEFINED)

    void LCD_1_LoadCustomFonts(const uint8 * customData)
                            ;

#endif /* ((LCD_1_CUSTOM_CHAR_SET == LCD_1_USER_DEFINED) */

#if(LCD_1_CONVERSION_ROUTINES == 1u)

    /* ASCII Conversion Routines */
    void LCD_1_PrintHexUint8(uint8 value) ;
    void LCD_1_PrintHexUint16(uint16 value) ;
    void LCD_1_PrintDecUint16(uint16 value) ;

    #define LCD_1_PrintNumber(x)       LCD_1_PrintDecUint16(x)
    #define LCD_1_PrintInt8(x)         LCD_1_PrintHexUint8(x)
    #define LCD_1_PrintInt16(x)        LCD_1_PrintHexUint16(x)

#endif /* LCD_1_CONVERSION_ROUTINES == 1u */

/* Clear Macro */
#define LCD_1_ClearDisplay() LCD_1_WriteControl(LCD_1_CLEAR_DISPLAY)

/* Off Macro */
#define LCD_1_DisplayOff() LCD_1_WriteControl(LCD_1_DISPLAY_CURSOR_OFF)

/* On Macro */
#define LCD_1_DisplayOn() LCD_1_WriteControl(LCD_1_DISPLAY_ON_CURSOR_OFF)


/***************************************
*           API Constants
***************************************/

/* Full Byte Commands Sent as Two Nibbles */
#define LCD_1_DISPLAY_8_BIT_INIT       (0x03u)
#define LCD_1_DISPLAY_4_BIT_INIT       (0x02u)
#define LCD_1_DISPLAY_CURSOR_OFF       (0x08u)
#define LCD_1_CLEAR_DISPLAY            (0x01u)
#define LCD_1_CURSOR_AUTO_INCR_ON      (0x06u)
#define LCD_1_DISPLAY_CURSOR_ON        (0x0Eu)
#define LCD_1_DISPLAY_2_LINES_5x10     (0x2Cu)
#define LCD_1_DISPLAY_ON_CURSOR_OFF    (0x0Cu)

#define LCD_1_RESET_CURSOR_POSITION    (0x03u)
#define LCD_1_CURSOR_WINK              (0x0Du)
#define LCD_1_CURSOR_BLINK             (0x0Fu)
#define LCD_1_CURSOR_SH_LEFT           (0x10u)
#define LCD_1_CURSOR_SH_RIGHT          (0x14u)
#define LCD_1_CURSOR_HOME              (0x02u)
#define LCD_1_CURSOR_LEFT              (0x04u)
#define LCD_1_CURSOR_RIGHT             (0x06u)

/* Point to Character Generator Ram 0 */
#define LCD_1_CGRAM_0                  (0x40u)

/* Point to Display Data Ram 0 */
#define LCD_1_DDRAM_0                  (0x80u)

/* LCD Characteristics */
#define LCD_1_CHARACTER_WIDTH          (0x05u)
#define LCD_1_CHARACTER_HEIGHT         (0x08u)

#if(LCD_1_CONVERSION_ROUTINES == 1u)
    #define LCD_1_NUMBER_OF_REMAINDERS (0x05u)
    #define LCD_1_TEN                  (0x0Au)
#endif /* LCD_1_CONVERSION_ROUTINES == 1u */

/* Nibble Offset and Mask */
#define LCD_1_NIBBLE_SHIFT             (0x04u)
#define LCD_1_NIBBLE_MASK              (0x0Fu)

/* LCD Module Address Constants */
#define LCD_1_ROW_0_START              (0x80u)
#define LCD_1_ROW_1_START              (0xC0u)
#define LCD_1_ROW_2_START              (0x94u)
#define LCD_1_ROW_3_START              (0xD4u)

/* Custom Character References */
#define LCD_1_CUSTOM_0                 (0x00u)
#define LCD_1_CUSTOM_1                 (0x01u)
#define LCD_1_CUSTOM_2                 (0x02u)
#define LCD_1_CUSTOM_3                 (0x03u)
#define LCD_1_CUSTOM_4                 (0x04u)
#define LCD_1_CUSTOM_5                 (0x05u)
#define LCD_1_CUSTOM_6                 (0x06u)
#define LCD_1_CUSTOM_7                 (0x07u)

/* Other constants */
#define LCD_1_BYTE_UPPER_NIBBLE_SHIFT   (0x04u)
#define LCD_1_BYTE_LOWER_NIBBLE_MASK    (0x0Fu)
#define LCD_1_U16_UPPER_BYTE_SHIFT      (0x08u)
#define LCD_1_U16_LOWER_BYTE_MASK       (0xFFu)
#define LCD_1_CUSTOM_CHAR_SET_LEN       (0x40u)


/***************************************
*             Registers
***************************************/

/* Port Register Definitions */
#define LCD_1_PORT_DR_REG              (*(reg8 *) LCD_1_LCDPort__DR)   /* Data Output Register */
#define LCD_1_PORT_DR_PTR              ( (reg8 *) LCD_1_LCDPort__DR)
#define LCD_1_PORT_PS_REG              (*(reg8 *) LCD_1_LCDPort__PS)   /* Pin State Register */
#define LCD_1_PORT_PS_PTR              ( (reg8 *) LCD_1_LCDPort__PS)
#define LCD_1_PORT_DM0_REG             (*(reg8 *) LCD_1_LCDPort__DM0)  /* Port Drive Mode 0 */
#define LCD_1_PORT_DM0_PTR             ( (reg8 *) LCD_1_LCDPort__DM0)
#define LCD_1_PORT_DM1_REG             (*(reg8 *) LCD_1_LCDPort__DM1)  /* Port Drive Mode 1 */
#define LCD_1_PORT_DM1_PTR             ( (reg8 *) LCD_1_LCDPort__DM1)
#define LCD_1_PORT_DM2_REG             (*(reg8 *) LCD_1_LCDPort__DM2)  /* Port Drive Mode 2 */
#define LCD_1_PORT_DM2_PTR             ( (reg8 *) LCD_1_LCDPort__DM2)

/* These names are obsolete and will be removed in future revisions */
#define LCD_1_PORT_DR                  LCD_1_PORT_DR_REG
#define LCD_1_PORT_PS                  LCD_1_PORT_PS_REG
#define LCD_1_PORT_DM0                 LCD_1_PORT_DM0_REG
#define LCD_1_PORT_DM1                 LCD_1_PORT_DM1_REG
#define LCD_1_PORT_DM2                 LCD_1_PORT_DM2_REG


/***************************************
*       Register Constants
***************************************/

/* SHIFT must be 1 or 0 */
#define LCD_1_PORT_SHIFT               LCD_1_LCDPort__SHIFT
#define LCD_1_PORT_MASK                LCD_1_LCDPort__MASK

/* Drive Mode register values for High Z */
#define LCD_1_HIGH_Z_DM0               (0xFFu)
#define LCD_1_HIGH_Z_DM1               (0x00u)
#define LCD_1_HIGH_Z_DM2               (0x00u)

/* Drive Mode register values for High Z Analog */
#define LCD_1_HIGH_Z_A_DM0             (0x00u)
#define LCD_1_HIGH_Z_A_DM1             (0x00u)
#define LCD_1_HIGH_Z_A_DM2             (0x00u)

/* Drive Mode register values for Strong */
#define LCD_1_STRONG_DM0               (0x00u)
#define LCD_1_STRONG_DM1               (0xFFu)
#define LCD_1_STRONG_DM2               (0xFFu)

/* Pin Masks */
#define LCD_1_RS                       (0x20u << LCD_1_LCDPort__SHIFT)
#define LCD_1_RW                       (0x40u << LCD_1_LCDPort__SHIFT)
#define LCD_1_E                        (0x10u << LCD_1_LCDPort__SHIFT)
#define LCD_1_READY_BIT                (0x08u << LCD_1_LCDPort__SHIFT)
#define LCD_1_DATA_MASK                (0x0Fu << LCD_1_LCDPort__SHIFT)

#endif /* CY_CHARLCD_LCD_1_H */


/* [] END OF FILE */
