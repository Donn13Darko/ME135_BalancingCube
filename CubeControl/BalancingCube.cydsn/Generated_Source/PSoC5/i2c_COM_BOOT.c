/*******************************************************************************
* File Name: i2c_COM_BOOT.c
* Version 3.50
*
* Description:
*  This file provides the source code of the bootloader communication APIs for the
*  I2C component.
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "i2c_COM_PVT.h"

#if defined(CYDEV_BOOTLOADER_IO_COMP) && (i2c_COM_BOOTLOADER_INTERFACE_ENABLED)


/***************************************
*    Bootloader Internal Variables
***************************************/

/* I2C write buffer - The host writes commands here */
static uint8 XDATA i2c_COM_slReadBuf[i2c_COM_BTLDR_SIZEOF_READ_BUFFER];

/* I2C read buffer - The host reads responses from it */
static uint8 XDATA i2c_COM_slWriteBuf[i2c_COM_BTLDR_SIZEOF_WRITE_BUFFER];


/*******************************************************************************
* Function Name: i2c_COM_CyBtldrCommStart
********************************************************************************
*
* Summary:
*  Starts the communication component and enables the interrupt.
*  The read buffer initial state is full and the read always is 0xFFu.
*  The write buffer is clear and ready to receive a commmand.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Side Effects:
*  This function enables the component interrupt. If I2C is enabled
*  without the interrupt enabled, it can lock up the I2C bus.
*
* Global variables:
*  i2c_COM_slWriteBuf - The global variable used to store a received
*                                command.
*  i2c_COM_slReadBuf -  The global variable used to store a response.
*  i2c_COM_slRdBufIndex - The global variable used to store a current
*                                  index within the slave read buffer.
*
*******************************************************************************/
void i2c_COM_CyBtldrCommStart(void) CYSMALL 
{
    /* Read returns 0xFF when buffer is zero. Write transaction is expected. */
    i2c_COM_SlaveInitWriteBuf(i2c_COM_slWriteBuf, i2c_COM_BTLDR_SIZEOF_WRITE_BUFFER);
    i2c_COM_SlaveInitReadBuf (i2c_COM_slReadBuf, 0u);

    i2c_COM_Start();
}


/*******************************************************************************
* Function Name: i2c_COM_CyBtldrCommStop
********************************************************************************
*
* Summary:
*  Disables the communication component and disables the interrupt.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void i2c_COM_CyBtldrCommStop(void) CYSMALL 
{
    i2c_COM_Stop();
}


/*******************************************************************************
* Function Name: i2c_COM_CyBtldrCommReset
********************************************************************************
*
* Summary:
*  Set buffers to the initial state and reset the statuses.
*  The read buffer initial state is full and the read always is 0xFFu.
*  The write buffer is clear and ready to receive a command.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  i2c_COM_slRdBufIndex - The global variable used to store a current
*                                  index within the slave read buffer.
*
*******************************************************************************/
void i2c_COM_CyBtldrCommReset(void) CYSMALL 
{
    /* Make Read buffer full */
    i2c_COM_slRdBufSize = 0u;

    /* Reset Write buffer and Read buffer */
    i2c_COM_slRdBufIndex = 0u;
    i2c_COM_slWrBufIndex = 0u;

    /* Clear read and write status */
    i2c_COM_slStatus = 0u;
}


/*******************************************************************************
* Function Name: i2c_COM_CyBtldrCommWrite
********************************************************************************
*
* Summary:
*  Transmits the status of executed command to the Host.
*  The function updates the I2C read buffer with response and realeases it to
*  the host. All reads return 0xFF till the buffer will be released. All bytes
*  are transfered by the I2C ISR.
*  The function waits with timeout till all bytes will be read.
*  After exist this function the reads return 0xFF.
*
* Parameters:
*  pData:    pointer to data buffer with response command.
*  size:     number of bytes required to be transmitted.
*  count:    actual size of data was transmitted.
*  timeOut:  timeout value in tries of 10uS.
*
* Return:
*  The status of transmit operation.
*
* Global variables:
*  i2c_COM_slReadBuf - The global variable used to store a response.
*  i2c_COM_slRdBufIndex - The global variable used to store a current
*                                  index within the slave read buffer.
*
*******************************************************************************/
cystatus i2c_COM_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
         
{
    cystatus status;
    uint16 timeoutMs;

    status = CYRET_BAD_PARAM;

    if((NULL != pData) && (size > 0u))
    {
        status = CYRET_TIMEOUT;
        timeoutMs = ((uint16) 10u * timeOut);  /* Convert from 10mS check to 1mS checks */

        /* Copy pData to component buffer */
        (void) memcpy((void *) i2c_COM_slReadBuf, (void *) pData, size);
        *count = size;  /* Buffer was copied to I2C buffer */

        /* Buffer is free to be read */
        i2c_COM_slRdBufSize = ((uint8) size);

        while(0u != timeoutMs)  /* Wait till response is read */
        {
            /* Check if host complete read */
            if(i2c_COM_slRdBufIndex == ((uint8) size))
            {
                i2c_COM_slRdBufSize  = 0u;
                i2c_COM_slRdBufIndex = 0u;

                status = CYRET_SUCCESS;
                break;
            }

            CyDelay(i2c_COM_WAIT_1_MS);
            timeoutMs--;
        }
    }

    return(status);
}


/*******************************************************************************
* Function Name: i2c_COM_CyBtldrCommRead
********************************************************************************
*
* Summary:
*  Receives the command from the Host.
*  All bytes are received by the I2C ISR and stored in internal I2C buffer. The
*  function checks status with timeout to detemine the end of transfer and
*  then copy data to bootloader buffer.
*  After exist this function the I2C ISR is able to receive more data.
*
* Parameters:
*  pData:    pointer to data buffer to store command.
*  size:     maximum number of bytes which could to be passed back.
*  count:    actual size of data was received.
*  timeOut:  timeout value in tries of 10uS.
*
* Return:
*  The status of receive operation.
*
* Global variables:
*  i2c_COM_slWriteBuf - The global variable used to store a
*                                received command.
*
*******************************************************************************/
cystatus i2c_COM_CyBtldrCommRead(uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
         
{
    cystatus status;
    uint16 timeoutMs;
    uint8 byteCount;

    status = CYRET_BAD_PARAM;

    if((NULL != pData) && (size > 0u))
    {
        status = CYRET_TIMEOUT;
        timeoutMs = ((uint16) 10u * timeOut);  /* Convert from 10mS checks to 1mS checks */

        while(0u != timeoutMs)  /* Wait for command from host */
        {
            /* Check if host completes write */
            if(0u != (i2c_COM_slStatus & i2c_COM_SSTAT_WR_CMPLT))
            {
                /* Define how many bytes host has been written */
                byteCount = i2c_COM_slWrBufIndex;
                *count = (uint16) byteCount;

                /* Copy command to pData buffer */
                (void) memcpy((void *) pData, (void *) i2c_COM_slWriteBuf,
                              i2c_COM_MIN_UNT16(byteCount, size));

                /* Clear I2C write buffer and status */
                i2c_COM_slStatus     = 0u;
                i2c_COM_slWrBufIndex = 0u;

                status = CYRET_SUCCESS;
                break;
            }

            CyDelay(i2c_COM_WAIT_1_MS);
            timeoutMs--;
        }
    }

    return(status);
}

#endif /* defined(CYDEV_BOOTLOADER_IO_COMP) && (i2c_COM_BOOTLOADER_INTERFACE_ENABLED) */


/* [] END OF FILE */
