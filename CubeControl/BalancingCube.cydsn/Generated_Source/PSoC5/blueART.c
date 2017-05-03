/*******************************************************************************
* File Name: blueART.c
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

#include "blueART.h"
#if (blueART_INTERNAL_CLOCK_USED)
    #include "blueART_IntClock.h"
#endif /* End blueART_INTERNAL_CLOCK_USED */


/***************************************
* Global data allocation
***************************************/

uint8 blueART_initVar = 0u;

#if (blueART_TX_INTERRUPT_ENABLED && blueART_TX_ENABLED)
    volatile uint8 blueART_txBuffer[blueART_TX_BUFFER_SIZE];
    volatile uint8 blueART_txBufferRead = 0u;
    uint8 blueART_txBufferWrite = 0u;
#endif /* (blueART_TX_INTERRUPT_ENABLED && blueART_TX_ENABLED) */

#if (blueART_RX_INTERRUPT_ENABLED && (blueART_RX_ENABLED || blueART_HD_ENABLED))
    uint8 blueART_errorStatus = 0u;
    volatile uint8 blueART_rxBuffer[blueART_RX_BUFFER_SIZE];
    volatile uint8 blueART_rxBufferRead  = 0u;
    volatile uint8 blueART_rxBufferWrite = 0u;
    volatile uint8 blueART_rxBufferLoopDetect = 0u;
    volatile uint8 blueART_rxBufferOverflow   = 0u;
    #if (blueART_RXHW_ADDRESS_ENABLED)
        volatile uint8 blueART_rxAddressMode = blueART_RX_ADDRESS_MODE;
        volatile uint8 blueART_rxAddressDetected = 0u;
    #endif /* (blueART_RXHW_ADDRESS_ENABLED) */
#endif /* (blueART_RX_INTERRUPT_ENABLED && (blueART_RX_ENABLED || blueART_HD_ENABLED)) */


/*******************************************************************************
* Function Name: blueART_Start
********************************************************************************
*
* Summary:
*  This is the preferred method to begin component operation.
*  blueART_Start() sets the initVar variable, calls the
*  blueART_Init() function, and then calls the
*  blueART_Enable() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  The blueART_intiVar variable is used to indicate initial
*  configuration of this component. The variable is initialized to zero (0u)
*  and set to one (1u) the first time blueART_Start() is called. This
*  allows for component initialization without re-initialization in all
*  subsequent calls to the blueART_Start() routine.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void blueART_Start(void) 
{
    /* If not initialized then initialize all required hardware and software */
    if(blueART_initVar == 0u)
    {
        blueART_Init();
        blueART_initVar = 1u;
    }

    blueART_Enable();
}


/*******************************************************************************
* Function Name: blueART_Init
********************************************************************************
*
* Summary:
*  Initializes or restores the component according to the customizer Configure
*  dialog settings. It is not necessary to call blueART_Init() because
*  the blueART_Start() API calls this function and is the preferred
*  method to begin component operation.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void blueART_Init(void) 
{
    #if(blueART_RX_ENABLED || blueART_HD_ENABLED)

        #if (blueART_RX_INTERRUPT_ENABLED)
            /* Set RX interrupt vector and priority */
            (void) CyIntSetVector(blueART_RX_VECT_NUM, &blueART_RXISR);
            CyIntSetPriority(blueART_RX_VECT_NUM, blueART_RX_PRIOR_NUM);
            blueART_errorStatus = 0u;
        #endif /* (blueART_RX_INTERRUPT_ENABLED) */

        #if (blueART_RXHW_ADDRESS_ENABLED)
            blueART_SetRxAddressMode(blueART_RX_ADDRESS_MODE);
            blueART_SetRxAddress1(blueART_RX_HW_ADDRESS1);
            blueART_SetRxAddress2(blueART_RX_HW_ADDRESS2);
        #endif /* End blueART_RXHW_ADDRESS_ENABLED */

        /* Init Count7 period */
        blueART_RXBITCTR_PERIOD_REG = blueART_RXBITCTR_INIT;
        /* Configure the Initial RX interrupt mask */
        blueART_RXSTATUS_MASK_REG  = blueART_INIT_RX_INTERRUPTS_MASK;
    #endif /* End blueART_RX_ENABLED || blueART_HD_ENABLED*/

    #if(blueART_TX_ENABLED)
        #if (blueART_TX_INTERRUPT_ENABLED)
            /* Set TX interrupt vector and priority */
            (void) CyIntSetVector(blueART_TX_VECT_NUM, &blueART_TXISR);
            CyIntSetPriority(blueART_TX_VECT_NUM, blueART_TX_PRIOR_NUM);
        #endif /* (blueART_TX_INTERRUPT_ENABLED) */

        /* Write Counter Value for TX Bit Clk Generator*/
        #if (blueART_TXCLKGEN_DP)
            blueART_TXBITCLKGEN_CTR_REG = blueART_BIT_CENTER;
            blueART_TXBITCLKTX_COMPLETE_REG = ((blueART_NUMBER_OF_DATA_BITS +
                        blueART_NUMBER_OF_START_BIT) * blueART_OVER_SAMPLE_COUNT) - 1u;
        #else
            blueART_TXBITCTR_PERIOD_REG = ((blueART_NUMBER_OF_DATA_BITS +
                        blueART_NUMBER_OF_START_BIT) * blueART_OVER_SAMPLE_8) - 1u;
        #endif /* End blueART_TXCLKGEN_DP */

        /* Configure the Initial TX interrupt mask */
        #if (blueART_TX_INTERRUPT_ENABLED)
            blueART_TXSTATUS_MASK_REG = blueART_TX_STS_FIFO_EMPTY;
        #else
            blueART_TXSTATUS_MASK_REG = blueART_INIT_TX_INTERRUPTS_MASK;
        #endif /*End blueART_TX_INTERRUPT_ENABLED*/

    #endif /* End blueART_TX_ENABLED */

    #if(blueART_PARITY_TYPE_SW)  /* Write Parity to Control Register */
        blueART_WriteControlRegister( \
            (blueART_ReadControlRegister() & (uint8)~blueART_CTRL_PARITY_TYPE_MASK) | \
            (uint8)(blueART_PARITY_TYPE << blueART_CTRL_PARITY_TYPE0_SHIFT) );
    #endif /* End blueART_PARITY_TYPE_SW */
}


