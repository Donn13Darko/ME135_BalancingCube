/*******************************************************************************
* File Name: bluetooth_COM.c
* Version 2.50
*
* Description:
*  This file provides all API functionality of the UART component
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "bluetooth_COM.h"
#if (bluetooth_COM_INTERNAL_CLOCK_USED)
    #include "bluetooth_COM_IntClock.h"
#endif /* End bluetooth_COM_INTERNAL_CLOCK_USED */


/***************************************
* Global data allocation
***************************************/

uint8 bluetooth_COM_initVar = 0u;

#if (bluetooth_COM_TX_INTERRUPT_ENABLED && bluetooth_COM_TX_ENABLED)
    volatile uint8 bluetooth_COM_txBuffer[bluetooth_COM_TX_BUFFER_SIZE];
    volatile uint8 bluetooth_COM_txBufferRead = 0u;
    uint8 bluetooth_COM_txBufferWrite = 0u;
#endif /* (bluetooth_COM_TX_INTERRUPT_ENABLED && bluetooth_COM_TX_ENABLED) */

#if (bluetooth_COM_RX_INTERRUPT_ENABLED && (bluetooth_COM_RX_ENABLED || bluetooth_COM_HD_ENABLED))
    uint8 bluetooth_COM_errorStatus = 0u;
    volatile uint8 bluetooth_COM_rxBuffer[bluetooth_COM_RX_BUFFER_SIZE];
    volatile uint8 bluetooth_COM_rxBufferRead  = 0u;
    volatile uint8 bluetooth_COM_rxBufferWrite = 0u;
    volatile uint8 bluetooth_COM_rxBufferLoopDetect = 0u;
    volatile uint8 bluetooth_COM_rxBufferOverflow   = 0u;
    #if (bluetooth_COM_RXHW_ADDRESS_ENABLED)
        volatile uint8 bluetooth_COM_rxAddressMode = bluetooth_COM_RX_ADDRESS_MODE;
        volatile uint8 bluetooth_COM_rxAddressDetected = 0u;
    #endif /* (bluetooth_COM_RXHW_ADDRESS_ENABLED) */
#endif /* (bluetooth_COM_RX_INTERRUPT_ENABLED && (bluetooth_COM_RX_ENABLED || bluetooth_COM_HD_ENABLED)) */


/*******************************************************************************
* Function Name: bluetooth_COM_Start
********************************************************************************
*
* Summary:
*  This is the preferred method to begin component operation.
*  bluetooth_COM_Start() sets the initVar variable, calls the
*  bluetooth_COM_Init() function, and then calls the
*  bluetooth_COM_Enable() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  The bluetooth_COM_intiVar variable is used to indicate initial
*  configuration of this component. The variable is initialized to zero (0u)
*  and set to one (1u) the first time bluetooth_COM_Start() is called. This
*  allows for component initialization without re-initialization in all
*  subsequent calls to the bluetooth_COM_Start() routine.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void bluetooth_COM_Start(void) 
{
    /* If not initialized then initialize all required hardware and software */
    if(bluetooth_COM_initVar == 0u)
    {
        bluetooth_COM_Init();
        bluetooth_COM_initVar = 1u;
    }

    bluetooth_COM_Enable();
}


/*******************************************************************************
* Function Name: bluetooth_COM_Init
********************************************************************************
*
* Summary:
*  Initializes or restores the component according to the customizer Configure
*  dialog settings. It is not necessary to call bluetooth_COM_Init() because
*  the bluetooth_COM_Start() API calls this function and is the preferred
*  method to begin component operation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void bluetooth_COM_Init(void) 
{
    #if(bluetooth_COM_RX_ENABLED || bluetooth_COM_HD_ENABLED)

        #if (bluetooth_COM_RX_INTERRUPT_ENABLED)
            /* Set RX interrupt vector and priority */
            (void) CyIntSetVector(bluetooth_COM_RX_VECT_NUM, &bluetooth_COM_RXISR);
            CyIntSetPriority(bluetooth_COM_RX_VECT_NUM, bluetooth_COM_RX_PRIOR_NUM);
            bluetooth_COM_errorStatus = 0u;
        #endif /* (bluetooth_COM_RX_INTERRUPT_ENABLED) */

        #if (bluetooth_COM_RXHW_ADDRESS_ENABLED)
            bluetooth_COM_SetRxAddressMode(bluetooth_COM_RX_ADDRESS_MODE);
            bluetooth_COM_SetRxAddress1(bluetooth_COM_RX_HW_ADDRESS1);
            bluetooth_COM_SetRxAddress2(bluetooth_COM_RX_HW_ADDRESS2);
        #endif /* End bluetooth_COM_RXHW_ADDRESS_ENABLED */

        /* Init Count7 period */
        bluetooth_COM_RXBITCTR_PERIOD_REG = bluetooth_COM_RXBITCTR_INIT;
        /* Configure the Initial RX interrupt mask */
        bluetooth_COM_RXSTATUS_MASK_REG  = bluetooth_COM_INIT_RX_INTERRUPTS_MASK;
    #endif /* End bluetooth_COM_RX_ENABLED || bluetooth_COM_HD_ENABLED*/

    #if(bluetooth_COM_TX_ENABLED)
        #if (bluetooth_COM_TX_INTERRUPT_ENABLED)
            /* Set TX interrupt vector and priority */
            (void) CyIntSetVector(bluetooth_COM_TX_VECT_NUM, &bluetooth_COM_TXISR);
            CyIntSetPriority(bluetooth_COM_TX_VECT_NUM, bluetooth_COM_TX_PRIOR_NUM);
        #endif /* (bluetooth_COM_TX_INTERRUPT_ENABLED) */

        /* Write Counter Value for TX Bit Clk Generator*/
        #if (bluetooth_COM_TXCLKGEN_DP)
            bluetooth_COM_TXBITCLKGEN_CTR_REG = bluetooth_COM_BIT_CENTER;
            bluetooth_COM_TXBITCLKTX_COMPLETE_REG = ((bluetooth_COM_NUMBER_OF_DATA_BITS +
                        bluetooth_COM_NUMBER_OF_START_BIT) * bluetooth_COM_OVER_SAMPLE_COUNT) - 1u;
        #else
            bluetooth_COM_TXBITCTR_PERIOD_REG = ((bluetooth_COM_NUMBER_OF_DATA_BITS +
                        bluetooth_COM_NUMBER_OF_START_BIT) * bluetooth_COM_OVER_SAMPLE_8) - 1u;
        #endif /* End bluetooth_COM_TXCLKGEN_DP */

        /* Configure the Initial TX interrupt mask */
        #if (bluetooth_COM_TX_INTERRUPT_ENABLED)
            bluetooth_COM_TXSTATUS_MASK_REG = bluetooth_COM_TX_STS_FIFO_EMPTY;
        #else
            bluetooth_COM_TXSTATUS_MASK_REG = bluetooth_COM_INIT_TX_INTERRUPTS_MASK;
        #endif /*End bluetooth_COM_TX_INTERRUPT_ENABLED*/

    #endif /* End bluetooth_COM_TX_ENABLED */

    #if(bluetooth_COM_PARITY_TYPE_SW)  /* Write Parity to Control Register */
        bluetooth_COM_WriteControlRegister( \
            (bluetooth_COM_ReadControlRegister() & (uint8)~bluetooth_COM_CTRL_PARITY_TYPE_MASK) | \
            (uint8)(bluetooth_COM_PARITY_TYPE << bluetooth_COM_CTRL_PARITY_TYPE0_SHIFT) );
    #endif /* End bluetooth_COM_PARITY_TYPE_SW */
}


