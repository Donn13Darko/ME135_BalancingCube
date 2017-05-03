/*******************************************************************************
* File Name: blueARTINT.c
* Version 2.50
*
* Description:
*  This file provides all Interrupt Service functionality of the UART component
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "blueART.h"
#include "cyapicallbacks.h"


/***************************************
* Custom Declarations
***************************************/
/* `#START CUSTOM_DECLARATIONS` Place your declaration here */

/* `#END` */

#if (blueART_RX_INTERRUPT_ENABLED && (blueART_RX_ENABLED || blueART_HD_ENABLED))
    /*******************************************************************************
    * Function Name: blueART_RXISR
    ********************************************************************************
    *
    * Summary:
    *  Interrupt Service Routine for RX portion of the UART
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  blueART_rxBuffer - RAM buffer pointer for save received data.
    *  blueART_rxBufferWrite - cyclic index for write to rxBuffer,
    *     increments after each byte saved to buffer.
    *  blueART_rxBufferRead - cyclic index for read from rxBuffer,
    *     checked to detect overflow condition.
    *  blueART_rxBufferOverflow - software overflow flag. Set to one
    *     when blueART_rxBufferWrite index overtakes
    *     blueART_rxBufferRead index.
    *  blueART_rxBufferLoopDetect - additional variable to detect overflow.
    *     Set to one when blueART_rxBufferWrite is equal to
    *    blueART_rxBufferRead
    *  blueART_rxAddressMode - this variable contains the Address mode,
    *     selected in customizer or set by UART_SetRxAddressMode() API.
    *  blueART_rxAddressDetected - set to 1 when correct address received,
    *     and analysed to store following addressed data bytes to the buffer.
    *     When not correct address received, set to 0 to skip following data bytes.
    *
    *******************************************************************************/
    CY_ISR(blueART_RXISR)
    {
        uint8 readData;
        uint8 readStatus;
        uint8 increment_pointer = 0u;

    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef blueART_RXISR_ENTRY_CALLBACK
        blueART_RXISR_EntryCallback();
    #endif /* blueART_RXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START blueART_RXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        do
        {
            /* Read receiver status register */
            readStatus = blueART_RXSTATUS_REG;
            /* Copy the same status to readData variable for backward compatibility support 
            *  of the user code in blueART_RXISR_ERROR` section. 
            */
            readData = readStatus;

            if((readStatus & (blueART_RX_STS_BREAK | 
                            blueART_RX_STS_PAR_ERROR |
                            blueART_RX_STS_STOP_ERROR | 
                            blueART_RX_STS_OVERRUN)) != 0u)
            {
                /* ERROR handling. */
                blueART_errorStatus |= readStatus & ( blueART_RX_STS_BREAK | 
                                                            blueART_RX_STS_PAR_ERROR | 
                                                            blueART_RX_STS_STOP_ERROR | 
                                                            blueART_RX_STS_OVERRUN);
                /* `#START blueART_RXISR_ERROR` */

                /* `#END` */
                
            #ifdef blueART_RXISR_ERROR_CALLBACK
                blueART_RXISR_ERROR_Callback();
            #endif /* blueART_RXISR_ERROR_CALLBACK */
            }
            
            if((readStatus & blueART_RX_STS_FIFO_NOTEMPTY) != 0u)
            {
                /* Read data from the RX data register */
                readData = blueART_RXDATA_REG;
            #if (blueART_RXHW_ADDRESS_ENABLED)
                if(blueART_rxAddressMode == (uint8)blueART__B_UART__AM_SW_DETECT_TO_BUFFER)
                {
                    if((readStatus & blueART_RX_STS_MRKSPC) != 0u)
                    {
                        if ((readStatus & blueART_RX_STS_ADDR_MATCH) != 0u)
                        {
                            blueART_rxAddressDetected = 1u;
                        }
                        else
                        {
                            blueART_rxAddressDetected = 0u;
                        }
                    }
                    if(blueART_rxAddressDetected != 0u)
                    {   /* Store only addressed data */
                        blueART_rxBuffer[blueART_rxBufferWrite] = readData;
                        increment_pointer = 1u;
                    }
                }
                else /* Without software addressing */
                {
                    blueART_rxBuffer[blueART_rxBufferWrite] = readData;
                    increment_pointer = 1u;
                }
            #else  /* Without addressing */
                blueART_rxBuffer[blueART_rxBufferWrite] = readData;
                increment_pointer = 1u;
            #endif /* (blueART_RXHW_ADDRESS_ENABLED) */

                /* Do not increment buffer pointer when skip not addressed data */
                if(increment_pointer != 0u)
                {
                    if(blueART_rxBufferLoopDetect != 0u)
                    {   /* Set Software Buffer status Overflow */
                        blueART_rxBufferOverflow = 1u;
                    }
                    /* Set next pointer. */
                    blueART_rxBufferWrite++;

                    /* Check pointer for a loop condition */
                    if(blueART_rxBufferWrite >= blueART_RX_BUFFER_SIZE)
                    {
                        blueART_rxBufferWrite = 0u;
                    }

                    /* Detect pre-overload condition and set flag */
                    if(blueART_rxBufferWrite == blueART_rxBufferRead)
                    {
                        blueART_rxBufferLoopDetect = 1u;
                        /* When Hardware Flow Control selected */
                        #if (blueART_FLOW_CONTROL != 0u)
                            /* Disable RX interrupt mask, it is enabled when user read data from the buffer using APIs */
                            blueART_RXSTATUS_MASK_REG  &= (uint8)~blueART_RX_STS_FIFO_NOTEMPTY;
                            CyIntClearPending(blueART_RX_VECT_NUM);
                            break; /* Break the reading of the FIFO loop, leave the data there for generating RTS signal */
                        #endif /* (blueART_FLOW_CONTROL != 0u) */
                    }
                }
            }
        }while((readStatus & blueART_RX_STS_FIFO_NOTEMPTY) != 0u);

        /* User code required at end of ISR (Optional) */
        /* `#START blueART_RXISR_END` */

        /* `#END` */

    #ifdef blueART_RXISR_EXIT_CALLBACK
        blueART_RXISR_ExitCallback();
    #endif /* blueART_RXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
    }
    
