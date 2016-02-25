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

int main()
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    isr_ToggleLED_Start();
    LED_Mux_Start();
    LED_Mux_Select(1);
    VDAC8_Start();
    OpAmp_Start();
    PhotoDiode_Amp_Start();
    Diode_Follower_Start();
    Photodiode_Source_Start();
    
    for(;;)
    {
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
