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

#include "hanning.h"

void Hanning_Init(Hanning_t *hanning)
{
    hanning->yn_1 = 0;
    hanning->yn_2 = 0;
}

int16_t Hanning_Process(Hanning_t *hanning, int16_t data)
{
    int16_t result = (data + 2*hanning->yn_1 + hanning->yn_2) / 4;
    
    // Update the filter components
    hanning->yn_2 = hanning->yn_1;
    //hanning->yn_1 = result;
    hanning->yn_1 = data;
    
    return result;
}

/* [] END OF FILE */
