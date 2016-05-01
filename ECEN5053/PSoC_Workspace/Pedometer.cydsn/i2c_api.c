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

#include "i2c_api.h"
#include "I2C.h"

void I2C_WriteBuffer(uint8_t slave_addr, uint8_t reg_addr, uint8_t *tx_buf, uint32_t tx_cnt)
{
    uint8_t return_status;
    uint8_t tx_remaining = tx_cnt;
    
    return_status = I2C_MasterSendStart(slave_addr, 0);
    return_status |= I2C_MasterWriteByte(reg_addr);
    
    while (tx_remaining > 0)
    {
        return_status |= I2C_MasterWriteByte(*(tx_buf++));       
        tx_remaining--;        
    }
    return_status |= I2C_MasterSendStop();
}

void I2C_ReadBuffer(uint8_t slave_addr, uint8_t reg_addr, uint8_t *rx_buf, uint32_t rx_cnt)
{
    uint8_t return_status;
    uint8_t rx_remaining = rx_cnt;
    
    return_status = I2C_MasterSendStart(slave_addr, 0);
    return_status |= I2C_MasterWriteByte(reg_addr);
    
    return_status |= I2C_MasterSendRestart(slave_addr, 1);    
    
    while (rx_remaining > 0)
    {
        rx_remaining--;        
        if (rx_remaining == 0)
            *(rx_buf) = I2C_MasterReadByte(0);       
        else
            *(rx_buf++) = I2C_MasterReadByte(1);
    }
    return_status |= I2C_MasterSendStop();

}

/* [] END OF FILE */
