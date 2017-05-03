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
#include "I2C.h"

/* NOTE: I2C device must be named I2CM in design schematic
   otherwise project will fail to compile! */

void
I2CReadBytes (uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *value)
{
	uint8_t i=0;
	I2CM_MasterSendStart (devAddr, I2CM_WRITE_XFER_MODE);
	I2CM_MasterWriteByte (regAddr);
	I2CM_MasterSendRestart (devAddr, I2CM_READ_XFER_MODE);
	while (i++ < (length-1))
    {
		*value++ = I2CM_MasterReadByte (I2CM_ACK_DATA);
	}
	*value = I2CM_MasterReadByte (I2CM_NAK_DATA);
	I2CM_MasterSendStop ();	
}

void
I2CReadByte (uint8_t devAddr, uint8_t regAddr, uint8_t *value)
{
	I2CReadBytes (devAddr, regAddr, 1, value);
}

void
I2CReadBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *value)
{
   	uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
    I2CReadByte (devAddr, regAddr, value);
    *value &= mask;
    *value >>= (bitStart - length + 1);
}

void
I2CReadBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *value)
{
	I2CReadByte (devAddr, regAddr, value);
	*value = *value & (1 << bitNum);
}
	
void
I2CWriteBytes (uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *value)
{
	uint8_t i=0;
	I2CM_MasterSendStart (devAddr, I2CM_WRITE_XFER_MODE);
	I2CM_MasterWriteByte (regAddr);
	while (i++ < length)
    {
		I2CM_MasterWriteByte (*value++);
	}
	I2CM_MasterSendStop();	
}

void
I2CWriteByte(uint8_t devAddr, uint8_t regAddr, uint8_t value)
{
	I2CWriteBytes (devAddr, regAddr, 1, &value);
}

void
I2CWriteBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t value)
{
	uint8_t b;
	uint8_t mask = ((1 << length) - 1) << (bitStart - length + 1);
	I2CReadByte (devAddr, regAddr, &b);
	value <<= (bitStart - length + 1);
	value &= mask;
	b &= ~(mask);
	b |= value;
	I2CWriteByte (devAddr, regAddr, b);	
}

void
I2CWriteBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t value)
{
	uint8_t b;
	I2CReadByte (devAddr, regAddr, &b);
	b = (value != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
	I2CWriteByte (devAddr, regAddr, b);
}

void
I2CWriteWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *value)
{
	uint8_t i=0;
	I2CM_MasterSendStart (devAddr, I2CM_WRITE_XFER_MODE);
	I2CM_MasterWriteByte (regAddr);
	while (i++ < length)
    {
		I2CM_MasterWriteByte (((uint8_t)*value) >> 8);
		I2CM_MasterWriteByte ((uint8_t)*value++);
	}
	I2CM_MasterSendStop();		
}

void
I2CWriteWord(uint8_t devAddr, uint8_t regAddr, uint16_t value)
{
	I2CWriteWords (devAddr, regAddr, 1, &value);
}

/* [] END OF FILE */