/*******************************************************************************
* Function Name: bluetooth_COM_Enable
********************************************************************************
*
* Summary:
*  Activates the hardware and begins component operation. It is not necessary
*  to call bluetooth_COM_Enable() because the bluetooth_COM_Start() API
*  calls this function, which is the preferred method to begin component
*  operation.

* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  bluetooth_COM_rxAddressDetected - set to initial state (0).
*
*******************************************************************************/
void bluetooth_COM_Enable(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    #if (bluetooth_COM_RX_ENABLED || bluetooth_COM_HD_ENABLED)
        /* RX Counter (Count7) Enable */
        bluetooth_COM_RXBITCTR_CONTROL_REG |= bluetooth_COM_CNTR_ENABLE;

        /* Enable the RX Interrupt */
        bluetooth_COM_RXSTATUS_ACTL_REG  |= bluetooth_COM_INT_ENABLE;

        #if (bluetooth_COM_RX_INTERRUPT_ENABLED)
            bluetooth_COM_EnableRxInt();

            #if (bluetooth_COM_RXHW_ADDRESS_ENABLED)
                bluetooth_COM_rxAddressDetected = 0u;
            #endif /* (bluetooth_COM_RXHW_ADDRESS_ENABLED) */
        #endif /* (bluetooth_COM_RX_INTERRUPT_ENABLED) */
    #endif /* (bluetooth_COM_RX_ENABLED || bluetooth_COM_HD_ENABLED) */

    #if(bluetooth_COM_TX_ENABLED)
        /* TX Counter (DP/Count7) Enable */
        #if(!bluetooth_COM_TXCLKGEN_DP)
            bluetooth_COM_TXBITCTR_CONTROL_REG |= bluetooth_COM_CNTR_ENABLE;
        #endif /* End bluetooth_COM_TXCLKGEN_DP */

        /* Enable the TX Interrupt */
        bluetooth_COM_TXSTATUS_ACTL_REG |= bluetooth_COM_INT_ENABLE;
        #if (bluetooth_COM_TX_INTERRUPT_ENABLED)
            bluetooth_COM_ClearPendingTxInt(); /* Clear history of TX_NOT_EMPTY */
            bluetooth_COM_EnableTxInt();
        #endif /* (bluetooth_COM_TX_INTERRUPT_ENABLED) */
     #endif /* (bluetooth_COM_TX_INTERRUPT_ENABLED) */

    #if (bluetooth_COM_INTERNAL_CLOCK_USED)
        bluetooth_COM_IntClock_Start();  /* Enable the clock */
    #endif /* (bluetooth_COM_INTERNAL_CLOCK_USED) */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: bluetooth_COM_Stop
********************************************************************************
*
* Summary:
*  Disables the UART operation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void bluetooth_COM_Stop(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    /* Write Bit Counter Disable */
    #if (bluetooth_COM_RX_ENABLED || bluetooth_COM_HD_ENABLED)
        bluetooth_COM_RXBITCTR_CONTROL_REG &= (uint8) ~bluetooth_COM_CNTR_ENABLE;
    #endif /* (bluetooth_COM_RX_ENABLED || bluetooth_COM_HD_ENABLED) */

    #if (bluetooth_COM_TX_ENABLED)
        #if(!bluetooth_COM_TXCLKGEN_DP)
            bluetooth_COM_TXBITCTR_CONTROL_REG &= (uint8) ~bluetooth_COM_CNTR_ENABLE;
        #endif /* (!bluetooth_COM_TXCLKGEN_DP) */
    #endif /* (bluetooth_COM_TX_ENABLED) */

    #if (bluetooth_COM_INTERNAL_CLOCK_USED)
        bluetooth_COM_IntClock_Stop();   /* Disable the clock */
    #endif /* (bluetooth_COM_INTERNAL_CLOCK_USED) */

    /* Disable internal interrupt component */
    #if (bluetooth_COM_RX_ENABLED || bluetooth_COM_HD_ENABLED)
        bluetooth_COM_RXSTATUS_ACTL_REG  &= (uint8) ~bluetooth_COM_INT_ENABLE;

        #if (bluetooth_COM_RX_INTERRUPT_ENABLED)
            bluetooth_COM_DisableRxInt();
        #endif /* (bluetooth_COM_RX_INTERRUPT_ENABLED) */
    #endif /* (bluetooth_COM_RX_ENABLED || bluetooth_COM_HD_ENABLED) */

    #if (bluetooth_COM_TX_ENABLED)
        bluetooth_COM_TXSTATUS_ACTL_REG &= (uint8) ~bluetooth_COM_INT_ENABLE;

        #if (bluetooth_COM_TX_INTERRUPT_ENABLED)
            bluetooth_COM_DisableTxInt();
        #endif /* (bluetooth_COM_TX_INTERRUPT_ENABLED) */
    #endif /* (bluetooth_COM_TX_ENABLED) */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: bluetooth_COM_ReadControlRegister
********************************************************************************
*
* Summary:
*  Returns the current value of the control register.
*
* Parameters:
*  None.
*
* Return:
*  Contents of the control register.
*
*******************************************************************************/
uint8 bluetooth_COM_ReadControlRegister(void) 
{
    #if (bluetooth_COM_CONTROL_REG_REMOVED)
        return(0u);
    #else
        return(bluetooth_COM_CONTROL_REG);
    #endif /* (bluetooth_COM_CONTROL_REG_REMOVED) */
}


/*******************************************************************************
* Function Name: bluetooth_COM_WriteControlRegister
********************************************************************************
*
* Summary:
*  Writes an 8-bit value into the control register
*
* Parameters:
*  control:  control register value
*
* Return:
*  None.
*
*******************************************************************************/
void  bluetooth_COM_WriteControlRegister(uint8 control) 
{
    #if (bluetooth_COM_CONTROL_REG_REMOVED)
        if(0u != control)
        {
            /* Suppress compiler warning */
        }
    #else
       bluetooth_COM_CONTROL_REG = control;
    #endif /* (bluetooth_COM_CONTROL_REG_REMOVED) */
}


#if(bluetooth_COM_RX_ENABLED || bluetooth_COM_HD_ENABLED)
    /*******************************************************************************
    * Function Name: bluetooth_COM_SetRxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the RX interrupt sources enabled.
    *
    * Parameters:
    *  IntSrc:  Bit field containing the RX interrupts to enable. Based on the 
    *  bit-field arrangement of the status register. This value must be a 
    *  combination of status register bit-masks shown below:
    *      bluetooth_COM_RX_STS_FIFO_NOTEMPTY    Interrupt on byte received.
    *      bluetooth_COM_RX_STS_PAR_ERROR        Interrupt on parity error.
    *      bluetooth_COM_RX_STS_STOP_ERROR       Interrupt on stop error.
    *      bluetooth_COM_RX_STS_BREAK            Interrupt on break.
    *      bluetooth_COM_RX_STS_OVERRUN          Interrupt on overrun error.
    *      bluetooth_COM_RX_STS_ADDR_MATCH       Interrupt on address match.
    *      bluetooth_COM_RX_STS_MRKSPC           Interrupt on address detect.
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void bluetooth_COM_SetRxInterruptMode(uint8 intSrc) 
    {
        bluetooth_COM_RXSTATUS_MASK_REG  = intSrc;
    }


    /*******************************************************************************
    * Function Name: bluetooth_COM_ReadRxData
    ********************************************************************************
    *
    * Summary:
    *  Returns the next byte of received data. This function returns data without
    *  checking the status. You must check the status separately.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Received data from RX register
    *
    * Global Variables:
    *  bluetooth_COM_rxBuffer - RAM buffer pointer for save received data.
    *  bluetooth_COM_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  bluetooth_COM_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  bluetooth_COM_rxBufferLoopDetect - cleared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 bluetooth_COM_ReadRxData(void) 
    {
        uint8 rxData;

    #if (bluetooth_COM_RX_INTERRUPT_ENABLED)

        uint8 locRxBufferRead;
        uint8 locRxBufferWrite;

        /* Protect variables that could change on interrupt */
        bluetooth_COM_DisableRxInt();

        locRxBufferRead  = bluetooth_COM_rxBufferRead;
        locRxBufferWrite = bluetooth_COM_rxBufferWrite;

        if( (bluetooth_COM_rxBufferLoopDetect != 0u) || (locRxBufferRead != locRxBufferWrite) )
        {
            rxData = bluetooth_COM_rxBuffer[locRxBufferRead];
            locRxBufferRead++;

            if(locRxBufferRead >= bluetooth_COM_RX_BUFFER_SIZE)
            {
                locRxBufferRead = 0u;
            }
            /* Update the real pointer */
            bluetooth_COM_rxBufferRead = locRxBufferRead;

            if(bluetooth_COM_rxBufferLoopDetect != 0u)
            {
                bluetooth_COM_rxBufferLoopDetect = 0u;
                #if ((bluetooth_COM_RX_INTERRUPT_ENABLED) && (bluetooth_COM_FLOW_CONTROL != 0u))
                    /* When Hardware Flow Control selected - return RX mask */
                    #if( bluetooth_COM_HD_ENABLED )
                        if((bluetooth_COM_CONTROL_REG & bluetooth_COM_CTRL_HD_SEND) == 0u)
                        {   /* In Half duplex mode return RX mask only in RX
                            *  configuration set, otherwise
                            *  mask will be returned in LoadRxConfig() API.
                            */
                            bluetooth_COM_RXSTATUS_MASK_REG  |= bluetooth_COM_RX_STS_FIFO_NOTEMPTY;
                        }
                    #else
                        bluetooth_COM_RXSTATUS_MASK_REG  |= bluetooth_COM_RX_STS_FIFO_NOTEMPTY;
                    #endif /* end bluetooth_COM_HD_ENABLED */
                #endif /* ((bluetooth_COM_RX_INTERRUPT_ENABLED) && (bluetooth_COM_FLOW_CONTROL != 0u)) */
            }
        }
        else
        {   /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit */
            rxData = bluetooth_COM_RXDATA_REG;
        }

        bluetooth_COM_EnableRxInt();

    #else

        /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit */
        rxData = bluetooth_COM_RXDATA_REG;

    #endif /* (bluetooth_COM_RX_INTERRUPT_ENABLED) */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: bluetooth_COM_ReadRxStatus
    ********************************************************************************
    *
    * Summary:
    *  Returns the current state of the receiver status register and the software
    *  buffer overflow status.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Current state of the status register.
    *
    * Side Effect:
    *  All status register bits are clear-on-read except
    *  bluetooth_COM_RX_STS_FIFO_NOTEMPTY.
    *  bluetooth_COM_RX_STS_FIFO_NOTEMPTY clears immediately after RX data
    *  register read.
    *
    * Global Variables:
    *  bluetooth_COM_rxBufferOverflow - used to indicate overload condition.
    *   It set to one in RX interrupt when there isn't free space in
    *   bluetooth_COM_rxBufferRead to write new data. This condition returned
    *   and cleared to zero by this API as an
    *   bluetooth_COM_RX_STS_SOFT_BUFF_OVER bit along with RX Status register
    *   bits.
    *
    *******************************************************************************/
    uint8 bluetooth_COM_ReadRxStatus(void) 
    {
        uint8 status;

        status = bluetooth_COM_RXSTATUS_REG & bluetooth_COM_RX_HW_MASK;

    #if (bluetooth_COM_RX_INTERRUPT_ENABLED)
        if(bluetooth_COM_rxBufferOverflow != 0u)
        {
            status |= bluetooth_COM_RX_STS_SOFT_BUFF_OVER;
            bluetooth_COM_rxBufferOverflow = 0u;
        }
    #endif /* (bluetooth_COM_RX_INTERRUPT_ENABLED) */

        return(status);
    }


    /*******************************************************************************
    * Function Name: bluetooth_COM_GetChar
    ********************************************************************************
    *
    * Summary:
    *  Returns the last received byte of data. bluetooth_COM_GetChar() is
    *  designed for ASCII characters and returns a uint8 where 1 to 255 are values
    *  for valid characters and 0 indicates an error occurred or no data is present.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Character read from UART RX buffer. ASCII characters from 1 to 255 are valid.
    *  A returned zero signifies an error condition or no data available.
    *
    * Global Variables:
    *  bluetooth_COM_rxBuffer - RAM buffer pointer for save received data.
    *  bluetooth_COM_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  bluetooth_COM_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  bluetooth_COM_rxBufferLoopDetect - cleared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 bluetooth_COM_GetChar(void) 
    {
        uint8 rxData = 0u;
        uint8 rxStatus;

    #if (bluetooth_COM_RX_INTERRUPT_ENABLED)
        uint8 locRxBufferRead;
        uint8 locRxBufferWrite;

        /* Protect variables that could change on interrupt */
        bluetooth_COM_DisableRxInt();

        locRxBufferRead  = bluetooth_COM_rxBufferRead;
        locRxBufferWrite = bluetooth_COM_rxBufferWrite;

        if( (bluetooth_COM_rxBufferLoopDetect != 0u) || (locRxBufferRead != locRxBufferWrite) )
        {
            rxData = bluetooth_COM_rxBuffer[locRxBufferRead];
            locRxBufferRead++;
            if(locRxBufferRead >= bluetooth_COM_RX_BUFFER_SIZE)
            {
                locRxBufferRead = 0u;
            }
            /* Update the real pointer */
            bluetooth_COM_rxBufferRead = locRxBufferRead;

            if(bluetooth_COM_rxBufferLoopDetect != 0u)
            {
                bluetooth_COM_rxBufferLoopDetect = 0u;
                #if( (bluetooth_COM_RX_INTERRUPT_ENABLED) && (bluetooth_COM_FLOW_CONTROL != 0u) )
                    /* When Hardware Flow Control selected - return RX mask */
                    #if( bluetooth_COM_HD_ENABLED )
                        if((bluetooth_COM_CONTROL_REG & bluetooth_COM_CTRL_HD_SEND) == 0u)
                        {   /* In Half duplex mode return RX mask only if
                            *  RX configuration set, otherwise
                            *  mask will be returned in LoadRxConfig() API.
                            */
                            bluetooth_COM_RXSTATUS_MASK_REG |= bluetooth_COM_RX_STS_FIFO_NOTEMPTY;
                        }
                    #else
                        bluetooth_COM_RXSTATUS_MASK_REG |= bluetooth_COM_RX_STS_FIFO_NOTEMPTY;
                    #endif /* end bluetooth_COM_HD_ENABLED */
                #endif /* bluetooth_COM_RX_INTERRUPT_ENABLED and Hardware flow control*/
            }

        }
        else
        {   rxStatus = bluetooth_COM_RXSTATUS_REG;
            if((rxStatus & bluetooth_COM_RX_STS_FIFO_NOTEMPTY) != 0u)
            {   /* Read received data from FIFO */
                rxData = bluetooth_COM_RXDATA_REG;
                /*Check status on error*/
                if((rxStatus & (bluetooth_COM_RX_STS_BREAK | bluetooth_COM_RX_STS_PAR_ERROR |
                                bluetooth_COM_RX_STS_STOP_ERROR | bluetooth_COM_RX_STS_OVERRUN)) != 0u)
                {
                    rxData = 0u;
                }
            }
        }

        bluetooth_COM_EnableRxInt();

    #else

        rxStatus =bluetooth_COM_RXSTATUS_REG;
        if((rxStatus & bluetooth_COM_RX_STS_FIFO_NOTEMPTY) != 0u)
        {
            /* Read received data from FIFO */
            rxData = bluetooth_COM_RXDATA_REG;

            /*Check status on error*/
            if((rxStatus & (bluetooth_COM_RX_STS_BREAK | bluetooth_COM_RX_STS_PAR_ERROR |
                            bluetooth_COM_RX_STS_STOP_ERROR | bluetooth_COM_RX_STS_OVERRUN)) != 0u)
            {
                rxData = 0u;
            }
        }
    #endif /* (bluetooth_COM_RX_INTERRUPT_ENABLED) */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: bluetooth_COM_GetByte
    ********************************************************************************
    *
    * Summary:
    *  Reads UART RX buffer immediately, returns received character and error
    *  condition.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  MSB contains status and LSB contains UART RX data. If the MSB is nonzero,
    *  an error has occurred.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint16 bluetooth_COM_GetByte(void) 
    {
        
    #if (bluetooth_COM_RX_INTERRUPT_ENABLED)
        uint16 locErrorStatus;
        /* Protect variables that could change on interrupt */
        bluetooth_COM_DisableRxInt();
        locErrorStatus = (uint16)bluetooth_COM_errorStatus;
        bluetooth_COM_errorStatus = 0u;
        bluetooth_COM_EnableRxInt();
        return ( (uint16)(locErrorStatus << 8u) | bluetooth_COM_ReadRxData() );
    #else
        return ( ((uint16)bluetooth_COM_ReadRxStatus() << 8u) | bluetooth_COM_ReadRxData() );
    #endif /* bluetooth_COM_RX_INTERRUPT_ENABLED */
        
    }


    /*******************************************************************************
    * Function Name: bluetooth_COM_GetRxBufferSize
    ********************************************************************************
    *
    * Summary:
    *  Returns the number of received bytes available in the RX buffer.
    *  * RX software buffer is disabled (RX Buffer Size parameter is equal to 4): 
    *    returns 0 for empty RX FIFO or 1 for not empty RX FIFO.
    *  * RX software buffer is enabled: returns the number of bytes available in 
    *    the RX software buffer. Bytes available in the RX FIFO do not take to 
    *    account.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  uint8: Number of bytes in the RX buffer. 
    *    Return value type depends on RX Buffer Size parameter.
    *
    * Global Variables:
    *  bluetooth_COM_rxBufferWrite - used to calculate left bytes.
    *  bluetooth_COM_rxBufferRead - used to calculate left bytes.
    *  bluetooth_COM_rxBufferLoopDetect - checked to decide left bytes amount.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the RX Buffer is.
    *
    *******************************************************************************/
    uint8 bluetooth_COM_GetRxBufferSize(void)
                                                            
    {
        uint8 size;

    #if (bluetooth_COM_RX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt */
        bluetooth_COM_DisableRxInt();

        if(bluetooth_COM_rxBufferRead == bluetooth_COM_rxBufferWrite)
        {
            if(bluetooth_COM_rxBufferLoopDetect != 0u)
            {
                size = bluetooth_COM_RX_BUFFER_SIZE;
            }
            else
            {
                size = 0u;
            }
        }
        else if(bluetooth_COM_rxBufferRead < bluetooth_COM_rxBufferWrite)
        {
            size = (bluetooth_COM_rxBufferWrite - bluetooth_COM_rxBufferRead);
        }
        else
        {
            size = (bluetooth_COM_RX_BUFFER_SIZE - bluetooth_COM_rxBufferRead) + bluetooth_COM_rxBufferWrite;
        }

        bluetooth_COM_EnableRxInt();

    #else

        /* We can only know if there is data in the fifo. */
        size = ((bluetooth_COM_RXSTATUS_REG & bluetooth_COM_RX_STS_FIFO_NOTEMPTY) != 0u) ? 1u : 0u;

    #endif /* (bluetooth_COM_RX_INTERRUPT_ENABLED) */

        return(size);
    }


    /*******************************************************************************
    * Function Name: bluetooth_COM_ClearRxBuffer
    ********************************************************************************
    *
    * Summary:
    *  Clears the receiver memory buffer and hardware RX FIFO of all received data.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  bluetooth_COM_rxBufferWrite - cleared to zero.
    *  bluetooth_COM_rxBufferRead - cleared to zero.
    *  bluetooth_COM_rxBufferLoopDetect - cleared to zero.
    *  bluetooth_COM_rxBufferOverflow - cleared to zero.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Setting the pointers to zero makes the system believe there is no data to
    *  read and writing will resume at address 0 overwriting any data that may
    *  have remained in the RAM.
    *
    * Side Effects:
    *  Any received data not read from the RAM or FIFO buffer will be lost.
    *
    *******************************************************************************/
    void bluetooth_COM_ClearRxBuffer(void) 
    {
        uint8 enableInterrupts;

        /* Clear the HW FIFO */
        enableInterrupts = CyEnterCriticalSection();
        bluetooth_COM_RXDATA_AUX_CTL_REG |= (uint8)  bluetooth_COM_RX_FIFO_CLR;
        bluetooth_COM_RXDATA_AUX_CTL_REG &= (uint8) ~bluetooth_COM_RX_FIFO_CLR;
        CyExitCriticalSection(enableInterrupts);

    #if (bluetooth_COM_RX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        bluetooth_COM_DisableRxInt();

        bluetooth_COM_rxBufferRead = 0u;
        bluetooth_COM_rxBufferWrite = 0u;
        bluetooth_COM_rxBufferLoopDetect = 0u;
        bluetooth_COM_rxBufferOverflow = 0u;

        bluetooth_COM_EnableRxInt();

    #endif /* (bluetooth_COM_RX_INTERRUPT_ENABLED) */

    }


    /*******************************************************************************
    * Function Name: bluetooth_COM_SetRxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Sets the software controlled Addressing mode used by the RX portion of the
    *  UART.
    *
    * Parameters:
    *  addressMode: Enumerated value indicating the mode of RX addressing
    *  bluetooth_COM__B_UART__AM_SW_BYTE_BYTE -  Software Byte-by-Byte address
    *                                               detection
    *  bluetooth_COM__B_UART__AM_SW_DETECT_TO_BUFFER - Software Detect to Buffer
    *                                               address detection
    *  bluetooth_COM__B_UART__AM_HW_BYTE_BY_BYTE - Hardware Byte-by-Byte address
    *                                               detection
    *  bluetooth_COM__B_UART__AM_HW_DETECT_TO_BUFFER - Hardware Detect to Buffer
    *                                               address detection
    *  bluetooth_COM__B_UART__AM_NONE - No address detection
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  bluetooth_COM_rxAddressMode - the parameter stored in this variable for
    *   the farther usage in RX ISR.
    *  bluetooth_COM_rxAddressDetected - set to initial state (0).
    *
    *******************************************************************************/
    void bluetooth_COM_SetRxAddressMode(uint8 addressMode)
                                                        
    {
        #if(bluetooth_COM_RXHW_ADDRESS_ENABLED)
            #if(bluetooth_COM_CONTROL_REG_REMOVED)
                if(0u != addressMode)
                {
                    /* Suppress compiler warning */
                }
            #else /* bluetooth_COM_CONTROL_REG_REMOVED */
                uint8 tmpCtrl;
                tmpCtrl = bluetooth_COM_CONTROL_REG & (uint8)~bluetooth_COM_CTRL_RXADDR_MODE_MASK;
                tmpCtrl |= (uint8)(addressMode << bluetooth_COM_CTRL_RXADDR_MODE0_SHIFT);
                bluetooth_COM_CONTROL_REG = tmpCtrl;

                #if(bluetooth_COM_RX_INTERRUPT_ENABLED && \
                   (bluetooth_COM_RXBUFFERSIZE > bluetooth_COM_FIFO_LENGTH) )
                    bluetooth_COM_rxAddressMode = addressMode;
                    bluetooth_COM_rxAddressDetected = 0u;
                #endif /* End bluetooth_COM_RXBUFFERSIZE > bluetooth_COM_FIFO_LENGTH*/
            #endif /* End bluetooth_COM_CONTROL_REG_REMOVED */
        #else /* bluetooth_COM_RXHW_ADDRESS_ENABLED */
            if(0u != addressMode)
            {
                /* Suppress compiler warning */
            }
        #endif /* End bluetooth_COM_RXHW_ADDRESS_ENABLED */
    }


    /*******************************************************************************
    * Function Name: bluetooth_COM_SetRxAddress1
    ********************************************************************************
    *
    * Summary:
    *  Sets the first of two hardware-detectable receiver addresses.
    *
    * Parameters:
    *  address: Address #1 for hardware address detection.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void bluetooth_COM_SetRxAddress1(uint8 address) 
    {
        bluetooth_COM_RXADDRESS1_REG = address;
    }


    /*******************************************************************************
    * Function Name: bluetooth_COM_SetRxAddress2
    ********************************************************************************
    *
    * Summary:
    *  Sets the second of two hardware-detectable receiver addresses.
    *
    * Parameters:
    *  address: Address #2 for hardware address detection.
    *
    * Return:
    *  None.
    *
    *******************************************************************************/
    void bluetooth_COM_SetRxAddress2(uint8 address) 
    {
        bluetooth_COM_RXADDRESS2_REG = address;
    }

#endif  /* bluetooth_COM_RX_ENABLED || bluetooth_COM_HD_ENABLED*/


#if( (bluetooth_COM_TX_ENABLED) || (bluetooth_COM_HD_ENABLED) )
    /*******************************************************************************
    * Function Name: bluetooth_COM_SetTxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the TX interrupt sources to be enabled, but does not enable the
    *  interrupt.
    *
    * Parameters:
    *  intSrc: Bit field containing the TX interrupt sources to enable
    *   bluetooth_COM_TX_STS_COMPLETE        Interrupt on TX byte complete
    *   bluetooth_COM_TX_STS_FIFO_EMPTY      Interrupt when TX FIFO is empty
    *   bluetooth_COM_TX_STS_FIFO_FULL       Interrupt when TX FIFO is full
    *   bluetooth_COM_TX_STS_FIFO_NOT_FULL   Interrupt when TX FIFO is not full
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void bluetooth_COM_SetTxInterruptMode(uint8 intSrc) 
    {
        bluetooth_COM_TXSTATUS_MASK_REG = intSrc;
    }


    /*******************************************************************************
    * Function Name: bluetooth_COM_WriteTxData
    ********************************************************************************
    *
    * Summary:
    *  Places a byte of data into the transmit buffer to be sent when the bus is
    *  available without checking the TX status register. You must check status
    *  separately.
    *
    * Parameters:
    *  txDataByte: data byte
    *
    * Return:
    * None.
    *
    * Global Variables:
    *  bluetooth_COM_txBuffer - RAM buffer pointer for save data for transmission
    *  bluetooth_COM_txBufferWrite - cyclic index for write to txBuffer,
    *    incremented after each byte saved to buffer.
    *  bluetooth_COM_txBufferRead - cyclic index for read from txBuffer,
    *    checked to identify the condition to write to FIFO directly or to TX buffer
    *  bluetooth_COM_initVar - checked to identify that the component has been
    *    initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void bluetooth_COM_WriteTxData(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function*/
        if(bluetooth_COM_initVar != 0u)
        {
        #if (bluetooth_COM_TX_INTERRUPT_ENABLED)

            /* Protect variables that could change on interrupt. */
            bluetooth_COM_DisableTxInt();

            if( (bluetooth_COM_txBufferRead == bluetooth_COM_txBufferWrite) &&
                ((bluetooth_COM_TXSTATUS_REG & bluetooth_COM_TX_STS_FIFO_FULL) == 0u) )
            {
                /* Add directly to the FIFO. */
                bluetooth_COM_TXDATA_REG = txDataByte;
            }
            else
            {
                if(bluetooth_COM_txBufferWrite >= bluetooth_COM_TX_BUFFER_SIZE)
                {
                    bluetooth_COM_txBufferWrite = 0u;
                }

                bluetooth_COM_txBuffer[bluetooth_COM_txBufferWrite] = txDataByte;

                /* Add to the software buffer. */
                bluetooth_COM_txBufferWrite++;
            }

            bluetooth_COM_EnableTxInt();

        #else

            /* Add directly to the FIFO. */
            bluetooth_COM_TXDATA_REG = txDataByte;

        #endif /*(bluetooth_COM_TX_INTERRUPT_ENABLED) */
        }
    }


    /*******************************************************************************
    * Function Name: bluetooth_COM_ReadTxStatus
    ********************************************************************************
    *
    * Summary:
    *  Reads the status register for the TX portion of the UART.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Contents of the status register
    *
    * Theory:
    *  This function reads the TX status register, which is cleared on read.
    *  It is up to the user to handle all bits in this return value accordingly,
    *  even if the bit was not enabled as an interrupt source the event happened
    *  and must be handled accordingly.
    *
    *******************************************************************************/
    uint8 bluetooth_COM_ReadTxStatus(void) 
    {
        return(bluetooth_COM_TXSTATUS_REG);
    }


    /*******************************************************************************
    * Function Name: bluetooth_COM_PutChar
    ********************************************************************************
    *
    * Summary:
    *  Puts a byte of data into the transmit buffer to be sent when the bus is
    *  available. This is a blocking API that waits until the TX buffer has room to
    *  hold the data.
    *
    * Parameters:
    *  txDataByte: Byte containing the data to transmit
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  bluetooth_COM_txBuffer - RAM buffer pointer for save data for transmission
    *  bluetooth_COM_txBufferWrite - cyclic index for write to txBuffer,
    *     checked to identify free space in txBuffer and incremented after each byte
    *     saved to buffer.
    *  bluetooth_COM_txBufferRead - cyclic index for read from txBuffer,
    *     checked to identify free space in txBuffer.
    *  bluetooth_COM_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to transmit any byte of data in a single transfer
    *
    *******************************************************************************/
    void bluetooth_COM_PutChar(uint8 txDataByte) 
    {
    #if (bluetooth_COM_TX_INTERRUPT_ENABLED)
        /* The temporary output pointer is used since it takes two instructions
        *  to increment with a wrap, and we can't risk doing that with the real
        *  pointer and getting an interrupt in between instructions.
        */
        uint8 locTxBufferWrite;
        uint8 locTxBufferRead;

        do
        { /* Block if software buffer is full, so we don't overwrite. */

        #if ((bluetooth_COM_TX_BUFFER_SIZE > bluetooth_COM_MAX_BYTE_VALUE) && (CY_PSOC3))
            /* Disable TX interrupt to protect variables from modification */
            bluetooth_COM_DisableTxInt();
        #endif /* (bluetooth_COM_TX_BUFFER_SIZE > bluetooth_COM_MAX_BYTE_VALUE) && (CY_PSOC3) */

            locTxBufferWrite = bluetooth_COM_txBufferWrite;
            locTxBufferRead  = bluetooth_COM_txBufferRead;

        #if ((bluetooth_COM_TX_BUFFER_SIZE > bluetooth_COM_MAX_BYTE_VALUE) && (CY_PSOC3))
            /* Enable interrupt to continue transmission */
            bluetooth_COM_EnableTxInt();
        #endif /* (bluetooth_COM_TX_BUFFER_SIZE > bluetooth_COM_MAX_BYTE_VALUE) && (CY_PSOC3) */
        }
        while( (locTxBufferWrite < locTxBufferRead) ? (locTxBufferWrite == (locTxBufferRead - 1u)) :
                                ((locTxBufferWrite - locTxBufferRead) ==
                                (uint8)(bluetooth_COM_TX_BUFFER_SIZE - 1u)) );

        if( (locTxBufferRead == locTxBufferWrite) &&
            ((bluetooth_COM_TXSTATUS_REG & bluetooth_COM_TX_STS_FIFO_FULL) == 0u) )
        {
            /* Add directly to the FIFO */
            bluetooth_COM_TXDATA_REG = txDataByte;
        }
        else
        {
            if(locTxBufferWrite >= bluetooth_COM_TX_BUFFER_SIZE)
            {
                locTxBufferWrite = 0u;
            }
            /* Add to the software buffer. */
            bluetooth_COM_txBuffer[locTxBufferWrite] = txDataByte;
            locTxBufferWrite++;

            /* Finally, update the real output pointer */
        #if ((bluetooth_COM_TX_BUFFER_SIZE > bluetooth_COM_MAX_BYTE_VALUE) && (CY_PSOC3))
            bluetooth_COM_DisableTxInt();
        #endif /* (bluetooth_COM_TX_BUFFER_SIZE > bluetooth_COM_MAX_BYTE_VALUE) && (CY_PSOC3) */

            bluetooth_COM_txBufferWrite = locTxBufferWrite;

        #if ((bluetooth_COM_TX_BUFFER_SIZE > bluetooth_COM_MAX_BYTE_VALUE) && (CY_PSOC3))
            bluetooth_COM_EnableTxInt();
        #endif /* (bluetooth_COM_TX_BUFFER_SIZE > bluetooth_COM_MAX_BYTE_VALUE) && (CY_PSOC3) */

            if(0u != (bluetooth_COM_TXSTATUS_REG & bluetooth_COM_TX_STS_FIFO_EMPTY))
            {
                /* Trigger TX interrupt to send software buffer */
                bluetooth_COM_SetPendingTxInt();
            }
        }

    #else

        while((bluetooth_COM_TXSTATUS_REG & bluetooth_COM_TX_STS_FIFO_FULL) != 0u)
        {
            /* Wait for room in the FIFO */
        }

        /* Add directly to the FIFO */
        bluetooth_COM_TXDATA_REG = txDataByte;

    #endif /* bluetooth_COM_TX_INTERRUPT_ENABLED */
    }


    /*******************************************************************************
    * Function Name: bluetooth_COM_PutString
    ********************************************************************************
    *
    * Summary:
    *  Sends a NULL terminated string to the TX buffer for transmission.
    *
    * Parameters:
    *  string[]: Pointer to the null terminated string array residing in RAM or ROM
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  bluetooth_COM_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  If there is not enough memory in the TX buffer for the entire string, this
    *  function blocks until the last character of the string is loaded into the
    *  TX buffer.
    *
    *******************************************************************************/
    void bluetooth_COM_PutString(const char8 string[]) 
    {
        uint16 bufIndex = 0u;

        /* If not Initialized then skip this function */
        if(bluetooth_COM_initVar != 0u)
        {
            /* This is a blocking function, it will not exit until all data is sent */
            while(string[bufIndex] != (char8) 0)
            {
                bluetooth_COM_PutChar((uint8)string[bufIndex]);
                bufIndex++;
            }
        }
    }


    /*******************************************************************************
    * Function Name: bluetooth_COM_PutArray
    ********************************************************************************
    *
    * Summary:
    *  Places N bytes of data from a memory array into the TX buffer for
    *  transmission.
    *
    * Parameters:
    *  string[]: Address of the memory array residing in RAM or ROM.
    *  byteCount: Number of bytes to be transmitted. The type depends on TX Buffer
    *             Size parameter.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  bluetooth_COM_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  If there is not enough memory in the TX buffer for the entire string, this
    *  function blocks until the last character of the string is loaded into the
    *  TX buffer.
    *
    *******************************************************************************/
    void bluetooth_COM_PutArray(const uint8 string[], uint8 byteCount)
                                                                    
    {
        uint8 bufIndex = 0u;

        /* If not Initialized then skip this function */
        if(bluetooth_COM_initVar != 0u)
        {
            while(bufIndex < byteCount)
            {
                bluetooth_COM_PutChar(string[bufIndex]);
                bufIndex++;
            }
        }
    }


    /*******************************************************************************
    * Function Name: bluetooth_COM_PutCRLF
    ********************************************************************************
    *
    * Summary:
    *  Writes a byte of data followed by a carriage return (0x0D) and line feed
    *  (0x0A) to the transmit buffer.
    *
    * Parameters:
    *  txDataByte: Data byte to transmit before the carriage return and line feed.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  bluetooth_COM_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void bluetooth_COM_PutCRLF(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function */
        if(bluetooth_COM_initVar != 0u)
        {
            bluetooth_COM_PutChar(txDataByte);
            bluetooth_COM_PutChar(0x0Du);
            bluetooth_COM_PutChar(0x0Au);
        }
    }


    /*******************************************************************************
    * Function Name: bluetooth_COM_GetTxBufferSize
    ********************************************************************************
    *
    * Summary:
    *  Returns the number of bytes in the TX buffer which are waiting to be 
    *  transmitted.
    *  * TX software buffer is disabled (TX Buffer Size parameter is equal to 4): 
    *    returns 0 for empty TX FIFO, 1 for not full TX FIFO or 4 for full TX FIFO.
    *  * TX software buffer is enabled: returns the number of bytes in the TX 
    *    software buffer which are waiting to be transmitted. Bytes available in the
    *    TX FIFO do not count.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  Number of bytes used in the TX buffer. Return value type depends on the TX 
    *  Buffer Size parameter.
    *
    * Global Variables:
    *  bluetooth_COM_txBufferWrite - used to calculate left space.
    *  bluetooth_COM_txBufferRead - used to calculate left space.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the TX Buffer is.
    *
    *******************************************************************************/
    uint8 bluetooth_COM_GetTxBufferSize(void)
                                                            
    {
        uint8 size;

    #if (bluetooth_COM_TX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        bluetooth_COM_DisableTxInt();

        if(bluetooth_COM_txBufferRead == bluetooth_COM_txBufferWrite)
        {
            size = 0u;
        }
        else if(bluetooth_COM_txBufferRead < bluetooth_COM_txBufferWrite)
        {
            size = (bluetooth_COM_txBufferWrite - bluetooth_COM_txBufferRead);
        }
        else
        {
            size = (bluetooth_COM_TX_BUFFER_SIZE - bluetooth_COM_txBufferRead) +
                    bluetooth_COM_txBufferWrite;
        }

        bluetooth_COM_EnableTxInt();

    #else

        size = bluetooth_COM_TXSTATUS_REG;

        /* Is the fifo is full. */
        if((size & bluetooth_COM_TX_STS_FIFO_FULL) != 0u)
        {
            size = bluetooth_COM_FIFO_LENGTH;
        }
        else if((size & bluetooth_COM_TX_STS_FIFO_EMPTY) != 0u)
        {
            size = 0u;
        }
        else
        {
            /* We only know there is data in the fifo. */
            size = 1u;
        }

    #endif /* (bluetooth_COM_TX_INTERRUPT_ENABLED) */

    return(size);
    }


    /*******************************************************************************
    * Function Name: bluetooth_COM_ClearTxBuffer
    ********************************************************************************
    *
    * Summary:
    *  Clears all data from the TX buffer and hardware TX FIFO.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  bluetooth_COM_txBufferWrite - cleared to zero.
    *  bluetooth_COM_txBufferRead - cleared to zero.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Setting the pointers to zero makes the system believe there is no data to
    *  read and writing will resume at address 0 overwriting any data that may have
    *  remained in the RAM.
    *
    * Side Effects:
    *  Data waiting in the transmit buffer is not sent; a byte that is currently
    *  transmitting finishes transmitting.
    *
    *******************************************************************************/
    void bluetooth_COM_ClearTxBuffer(void) 
    {
        uint8 enableInterrupts;

        enableInterrupts = CyEnterCriticalSection();
        /* Clear the HW FIFO */
        bluetooth_COM_TXDATA_AUX_CTL_REG |= (uint8)  bluetooth_COM_TX_FIFO_CLR;
        bluetooth_COM_TXDATA_AUX_CTL_REG &= (uint8) ~bluetooth_COM_TX_FIFO_CLR;
        CyExitCriticalSection(enableInterrupts);

    #if (bluetooth_COM_TX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        bluetooth_COM_DisableTxInt();

        bluetooth_COM_txBufferRead = 0u;
        bluetooth_COM_txBufferWrite = 0u;

        /* Enable Tx interrupt. */
        bluetooth_COM_EnableTxInt();

    #endif /* (bluetooth_COM_TX_INTERRUPT_ENABLED) */
    }


    /*******************************************************************************
    * Function Name: bluetooth_COM_SendBreak
    ********************************************************************************
    *
    * Summary:
    *  Transmits a break signal on the bus.
    *
    * Parameters:
    *  uint8 retMode:  Send Break return mode. See the following table for options.
    *   bluetooth_COM_SEND_BREAK - Initialize registers for break, send the Break
    *       signal and return immediately.
    *   bluetooth_COM_WAIT_FOR_COMPLETE_REINIT - Wait until break transmission is
    *       complete, reinitialize registers to normal transmission mode then return
    *   bluetooth_COM_REINIT - Reinitialize registers to normal transmission mode
    *       then return.
    *   bluetooth_COM_SEND_WAIT_REINIT - Performs both options: 
    *      bluetooth_COM_SEND_BREAK and bluetooth_COM_WAIT_FOR_COMPLETE_REINIT.
    *      This option is recommended for most cases.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  bluetooth_COM_initVar - checked to identify that the component has been
    *     initialized.
    *  txPeriod - static variable, used for keeping TX period configuration.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  SendBreak function initializes registers to send 13-bit break signal. It is
    *  important to return the registers configuration to normal for continue 8-bit
    *  operation.
    *  There are 3 variants for this API usage:
    *  1) SendBreak(3) - function will send the Break signal and take care on the
    *     configuration returning. Function will block CPU until transmission
    *     complete.
    *  2) User may want to use blocking time if UART configured to the low speed
    *     operation
    *     Example for this case:
    *     SendBreak(0);     - initialize Break signal transmission
    *         Add your code here to use CPU time
    *     SendBreak(1);     - complete Break operation
    *  3) Same to 2) but user may want to initialize and use the interrupt to
    *     complete break operation.
    *     Example for this case:
    *     Initialize TX interrupt with "TX - On TX Complete" parameter
    *     SendBreak(0);     - initialize Break signal transmission
    *         Add your code here to use CPU time
    *     When interrupt appear with bluetooth_COM_TX_STS_COMPLETE status:
    *     SendBreak(2);     - complete Break operation
    *
    * Side Effects:
    *  The bluetooth_COM_SendBreak() function initializes registers to send a
    *  break signal.
    *  Break signal length depends on the break signal bits configuration.
    *  The register configuration should be reinitialized before normal 8-bit
    *  communication can continue.
    *
    *******************************************************************************/
    void bluetooth_COM_SendBreak(uint8 retMode) 
    {

        /* If not Initialized then skip this function*/
        if(bluetooth_COM_initVar != 0u)
        {
            /* Set the Counter to 13-bits and transmit a 00 byte */
            /* When that is done then reset the counter value back */
            uint8 tmpStat;

        #if(bluetooth_COM_HD_ENABLED) /* Half Duplex mode*/

            if( (retMode == bluetooth_COM_SEND_BREAK) ||
                (retMode == bluetooth_COM_SEND_WAIT_REINIT ) )
            {
                /* CTRL_HD_SEND_BREAK - sends break bits in HD mode */
                bluetooth_COM_WriteControlRegister(bluetooth_COM_ReadControlRegister() |
                                                      bluetooth_COM_CTRL_HD_SEND_BREAK);
                /* Send zeros */
                bluetooth_COM_TXDATA_REG = 0u;

                do /* Wait until transmit starts */
                {
                    tmpStat = bluetooth_COM_TXSTATUS_REG;
                }
                while((tmpStat & bluetooth_COM_TX_STS_FIFO_EMPTY) != 0u);
            }

            if( (retMode == bluetooth_COM_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == bluetooth_COM_SEND_WAIT_REINIT) )
            {
                do /* Wait until transmit complete */
                {
                    tmpStat = bluetooth_COM_TXSTATUS_REG;
                }
                while(((uint8)~tmpStat & bluetooth_COM_TX_STS_COMPLETE) != 0u);
            }

            if( (retMode == bluetooth_COM_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == bluetooth_COM_REINIT) ||
                (retMode == bluetooth_COM_SEND_WAIT_REINIT) )
            {
                bluetooth_COM_WriteControlRegister(bluetooth_COM_ReadControlRegister() &
                                              (uint8)~bluetooth_COM_CTRL_HD_SEND_BREAK);
            }

        #else /* bluetooth_COM_HD_ENABLED Full Duplex mode */

            static uint8 txPeriod;

            if( (retMode == bluetooth_COM_SEND_BREAK) ||
                (retMode == bluetooth_COM_SEND_WAIT_REINIT) )
            {
                /* CTRL_HD_SEND_BREAK - skip to send parity bit at Break signal in Full Duplex mode */
                #if( (bluetooth_COM_PARITY_TYPE != bluetooth_COM__B_UART__NONE_REVB) || \
                                    (bluetooth_COM_PARITY_TYPE_SW != 0u) )
                    bluetooth_COM_WriteControlRegister(bluetooth_COM_ReadControlRegister() |
                                                          bluetooth_COM_CTRL_HD_SEND_BREAK);
                #endif /* End bluetooth_COM_PARITY_TYPE != bluetooth_COM__B_UART__NONE_REVB  */

                #if(bluetooth_COM_TXCLKGEN_DP)
                    txPeriod = bluetooth_COM_TXBITCLKTX_COMPLETE_REG;
                    bluetooth_COM_TXBITCLKTX_COMPLETE_REG = bluetooth_COM_TXBITCTR_BREAKBITS;
                #else
                    txPeriod = bluetooth_COM_TXBITCTR_PERIOD_REG;
                    bluetooth_COM_TXBITCTR_PERIOD_REG = bluetooth_COM_TXBITCTR_BREAKBITS8X;
                #endif /* End bluetooth_COM_TXCLKGEN_DP */

                /* Send zeros */
                bluetooth_COM_TXDATA_REG = 0u;

                do /* Wait until transmit starts */
                {
                    tmpStat = bluetooth_COM_TXSTATUS_REG;
                }
                while((tmpStat & bluetooth_COM_TX_STS_FIFO_EMPTY) != 0u);
            }

            if( (retMode == bluetooth_COM_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == bluetooth_COM_SEND_WAIT_REINIT) )
            {
                do /* Wait until transmit complete */
                {
                    tmpStat = bluetooth_COM_TXSTATUS_REG;
                }
                while(((uint8)~tmpStat & bluetooth_COM_TX_STS_COMPLETE) != 0u);
            }

            if( (retMode == bluetooth_COM_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == bluetooth_COM_REINIT) ||
                (retMode == bluetooth_COM_SEND_WAIT_REINIT) )
            {

            #if(bluetooth_COM_TXCLKGEN_DP)
                bluetooth_COM_TXBITCLKTX_COMPLETE_REG = txPeriod;
            #else
                bluetooth_COM_TXBITCTR_PERIOD_REG = txPeriod;
            #endif /* End bluetooth_COM_TXCLKGEN_DP */

            #if( (bluetooth_COM_PARITY_TYPE != bluetooth_COM__B_UART__NONE_REVB) || \
                 (bluetooth_COM_PARITY_TYPE_SW != 0u) )
                bluetooth_COM_WriteControlRegister(bluetooth_COM_ReadControlRegister() &
                                                      (uint8) ~bluetooth_COM_CTRL_HD_SEND_BREAK);
            #endif /* End bluetooth_COM_PARITY_TYPE != NONE */
            }
        #endif    /* End bluetooth_COM_HD_ENABLED */
        }
    }


    /*******************************************************************************
    * Function Name: bluetooth_COM_SetTxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the transmitter to signal the next bytes is address or data.
    *
    * Parameters:
    *  addressMode: 
    *       bluetooth_COM_SET_SPACE - Configure the transmitter to send the next
    *                                    byte as a data.
    *       bluetooth_COM_SET_MARK  - Configure the transmitter to send the next
    *                                    byte as an address.
    *
    * Return:
    *  None.
    *
    * Side Effects:
    *  This function sets and clears bluetooth_COM_CTRL_MARK bit in the Control
    *  register.
    *
    *******************************************************************************/
    void bluetooth_COM_SetTxAddressMode(uint8 addressMode) 
    {
        /* Mark/Space sending enable */
        if(addressMode != 0u)
        {
        #if( bluetooth_COM_CONTROL_REG_REMOVED == 0u )
            bluetooth_COM_WriteControlRegister(bluetooth_COM_ReadControlRegister() |
                                                  bluetooth_COM_CTRL_MARK);
        #endif /* End bluetooth_COM_CONTROL_REG_REMOVED == 0u */
        }
        else
        {
        #if( bluetooth_COM_CONTROL_REG_REMOVED == 0u )
            bluetooth_COM_WriteControlRegister(bluetooth_COM_ReadControlRegister() &
                                                  (uint8) ~bluetooth_COM_CTRL_MARK);
        #endif /* End bluetooth_COM_CONTROL_REG_REMOVED == 0u */
        }
    }