#endif /* (blueART_RX_INTERRUPT_ENABLED && (blueART_RX_ENABLED || blueART_HD_ENABLED)) */


#if (blueART_TX_INTERRUPT_ENABLED && blueART_TX_ENABLED)
    /*******************************************************************************
    * Function Name: blueART_TXISR
    ********************************************************************************
    *
    * Summary:
    * Interrupt Service Routine for the TX portion of the UART
    *
    * Parameters:
    *  None.
    *
    * Return:
    *  None.
    *
    * Global Variables:
    *  blueART_txBuffer - RAM buffer pointer for transmit data from.
    *  blueART_txBufferRead - cyclic index for read and transmit data
    *     from txBuffer, increments after each transmitted byte.
    *  blueART_rxBufferWrite - cyclic index for write to txBuffer,
    *     checked to detect available for transmission bytes.
    *
    *******************************************************************************/
    CY_ISR(blueART_TXISR)
    {
    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef blueART_TXISR_ENTRY_CALLBACK
        blueART_TXISR_EntryCallback();
    #endif /* blueART_TXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START blueART_TXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        while((blueART_txBufferRead != blueART_txBufferWrite) &&
             ((blueART_TXSTATUS_REG & blueART_TX_STS_FIFO_FULL) == 0u))
        {
            /* Check pointer wrap around */
            if(blueART_txBufferRead >= blueART_TX_BUFFER_SIZE)
            {
                blueART_txBufferRead = 0u;
            }

            blueART_TXDATA_REG = blueART_txBuffer[blueART_txBufferRead];

            /* Set next pointer */
            blueART_txBufferRead++;
        }

        /* User code required at end of ISR (Optional) */
        /* `#START blueART_TXISR_END` */

        /* `#END` */

    #ifdef blueART_TXISR_EXIT_CALLBACK
        blueART_TXISR_ExitCallback();
    #endif /* blueART_TXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
   }
#endif /* (blueART_TX_INTERRUPT_ENABLED && blueART_TX_ENABLED) */


/* [] END OF FILE */
