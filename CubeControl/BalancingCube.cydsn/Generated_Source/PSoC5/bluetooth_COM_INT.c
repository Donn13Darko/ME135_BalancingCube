/*******************************************************************************
* File Name: bluetooth_COMINT.c
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

#include "bluetooth_COM.h"
#include "cyapicallbacks.h"


/***************************************
* Custom Declarations
***************************************/
/* `#START CUSTOM_DECLARATIONS` Place your declaration here */

/* `#END` */

#if (bluetooth_COM_RX_INTERRUPT_ENABLED && (bluetooth_COM_RX_ENABLED || bluetooth_COM_HD_ENABLED))
    /*******************************************************************************
    * Function Name: bluetooth_COM_RXISR
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
    *  bluetooth_COM_rxBuffer - RAM buffer pointer for save received data.
    *  bluetooth_COM_rxBufferWrite - cyclic index for write to rxBuffer,
    *     increments after each byte saved to buffer.
    *  bluetooth_COM_rxBufferRead - cyclic index for read from rxBuffer,
    *     checked to detect overflow condition.
    *  bluetooth_COM_rxBufferOverflow - software overflow flag. Set to one
    *     when bluetooth_COM_rxBufferWrite index overtakes
    *     bluetooth_COM_rxBufferRead index.
    *  bluetooth_COM_rxBufferLoopDetect - additional variable to detect overflow.
    *     Set to one when bluetooth_COM_rxBufferWrite is equal to
    *    bluetooth_COM_rxBufferRead
    *  bluetooth_COM_rxAddressMode - this variable contains the Address mode,
    *     selected in customizer or set by UART_SetRxAddressMode() API.
    *  bluetooth_COM_rxAddressDetected - set to 1 when correct address received,
    *     and analysed to store following addressed data bytes to the buffer.
    *     When not correct address received, set to 0 to skip following data bytes.
    *
    *******************************************************************************/
    CY_ISR(bluetooth_COM_RXISR)
    {
        uint8 readData;
        uint8 readStatus;
        uint8 increment_pointer = 0u;

    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef bluetooth_COM_RXISR_ENTRY_CALLBACK
        bluetooth_COM_RXISR_EntryCallback();
    #endif /* bluetooth_COM_RXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START bluetooth_COM_RXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        do
        {
            /* Read receiver status register */
            readStatus = bluetooth_COM_RXSTATUS_REG;
            /* Copy the same status to readData variable for backward compatibility support 
            *  of the user code in bluetooth_COM_RXISR_ERROR` section. 
            */
            readData = readStatus;

            if((readStatus & (bluetooth_COM_RX_STS_BREAK | 
                            bluetooth_COM_RX_STS_PAR_ERROR |
                            bluetooth_COM_RX_STS_STOP_ERROR | 
                            bluetooth_COM_RX_STS_OVERRUN)) != 0u)
            {
                /* ERROR handling. */
                bluetooth_COM_errorStatus |= readStatus & ( bluetooth_COM_RX_STS_BREAK | 
                                                            bluetooth_COM_RX_STS_PAR_ERROR | 
                                                            bluetooth_COM_RX_STS_STOP_ERROR | 
                                                            bluetooth_COM_RX_STS_OVERRUN);
                /* `#START bluetooth_COM_RXISR_ERROR` */

                /* `#END` */
                
            #ifdef bluetooth_COM_RXISR_ERROR_CALLBACK
                bluetooth_COM_RXISR_ERROR_Callback();
            #endif /* bluetooth_COM_RXISR_ERROR_CALLBACK */
            }
            
            if((readStatus & bluetooth_COM_RX_STS_FIFO_NOTEMPTY) != 0u)
            {
                /* Read data from the RX data register */
                readData = bluetooth_COM_RXDATA_REG;
            #if (bluetooth_COM_RXHW_ADDRESS_ENABLED)
                if(bluetooth_COM_rxAddressMode == (uint8)bluetooth_COM__B_UART__AM_SW_DETECT_TO_BUFFER)
                {
                    if((readStatus & bluetooth_COM_RX_STS_MRKSPC) != 0u)
                    {
                        if ((readStatus & bluetooth_COM_RX_STS_ADDR_MATCH) != 0u)
                        {
                            bluetooth_COM_rxAddressDetected = 1u;
                        }
                        else
                        {
                            bluetooth_COM_rxAddressDetected = 0u;
                        }
                    }
                    if(bluetooth_COM_rxAddressDetected != 0u)
                    {   /* Store only addressed data */
                        bluetooth_COM_rxBuffer[bluetooth_COM_rxBufferWrite] = readData;
                        increment_pointer = 1u;
                    }
                }
                else /* Without software addressing */
                {
                    bluetooth_COM_rxBuffer[bluetooth_COM_rxBufferWrite] = readData;
                    increment_pointer = 1u;
                }
            #else  /* Without addressing */
                bluetooth_COM_rxBuffer[bluetooth_COM_rxBufferWrite] = readData;
                increment_pointer = 1u;
            #endif /* (bluetooth_COM_RXHW_ADDRESS_ENABLED) */

                /* Do not increment buffer pointer when skip not addressed data */
                if(increment_pointer != 0u)
                {
                    if(bluetooth_COM_rxBufferLoopDetect != 0u)
                    {   /* Set Software Buffer status Overflow */
                        bluetooth_COM_rxBufferOverflow = 1u;
                    }
                    /* Set next pointer. */
                    bluetooth_COM_rxBufferWrite++;

                    /* Check pointer for a loop condition */
                    if(bluetooth_COM_rxBufferWrite >= bluetooth_COM_RX_BUFFER_SIZE)
                    {
                        bluetooth_COM_rxBufferWrite = 0u;
                    }

                    /* Detect pre-overload condition and set flag */
                    if(bluetooth_COM_rxBufferWrite == bluetooth_COM_rxBufferRead)
                    {
                        bluetooth_COM_rxBufferLoopDetect = 1u;
                        /* When Hardware Flow Control selected */
                        #if (bluetooth_COM_FLOW_CONTROL != 0u)
                            /* Disable RX interrupt mask, it is enabled when user read data from the buffer using APIs */
                            bluetooth_COM_RXSTATUS_MASK_REG  &= (uint8)~bluetooth_COM_RX_STS_FIFO_NOTEMPTY;
                            CyIntClearPending(bluetooth_COM_RX_VECT_NUM);
                            break; /* Break the reading of the FIFO loop, leave the data there for generating RTS signal */
                        #endif /* (bluetooth_COM_FLOW_CONTROL != 0u) */
                    }
                }
            }
        }while((readStatus & bluetooth_COM_RX_STS_FIFO_NOTEMPTY) != 0u);

        /* User code required at end of ISR (Optional) */
        /* `#START bluetooth_COM_RXISR_END` */

        /* `#END` */

    #ifdef bluetooth_COM_RXISR_EXIT_CALLBACK
        bluetooth_COM_RXISR_ExitCallback();
    #endif /* bluetooth_COM_RXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
    }
    
