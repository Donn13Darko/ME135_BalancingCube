/*******************************************************************************
* File Name: bluetooth_COM.h
* Version 2.50
*
* Description:
*  Contains the function prototypes and constants available to the UART
*  user module.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/


#if !defined(CY_UART_bluetooth_COM_H)
#define CY_UART_bluetooth_COM_H

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h"


/***************************************
* Conditional Compilation Parameters
***************************************/

#define bluetooth_COM_RX_ENABLED                     (1u)
#define bluetooth_COM_TX_ENABLED                     (1u)
#define bluetooth_COM_HD_ENABLED                     (0u)
#define bluetooth_COM_RX_INTERRUPT_ENABLED           (0u)
#define bluetooth_COM_TX_INTERRUPT_ENABLED           (0u)
#define bluetooth_COM_INTERNAL_CLOCK_USED            (1u)
#define bluetooth_COM_RXHW_ADDRESS_ENABLED           (0u)
#define bluetooth_COM_OVER_SAMPLE_COUNT              (8u)
#define bluetooth_COM_PARITY_TYPE                    (0u)
#define bluetooth_COM_PARITY_TYPE_SW                 (0u)
#define bluetooth_COM_BREAK_DETECT                   (0u)
#define bluetooth_COM_BREAK_BITS_TX                  (13u)
#define bluetooth_COM_BREAK_BITS_RX                  (13u)
#define bluetooth_COM_TXCLKGEN_DP                    (1u)
#define bluetooth_COM_USE23POLLING                   (1u)
#define bluetooth_COM_FLOW_CONTROL                   (0u)
#define bluetooth_COM_CLK_FREQ                       (0u)
#define bluetooth_COM_TX_BUFFER_SIZE                 (4u)
#define bluetooth_COM_RX_BUFFER_SIZE                 (4u)

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component UART_v2_50 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */

#if defined(bluetooth_COM_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG)
    #define bluetooth_COM_CONTROL_REG_REMOVED            (0u)
#else
    #define bluetooth_COM_CONTROL_REG_REMOVED            (1u)
#endif /* End bluetooth_COM_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */


/***************************************
*      Data Structure Definition
***************************************/

/* Sleep Mode API Support */
typedef struct bluetooth_COM_backupStruct_
{
    uint8 enableState;

    #if(bluetooth_COM_CONTROL_REG_REMOVED == 0u)
        uint8 cr;
    #endif /* End bluetooth_COM_CONTROL_REG_REMOVED */

} bluetooth_COM_BACKUP_STRUCT;


/***************************************
*       Function Prototypes
***************************************/

void bluetooth_COM_Start(void) ;
void bluetooth_COM_Stop(void) ;
uint8 bluetooth_COM_ReadControlRegister(void) ;
void bluetooth_COM_WriteControlRegister(uint8 control) ;

void bluetooth_COM_Init(void) ;
void bluetooth_COM_Enable(void) ;
void bluetooth_COM_SaveConfig(void) ;
void bluetooth_COM_RestoreConfig(void) ;
void bluetooth_COM_Sleep(void) ;
void bluetooth_COM_Wakeup(void) ;

/* Only if RX is enabled */
#if( (bluetooth_COM_RX_ENABLED) || (bluetooth_COM_HD_ENABLED) )

    #if (bluetooth_COM_RX_INTERRUPT_ENABLED)
        #define bluetooth_COM_EnableRxInt()  CyIntEnable (bluetooth_COM_RX_VECT_NUM)
        #define bluetooth_COM_DisableRxInt() CyIntDisable(bluetooth_COM_RX_VECT_NUM)
        CY_ISR_PROTO(bluetooth_COM_RXISR);
    #endif /* bluetooth_COM_RX_INTERRUPT_ENABLED */

    void bluetooth_COM_SetRxAddressMode(uint8 addressMode)
                                                           ;
    void bluetooth_COM_SetRxAddress1(uint8 address) ;
    void bluetooth_COM_SetRxAddress2(uint8 address) ;

    void  bluetooth_COM_SetRxInterruptMode(uint8 intSrc) ;
    uint8 bluetooth_COM_ReadRxData(void) ;
    uint8 bluetooth_COM_ReadRxStatus(void) ;
    uint8 bluetooth_COM_GetChar(void) ;
    uint16 bluetooth_COM_GetByte(void) ;
    uint8 bluetooth_COM_GetRxBufferSize(void)
                                                            ;
    void bluetooth_COM_ClearRxBuffer(void) ;

    /* Obsolete functions, defines for backward compatible */
    #define bluetooth_COM_GetRxInterruptSource   bluetooth_COM_ReadRxStatus

#endif /* End (bluetooth_COM_RX_ENABLED) || (bluetooth_COM_HD_ENABLED) */

