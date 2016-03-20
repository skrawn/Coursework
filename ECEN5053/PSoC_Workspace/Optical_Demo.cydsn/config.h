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

#if !defined(config_H)
#define config_H

#include <project.h>

#define GREEN_MUX_CH 3
#define RED_MUX_CH   2
#define IR_MUX_CH    1
#define OFF_MUX_CH   0

#define PT_MUX_CH   0
#define PR_MUX_CH   1
#define PD_MUX_CH   2

extern uint8_t LED_Mux_CH, Sensor_Mux_CH, Drive_Value;
    
void printLED_Sensor(void);
void printVoltages(void);
    
#endif

/* [] END OF FILE */
