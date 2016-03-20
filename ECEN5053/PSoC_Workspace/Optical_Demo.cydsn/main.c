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
#include "config.h"

uint8_t LED_Mux_CH, Sensor_Mux_CH, Drive_Value;
uint16_t Sensor_Result;

int main()
{    
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    isr_ToggleLED_Start();
    isr_ToggleSensor_Start();
    
    CapSense_Start();
    CapSense_InitializeAllBaselines();
    
    /* Wait until CapSense Finishes */
	while (CapSense_IsBusy()) {};
    
    LCD_Start();
    
    LED_DAC_Start();
    LED_Follower_Start();
    LED_Mux_Start();
    LED_Mux_Select(GREEN_MUX_CH);
    LED_Mux_CH = GREEN_MUX_CH;
    
    PhotoDiode_Amp_Start();
    
    Sensor_Mux_Start();
    Sensor_Mux_Select(PT_MUX_CH);  
    Sensor_Mux_CH = PT_MUX_CH;
    
    Drive_Value = 0xFF;
    
    ADC_Buffer_Start();
    ADC_SAR_Start();
           
    printLED_Sensor();
    
    for(;;)
    {
        /* Place your application code here. */
        ADC_SAR_StartConvert();
        ADC_SAR_IsEndConversion(ADC_SAR_WAIT_FOR_RESULT);
        Sensor_Result = ADC_SAR_GetResult16();
        ADC_SAR_StopConvert();
        printVoltages();
        CyDelay(100);
    }
}

void printLED_Sensor(void)
{
    char displayStr[16]={'\0'};
    char LED[3] = {'\0'};
    char sensor[2] = {'\0'};
    
    switch (LED_Mux_CH)
    {
        case OFF_MUX_CH:  
            memcpy(LED, "OFF", 3);
            break;
        
        case IR_MUX_CH:
            memcpy(LED, "IR ", 3);
            break;
        
        case RED_MUX_CH:
            memcpy(LED, "RED", 3);
            break;
        
        case GREEN_MUX_CH:
            memcpy(LED, "GRE", 3);
            break;
    }
    
    switch (Sensor_Mux_CH)
    {
        case PT_MUX_CH:  
            memcpy(sensor, "PT", 2);
            break;
        
        case PR_MUX_CH:
            memcpy(sensor, "PR", 2);
            break;
        
        case PD_MUX_CH:
            memcpy(sensor, "PD", 2);
            break;
    }
    LCD_Position(0,0);   
    LCD_PrintString(displayStr);
    sprintf(displayStr, "LED:    SNSR:   ");
    memcpy(&displayStr[4], LED, 3);
    memcpy(&displayStr[13], sensor, 2);
    LCD_Position(0,0);    
    LCD_PrintString(displayStr);
}

void printVoltages(void)
{
    char displayStr[16]={'\0'};
    LCD_Position(1,0);   
    LCD_PrintString(displayStr);
    sprintf(displayStr, "I:%0.3f  O:%0.3f", (float) (Drive_Value*0.016), (float) (Sensor_Result*0.00122));
    LCD_Position(1,0);   
    LCD_PrintString(displayStr);
}

/* [] END OF FILE */