/* Only if TX is enabled */
#if(bluetooth_COM_TX_ENABLED || bluetooth_COM_HD_ENABLED)

    #if(bluetooth_COM_TX_INTERRUPT_ENABLED)
        #define bluetooth_COM_EnableTxInt()  CyIntEnable (bluetooth_COM_TX_VECT_NUM)
        #define bluetooth_COM_DisableTxInt() CyIntDisable(bluetooth_COM_TX_VECT_NUM)
        #define bluetooth_COM_SetPendingTxInt() CyIntSetPending(bluetooth_COM_TX_VECT_NUM)
        #define bluetooth_COM_ClearPendingTxInt() CyIntClearPending(bluetooth_COM_TX_VECT_NUM)
        CY_ISR_PROTO(bluetooth_COM_TXISR);
    #endif /* bluetooth_COM_TX_INTERRUPT_ENABLED */

    void bluetooth_COM_SetTxInterruptMode(uint8 intSrc) ;
    void bluetooth_COM_WriteTxData(uint8 txDataByte) ;
    uint8 bluetooth_COM_ReadTxStatus(void) ;
    void bluetooth_COM_PutChar(uint8 txDataByte) ;
    void bluetooth_COM_PutString(const char8 string[]) ;
    void bluetooth_COM_PutArray(const uint8 string[], uint8 byteCount)
                                                            ;
    void bluetooth_COM_PutCRLF(uint8 txDataByte) ;
    void bluetooth_COM_ClearTxBuffer(void) ;
    void bluetooth_COM_SetTxAddressMode(uint8 addressMode) ;
    void bluetooth_COM_SendBreak(uint8 retMode) ;
    uint8 bluetooth_COM_GetTxBufferSize(void)
                                                            ;
    /* Obsolete functions, defines for backward compatible */
    #define bluetooth_COM_PutStringConst         bluetooth_COM_PutString
    #define bluetooth_COM_PutArrayConst          bluetooth_COM_PutArray
    #define bluetooth_COM_GetTxInterruptSource   bluetooth_COM_ReadTxStatus

#endif /* End bluetooth_COM_TX_ENABLED || bluetooth_COM_HD_ENABLED */

#if(bluetooth_COM_HD_ENABLED)
    void bluetooth_COM_LoadRxConfig(void) ;
    void bluetooth_COM_LoadTxConfig(void) ;
#endif /* End bluetooth_COM_HD_ENABLED */