/*******************************************************************************
* Function Name: blueART_Enable
********************************************************************************
*
* Summary:
*  Activates the hardware and begins component operation. It is not necessary
*  to call blueART_Enable() because the blueART_Start() API
*  calls this function, which is the preferred method to begin component
*  operation.

* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  blueART_rxAddressDetected - set to initial state (0).
*
*******************************************************************************/
void blueART_Enable(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    #if (blueART_RX_ENABLED || blueART_HD_ENABLED)
        /* RX Counter (Count7) Enable */
        blueART_RXBITCTR_CONTROL_REG |= blueART_CNTR_ENABLE;

        /* Enable the RX Interrupt */
        blueART_RXSTATUS_ACTL_REG  |= blueART_INT_ENABLE;

        #if (blueART_RX_INTERRUPT_ENABLED)
            blueART_EnableRxInt();

            #if (blueART_RXHW_ADDRESS_ENABLED)
                blueART_rxAddressDetected = 0u;
            #endif /* (blueART_RXHW_ADDRESS_ENABLED) */
        #endif /* (blueART_RX_INTERRUPT_ENABLED) */
    #endif /* (blueART_RX_ENABLED || blueART_HD_ENABLED) */

    #if(blueART_TX_ENABLED)
        /* TX Counter (DP/Count7) Enable */
        #if(!blueART_TXCLKGEN_DP)
            blueART_TXBITCTR_CONTROL_REG |= blueART_CNTR_ENABLE;
        #endif /* End blueART_TXCLKGEN_DP */

        /* Enable the TX Interrupt */
        blueART_TXSTATUS_ACTL_REG |= blueART_INT_ENABLE;
        #if (blueART_TX_INTERRUPT_ENABLED)
            blueART_ClearPendingTxInt(); /* Clear history of TX_NOT_EMPTY */
            blueART_EnableTxInt();
        #endif /* (blueART_TX_INTERRUPT_ENABLED) */
     #endif /* (blueART_TX_INTERRUPT_ENABLED) */

    #if (blueART_INTERNAL_CLOCK_USED)
        blueART_IntClock_Start();  /* Enable the clock */
    #endif /* (blueART_INTERNAL_CLOCK_USED) */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: blueART_Stop
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
void blueART_Stop(void) 
{
    uint8 enableInterrupts;
    enableInterrupts = CyEnterCriticalSection();

    /* Write Bit Counter Disable */
    #if (blueART_RX_ENABLED || blueART_HD_ENABLED)
        blueART_RXBITCTR_CONTROL_REG &= (uint8) ~blueART_CNTR_ENABLE;
    #endif /* (blueART_RX_ENABLED || blueART_HD_ENABLED) */

    #if (blueART_TX_ENABLED)
        #if(!blueART_TXCLKGEN_DP)
            blueART_TXBITCTR_CONTROL_REG &= (uint8) ~blueART_CNTR_ENABLE;
        #endif /* (!blueART_TXCLKGEN_DP) */
    #endif /* (blueART_TX_ENABLED) */

    #if (blueART_INTERNAL_CLOCK_USED)
        blueART_IntClock_Stop();   /* Disable the clock */
    #endif /* (blueART_INTERNAL_CLOCK_USED) */

    /* Disable internal interrupt component */
    #if (blueART_RX_ENABLED || blueART_HD_ENABLED)
        blueART_RXSTATUS_ACTL_REG  &= (uint8) ~blueART_INT_ENABLE;

        #if (blueART_RX_INTERRUPT_ENABLED)
            blueART_DisableRxInt();
        #endif /* (blueART_RX_INTERRUPT_ENABLED) */
    #endif /* (blueART_RX_ENABLED || blueART_HD_ENABLED) */

    #if (blueART_TX_ENABLED)
        blueART_TXSTATUS_ACTL_REG &= (uint8) ~blueART_INT_ENABLE;

        #if (blueART_TX_INTERRUPT_ENABLED)
            blueART_DisableTxInt();
        #endif /* (blueART_TX_INTERRUPT_ENABLED) */
    #endif /* (blueART_TX_ENABLED) */

    CyExitCriticalSection(enableInterrupts);
}


/*******************************************************************************
* Function Name: blueART_ReadControlRegister
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
uint8 blueART_ReadControlRegister(void) 
{
    #if (blueART_CONTROL_REG_REMOVED)
        return(0u);
    #else
        return(blueART_CONTROL_REG);
    #endif /* (blueART_CONTROL_REG_REMOVED) */
}


/*******************************************************************************
* Function Name: blueART_WriteControlRegister
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
void  blueART_WriteControlRegister(uint8 control) 
{
    #if (blueART_CONTROL_REG_REMOVED)
        if(0u != control)
        {
            /* Suppress compiler warning */
        }
    #else
       blueART_CONTROL_REG = control;
    #endif /* (blueART_CONTROL_REG_REMOVED) */
}


