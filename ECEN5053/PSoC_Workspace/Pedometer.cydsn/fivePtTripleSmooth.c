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

#include "fivePtTripleSmooth.h"

void fivePtTripleSmooth_InitFilter(fivePtTripleSmooth_t *filter)
{
    filter->x_n1 = 0;
    filter->x_n2 = 0;
    filter->x_n3 = 0;
    filter->x_n4 = 0;
}

int16_t fivePtTripleSmooth_Process(fivePtTripleSmooth_t *filter, int16_t x_n)
{
    int16_t result = 0;
    
    result = (-(filter->x_n4) + 4*(filter->x_n3) - 6*(filter->x_n2) +
        4*(filter->x_n1) + 69*x_n) / 70;
    
    // Shift the filter parameters
    filter->x_n4 = filter->x_n3;
    filter->x_n3 = filter->x_n2;
    filter->x_n2 = filter->x_n1;
    filter->x_n1 = x_n;
    
    return result;
}

/* [] END OF FILE */