/* Communication bootloader APIs */
#if defined(CYDEV_BOOTLOADER_IO_COMP) && ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_bluetooth_COM) || \
                                          (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface))
    /* Physical layer functions */
    void    bluetooth_COM_CyBtldrCommStart(void) CYSMALL ;
    void    bluetooth_COM_CyBtldrCommStop(void) CYSMALL ;
    void    bluetooth_COM_CyBtldrCommReset(void) CYSMALL ;
    cystatus bluetooth_COM_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;
    cystatus bluetooth_COM_CyBtldrCommRead(uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;

    #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_bluetooth_COM)
        #define CyBtldrCommStart    bluetooth_COM_CyBtldrCommStart
        #define CyBtldrCommStop     bluetooth_COM_CyBtldrCommStop
        #define CyBtldrCommReset    bluetooth_COM_CyBtldrCommReset
        #define CyBtldrCommWrite    bluetooth_COM_CyBtldrCommWrite
        #define CyBtldrCommRead     bluetooth_COM_CyBtldrCommRead
    #endif  /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_bluetooth_COM) */

    /* Byte to Byte time out for detecting end of block data from host */
    #define bluetooth_COM_BYTE2BYTE_TIME_OUT (25u)
    #define bluetooth_COM_PACKET_EOP         (0x17u) /* End of packet defined by bootloader */
    #define bluetooth_COM_WAIT_EOP_DELAY     (5u)    /* Additional 5ms to wait for End of packet */
    #define bluetooth_COM_BL_CHK_DELAY_MS    (1u)    /* Time Out quantity equal 1mS */

#endif /* CYDEV_BOOTLOADER_IO_COMP */


/***************************************
*          API Constants
***************************************/
/* Parameters for SetTxAddressMode API*/
#define bluetooth_COM_SET_SPACE      (0x00u)
#define bluetooth_COM_SET_MARK       (0x01u)

/* Status Register definitions */
#if( (bluetooth_COM_TX_ENABLED) || (bluetooth_COM_HD_ENABLED) )
    #if(bluetooth_COM_TX_INTERRUPT_ENABLED)
        #define bluetooth_COM_TX_VECT_NUM            (uint8)bluetooth_COM_TXInternalInterrupt__INTC_NUMBER
        #define bluetooth_COM_TX_PRIOR_NUM           (uint8)bluetooth_COM_TXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* bluetooth_COM_TX_INTERRUPT_ENABLED */

    #define bluetooth_COM_TX_STS_COMPLETE_SHIFT          (0x00u)
    #define bluetooth_COM_TX_STS_FIFO_EMPTY_SHIFT        (0x01u)
    #define bluetooth_COM_TX_STS_FIFO_NOT_FULL_SHIFT     (0x03u)
    #if(bluetooth_COM_TX_ENABLED)
        #define bluetooth_COM_TX_STS_FIFO_FULL_SHIFT     (0x02u)
    #else /* (bluetooth_COM_HD_ENABLED) */
        #define bluetooth_COM_TX_STS_FIFO_FULL_SHIFT     (0x05u)  /* Needs MD=0 */
    #endif /* (bluetooth_COM_TX_ENABLED) */

    #define bluetooth_COM_TX_STS_COMPLETE            (uint8)(0x01u << bluetooth_COM_TX_STS_COMPLETE_SHIFT)
    #define bluetooth_COM_TX_STS_FIFO_EMPTY          (uint8)(0x01u << bluetooth_COM_TX_STS_FIFO_EMPTY_SHIFT)
    #define bluetooth_COM_TX_STS_FIFO_FULL           (uint8)(0x01u << bluetooth_COM_TX_STS_FIFO_FULL_SHIFT)
    #define bluetooth_COM_TX_STS_FIFO_NOT_FULL       (uint8)(0x01u << bluetooth_COM_TX_STS_FIFO_NOT_FULL_SHIFT)
#endif /* End (bluetooth_COM_TX_ENABLED) || (bluetooth_COM_HD_ENABLED)*/

#if( (bluetooth_COM_RX_ENABLED) || (bluetooth_COM_HD_ENABLED) )
    #if(bluetooth_COM_RX_INTERRUPT_ENABLED)
        #define bluetooth_COM_RX_VECT_NUM            (uint8)bluetooth_COM_RXInternalInterrupt__INTC_NUMBER
        #define bluetooth_COM_RX_PRIOR_NUM           (uint8)bluetooth_COM_RXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* bluetooth_COM_RX_INTERRUPT_ENABLED */
    #define bluetooth_COM_RX_STS_MRKSPC_SHIFT            (0x00u)
    #define bluetooth_COM_RX_STS_BREAK_SHIFT             (0x01u)
    #define bluetooth_COM_RX_STS_PAR_ERROR_SHIFT         (0x02u)
    #define bluetooth_COM_RX_STS_STOP_ERROR_SHIFT        (0x03u)
    #define bluetooth_COM_RX_STS_OVERRUN_SHIFT           (0x04u)
    #define bluetooth_COM_RX_STS_FIFO_NOTEMPTY_SHIFT     (0x05u)
    #define bluetooth_COM_RX_STS_ADDR_MATCH_SHIFT        (0x06u)
    #define bluetooth_COM_RX_STS_SOFT_BUFF_OVER_SHIFT    (0x07u)

    #define bluetooth_COM_RX_STS_MRKSPC           (uint8)(0x01u << bluetooth_COM_RX_STS_MRKSPC_SHIFT)
    #define bluetooth_COM_RX_STS_BREAK            (uint8)(0x01u << bluetooth_COM_RX_STS_BREAK_SHIFT)
    #define bluetooth_COM_RX_STS_PAR_ERROR        (uint8)(0x01u << bluetooth_COM_RX_STS_PAR_ERROR_SHIFT)
    #define bluetooth_COM_RX_STS_STOP_ERROR       (uint8)(0x01u << bluetooth_COM_RX_STS_STOP_ERROR_SHIFT)
    #define bluetooth_COM_RX_STS_OVERRUN          (uint8)(0x01u << bluetooth_COM_RX_STS_OVERRUN_SHIFT)
    #define bluetooth_COM_RX_STS_FIFO_NOTEMPTY    (uint8)(0x01u << bluetooth_COM_RX_STS_FIFO_NOTEMPTY_SHIFT)
    #define bluetooth_COM_RX_STS_ADDR_MATCH       (uint8)(0x01u << bluetooth_COM_RX_STS_ADDR_MATCH_SHIFT)
    #define bluetooth_COM_RX_STS_SOFT_BUFF_OVER   (uint8)(0x01u << bluetooth_COM_RX_STS_SOFT_BUFF_OVER_SHIFT)
    #define bluetooth_COM_RX_HW_MASK                     (0x7Fu)
#endif /* End (bluetooth_COM_RX_ENABLED) || (bluetooth_COM_HD_ENABLED) */

/* Control Register definitions */
#define bluetooth_COM_CTRL_HD_SEND_SHIFT                 (0x00u) /* 1 enable TX part in Half Duplex mode */
#define bluetooth_COM_CTRL_HD_SEND_BREAK_SHIFT           (0x01u) /* 1 send BREAK signal in Half Duplez mode */
#define bluetooth_COM_CTRL_MARK_SHIFT                    (0x02u) /* 1 sets mark, 0 sets space */
#define bluetooth_COM_CTRL_PARITY_TYPE0_SHIFT            (0x03u) /* Defines the type of parity implemented */
#define bluetooth_COM_CTRL_PARITY_TYPE1_SHIFT            (0x04u) /* Defines the type of parity implemented */
#define bluetooth_COM_CTRL_RXADDR_MODE0_SHIFT            (0x05u)
#define bluetooth_COM_CTRL_RXADDR_MODE1_SHIFT            (0x06u)
#define bluetooth_COM_CTRL_RXADDR_MODE2_SHIFT            (0x07u)

#define bluetooth_COM_CTRL_HD_SEND               (uint8)(0x01u << bluetooth_COM_CTRL_HD_SEND_SHIFT)
#define bluetooth_COM_CTRL_HD_SEND_BREAK         (uint8)(0x01u << bluetooth_COM_CTRL_HD_SEND_BREAK_SHIFT)
#define bluetooth_COM_CTRL_MARK                  (uint8)(0x01u << bluetooth_COM_CTRL_MARK_SHIFT)
#define bluetooth_COM_CTRL_PARITY_TYPE_MASK      (uint8)(0x03u << bluetooth_COM_CTRL_PARITY_TYPE0_SHIFT)
#define bluetooth_COM_CTRL_RXADDR_MODE_MASK      (uint8)(0x07u << bluetooth_COM_CTRL_RXADDR_MODE0_SHIFT)

/* StatusI Register Interrupt Enable Control Bits. As defined by the Register map for the AUX Control Register */
#define bluetooth_COM_INT_ENABLE                         (0x10u)

/* Bit Counter (7-bit) Control Register Bit Definitions. As defined by the Register map for the AUX Control Register */
#define bluetooth_COM_CNTR_ENABLE                        (0x20u)

/*   Constants for SendBreak() "retMode" parameter  */
#define bluetooth_COM_SEND_BREAK                         (0x00u)
#define bluetooth_COM_WAIT_FOR_COMPLETE_REINIT           (0x01u)
#define bluetooth_COM_REINIT                             (0x02u)
#define bluetooth_COM_SEND_WAIT_REINIT                   (0x03u)

#define bluetooth_COM_OVER_SAMPLE_8                      (8u)
#define bluetooth_COM_OVER_SAMPLE_16                     (16u)

#define bluetooth_COM_BIT_CENTER                         (bluetooth_COM_OVER_SAMPLE_COUNT - 2u)

#define bluetooth_COM_FIFO_LENGTH                        (4u)
#define bluetooth_COM_NUMBER_OF_START_BIT                (1u)
#define bluetooth_COM_MAX_BYTE_VALUE                     (0xFFu)

/* 8X always for count7 implementation */
#define bluetooth_COM_TXBITCTR_BREAKBITS8X   ((bluetooth_COM_BREAK_BITS_TX * bluetooth_COM_OVER_SAMPLE_8) - 1u)
/* 8X or 16X for DP implementation */
#define bluetooth_COM_TXBITCTR_BREAKBITS ((bluetooth_COM_BREAK_BITS_TX * bluetooth_COM_OVER_SAMPLE_COUNT) - 1u)

#define bluetooth_COM_HALF_BIT_COUNT   \
                            (((bluetooth_COM_OVER_SAMPLE_COUNT / 2u) + (bluetooth_COM_USE23POLLING * 1u)) - 2u)
#if (bluetooth_COM_OVER_SAMPLE_COUNT == bluetooth_COM_OVER_SAMPLE_8)
    #define bluetooth_COM_HD_TXBITCTR_INIT   (((bluetooth_COM_BREAK_BITS_TX + \
                            bluetooth_COM_NUMBER_OF_START_BIT) * bluetooth_COM_OVER_SAMPLE_COUNT) - 1u)

    /* This parameter is increased on the 2 in 2 out of 3 mode to sample voting in the middle */
    #define bluetooth_COM_RXBITCTR_INIT  ((((bluetooth_COM_BREAK_BITS_RX + bluetooth_COM_NUMBER_OF_START_BIT) \
                            * bluetooth_COM_OVER_SAMPLE_COUNT) + bluetooth_COM_HALF_BIT_COUNT) - 1u)