#if(blueART_RX_ENABLED || blueART_HD_ENABLED)
    /*******************************************************************************
    * Function Name: blueART_SetRxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the RX interrupt sources enabled.
    *
    * Parameters:
    *  IntSrc:  Bit field containing the RX interrupts to enable. Based on the 
    *  bit-field arrangement of the status register. This value must be a 
    *  combination of status register bit-masks shown below:
    *      blueART_RX_STS_FIFO_NOTEMPTY    Interrupt on byte received.
    *      blueART_RX_STS_PAR_ERROR        Interrupt on parity error.
    *      blueART_RX_STS_STOP_ERROR       Interrupt on stop error.
    *      blueART_RX_STS_BREAK            Interrupt on break.
    *      blueART_RX_STS_OVERRUN          Interrupt on overrun error.
    *      blueART_RX_STS_ADDR_MATCH       Interrupt on address match.
    *      blueART_RX_STS_MRKSPC           Interrupt on address detect.
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void blueART_SetRxInterruptMode(uint8 intSrc) 
    {
        blueART_RXSTATUS_MASK_REG  = intSrc;
    }


    /*******************************************************************************
    * Function Name: blueART_ReadRxData
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
    *  blueART_rxBuffer - RAM buffer pointer for save received data.
    *  blueART_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  blueART_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  blueART_rxBufferLoopDetect - cleared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 blueART_ReadRxData(void) 
    {
        uint8 rxData;

    #if (blueART_RX_INTERRUPT_ENABLED)

        uint8 locRxBufferRead;
        uint8 locRxBufferWrite;

        /* Protect variables that could change on interrupt */
        blueART_DisableRxInt();

        locRxBufferRead  = blueART_rxBufferRead;
        locRxBufferWrite = blueART_rxBufferWrite;

        if( (blueART_rxBufferLoopDetect != 0u) || (locRxBufferRead != locRxBufferWrite) )
        {
            rxData = blueART_rxBuffer[locRxBufferRead];
            locRxBufferRead++;

            if(locRxBufferRead >= blueART_RX_BUFFER_SIZE)
            {
                locRxBufferRead = 0u;
            }
            /* Update the real pointer */
            blueART_rxBufferRead = locRxBufferRead;

            if(blueART_rxBufferLoopDetect != 0u)
            {
                blueART_rxBufferLoopDetect = 0u;
                #if ((blueART_RX_INTERRUPT_ENABLED) && (blueART_FLOW_CONTROL != 0u))
                    /* When Hardware Flow Control selected - return RX mask */
                    #if( blueART_HD_ENABLED )
                        if((blueART_CONTROL_REG & blueART_CTRL_HD_SEND) == 0u)
                        {   /* In Half duplex mode return RX mask only in RX
                            *  configuration set, otherwise
                            *  mask will be returned in LoadRxConfig() API.
                            */
                            blueART_RXSTATUS_MASK_REG  |= blueART_RX_STS_FIFO_NOTEMPTY;
                        }
                    #else
                        blueART_RXSTATUS_MASK_REG  |= blueART_RX_STS_FIFO_NOTEMPTY;
                    #endif /* end blueART_HD_ENABLED */
                #endif /* ((blueART_RX_INTERRUPT_ENABLED) && (blueART_FLOW_CONTROL != 0u)) */
            }
        }
        else
        {   /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit */
            rxData = blueART_RXDATA_REG;
        }

        blueART_EnableRxInt();

    #else

        /* Needs to check status for RX_STS_FIFO_NOTEMPTY bit */
        rxData = blueART_RXDATA_REG;

    #endif /* (blueART_RX_INTERRUPT_ENABLED) */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: blueART_ReadRxStatus
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
    *  blueART_RX_STS_FIFO_NOTEMPTY.
    *  blueART_RX_STS_FIFO_NOTEMPTY clears immediately after RX data
    *  register read.
    *
    * Global Variables:
    *  blueART_rxBufferOverflow - used to indicate overload condition.
    *   It set to one in RX interrupt when there isn't free space in
    *   blueART_rxBufferRead to write new data. This condition returned
    *   and cleared to zero by this API as an
    *   blueART_RX_STS_SOFT_BUFF_OVER bit along with RX Status register
    *   bits.
    *
    *******************************************************************************/
    uint8 blueART_ReadRxStatus(void) 
    {
        uint8 status;

        status = blueART_RXSTATUS_REG & blueART_RX_HW_MASK;

    #if (blueART_RX_INTERRUPT_ENABLED)
        if(blueART_rxBufferOverflow != 0u)
        {
            status |= blueART_RX_STS_SOFT_BUFF_OVER;
            blueART_rxBufferOverflow = 0u;
        }
    #endif /* (blueART_RX_INTERRUPT_ENABLED) */

        return(status);
    }


    /*******************************************************************************
    * Function Name: blueART_GetChar
    ********************************************************************************
    *
    * Summary:
    *  Returns the last received byte of data. blueART_GetChar() is
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
    *  blueART_rxBuffer - RAM buffer pointer for save received data.
    *  blueART_rxBufferWrite - cyclic index for write to rxBuffer,
    *     checked to identify new data.
    *  blueART_rxBufferRead - cyclic index for read from rxBuffer,
    *     incremented after each byte has been read from buffer.
    *  blueART_rxBufferLoopDetect - cleared if loop condition was detected
    *     in RX ISR.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    uint8 blueART_GetChar(void) 
    {
        uint8 rxData = 0u;
        uint8 rxStatus;

    #if (blueART_RX_INTERRUPT_ENABLED)
        uint8 locRxBufferRead;
        uint8 locRxBufferWrite;

        /* Protect variables that could change on interrupt */
        blueART_DisableRxInt();

        locRxBufferRead  = blueART_rxBufferRead;
        locRxBufferWrite = blueART_rxBufferWrite;

        if( (blueART_rxBufferLoopDetect != 0u) || (locRxBufferRead != locRxBufferWrite) )
        {
            rxData = blueART_rxBuffer[locRxBufferRead];
            locRxBufferRead++;
            if(locRxBufferRead >= blueART_RX_BUFFER_SIZE)
            {
                locRxBufferRead = 0u;
            }
            /* Update the real pointer */
            blueART_rxBufferRead = locRxBufferRead;

            if(blueART_rxBufferLoopDetect != 0u)
            {
                blueART_rxBufferLoopDetect = 0u;
                #if( (blueART_RX_INTERRUPT_ENABLED) && (blueART_FLOW_CONTROL != 0u) )
                    /* When Hardware Flow Control selected - return RX mask */
                    #if( blueART_HD_ENABLED )
                        if((blueART_CONTROL_REG & blueART_CTRL_HD_SEND) == 0u)
                        {   /* In Half duplex mode return RX mask only if
                            *  RX configuration set, otherwise
                            *  mask will be returned in LoadRxConfig() API.
                            */
                            blueART_RXSTATUS_MASK_REG |= blueART_RX_STS_FIFO_NOTEMPTY;
                        }
                    #else
                        blueART_RXSTATUS_MASK_REG |= blueART_RX_STS_FIFO_NOTEMPTY;
                    #endif /* end blueART_HD_ENABLED */
                #endif /* blueART_RX_INTERRUPT_ENABLED and Hardware flow control*/
            }

        }
        else
        {   rxStatus = blueART_RXSTATUS_REG;
            if((rxStatus & blueART_RX_STS_FIFO_NOTEMPTY) != 0u)
            {   /* Read received data from FIFO */
                rxData = blueART_RXDATA_REG;
                /*Check status on error*/
                if((rxStatus & (blueART_RX_STS_BREAK | blueART_RX_STS_PAR_ERROR |
                                blueART_RX_STS_STOP_ERROR | blueART_RX_STS_OVERRUN)) != 0u)
                {
                    rxData = 0u;
                }
            }
        }

        blueART_EnableRxInt();

    #else

        rxStatus =blueART_RXSTATUS_REG;
        if((rxStatus & blueART_RX_STS_FIFO_NOTEMPTY) != 0u)
        {
            /* Read received data from FIFO */
            rxData = blueART_RXDATA_REG;

            /*Check status on error*/
            if((rxStatus & (blueART_RX_STS_BREAK | blueART_RX_STS_PAR_ERROR |
                            blueART_RX_STS_STOP_ERROR | blueART_RX_STS_OVERRUN)) != 0u)
            {
                rxData = 0u;
            }
        }
    #endif /* (blueART_RX_INTERRUPT_ENABLED) */

        return(rxData);
    }


    /*******************************************************************************
    * Function Name: blueART_GetByte
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
    uint16 blueART_GetByte(void) 
    {
        
    #if (blueART_RX_INTERRUPT_ENABLED)
        uint16 locErrorStatus;
        /* Protect variables that could change on interrupt */
        blueART_DisableRxInt();
        locErrorStatus = (uint16)blueART_errorStatus;
        blueART_errorStatus = 0u;
        blueART_EnableRxInt();
        return ( (uint16)(locErrorStatus << 8u) | blueART_ReadRxData() );
    #else
        return ( ((uint16)blueART_ReadRxStatus() << 8u) | blueART_ReadRxData() );
    #endif /* blueART_RX_INTERRUPT_ENABLED */
        
    }


    /*******************************************************************************
    * Function Name: blueART_GetRxBufferSize
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
    *  blueART_rxBufferWrite - used to calculate left bytes.
    *  blueART_rxBufferRead - used to calculate left bytes.
    *  blueART_rxBufferLoopDetect - checked to decide left bytes amount.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the RX Buffer is.
    *
    *******************************************************************************/
    uint8 blueART_GetRxBufferSize(void)
                                                            
    {
        uint8 size;

    #if (blueART_RX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt */
        blueART_DisableRxInt();

        if(blueART_rxBufferRead == blueART_rxBufferWrite)
        {
            if(blueART_rxBufferLoopDetect != 0u)
            {
                size = blueART_RX_BUFFER_SIZE;
            }
            else
            {
                size = 0u;
            }
        }
        else if(blueART_rxBufferRead < blueART_rxBufferWrite)
        {
            size = (blueART_rxBufferWrite - blueART_rxBufferRead);
        }
        else
        {
            size = (blueART_RX_BUFFER_SIZE - blueART_rxBufferRead) + blueART_rxBufferWrite;
        }

        blueART_EnableRxInt();

    #else

        /* We can only know if there is data in the fifo. */
        size = ((blueART_RXSTATUS_REG & blueART_RX_STS_FIFO_NOTEMPTY) != 0u) ? 1u : 0u;

    #endif /* (blueART_RX_INTERRUPT_ENABLED) */

        return(size);
    }


    /*******************************************************************************
    * Function Name: blueART_ClearRxBuffer
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
    *  blueART_rxBufferWrite - cleared to zero.
    *  blueART_rxBufferRead - cleared to zero.
    *  blueART_rxBufferLoopDetect - cleared to zero.
    *  blueART_rxBufferOverflow - cleared to zero.
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
    void blueART_ClearRxBuffer(void) 
    {
        uint8 enableInterrupts;

        /* Clear the HW FIFO */
        enableInterrupts = CyEnterCriticalSection();
        blueART_RXDATA_AUX_CTL_REG |= (uint8)  blueART_RX_FIFO_CLR;
        blueART_RXDATA_AUX_CTL_REG &= (uint8) ~blueART_RX_FIFO_CLR;
        CyExitCriticalSection(enableInterrupts);

    #if (blueART_RX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        blueART_DisableRxInt();

        blueART_rxBufferRead = 0u;
        blueART_rxBufferWrite = 0u;
        blueART_rxBufferLoopDetect = 0u;
        blueART_rxBufferOverflow = 0u;

        blueART_EnableRxInt();

    #endif /* (blueART_RX_INTERRUPT_ENABLED) */

    }


    /*******************************************************************************
    * Function Name: blueART_SetRxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Sets the software controlled Addressing mode used by the RX portion of the
    *  UART.
    *
    * Parameters:
    *  addressMode: Enumerated value indicating the mode of RX addressing
    *  blueART__B_UART__AM_SW_BYTE_BYTE -  Software Byte-by-Byte address
    *                                               detection
    *  blueART__B_UART__AM_SW_DETECT_TO_BUFFER - Software Detect to Buffer
    *                                               address detection
    *  blueART__B_UART__AM_HW_BYTE_BY_BYTE - Hardware Byte-by-Byte address
    *                                               detection
    *  blueART__B_UART__AM_HW_DETECT_TO_BUFFER - Hardware Detect to Buffer
    *                                               address detection
    *  blueART__B_UART__AM_NONE - No address detection
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  blueART_rxAddressMode - the parameter stored in this variable for
    *   the farther usage in RX ISR.
    *  blueART_rxAddressDetected - set to initial state (0).
    *
    *******************************************************************************/
    void blueART_SetRxAddressMode(uint8 addressMode)
                                                        
    {
        #if(blueART_RXHW_ADDRESS_ENABLED)
            #if(blueART_CONTROL_REG_REMOVED)
                if(0u != addressMode)
                {
                    /* Suppress compiler warning */
                }
            #else /* blueART_CONTROL_REG_REMOVED */
                uint8 tmpCtrl;
                tmpCtrl = blueART_CONTROL_REG & (uint8)~blueART_CTRL_RXADDR_MODE_MASK;
                tmpCtrl |= (uint8)(addressMode << blueART_CTRL_RXADDR_MODE0_SHIFT);
                blueART_CONTROL_REG = tmpCtrl;

                #if(blueART_RX_INTERRUPT_ENABLED && \
                   (blueART_RXBUFFERSIZE > blueART_FIFO_LENGTH) )
                    blueART_rxAddressMode = addressMode;
                    blueART_rxAddressDetected = 0u;
                #endif /* End blueART_RXBUFFERSIZE > blueART_FIFO_LENGTH*/
            #endif /* End blueART_CONTROL_REG_REMOVED */
        #else /* blueART_RXHW_ADDRESS_ENABLED */
            if(0u != addressMode)
            {
                /* Suppress compiler warning */
            }
        #endif /* End blueART_RXHW_ADDRESS_ENABLED */
    }


    /*******************************************************************************
    * Function Name: blueART_SetRxAddress1
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
    void blueART_SetRxAddress1(uint8 address) 
    {
        blueART_RXADDRESS1_REG = address;
    }


    /*******************************************************************************
    * Function Name: blueART_SetRxAddress2
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
    void blueART_SetRxAddress2(uint8 address) 
    {
        blueART_RXADDRESS2_REG = address;
    }

#endif  /* blueART_RX_ENABLED || blueART_HD_ENABLED*/


