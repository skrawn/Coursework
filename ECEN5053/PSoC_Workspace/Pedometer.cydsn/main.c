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
#include <stdio.h>
#include <math.h>
#include "mma8452q.h"
#include "eeprom_api.h"
#include "steps.h"
#include "fivePtTripleSmooth.h"

uint32_t step_count = 0;
uint32_t time_ticks = 0;

int main()
{
    char display_str[16] = {0};
    
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    LCD_Start();
    I2C_Start();
    isr_INT1_Start();
    isr_INT2_Start();
    isr_ResetSteps_Start();
    isr_ZeroAccel_Start();
    isr_ms_tick_Start();
    Clock_1_Start();
    ms_Clock_Start();
    AMux_Start();
    
    AMux_Select(3);
    
    EEPROM_API_Init();
    MMA8452Q_Init();
    MMA8452Q_Realign();

    for(;;)
    {
                      
        /*MMA8452Q_ReadAll();
        memset(display_str, 0, 16);
        sprintf(display_str, "X:%d.%02d Y:%d.%02d", MMA8452Q_GetXData() / 100, 
            abs(MMA8452Q_GetXData() % 100), MMA8452Q_GetYData() / 100, abs(MMA8452Q_GetYData() % 100));
        LCD_Position(0,0);
        LCD_PrintString(display_str);
        
        memset(display_str, 0, 16);
        sprintf(display_str, "Z:%d.%02d STEP: %04d", MMA8452Q_GetZData() / 100, 
            abs(MMA8452Q_GetZData() % 100), step_count);
        LCD_Position(1,0);
        LCD_PrintString(display_str);*/
        
        CyDelay(100);
    }
}

void Tick_Increment(void)
{
    time_ticks++;
}

uint32_t Tick_Get_Time_Diff(uint32_t last_tick)
{
    if (last_tick < time_ticks)    
        // Counter wrapped around
        return UINT_LEAST32_MAX - last_tick;    
    else
        return time_ticks - last_tick;
}

uint32_t Tick_Get_Ticks(void)
{
    return time_ticks;   
}

void Increment_Step_Count(void)
{
    step_count++;
}

uint32_t Get_Step_Count(void)
{
    return step_count;
}

void Clear_Step_Count(void)
{
    step_count = 0;    
}

/* [] END OF FILE */