#else /* bluetooth_COM_OVER_SAMPLE_COUNT == bluetooth_COM_OVER_SAMPLE_16 */
    #define bluetooth_COM_HD_TXBITCTR_INIT   ((8u * bluetooth_COM_OVER_SAMPLE_COUNT) - 1u)
    /* 7bit counter need one more bit for OverSampleCount = 16 */
    #define bluetooth_COM_RXBITCTR_INIT      (((7u * bluetooth_COM_OVER_SAMPLE_COUNT) - 1u) + \
                                                      bluetooth_COM_HALF_BIT_COUNT)
#endif /* End bluetooth_COM_OVER_SAMPLE_COUNT */

#define bluetooth_COM_HD_RXBITCTR_INIT                   bluetooth_COM_RXBITCTR_INIT


/***************************************
* Global variables external identifier
***************************************/

extern uint8 bluetooth_COM_initVar;
#if (bluetooth_COM_TX_INTERRUPT_ENABLED && bluetooth_COM_TX_ENABLED)
    extern volatile uint8 bluetooth_COM_txBuffer[bluetooth_COM_TX_BUFFER_SIZE];
    extern volatile uint8 bluetooth_COM_txBufferRead;
    extern uint8 bluetooth_COM_txBufferWrite;
#endif /* (bluetooth_COM_TX_INTERRUPT_ENABLED && bluetooth_COM_TX_ENABLED) */
#if (bluetooth_COM_RX_INTERRUPT_ENABLED && (bluetooth_COM_RX_ENABLED || bluetooth_COM_HD_ENABLED))
    extern uint8 bluetooth_COM_errorStatus;
    extern volatile uint8 bluetooth_COM_rxBuffer[bluetooth_COM_RX_BUFFER_SIZE];
    extern volatile uint8 bluetooth_COM_rxBufferRead;
    extern volatile uint8 bluetooth_COM_rxBufferWrite;
    extern volatile uint8 bluetooth_COM_rxBufferLoopDetect;
    extern volatile uint8 bluetooth_COM_rxBufferOverflow;
    #if (bluetooth_COM_RXHW_ADDRESS_ENABLED)
        extern volatile uint8 bluetooth_COM_rxAddressMode;
        extern volatile uint8 bluetooth_COM_rxAddressDetected;
    #endif /* (bluetooth_COM_RXHW_ADDRESS_ENABLED) */