#if( (blueART_TX_ENABLED) || (blueART_HD_ENABLED) )
    /*******************************************************************************
    * Function Name: blueART_SetTxInterruptMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the TX interrupt sources to be enabled, but does not enable the
    *  interrupt.
    *
    * Parameters:
    *  intSrc: Bit field containing the TX interrupt sources to enable
    *   blueART_TX_STS_COMPLETE        Interrupt on TX byte complete
    *   blueART_TX_STS_FIFO_EMPTY      Interrupt when TX FIFO is empty
    *   blueART_TX_STS_FIFO_FULL       Interrupt when TX FIFO is full
    *   blueART_TX_STS_FIFO_NOT_FULL   Interrupt when TX FIFO is not full
    *
    * Return:
    *  None.
    *
    * Theory:
    *  Enables the output of specific status bits to the interrupt controller
    *
    *******************************************************************************/
    void blueART_SetTxInterruptMode(uint8 intSrc) 
    {
        blueART_TXSTATUS_MASK_REG = intSrc;
    }


    /*******************************************************************************
    * Function Name: blueART_WriteTxData
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
    *  blueART_txBuffer - RAM buffer pointer for save data for transmission
    *  blueART_txBufferWrite - cyclic index for write to txBuffer,
    *    incremented after each byte saved to buffer.
    *  blueART_txBufferRead - cyclic index for read from txBuffer,
    *    checked to identify the condition to write to FIFO directly or to TX buffer
    *  blueART_initVar - checked to identify that the component has been
    *    initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void blueART_WriteTxData(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function*/
        if(blueART_initVar != 0u)
        {
        #if (blueART_TX_INTERRUPT_ENABLED)

            /* Protect variables that could change on interrupt. */
            blueART_DisableTxInt();

            if( (blueART_txBufferRead == blueART_txBufferWrite) &&
                ((blueART_TXSTATUS_REG & blueART_TX_STS_FIFO_FULL) == 0u) )
            {
                /* Add directly to the FIFO. */
                blueART_TXDATA_REG = txDataByte;
            }
            else
            {
                if(blueART_txBufferWrite >= blueART_TX_BUFFER_SIZE)
                {
                    blueART_txBufferWrite = 0u;
                }

                blueART_txBuffer[blueART_txBufferWrite] = txDataByte;

                /* Add to the software buffer. */
                blueART_txBufferWrite++;
            }

            blueART_EnableTxInt();

        #else

            /* Add directly to the FIFO. */
            blueART_TXDATA_REG = txDataByte;

        #endif /*(blueART_TX_INTERRUPT_ENABLED) */
        }
    }


    /*******************************************************************************
    * Function Name: blueART_ReadTxStatus
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
    uint8 blueART_ReadTxStatus(void) 
    {
        return(blueART_TXSTATUS_REG);
    }


    /*******************************************************************************
    * Function Name: blueART_PutChar
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
    *  blueART_txBuffer - RAM buffer pointer for save data for transmission
    *  blueART_txBufferWrite - cyclic index for write to txBuffer,
    *     checked to identify free space in txBuffer and incremented after each byte
    *     saved to buffer.
    *  blueART_txBufferRead - cyclic index for read from txBuffer,
    *     checked to identify free space in txBuffer.
    *  blueART_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to transmit any byte of data in a single transfer
    *
    *******************************************************************************/
    void blueART_PutChar(uint8 txDataByte) 
    {
    #if (blueART_TX_INTERRUPT_ENABLED)
        /* The temporary output pointer is used since it takes two instructions
        *  to increment with a wrap, and we can't risk doing that with the real
        *  pointer and getting an interrupt in between instructions.
        */
        uint8 locTxBufferWrite;
        uint8 locTxBufferRead;

        do
        { /* Block if software buffer is full, so we don't overwrite. */

        #if ((blueART_TX_BUFFER_SIZE > blueART_MAX_BYTE_VALUE) && (CY_PSOC3))
            /* Disable TX interrupt to protect variables from modification */
            blueART_DisableTxInt();
        #endif /* (blueART_TX_BUFFER_SIZE > blueART_MAX_BYTE_VALUE) && (CY_PSOC3) */

            locTxBufferWrite = blueART_txBufferWrite;
            locTxBufferRead  = blueART_txBufferRead;

        #if ((blueART_TX_BUFFER_SIZE > blueART_MAX_BYTE_VALUE) && (CY_PSOC3))
            /* Enable interrupt to continue transmission */
            blueART_EnableTxInt();
        #endif /* (blueART_TX_BUFFER_SIZE > blueART_MAX_BYTE_VALUE) && (CY_PSOC3) */
        }
        while( (locTxBufferWrite < locTxBufferRead) ? (locTxBufferWrite == (locTxBufferRead - 1u)) :
                                ((locTxBufferWrite - locTxBufferRead) ==
                                (uint8)(blueART_TX_BUFFER_SIZE - 1u)) );

        if( (locTxBufferRead == locTxBufferWrite) &&
            ((blueART_TXSTATUS_REG & blueART_TX_STS_FIFO_FULL) == 0u) )
        {
            /* Add directly to the FIFO */
            blueART_TXDATA_REG = txDataByte;
        }
        else
        {
            if(locTxBufferWrite >= blueART_TX_BUFFER_SIZE)
            {
                locTxBufferWrite = 0u;
            }
            /* Add to the software buffer. */
            blueART_txBuffer[locTxBufferWrite] = txDataByte;
            locTxBufferWrite++;

            /* Finally, update the real output pointer */
        #if ((blueART_TX_BUFFER_SIZE > blueART_MAX_BYTE_VALUE) && (CY_PSOC3))
            blueART_DisableTxInt();
        #endif /* (blueART_TX_BUFFER_SIZE > blueART_MAX_BYTE_VALUE) && (CY_PSOC3) */

            blueART_txBufferWrite = locTxBufferWrite;

        #if ((blueART_TX_BUFFER_SIZE > blueART_MAX_BYTE_VALUE) && (CY_PSOC3))
            blueART_EnableTxInt();
        #endif /* (blueART_TX_BUFFER_SIZE > blueART_MAX_BYTE_VALUE) && (CY_PSOC3) */

            if(0u != (blueART_TXSTATUS_REG & blueART_TX_STS_FIFO_EMPTY))
            {
                /* Trigger TX interrupt to send software buffer */
                blueART_SetPendingTxInt();
            }
        }

    #else

        while((blueART_TXSTATUS_REG & blueART_TX_STS_FIFO_FULL) != 0u)
        {
            /* Wait for room in the FIFO */
        }

        /* Add directly to the FIFO */
        blueART_TXDATA_REG = txDataByte;

    #endif /* blueART_TX_INTERRUPT_ENABLED */
    }


    /*******************************************************************************
    * Function Name: blueART_PutString
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
    *  blueART_initVar - checked to identify that the component has been
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
    void blueART_PutString(const char8 string[]) 
    {
        uint16 bufIndex = 0u;

        /* If not Initialized then skip this function */
        if(blueART_initVar != 0u)
        {
            /* This is a blocking function, it will not exit until all data is sent */
            while(string[bufIndex] != (char8) 0)
            {
                blueART_PutChar((uint8)string[bufIndex]);
                bufIndex++;
            }
        }
    }


    /*******************************************************************************
    * Function Name: blueART_PutArray
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
    *  blueART_initVar - checked to identify that the component has been
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
    void blueART_PutArray(const uint8 string[], uint8 byteCount)
                                                                    
    {
        uint8 bufIndex = 0u;

        /* If not Initialized then skip this function */
        if(blueART_initVar != 0u)
        {
            while(bufIndex < byteCount)
            {
                blueART_PutChar(string[bufIndex]);
                bufIndex++;
            }
        }
    }


    /*******************************************************************************
    * Function Name: blueART_PutCRLF
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
    *  blueART_initVar - checked to identify that the component has been
    *     initialized.
    *
    * Reentrant:
    *  No.
    *
    *******************************************************************************/
    void blueART_PutCRLF(uint8 txDataByte) 
    {
        /* If not Initialized then skip this function */
        if(blueART_initVar != 0u)
        {
            blueART_PutChar(txDataByte);
            blueART_PutChar(0x0Du);
            blueART_PutChar(0x0Au);
        }
    }


    /*******************************************************************************
    * Function Name: blueART_GetTxBufferSize
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
    *  blueART_txBufferWrite - used to calculate left space.
    *  blueART_txBufferRead - used to calculate left space.
    *
    * Reentrant:
    *  No.
    *
    * Theory:
    *  Allows the user to find out how full the TX Buffer is.
    *
    *******************************************************************************/
    uint8 blueART_GetTxBufferSize(void)
                                                            
    {
        uint8 size;

    #if (blueART_TX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        blueART_DisableTxInt();

        if(blueART_txBufferRead == blueART_txBufferWrite)
        {
            size = 0u;
        }
        else if(blueART_txBufferRead < blueART_txBufferWrite)
        {
            size = (blueART_txBufferWrite - blueART_txBufferRead);
        }
        else
        {
            size = (blueART_TX_BUFFER_SIZE - blueART_txBufferRead) +
                    blueART_txBufferWrite;
        }

        blueART_EnableTxInt();

    #else

        size = blueART_TXSTATUS_REG;

        /* Is the fifo is full. */
        if((size & blueART_TX_STS_FIFO_FULL) != 0u)
        {
            size = blueART_FIFO_LENGTH;
        }
        else if((size & blueART_TX_STS_FIFO_EMPTY) != 0u)
        {
            size = 0u;
        }
        else
        {
            /* We only know there is data in the fifo. */
            size = 1u;
        }

    #endif /* (blueART_TX_INTERRUPT_ENABLED) */

    return(size);
    }


    /*******************************************************************************
    * Function Name: blueART_ClearTxBuffer
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
    *  blueART_txBufferWrite - cleared to zero.
    *  blueART_txBufferRead - cleared to zero.
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
    void blueART_ClearTxBuffer(void) 
    {
        uint8 enableInterrupts;

        enableInterrupts = CyEnterCriticalSection();
        /* Clear the HW FIFO */
        blueART_TXDATA_AUX_CTL_REG |= (uint8)  blueART_TX_FIFO_CLR;
        blueART_TXDATA_AUX_CTL_REG &= (uint8) ~blueART_TX_FIFO_CLR;
        CyExitCriticalSection(enableInterrupts);

    #if (blueART_TX_INTERRUPT_ENABLED)

        /* Protect variables that could change on interrupt. */
        blueART_DisableTxInt();

        blueART_txBufferRead = 0u;
        blueART_txBufferWrite = 0u;

        /* Enable Tx interrupt. */
        blueART_EnableTxInt();

    #endif /* (blueART_TX_INTERRUPT_ENABLED) */
    }


    /*******************************************************************************
    * Function Name: blueART_SendBreak
    ********************************************************************************
    *
    * Summary:
    *  Transmits a break signal on the bus.
    *
    * Parameters:
    *  uint8 retMode:  Send Break return mode. See the following table for options.
    *   blueART_SEND_BREAK - Initialize registers for break, send the Break
    *       signal and return immediately.
    *   blueART_WAIT_FOR_COMPLETE_REINIT - Wait until break transmission is
    *       complete, reinitialize registers to normal transmission mode then return
    *   blueART_REINIT - Reinitialize registers to normal transmission mode
    *       then return.
    *   blueART_SEND_WAIT_REINIT - Performs both options: 
    *      blueART_SEND_BREAK and blueART_WAIT_FOR_COMPLETE_REINIT.
    *      This option is recommended for most cases.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  blueART_initVar - checked to identify that the component has been
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
    *     When interrupt appear with blueART_TX_STS_COMPLETE status:
    *     SendBreak(2);     - complete Break operation
    *
    * Side Effects:
    *  The blueART_SendBreak() function initializes registers to send a
    *  break signal.
    *  Break signal length depends on the break signal bits configuration.
    *  The register configuration should be reinitialized before normal 8-bit
    *  communication can continue.
    *
    *******************************************************************************/
    void blueART_SendBreak(uint8 retMode) 
    {

        /* If not Initialized then skip this function*/
        if(blueART_initVar != 0u)
        {
            /* Set the Counter to 13-bits and transmit a 00 byte */
            /* When that is done then reset the counter value back */
            uint8 tmpStat;

        #if(blueART_HD_ENABLED) /* Half Duplex mode*/

            if( (retMode == blueART_SEND_BREAK) ||
                (retMode == blueART_SEND_WAIT_REINIT ) )
            {
                /* CTRL_HD_SEND_BREAK - sends break bits in HD mode */
                blueART_WriteControlRegister(blueART_ReadControlRegister() |
                                                      blueART_CTRL_HD_SEND_BREAK);
                /* Send zeros */
                blueART_TXDATA_REG = 0u;

                do /* Wait until transmit starts */
                {
                    tmpStat = blueART_TXSTATUS_REG;
                }
                while((tmpStat & blueART_TX_STS_FIFO_EMPTY) != 0u);
            }

            if( (retMode == blueART_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == blueART_SEND_WAIT_REINIT) )
            {
                do /* Wait until transmit complete */
                {
                    tmpStat = blueART_TXSTATUS_REG;
                }
                while(((uint8)~tmpStat & blueART_TX_STS_COMPLETE) != 0u);
            }

            if( (retMode == blueART_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == blueART_REINIT) ||
                (retMode == blueART_SEND_WAIT_REINIT) )
            {
                blueART_WriteControlRegister(blueART_ReadControlRegister() &
                                              (uint8)~blueART_CTRL_HD_SEND_BREAK);
            }

        #else /* blueART_HD_ENABLED Full Duplex mode */

            static uint8 txPeriod;

            if( (retMode == blueART_SEND_BREAK) ||
                (retMode == blueART_SEND_WAIT_REINIT) )
            {
                /* CTRL_HD_SEND_BREAK - skip to send parity bit at Break signal in Full Duplex mode */
                #if( (blueART_PARITY_TYPE != blueART__B_UART__NONE_REVB) || \
                                    (blueART_PARITY_TYPE_SW != 0u) )
                    blueART_WriteControlRegister(blueART_ReadControlRegister() |
                                                          blueART_CTRL_HD_SEND_BREAK);
                #endif /* End blueART_PARITY_TYPE != blueART__B_UART__NONE_REVB  */

                #if(blueART_TXCLKGEN_DP)
                    txPeriod = blueART_TXBITCLKTX_COMPLETE_REG;
                    blueART_TXBITCLKTX_COMPLETE_REG = blueART_TXBITCTR_BREAKBITS;
                #else
                    txPeriod = blueART_TXBITCTR_PERIOD_REG;
                    blueART_TXBITCTR_PERIOD_REG = blueART_TXBITCTR_BREAKBITS8X;
                #endif /* End blueART_TXCLKGEN_DP */

                /* Send zeros */
                blueART_TXDATA_REG = 0u;

                do /* Wait until transmit starts */
                {
                    tmpStat = blueART_TXSTATUS_REG;
                }
                while((tmpStat & blueART_TX_STS_FIFO_EMPTY) != 0u);
            }

            if( (retMode == blueART_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == blueART_SEND_WAIT_REINIT) )
            {
                do /* Wait until transmit complete */
                {
                    tmpStat = blueART_TXSTATUS_REG;
                }
                while(((uint8)~tmpStat & blueART_TX_STS_COMPLETE) != 0u);
            }

            if( (retMode == blueART_WAIT_FOR_COMPLETE_REINIT) ||
                (retMode == blueART_REINIT) ||
                (retMode == blueART_SEND_WAIT_REINIT) )
            {

            #if(blueART_TXCLKGEN_DP)
                blueART_TXBITCLKTX_COMPLETE_REG = txPeriod;
            #else
                blueART_TXBITCTR_PERIOD_REG = txPeriod;
            #endif /* End blueART_TXCLKGEN_DP */

            #if( (blueART_PARITY_TYPE != blueART__B_UART__NONE_REVB) || \
                 (blueART_PARITY_TYPE_SW != 0u) )
                blueART_WriteControlRegister(blueART_ReadControlRegister() &
                                                      (uint8) ~blueART_CTRL_HD_SEND_BREAK);
            #endif /* End blueART_PARITY_TYPE != NONE */
            }
        #endif    /* End blueART_HD_ENABLED */
        }
    }


    /*******************************************************************************
    * Function Name: blueART_SetTxAddressMode
    ********************************************************************************
    *
    * Summary:
    *  Configures the transmitter to signal the next bytes is address or data.
    *
    * Parameters:
    *  addressMode: 
    *       blueART_SET_SPACE - Configure the transmitter to send the next
    *                                    byte as a data.
    *       blueART_SET_MARK  - Configure the transmitter to send the next
    *                                    byte as an address.
    *
    * Return:
    *  None.
    *
    * Side Effects:
    *  This function sets and clears blueART_CTRL_MARK bit in the Control
    *  register.
    *
    *******************************************************************************/
    void blueART_SetTxAddressMode(uint8 addressMode) 
    {
        /* Mark/Space sending enable */
        if(addressMode != 0u)
        {
        #if( blueART_CONTROL_REG_REMOVED == 0u )
            blueART_WriteControlRegister(blueART_ReadControlRegister() |
                                                  blueART_CTRL_MARK);
        #endif /* End blueART_CONTROL_REG_REMOVED == 0u */
        }
        else
        {
        #if( blueART_CONTROL_REG_REMOVED == 0u )
            blueART_WriteControlRegister(blueART_ReadControlRegister() &
                                                  (uint8) ~blueART_CTRL_MARK);
        #endif /* End blueART_CONTROL_REG_REMOVED == 0u */
        }
    }