#endif /* (bluetooth_COM_RX_INTERRUPT_ENABLED && (bluetooth_COM_RX_ENABLED || bluetooth_COM_HD_ENABLED)) */


#if (bluetooth_COM_TX_INTERRUPT_ENABLED && bluetooth_COM_TX_ENABLED)
    /*******************************************************************************
    * Function Name: bluetooth_COM_TXISR
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
    *  bluetooth_COM_txBuffer - RAM buffer pointer for transmit data from.
    *  bluetooth_COM_txBufferRead - cyclic index for read and transmit data
    *     from txBuffer, increments after each transmitted byte.
    *  bluetooth_COM_rxBufferWrite - cyclic index for write to txBuffer,
    *     checked to detect available for transmission bytes.
    *
    *******************************************************************************/
    CY_ISR(bluetooth_COM_TXISR)
    {
    #if(CY_PSOC3)
        uint8 int_en;
    #endif /* (CY_PSOC3) */

    #ifdef bluetooth_COM_TXISR_ENTRY_CALLBACK
        bluetooth_COM_TXISR_EntryCallback();
    #endif /* bluetooth_COM_TXISR_ENTRY_CALLBACK */

        /* User code required at start of ISR */
        /* `#START bluetooth_COM_TXISR_START` */

        /* `#END` */

    #if(CY_PSOC3)   /* Make sure nested interrupt is enabled */
        int_en = EA;
        CyGlobalIntEnable;
    #endif /* (CY_PSOC3) */

        while((bluetooth_COM_txBufferRead != bluetooth_COM_txBufferWrite) &&
             ((bluetooth_COM_TXSTATUS_REG & bluetooth_COM_TX_STS_FIFO_FULL) == 0u))
        {
            /* Check pointer wrap around */
            if(bluetooth_COM_txBufferRead >= bluetooth_COM_TX_BUFFER_SIZE)
            {
                bluetooth_COM_txBufferRead = 0u;
            }

            bluetooth_COM_TXDATA_REG = bluetooth_COM_txBuffer[bluetooth_COM_txBufferRead];

            /* Set next pointer */
            bluetooth_COM_txBufferRead++;
        }

        /* User code required at end of ISR (Optional) */
        /* `#START bluetooth_COM_TXISR_END` */

        /* `#END` */

    #ifdef bluetooth_COM_TXISR_EXIT_CALLBACK
        bluetooth_COM_TXISR_ExitCallback();
    #endif /* bluetooth_COM_TXISR_EXIT_CALLBACK */

    #if(CY_PSOC3)
        EA = int_en;
    #endif /* (CY_PSOC3) */
   }
#endif /* (bluetooth_COM_TX_INTERRUPT_ENABLED && bluetooth_COM_TX_ENABLED) */


/* [] END OF FILE */