#endif /* (bluetooth_COM_RX_INTERRUPT_ENABLED && (bluetooth_COM_RX_ENABLED || bluetooth_COM_HD_ENABLED)) */


/***************************************
* Enumerated Types and Parameters
***************************************/

#define bluetooth_COM__B_UART__AM_SW_BYTE_BYTE 1
#define bluetooth_COM__B_UART__AM_SW_DETECT_TO_BUFFER 2
#define bluetooth_COM__B_UART__AM_HW_BYTE_BY_BYTE 3
#define bluetooth_COM__B_UART__AM_HW_DETECT_TO_BUFFER 4
#define bluetooth_COM__B_UART__AM_NONE 0

#define bluetooth_COM__B_UART__NONE_REVB 0
#define bluetooth_COM__B_UART__EVEN_REVB 1
#define bluetooth_COM__B_UART__ODD_REVB 2
#define bluetooth_COM__B_UART__MARK_SPACE_REVB 3



/***************************************
*    Initial Parameter Constants
***************************************/

/* UART shifts max 8 bits, Mark/Space functionality working if 9 selected */
#define bluetooth_COM_NUMBER_OF_DATA_BITS    ((8u > 8u) ? 8u : 8u)
#define bluetooth_COM_NUMBER_OF_STOP_BITS    (1u)

#if (bluetooth_COM_RXHW_ADDRESS_ENABLED)
    #define bluetooth_COM_RX_ADDRESS_MODE    (0u)
    #define bluetooth_COM_RX_HW_ADDRESS1     (0u)
    #define bluetooth_COM_RX_HW_ADDRESS2     (0u)
#endif /* (bluetooth_COM_RXHW_ADDRESS_ENABLED) */

#define bluetooth_COM_INIT_RX_INTERRUPTS_MASK \
                                  (uint8)((1 << bluetooth_COM_RX_STS_FIFO_NOTEMPTY_SHIFT) \
                                        | (0 << bluetooth_COM_RX_STS_MRKSPC_SHIFT) \
                                        | (0 << bluetooth_COM_RX_STS_ADDR_MATCH_SHIFT) \
                                        | (0 << bluetooth_COM_RX_STS_PAR_ERROR_SHIFT) \
                                        | (0 << bluetooth_COM_RX_STS_STOP_ERROR_SHIFT) \
                                        | (0 << bluetooth_COM_RX_STS_BREAK_SHIFT) \
                                        | (0 << bluetooth_COM_RX_STS_OVERRUN_SHIFT))

#define bluetooth_COM_INIT_TX_INTERRUPTS_MASK \
                                  (uint8)((0 << bluetooth_COM_TX_STS_COMPLETE_SHIFT) \
                                        | (0 << bluetooth_COM_TX_STS_FIFO_EMPTY_SHIFT) \
                                        | (0 << bluetooth_COM_TX_STS_FIFO_FULL_SHIFT) \
                                        | (0 << bluetooth_COM_TX_STS_FIFO_NOT_FULL_SHIFT))


/***************************************
*              Registers
***************************************/

