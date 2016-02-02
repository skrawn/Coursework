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

#include <stdlib.h>
#include "median5.h"

int compare_i32(const void *a, const void *b);

void median5_Update(int32 *filter, int32 new_val)
{
    int i;
    
    for (i = 0; i < (MEDIAN_FILTER_ORDER - 1); i++)    
        // Shift the current filter components down by 1.
        filter[i] = filter[i+1];    
        
    // Add the new component
    filter[4] = new_val;
}

int32 median5_GetMedian(int32 *filter)
{
    // Copy the filter contents since qsort will over-write them
    int32 temp_filt[MEDIAN_FILTER_ORDER];
    memcpy(temp_filt, filter, sizeof(temp_filt));
    qsort(temp_filt, MEDIAN_FILTER_ORDER, sizeof(int32), compare_i32);
    return temp_filt[MEDIAN_FILTER_ORDER/2];
}

int compare_i32(const void *a, const void *b)
{
    return ( *(int *)a - *(int *)b);   
}

/* [] END OF FILE */
