/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include "EEPROM.h"
#include "eeprom_api.h"

#define FIRST_INIT_VAL              0xBE

#define DEFAULT_XAXISOFFSET			0
#define DEFAULT_YAXISOFFSET			0
#define DEFAULT_ZAXISOFFSET			0

/* Address Map 

0x000 - FIRST_INT_VAL   1
0x001 - XAXISOFFSET     1
0x002 - YAXISOFFSET     1
0x003 - ZAXISOFFSET     1

*/

#define ADDR_FIRST_INIT_VAL             0x000
#define ADDR_XAXISOFFSET                0x001
#define ADDR_YAXISOFFSET                0x002
#define ADDR_ZAXISOFFSET                0x003

EEPROM_Data_t EEPROM_Data;

void EEPROM_API_Init(void)
{
    uint8_t first_init_val = 0;    
    
    EEPROM_Start();
    
    first_init_val = EEPROM_ReadByte(ADDR_FIRST_INIT_VAL);
    if (first_init_val != FIRST_INIT_VAL)
    {
        EEPROM_WriteByte(ADDR_FIRST_INIT_VAL, FIRST_INIT_VAL);
        
        EEPROM_Data.XAxisOffset = 0;
        EEPROM_Data.YAxisOffset = 0;
        EEPROM_Data.ZAxisOffset = 0;
        
        EEPROM_Write_XAxisOffset(0);
        EEPROM_Write_YAxisOffset(0);
        EEPROM_Write_ZAxisOffset(0);
    }
    else
    {
        EEPROM_Data.XAxisOffset = (int8_t) EEPROM_ReadByte(ADDR_XAXISOFFSET);              
        EEPROM_Data.YAxisOffset = (int8_t) EEPROM_ReadByte(ADDR_YAXISOFFSET);                
        EEPROM_Data.ZAxisOffset = (int8_t) EEPROM_ReadByte(ADDR_ZAXISOFFSET);        
    }
}

void EEPROM_Write_XAxisOffset(int8_t offset)
{
    EEPROM_Data.XAxisOffset = offset;
    EEPROM_WriteByte(ADDR_XAXISOFFSET, offset);    
}

void EEPROM_Write_YAxisOffset(int8_t offset)
{
    EEPROM_Data.YAxisOffset = offset;
    EEPROM_WriteByte(ADDR_YAXISOFFSET, offset);    
}

void EEPROM_Write_ZAxisOffset(int8_t offset)
{
    EEPROM_Data.ZAxisOffset = offset;
    EEPROM_WriteByte(ADDR_ZAXISOFFSET, (int8_t) offset);    
}



/* [] END OF FILE */