#endif  /* EndblueART_TX_ENABLED */

#if(blueART_HD_ENABLED)


    /*******************************************************************************
    * Function Name: blueART_LoadRxConfig
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
    void blueART_LoadRxConfig(void) 
    {
        blueART_WriteControlRegister(blueART_ReadControlRegister() &
                                                (uint8)~blueART_CTRL_HD_SEND);
        blueART_RXBITCTR_PERIOD_REG = blueART_HD_RXBITCTR_INIT;

    #if (blueART_RX_INTERRUPT_ENABLED)
        /* Enable RX interrupt after set RX configuration */
        blueART_SetRxInterruptMode(blueART_INIT_RX_INTERRUPTS_MASK);
    #endif /* (blueART_RX_INTERRUPT_ENABLED) */
    }


    /*******************************************************************************
    * Function Name: blueART_LoadTxConfig
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
    void blueART_LoadTxConfig(void) 
    {
    #if (blueART_RX_INTERRUPT_ENABLED)
        /* Disable RX interrupts before set TX configuration */
        blueART_SetRxInterruptMode(0u);
    #endif /* (blueART_RX_INTERRUPT_ENABLED) */

        blueART_WriteControlRegister(blueART_ReadControlRegister() | blueART_CTRL_HD_SEND);
        blueART_RXBITCTR_PERIOD_REG = blueART_HD_TXBITCTR_INIT;
    }

#endif  /* blueART_HD_ENABLED */


/* [] END OF FILE */