#ifdef bluetooth_COM_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define bluetooth_COM_CONTROL_REG \
                            (* (reg8 *) bluetooth_COM_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
    #define bluetooth_COM_CONTROL_PTR \
                            (  (reg8 *) bluetooth_COM_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
#endif /* End bluetooth_COM_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(bluetooth_COM_TX_ENABLED)
    #define bluetooth_COM_TXDATA_REG          (* (reg8 *) bluetooth_COM_BUART_sTX_TxShifter_u0__F0_REG)
    #define bluetooth_COM_TXDATA_PTR          (  (reg8 *) bluetooth_COM_BUART_sTX_TxShifter_u0__F0_REG)
    #define bluetooth_COM_TXDATA_AUX_CTL_REG  (* (reg8 *) bluetooth_COM_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define bluetooth_COM_TXDATA_AUX_CTL_PTR  (  (reg8 *) bluetooth_COM_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define bluetooth_COM_TXSTATUS_REG        (* (reg8 *) bluetooth_COM_BUART_sTX_TxSts__STATUS_REG)
    #define bluetooth_COM_TXSTATUS_PTR        (  (reg8 *) bluetooth_COM_BUART_sTX_TxSts__STATUS_REG)
    #define bluetooth_COM_TXSTATUS_MASK_REG   (* (reg8 *) bluetooth_COM_BUART_sTX_TxSts__MASK_REG)
    #define bluetooth_COM_TXSTATUS_MASK_PTR   (  (reg8 *) bluetooth_COM_BUART_sTX_TxSts__MASK_REG)
    #define bluetooth_COM_TXSTATUS_ACTL_REG   (* (reg8 *) bluetooth_COM_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)
    #define bluetooth_COM_TXSTATUS_ACTL_PTR   (  (reg8 *) bluetooth_COM_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)

    /* DP clock */
    #if(bluetooth_COM_TXCLKGEN_DP)
        #define bluetooth_COM_TXBITCLKGEN_CTR_REG        \
                                        (* (reg8 *) bluetooth_COM_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define bluetooth_COM_TXBITCLKGEN_CTR_PTR        \
                                        (  (reg8 *) bluetooth_COM_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define bluetooth_COM_TXBITCLKTX_COMPLETE_REG    \
                                        (* (reg8 *) bluetooth_COM_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
        #define bluetooth_COM_TXBITCLKTX_COMPLETE_PTR    \
                                        (  (reg8 *) bluetooth_COM_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
    #else     /* Count7 clock*/
        #define bluetooth_COM_TXBITCTR_PERIOD_REG    \
                                        (* (reg8 *) bluetooth_COM_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define bluetooth_COM_TXBITCTR_PERIOD_PTR    \
                                        (  (reg8 *) bluetooth_COM_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define bluetooth_COM_TXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) bluetooth_COM_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define bluetooth_COM_TXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) bluetooth_COM_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define bluetooth_COM_TXBITCTR_COUNTER_REG   \
                                        (* (reg8 *) bluetooth_COM_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
        #define bluetooth_COM_TXBITCTR_COUNTER_PTR   \
                                        (  (reg8 *) bluetooth_COM_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
    #endif /* bluetooth_COM_TXCLKGEN_DP */

#endif /* End bluetooth_COM_TX_ENABLED */

#if(bluetooth_COM_HD_ENABLED)

    #define bluetooth_COM_TXDATA_REG             (* (reg8 *) bluetooth_COM_BUART_sRX_RxShifter_u0__F1_REG )
    #define bluetooth_COM_TXDATA_PTR             (  (reg8 *) bluetooth_COM_BUART_sRX_RxShifter_u0__F1_REG )
    #define bluetooth_COM_TXDATA_AUX_CTL_REG     (* (reg8 *) bluetooth_COM_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)
    #define bluetooth_COM_TXDATA_AUX_CTL_PTR     (  (reg8 *) bluetooth_COM_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define bluetooth_COM_TXSTATUS_REG           (* (reg8 *) bluetooth_COM_BUART_sRX_RxSts__STATUS_REG )
    #define bluetooth_COM_TXSTATUS_PTR           (  (reg8 *) bluetooth_COM_BUART_sRX_RxSts__STATUS_REG )
    #define bluetooth_COM_TXSTATUS_MASK_REG      (* (reg8 *) bluetooth_COM_BUART_sRX_RxSts__MASK_REG )
    #define bluetooth_COM_TXSTATUS_MASK_PTR      (  (reg8 *) bluetooth_COM_BUART_sRX_RxSts__MASK_REG )
    #define bluetooth_COM_TXSTATUS_ACTL_REG      (* (reg8 *) bluetooth_COM_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define bluetooth_COM_TXSTATUS_ACTL_PTR      (  (reg8 *) bluetooth_COM_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End bluetooth_COM_HD_ENABLED */

#if( (bluetooth_COM_RX_ENABLED) || (bluetooth_COM_HD_ENABLED) )
    #define bluetooth_COM_RXDATA_REG             (* (reg8 *) bluetooth_COM_BUART_sRX_RxShifter_u0__F0_REG )
    #define bluetooth_COM_RXDATA_PTR             (  (reg8 *) bluetooth_COM_BUART_sRX_RxShifter_u0__F0_REG )
    #define bluetooth_COM_RXADDRESS1_REG         (* (reg8 *) bluetooth_COM_BUART_sRX_RxShifter_u0__D0_REG )
    #define bluetooth_COM_RXADDRESS1_PTR         (  (reg8 *) bluetooth_COM_BUART_sRX_RxShifter_u0__D0_REG )
    #define bluetooth_COM_RXADDRESS2_REG         (* (reg8 *) bluetooth_COM_BUART_sRX_RxShifter_u0__D1_REG )
    #define bluetooth_COM_RXADDRESS2_PTR         (  (reg8 *) bluetooth_COM_BUART_sRX_RxShifter_u0__D1_REG )
    #define bluetooth_COM_RXDATA_AUX_CTL_REG     (* (reg8 *) bluetooth_COM_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define bluetooth_COM_RXBITCTR_PERIOD_REG    (* (reg8 *) bluetooth_COM_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define bluetooth_COM_RXBITCTR_PERIOD_PTR    (  (reg8 *) bluetooth_COM_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define bluetooth_COM_RXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) bluetooth_COM_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define bluetooth_COM_RXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) bluetooth_COM_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define bluetooth_COM_RXBITCTR_COUNTER_REG   (* (reg8 *) bluetooth_COM_BUART_sRX_RxBitCounter__COUNT_REG )
    #define bluetooth_COM_RXBITCTR_COUNTER_PTR   (  (reg8 *) bluetooth_COM_BUART_sRX_RxBitCounter__COUNT_REG )

    #define bluetooth_COM_RXSTATUS_REG           (* (reg8 *) bluetooth_COM_BUART_sRX_RxSts__STATUS_REG )
    #define bluetooth_COM_RXSTATUS_PTR           (  (reg8 *) bluetooth_COM_BUART_sRX_RxSts__STATUS_REG )
    #define bluetooth_COM_RXSTATUS_MASK_REG      (* (reg8 *) bluetooth_COM_BUART_sRX_RxSts__MASK_REG )
    #define bluetooth_COM_RXSTATUS_MASK_PTR      (  (reg8 *) bluetooth_COM_BUART_sRX_RxSts__MASK_REG )
    #define bluetooth_COM_RXSTATUS_ACTL_REG      (* (reg8 *) bluetooth_COM_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define bluetooth_COM_RXSTATUS_ACTL_PTR      (  (reg8 *) bluetooth_COM_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End  (bluetooth_COM_RX_ENABLED) || (bluetooth_COM_HD_ENABLED) */

#if(bluetooth_COM_INTERNAL_CLOCK_USED)
    /* Register to enable or disable the digital clocks */
    #define bluetooth_COM_INTCLOCK_CLKEN_REG     (* (reg8 *) bluetooth_COM_IntClock__PM_ACT_CFG)
    #define bluetooth_COM_INTCLOCK_CLKEN_PTR     (  (reg8 *) bluetooth_COM_IntClock__PM_ACT_CFG)

    /* Clock mask for this clock. */
    #define bluetooth_COM_INTCLOCK_CLKEN_MASK    bluetooth_COM_IntClock__PM_ACT_MSK
#endif /* End bluetooth_COM_INTERNAL_CLOCK_USED */


/***************************************
*       Register Constants
***************************************/

#if(bluetooth_COM_TX_ENABLED)
    #define bluetooth_COM_TX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End bluetooth_COM_TX_ENABLED */

#if(bluetooth_COM_HD_ENABLED)
    #define bluetooth_COM_TX_FIFO_CLR            (0x02u) /* FIFO1 CLR */
#endif /* End bluetooth_COM_HD_ENABLED */

#if( (bluetooth_COM_RX_ENABLED) || (bluetooth_COM_HD_ENABLED) )
    #define bluetooth_COM_RX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End  (bluetooth_COM_RX_ENABLED) || (bluetooth_COM_HD_ENABLED) */


/***************************************
* The following code is DEPRECATED and
* should not be used in new projects.
***************************************/

/* UART v2_40 obsolete definitions */
#define bluetooth_COM_WAIT_1_MS      bluetooth_COM_BL_CHK_DELAY_MS   

#define bluetooth_COM_TXBUFFERSIZE   bluetooth_COM_TX_BUFFER_SIZE
#define bluetooth_COM_RXBUFFERSIZE   bluetooth_COM_RX_BUFFER_SIZE

#if (bluetooth_COM_RXHW_ADDRESS_ENABLED)
    #define bluetooth_COM_RXADDRESSMODE  bluetooth_COM_RX_ADDRESS_MODE
    #define bluetooth_COM_RXHWADDRESS1   bluetooth_COM_RX_HW_ADDRESS1
    #define bluetooth_COM_RXHWADDRESS2   bluetooth_COM_RX_HW_ADDRESS2
    /* Backward compatible define */
    #define bluetooth_COM_RXAddressMode  bluetooth_COM_RXADDRESSMODE
#endif /* (bluetooth_COM_RXHW_ADDRESS_ENABLED) */

/* UART v2_30 obsolete definitions */
#define bluetooth_COM_initvar                    bluetooth_COM_initVar

#define bluetooth_COM_RX_Enabled                 bluetooth_COM_RX_ENABLED
#define bluetooth_COM_TX_Enabled                 bluetooth_COM_TX_ENABLED
#define bluetooth_COM_HD_Enabled                 bluetooth_COM_HD_ENABLED
#define bluetooth_COM_RX_IntInterruptEnabled     bluetooth_COM_RX_INTERRUPT_ENABLED
#define bluetooth_COM_TX_IntInterruptEnabled     bluetooth_COM_TX_INTERRUPT_ENABLED
#define bluetooth_COM_InternalClockUsed          bluetooth_COM_INTERNAL_CLOCK_USED
#define bluetooth_COM_RXHW_Address_Enabled       bluetooth_COM_RXHW_ADDRESS_ENABLED
#define bluetooth_COM_OverSampleCount            bluetooth_COM_OVER_SAMPLE_COUNT
#define bluetooth_COM_ParityType                 bluetooth_COM_PARITY_TYPE

#if( bluetooth_COM_TX_ENABLED && (bluetooth_COM_TXBUFFERSIZE > bluetooth_COM_FIFO_LENGTH))
    #define bluetooth_COM_TXBUFFER               bluetooth_COM_txBuffer
    #define bluetooth_COM_TXBUFFERREAD           bluetooth_COM_txBufferRead
    #define bluetooth_COM_TXBUFFERWRITE          bluetooth_COM_txBufferWrite
#endif /* End bluetooth_COM_TX_ENABLED */
#if( ( bluetooth_COM_RX_ENABLED || bluetooth_COM_HD_ENABLED ) && \
     (bluetooth_COM_RXBUFFERSIZE > bluetooth_COM_FIFO_LENGTH) )
    #define bluetooth_COM_RXBUFFER               bluetooth_COM_rxBuffer
    #define bluetooth_COM_RXBUFFERREAD           bluetooth_COM_rxBufferRead
    #define bluetooth_COM_RXBUFFERWRITE          bluetooth_COM_rxBufferWrite
    #define bluetooth_COM_RXBUFFERLOOPDETECT     bluetooth_COM_rxBufferLoopDetect
    #define bluetooth_COM_RXBUFFER_OVERFLOW      bluetooth_COM_rxBufferOverflow
#endif /* End bluetooth_COM_RX_ENABLED */

#ifdef bluetooth_COM_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define bluetooth_COM_CONTROL                bluetooth_COM_CONTROL_REG
#endif /* End bluetooth_COM_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(bluetooth_COM_TX_ENABLED)
    #define bluetooth_COM_TXDATA                 bluetooth_COM_TXDATA_REG
    #define bluetooth_COM_TXSTATUS               bluetooth_COM_TXSTATUS_REG
    #define bluetooth_COM_TXSTATUS_MASK          bluetooth_COM_TXSTATUS_MASK_REG
    #define bluetooth_COM_TXSTATUS_ACTL          bluetooth_COM_TXSTATUS_ACTL_REG
    /* DP clock */
    #if(bluetooth_COM_TXCLKGEN_DP)
        #define bluetooth_COM_TXBITCLKGEN_CTR        bluetooth_COM_TXBITCLKGEN_CTR_REG
        #define bluetooth_COM_TXBITCLKTX_COMPLETE    bluetooth_COM_TXBITCLKTX_COMPLETE_REG
    #else     /* Count7 clock*/
        #define bluetooth_COM_TXBITCTR_PERIOD        bluetooth_COM_TXBITCTR_PERIOD_REG
        #define bluetooth_COM_TXBITCTR_CONTROL       bluetooth_COM_TXBITCTR_CONTROL_REG
        #define bluetooth_COM_TXBITCTR_COUNTER       bluetooth_COM_TXBITCTR_COUNTER_REG
    #endif /* bluetooth_COM_TXCLKGEN_DP */
#endif /* End bluetooth_COM_TX_ENABLED */

#if(bluetooth_COM_HD_ENABLED)
    #define bluetooth_COM_TXDATA                 bluetooth_COM_TXDATA_REG
    #define bluetooth_COM_TXSTATUS               bluetooth_COM_TXSTATUS_REG
    #define bluetooth_COM_TXSTATUS_MASK          bluetooth_COM_TXSTATUS_MASK_REG
    #define bluetooth_COM_TXSTATUS_ACTL          bluetooth_COM_TXSTATUS_ACTL_REG
#endif /* End bluetooth_COM_HD_ENABLED */

#if( (bluetooth_COM_RX_ENABLED) || (bluetooth_COM_HD_ENABLED) )
    #define bluetooth_COM_RXDATA                 bluetooth_COM_RXDATA_REG
    #define bluetooth_COM_RXADDRESS1             bluetooth_COM_RXADDRESS1_REG
    #define bluetooth_COM_RXADDRESS2             bluetooth_COM_RXADDRESS2_REG
    #define bluetooth_COM_RXBITCTR_PERIOD        bluetooth_COM_RXBITCTR_PERIOD_REG
    #define bluetooth_COM_RXBITCTR_CONTROL       bluetooth_COM_RXBITCTR_CONTROL_REG
    #define bluetooth_COM_RXBITCTR_COUNTER       bluetooth_COM_RXBITCTR_COUNTER_REG
    #define bluetooth_COM_RXSTATUS               bluetooth_COM_RXSTATUS_REG
    #define bluetooth_COM_RXSTATUS_MASK          bluetooth_COM_RXSTATUS_MASK_REG
    #define bluetooth_COM_RXSTATUS_ACTL          bluetooth_COM_RXSTATUS_ACTL_REG
#endif /* End  (bluetooth_COM_RX_ENABLED) || (bluetooth_COM_HD_ENABLED) */

#if(bluetooth_COM_INTERNAL_CLOCK_USED)
    #define bluetooth_COM_INTCLOCK_CLKEN         bluetooth_COM_INTCLOCK_CLKEN_REG
#endif /* End bluetooth_COM_INTERNAL_CLOCK_USED */

#define bluetooth_COM_WAIT_FOR_COMLETE_REINIT    bluetooth_COM_WAIT_FOR_COMPLETE_REINIT

#endif  /* CY_UART_bluetooth_COM_H */


/* [] END OF FILE */
