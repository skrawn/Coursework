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
    int16_t x_n4;
    int16_t x_n3;
    int16_t x_n2;
    int16_t x_n1;
} fivePtTripleSmooth_t;

void fivePtTripleSmooth_InitFilter(fivePtTripleSmooth_t *filter);
int16_t fivePtTripleSmooth_Process(fivePtTripleSmooth_t *filter, int16_t x_n);

/* [] END OF FILE */