#endif  /* Endbluetooth_COM_TX_ENABLED */

#if(bluetooth_COM_HD_ENABLED)


    /*******************************************************************************
    * Function Name: bluetooth_COM_LoadRxConfig
    ********************************************************************************
    *
    * Summary:
    *  Loads the receiver configuration in half duplex mode. After calling this
    *  function, the UART is ready to receive data.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Side Effects:
    *  Valid only in half duplex mode. You must make sure that the previous
    *  transaction is complete and it is safe to unload the transmitter
    *  configuration.
    *
    *******************************************************************************/
    void bluetooth_COM_LoadRxConfig(void) 
    {
        bluetooth_COM_WriteControlRegister(bluetooth_COM_ReadControlRegister() &
                                                (uint8)~bluetooth_COM_CTRL_HD_SEND);
        bluetooth_COM_RXBITCTR_PERIOD_REG = bluetooth_COM_HD_RXBITCTR_INIT;

    #if (bluetooth_COM_RX_INTERRUPT_ENABLED)
        /* Enable RX interrupt after set RX configuration */
        bluetooth_COM_SetRxInterruptMode(bluetooth_COM_INIT_RX_INTERRUPTS_MASK);
    #endif /* (bluetooth_COM_RX_INTERRUPT_ENABLED) */
    }


    /*******************************************************************************
    * Function Name: bluetooth_COM_LoadTxConfig
    ********************************************************************************
    *
    * Summary:
    *  Loads the transmitter configuration in half duplex mode. After calling this
    *  function, the UART is ready to transmit data.
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Side Effects:
    *  Valid only in half duplex mode. You must make sure that the previous
    *  transaction is complete and it is safe to unload the receiver configuration.
    *
    *******************************************************************************/
    void bluetooth_COM_LoadTxConfig(void) 
    {
    #if (bluetooth_COM_RX_INTERRUPT_ENABLED)
        /* Disable RX interrupts before set TX configuration */
        bluetooth_COM_SetRxInterruptMode(0u);
    #endif /* (bluetooth_COM_RX_INTERRUPT_ENABLED) */

        bluetooth_COM_WriteControlRegister(bluetooth_COM_ReadControlRegister() | bluetooth_COM_CTRL_HD_SEND);
        bluetooth_COM_RXBITCTR_PERIOD_REG = bluetooth_COM_HD_TXBITCTR_INIT;
    }

#endif  /* bluetooth_COM_HD_ENABLED */


/* [] END OF FILE */
