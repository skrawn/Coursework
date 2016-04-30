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
#include "mma8452q.h"

int main()
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    LCD_Start();
    I2C_Start();
    isr_INT1_Start();
    isr_INT2_Start();
    isr_ResetSteps_Start();
    Clock_1_Start();
    EEPROM_Start();
    
    MMA8452Q_Init();

    for(;;)
    {
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
