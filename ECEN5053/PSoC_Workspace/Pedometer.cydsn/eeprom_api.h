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

#include <project.h>

typedef struct {
    int8_t XAxisOffset;
    int8_t YAxisOffset;
    int8_t ZAxisOffset;
} EEPROM_Data_t;

void EEPROM_API_Init(void);
void EEPROM_Write_XAxisOffset(int8_t offset);
void EEPROM_Write_YAxisOffset(int8_t offset);
void EEPROM_Write_ZAxisOffset(int8_t offset);

extern EEPROM_Data_t EEPROM_Data;

static inline int8_t EEPROM_Get_XAxisOffset(void) { return EEPROM_Data.XAxisOffset; }
static inline int8_t EEPROM_Get_YAxisOffset(void) { return EEPROM_Data.YAxisOffset; }
static inline int8_t EEPROM_Get_ZAxisOffset(void) { return EEPROM_Data.ZAxisOffset; }

/* [] END OF FILE */
