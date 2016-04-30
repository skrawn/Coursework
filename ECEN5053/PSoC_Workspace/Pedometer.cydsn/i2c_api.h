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

/* [] END OF FILE */

#include <project.h>

void I2C_WriteBuffer(uint8_t slave_addr, uint8_t reg_addr, uint8_t *tx_buf, uint32_t tx_cnt);
void I2C_ReadBuffer(uint8_t slave_addr, uint8_t reg_addr, uint8_t *rx_buf, uint32_t rx_cnt);
