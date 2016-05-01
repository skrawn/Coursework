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
    int16_t yn_1;
    int16_t yn_2;
} Hanning_t;

void Hanning_Init(Hanning_t *hanning);
int16_t Hanning_Process(Hanning_t *hanning, int16_t data);

/* [] END OF FILE */
