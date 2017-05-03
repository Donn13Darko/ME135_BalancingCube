/*******************************************************************************
* File Name: blueART.h
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


#if !defined(CY_UART_blueART_H)
#define CY_UART_blueART_H

#include "cytypes.h"
#include "cyfitter.h"
#include "CyLib.h"


/***************************************
* Conditional Compilation Parameters
***************************************/

#define blueART_RX_ENABLED                     (1u)
#define blueART_TX_ENABLED                     (1u)
#define blueART_HD_ENABLED                     (0u)
#define blueART_RX_INTERRUPT_ENABLED           (0u)
#define blueART_TX_INTERRUPT_ENABLED           (0u)
#define blueART_INTERNAL_CLOCK_USED            (1u)
#define blueART_RXHW_ADDRESS_ENABLED           (0u)
#define blueART_OVER_SAMPLE_COUNT              (8u)
#define blueART_PARITY_TYPE                    (0u)
#define blueART_PARITY_TYPE_SW                 (0u)
#define blueART_BREAK_DETECT                   (0u)
#define blueART_BREAK_BITS_TX                  (13u)
#define blueART_BREAK_BITS_RX                  (13u)
#define blueART_TXCLKGEN_DP                    (1u)
#define blueART_USE23POLLING                   (1u)
#define blueART_FLOW_CONTROL                   (0u)
#define blueART_CLK_FREQ                       (0u)
#define blueART_TX_BUFFER_SIZE                 (4u)
#define blueART_RX_BUFFER_SIZE                 (4u)

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component UART_v2_50 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */

#if defined(blueART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG)
    #define blueART_CONTROL_REG_REMOVED            (0u)
#else
    #define blueART_CONTROL_REG_REMOVED            (1u)
#endif /* End blueART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */


/***************************************
*      Data Structure Definition
***************************************/

/* Sleep Mode API Support */
typedef struct blueART_backupStruct_
{
    uint8 enableState;

    #if(blueART_CONTROL_REG_REMOVED == 0u)
        uint8 cr;
    #endif /* End blueART_CONTROL_REG_REMOVED */

} blueART_BACKUP_STRUCT;


/***************************************
*       Function Prototypes
***************************************/

void blueART_Start(void) ;
void blueART_Stop(void) ;
uint8 blueART_ReadControlRegister(void) ;
void blueART_WriteControlRegister(uint8 control) ;

void blueART_Init(void) ;
void blueART_Enable(void) ;
void blueART_SaveConfig(void) ;
void blueART_RestoreConfig(void) ;
void blueART_Sleep(void) ;
void blueART_Wakeup(void) ;

/* Only if RX is enabled */
#if( (blueART_RX_ENABLED) || (blueART_HD_ENABLED) )

    #if (blueART_RX_INTERRUPT_ENABLED)
        #define blueART_EnableRxInt()  CyIntEnable (blueART_RX_VECT_NUM)
        #define blueART_DisableRxInt() CyIntDisable(blueART_RX_VECT_NUM)
        CY_ISR_PROTO(blueART_RXISR);
    #endif /* blueART_RX_INTERRUPT_ENABLED */

    void blueART_SetRxAddressMode(uint8 addressMode)
                                                           ;
    void blueART_SetRxAddress1(uint8 address) ;
    void blueART_SetRxAddress2(uint8 address) ;

    void  blueART_SetRxInterruptMode(uint8 intSrc) ;
    uint8 blueART_ReadRxData(void) ;
    uint8 blueART_ReadRxStatus(void) ;
    uint8 blueART_GetChar(void) ;
    uint16 blueART_GetByte(void) ;
    uint8 blueART_GetRxBufferSize(void)
                                                            ;
    void blueART_ClearRxBuffer(void) ;

    /* Obsolete functions, defines for backward compatible */
    #define blueART_GetRxInterruptSource   blueART_ReadRxStatus

#endif /* End (blueART_RX_ENABLED) || (blueART_HD_ENABLED) */

/* Only if TX is enabled */
#if(blueART_TX_ENABLED || blueART_HD_ENABLED)

    #if(blueART_TX_INTERRUPT_ENABLED)
        #define blueART_EnableTxInt()  CyIntEnable (blueART_TX_VECT_NUM)
        #define blueART_DisableTxInt() CyIntDisable(blueART_TX_VECT_NUM)
        #define blueART_SetPendingTxInt() CyIntSetPending(blueART_TX_VECT_NUM)
        #define blueART_ClearPendingTxInt() CyIntClearPending(blueART_TX_VECT_NUM)
        CY_ISR_PROTO(blueART_TXISR);
    #endif /* blueART_TX_INTERRUPT_ENABLED */

    void blueART_SetTxInterruptMode(uint8 intSrc) ;
    void blueART_WriteTxData(uint8 txDataByte) ;
    uint8 blueART_ReadTxStatus(void) ;
    void blueART_PutChar(uint8 txDataByte) ;
    void blueART_PutString(const char8 string[]) ;
    void blueART_PutArray(const uint8 string[], uint8 byteCount)
                                                            ;
    void blueART_PutCRLF(uint8 txDataByte) ;
    void blueART_ClearTxBuffer(void) ;
    void blueART_SetTxAddressMode(uint8 addressMode) ;
    void blueART_SendBreak(uint8 retMode) ;
    uint8 blueART_GetTxBufferSize(void)
                                                            ;
    /* Obsolete functions, defines for backward compatible */
    #define blueART_PutStringConst         blueART_PutString
    #define blueART_PutArrayConst          blueART_PutArray
    #define blueART_GetTxInterruptSource   blueART_ReadTxStatus

#endif /* End blueART_TX_ENABLED || blueART_HD_ENABLED */

#if(blueART_HD_ENABLED)
    void blueART_LoadRxConfig(void) ;
    void blueART_LoadTxConfig(void) ;
#endif /* End blueART_HD_ENABLED */


/* Communication bootloader APIs */
#if defined(CYDEV_BOOTLOADER_IO_COMP) && ((CYDEV_BOOTLOADER_IO_COMP == CyBtldr_blueART) || \
                                          (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_Custom_Interface))
    /* Physical layer functions */
    void    blueART_CyBtldrCommStart(void) CYSMALL ;
    void    blueART_CyBtldrCommStop(void) CYSMALL ;
    void    blueART_CyBtldrCommReset(void) CYSMALL ;
    cystatus blueART_CyBtldrCommWrite(const uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;
    cystatus blueART_CyBtldrCommRead(uint8 pData[], uint16 size, uint16 * count, uint8 timeOut) CYSMALL
             ;

    #if (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_blueART)
        #define CyBtldrCommStart    blueART_CyBtldrCommStart
        #define CyBtldrCommStop     blueART_CyBtldrCommStop
        #define CyBtldrCommReset    blueART_CyBtldrCommReset
        #define CyBtldrCommWrite    blueART_CyBtldrCommWrite
        #define CyBtldrCommRead     blueART_CyBtldrCommRead
    #endif  /* (CYDEV_BOOTLOADER_IO_COMP == CyBtldr_blueART) */

    /* Byte to Byte time out for detecting end of block data from host */
    #define blueART_BYTE2BYTE_TIME_OUT (25u)
    #define blueART_PACKET_EOP         (0x17u) /* End of packet defined by bootloader */
    #define blueART_WAIT_EOP_DELAY     (5u)    /* Additional 5ms to wait for End of packet */
    #define blueART_BL_CHK_DELAY_MS    (1u)    /* Time Out quantity equal 1mS */

#endif /* CYDEV_BOOTLOADER_IO_COMP */


/***************************************
*          API Constants
***************************************/
/* Parameters for SetTxAddressMode API*/
#define blueART_SET_SPACE      (0x00u)
#define blueART_SET_MARK       (0x01u)

/* Status Register definitions */
#if( (blueART_TX_ENABLED) || (blueART_HD_ENABLED) )
    #if(blueART_TX_INTERRUPT_ENABLED)
        #define blueART_TX_VECT_NUM            (uint8)blueART_TXInternalInterrupt__INTC_NUMBER
        #define blueART_TX_PRIOR_NUM           (uint8)blueART_TXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* blueART_TX_INTERRUPT_ENABLED */

    #define blueART_TX_STS_COMPLETE_SHIFT          (0x00u)
    #define blueART_TX_STS_FIFO_EMPTY_SHIFT        (0x01u)
    #define blueART_TX_STS_FIFO_NOT_FULL_SHIFT     (0x03u)
    #if(blueART_TX_ENABLED)
        #define blueART_TX_STS_FIFO_FULL_SHIFT     (0x02u)
    #else /* (blueART_HD_ENABLED) */
        #define blueART_TX_STS_FIFO_FULL_SHIFT     (0x05u)  /* Needs MD=0 */
    #endif /* (blueART_TX_ENABLED) */

    #define blueART_TX_STS_COMPLETE            (uint8)(0x01u << blueART_TX_STS_COMPLETE_SHIFT)
    #define blueART_TX_STS_FIFO_EMPTY          (uint8)(0x01u << blueART_TX_STS_FIFO_EMPTY_SHIFT)
    #define blueART_TX_STS_FIFO_FULL           (uint8)(0x01u << blueART_TX_STS_FIFO_FULL_SHIFT)
    #define blueART_TX_STS_FIFO_NOT_FULL       (uint8)(0x01u << blueART_TX_STS_FIFO_NOT_FULL_SHIFT)
#endif /* End (blueART_TX_ENABLED) || (blueART_HD_ENABLED)*/

#if( (blueART_RX_ENABLED) || (blueART_HD_ENABLED) )
    #if(blueART_RX_INTERRUPT_ENABLED)
        #define blueART_RX_VECT_NUM            (uint8)blueART_RXInternalInterrupt__INTC_NUMBER
        #define blueART_RX_PRIOR_NUM           (uint8)blueART_RXInternalInterrupt__INTC_PRIOR_NUM
    #endif /* blueART_RX_INTERRUPT_ENABLED */
    #define blueART_RX_STS_MRKSPC_SHIFT            (0x00u)
    #define blueART_RX_STS_BREAK_SHIFT             (0x01u)
    #define blueART_RX_STS_PAR_ERROR_SHIFT         (0x02u)
    #define blueART_RX_STS_STOP_ERROR_SHIFT        (0x03u)
    #define blueART_RX_STS_OVERRUN_SHIFT           (0x04u)
    #define blueART_RX_STS_FIFO_NOTEMPTY_SHIFT     (0x05u)
    #define blueART_RX_STS_ADDR_MATCH_SHIFT        (0x06u)
    #define blueART_RX_STS_SOFT_BUFF_OVER_SHIFT    (0x07u)

    #define blueART_RX_STS_MRKSPC           (uint8)(0x01u << blueART_RX_STS_MRKSPC_SHIFT)
    #define blueART_RX_STS_BREAK            (uint8)(0x01u << blueART_RX_STS_BREAK_SHIFT)
    #define blueART_RX_STS_PAR_ERROR        (uint8)(0x01u << blueART_RX_STS_PAR_ERROR_SHIFT)
    #define blueART_RX_STS_STOP_ERROR       (uint8)(0x01u << blueART_RX_STS_STOP_ERROR_SHIFT)
    #define blueART_RX_STS_OVERRUN          (uint8)(0x01u << blueART_RX_STS_OVERRUN_SHIFT)
    #define blueART_RX_STS_FIFO_NOTEMPTY    (uint8)(0x01u << blueART_RX_STS_FIFO_NOTEMPTY_SHIFT)
    #define blueART_RX_STS_ADDR_MATCH       (uint8)(0x01u << blueART_RX_STS_ADDR_MATCH_SHIFT)
    #define blueART_RX_STS_SOFT_BUFF_OVER   (uint8)(0x01u << blueART_RX_STS_SOFT_BUFF_OVER_SHIFT)
    #define blueART_RX_HW_MASK                     (0x7Fu)
#endif /* End (blueART_RX_ENABLED) || (blueART_HD_ENABLED) */

/* Control Register definitions */
#define blueART_CTRL_HD_SEND_SHIFT                 (0x00u) /* 1 enable TX part in Half Duplex mode */
#define blueART_CTRL_HD_SEND_BREAK_SHIFT           (0x01u) /* 1 send BREAK signal in Half Duplez mode */
#define blueART_CTRL_MARK_SHIFT                    (0x02u) /* 1 sets mark, 0 sets space */
#define blueART_CTRL_PARITY_TYPE0_SHIFT            (0x03u) /* Defines the type of parity implemented */
#define blueART_CTRL_PARITY_TYPE1_SHIFT            (0x04u) /* Defines the type of parity implemented */
#define blueART_CTRL_RXADDR_MODE0_SHIFT            (0x05u)
#define blueART_CTRL_RXADDR_MODE1_SHIFT            (0x06u)
#define blueART_CTRL_RXADDR_MODE2_SHIFT            (0x07u)

#define blueART_CTRL_HD_SEND               (uint8)(0x01u << blueART_CTRL_HD_SEND_SHIFT)
#define blueART_CTRL_HD_SEND_BREAK         (uint8)(0x01u << blueART_CTRL_HD_SEND_BREAK_SHIFT)
#define blueART_CTRL_MARK                  (uint8)(0x01u << blueART_CTRL_MARK_SHIFT)
#define blueART_CTRL_PARITY_TYPE_MASK      (uint8)(0x03u << blueART_CTRL_PARITY_TYPE0_SHIFT)
#define blueART_CTRL_RXADDR_MODE_MASK      (uint8)(0x07u << blueART_CTRL_RXADDR_MODE0_SHIFT)

/* StatusI Register Interrupt Enable Control Bits. As defined by the Register map for the AUX Control Register */
#define blueART_INT_ENABLE                         (0x10u)

/* Bit Counter (7-bit) Control Register Bit Definitions. As defined by the Register map for the AUX Control Register */
#define blueART_CNTR_ENABLE                        (0x20u)

/*   Constants for SendBreak() "retMode" parameter  */
#define blueART_SEND_BREAK                         (0x00u)
#define blueART_WAIT_FOR_COMPLETE_REINIT           (0x01u)
#define blueART_REINIT                             (0x02u)
#define blueART_SEND_WAIT_REINIT                   (0x03u)

#define blueART_OVER_SAMPLE_8                      (8u)
#define blueART_OVER_SAMPLE_16                     (16u)

#define blueART_BIT_CENTER                         (blueART_OVER_SAMPLE_COUNT - 2u)

#define blueART_FIFO_LENGTH                        (4u)
#define blueART_NUMBER_OF_START_BIT                (1u)
#define blueART_MAX_BYTE_VALUE                     (0xFFu)

/* 8X always for count7 implementation */
#define blueART_TXBITCTR_BREAKBITS8X   ((blueART_BREAK_BITS_TX * blueART_OVER_SAMPLE_8) - 1u)
/* 8X or 16X for DP implementation */
#define blueART_TXBITCTR_BREAKBITS ((blueART_BREAK_BITS_TX * blueART_OVER_SAMPLE_COUNT) - 1u)

#define blueART_HALF_BIT_COUNT   \
                            (((blueART_OVER_SAMPLE_COUNT / 2u) + (blueART_USE23POLLING * 1u)) - 2u)
#if (blueART_OVER_SAMPLE_COUNT == blueART_OVER_SAMPLE_8)
    #define blueART_HD_TXBITCTR_INIT   (((blueART_BREAK_BITS_TX + \
                            blueART_NUMBER_OF_START_BIT) * blueART_OVER_SAMPLE_COUNT) - 1u)

    /* This parameter is increased on the 2 in 2 out of 3 mode to sample voting in the middle */
    #define blueART_RXBITCTR_INIT  ((((blueART_BREAK_BITS_RX + blueART_NUMBER_OF_START_BIT) \
                            * blueART_OVER_SAMPLE_COUNT) + blueART_HALF_BIT_COUNT) - 1u)

#else /* blueART_OVER_SAMPLE_COUNT == blueART_OVER_SAMPLE_16 */
    #define blueART_HD_TXBITCTR_INIT   ((8u * blueART_OVER_SAMPLE_COUNT) - 1u)
    /* 7bit counter need one more bit for OverSampleCount = 16 */
    #define blueART_RXBITCTR_INIT      (((7u * blueART_OVER_SAMPLE_COUNT) - 1u) + \
                                                      blueART_HALF_BIT_COUNT)
#endif /* End blueART_OVER_SAMPLE_COUNT */

#define blueART_HD_RXBITCTR_INIT                   blueART_RXBITCTR_INIT


/***************************************
* Global variables external identifier
***************************************/

extern uint8 blueART_initVar;
#if (blueART_TX_INTERRUPT_ENABLED && blueART_TX_ENABLED)
    extern volatile uint8 blueART_txBuffer[blueART_TX_BUFFER_SIZE];
    extern volatile uint8 blueART_txBufferRead;
    extern uint8 blueART_txBufferWrite;
#endif /* (blueART_TX_INTERRUPT_ENABLED && blueART_TX_ENABLED) */
#if (blueART_RX_INTERRUPT_ENABLED && (blueART_RX_ENABLED || blueART_HD_ENABLED))
    extern uint8 blueART_errorStatus;
    extern volatile uint8 blueART_rxBuffer[blueART_RX_BUFFER_SIZE];
    extern volatile uint8 blueART_rxBufferRead;
    extern volatile uint8 blueART_rxBufferWrite;
    extern volatile uint8 blueART_rxBufferLoopDetect;
    extern volatile uint8 blueART_rxBufferOverflow;
    #if (blueART_RXHW_ADDRESS_ENABLED)
        extern volatile uint8 blueART_rxAddressMode;
        extern volatile uint8 blueART_rxAddressDetected;
    #endif /* (blueART_RXHW_ADDRESS_ENABLED) */
#endif /* (blueART_RX_INTERRUPT_ENABLED && (blueART_RX_ENABLED || blueART_HD_ENABLED)) */


/***************************************
* Enumerated Types and Parameters
***************************************/

#define blueART__B_UART__AM_SW_BYTE_BYTE 1
#define blueART__B_UART__AM_SW_DETECT_TO_BUFFER 2
#define blueART__B_UART__AM_HW_BYTE_BY_BYTE 3
#define blueART__B_UART__AM_HW_DETECT_TO_BUFFER 4
#define blueART__B_UART__AM_NONE 0

#define blueART__B_UART__NONE_REVB 0
#define blueART__B_UART__EVEN_REVB 1
#define blueART__B_UART__ODD_REVB 2
#define blueART__B_UART__MARK_SPACE_REVB 3



/***************************************
*    Initial Parameter Constants
***************************************/

/* UART shifts max 8 bits, Mark/Space functionality working if 9 selected */
#define blueART_NUMBER_OF_DATA_BITS    ((8u > 8u) ? 8u : 8u)
#define blueART_NUMBER_OF_STOP_BITS    (1u)

#if (blueART_RXHW_ADDRESS_ENABLED)
    #define blueART_RX_ADDRESS_MODE    (0u)
    #define blueART_RX_HW_ADDRESS1     (0u)
    #define blueART_RX_HW_ADDRESS2     (0u)
#endif /* (blueART_RXHW_ADDRESS_ENABLED) */

#define blueART_INIT_RX_INTERRUPTS_MASK \
                                  (uint8)((1 << blueART_RX_STS_FIFO_NOTEMPTY_SHIFT) \
                                        | (0 << blueART_RX_STS_MRKSPC_SHIFT) \
                                        | (0 << blueART_RX_STS_ADDR_MATCH_SHIFT) \
                                        | (0 << blueART_RX_STS_PAR_ERROR_SHIFT) \
                                        | (0 << blueART_RX_STS_STOP_ERROR_SHIFT) \
                                        | (0 << blueART_RX_STS_BREAK_SHIFT) \
                                        | (0 << blueART_RX_STS_OVERRUN_SHIFT))

#define blueART_INIT_TX_INTERRUPTS_MASK \
                                  (uint8)((0 << blueART_TX_STS_COMPLETE_SHIFT) \
                                        | (0 << blueART_TX_STS_FIFO_EMPTY_SHIFT) \
                                        | (0 << blueART_TX_STS_FIFO_FULL_SHIFT) \
                                        | (0 << blueART_TX_STS_FIFO_NOT_FULL_SHIFT))


/***************************************
*              Registers
***************************************/

#ifdef blueART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define blueART_CONTROL_REG \
                            (* (reg8 *) blueART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
    #define blueART_CONTROL_PTR \
                            (  (reg8 *) blueART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG )
#endif /* End blueART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(blueART_TX_ENABLED)
    #define blueART_TXDATA_REG          (* (reg8 *) blueART_BUART_sTX_TxShifter_u0__F0_REG)
    #define blueART_TXDATA_PTR          (  (reg8 *) blueART_BUART_sTX_TxShifter_u0__F0_REG)
    #define blueART_TXDATA_AUX_CTL_REG  (* (reg8 *) blueART_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define blueART_TXDATA_AUX_CTL_PTR  (  (reg8 *) blueART_BUART_sTX_TxShifter_u0__DP_AUX_CTL_REG)
    #define blueART_TXSTATUS_REG        (* (reg8 *) blueART_BUART_sTX_TxSts__STATUS_REG)
    #define blueART_TXSTATUS_PTR        (  (reg8 *) blueART_BUART_sTX_TxSts__STATUS_REG)
    #define blueART_TXSTATUS_MASK_REG   (* (reg8 *) blueART_BUART_sTX_TxSts__MASK_REG)
    #define blueART_TXSTATUS_MASK_PTR   (  (reg8 *) blueART_BUART_sTX_TxSts__MASK_REG)
    #define blueART_TXSTATUS_ACTL_REG   (* (reg8 *) blueART_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)
    #define blueART_TXSTATUS_ACTL_PTR   (  (reg8 *) blueART_BUART_sTX_TxSts__STATUS_AUX_CTL_REG)

    /* DP clock */
    #if(blueART_TXCLKGEN_DP)
        #define blueART_TXBITCLKGEN_CTR_REG        \
                                        (* (reg8 *) blueART_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define blueART_TXBITCLKGEN_CTR_PTR        \
                                        (  (reg8 *) blueART_BUART_sTX_sCLOCK_TxBitClkGen__D0_REG)
        #define blueART_TXBITCLKTX_COMPLETE_REG    \
                                        (* (reg8 *) blueART_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
        #define blueART_TXBITCLKTX_COMPLETE_PTR    \
                                        (  (reg8 *) blueART_BUART_sTX_sCLOCK_TxBitClkGen__D1_REG)
    #else     /* Count7 clock*/
        #define blueART_TXBITCTR_PERIOD_REG    \
                                        (* (reg8 *) blueART_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define blueART_TXBITCTR_PERIOD_PTR    \
                                        (  (reg8 *) blueART_BUART_sTX_sCLOCK_TxBitCounter__PERIOD_REG)
        #define blueART_TXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) blueART_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define blueART_TXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) blueART_BUART_sTX_sCLOCK_TxBitCounter__CONTROL_AUX_CTL_REG)
        #define blueART_TXBITCTR_COUNTER_REG   \
                                        (* (reg8 *) blueART_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
        #define blueART_TXBITCTR_COUNTER_PTR   \
                                        (  (reg8 *) blueART_BUART_sTX_sCLOCK_TxBitCounter__COUNT_REG)
    #endif /* blueART_TXCLKGEN_DP */

#endif /* End blueART_TX_ENABLED */

#if(blueART_HD_ENABLED)

    #define blueART_TXDATA_REG             (* (reg8 *) blueART_BUART_sRX_RxShifter_u0__F1_REG )
    #define blueART_TXDATA_PTR             (  (reg8 *) blueART_BUART_sRX_RxShifter_u0__F1_REG )
    #define blueART_TXDATA_AUX_CTL_REG     (* (reg8 *) blueART_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)
    #define blueART_TXDATA_AUX_CTL_PTR     (  (reg8 *) blueART_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define blueART_TXSTATUS_REG           (* (reg8 *) blueART_BUART_sRX_RxSts__STATUS_REG )
    #define blueART_TXSTATUS_PTR           (  (reg8 *) blueART_BUART_sRX_RxSts__STATUS_REG )
    #define blueART_TXSTATUS_MASK_REG      (* (reg8 *) blueART_BUART_sRX_RxSts__MASK_REG )
    #define blueART_TXSTATUS_MASK_PTR      (  (reg8 *) blueART_BUART_sRX_RxSts__MASK_REG )
    #define blueART_TXSTATUS_ACTL_REG      (* (reg8 *) blueART_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define blueART_TXSTATUS_ACTL_PTR      (  (reg8 *) blueART_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End blueART_HD_ENABLED */

#if( (blueART_RX_ENABLED) || (blueART_HD_ENABLED) )
    #define blueART_RXDATA_REG             (* (reg8 *) blueART_BUART_sRX_RxShifter_u0__F0_REG )
    #define blueART_RXDATA_PTR             (  (reg8 *) blueART_BUART_sRX_RxShifter_u0__F0_REG )
    #define blueART_RXADDRESS1_REG         (* (reg8 *) blueART_BUART_sRX_RxShifter_u0__D0_REG )
    #define blueART_RXADDRESS1_PTR         (  (reg8 *) blueART_BUART_sRX_RxShifter_u0__D0_REG )
    #define blueART_RXADDRESS2_REG         (* (reg8 *) blueART_BUART_sRX_RxShifter_u0__D1_REG )
    #define blueART_RXADDRESS2_PTR         (  (reg8 *) blueART_BUART_sRX_RxShifter_u0__D1_REG )
    #define blueART_RXDATA_AUX_CTL_REG     (* (reg8 *) blueART_BUART_sRX_RxShifter_u0__DP_AUX_CTL_REG)

    #define blueART_RXBITCTR_PERIOD_REG    (* (reg8 *) blueART_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define blueART_RXBITCTR_PERIOD_PTR    (  (reg8 *) blueART_BUART_sRX_RxBitCounter__PERIOD_REG )
    #define blueART_RXBITCTR_CONTROL_REG   \
                                        (* (reg8 *) blueART_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define blueART_RXBITCTR_CONTROL_PTR   \
                                        (  (reg8 *) blueART_BUART_sRX_RxBitCounter__CONTROL_AUX_CTL_REG )
    #define blueART_RXBITCTR_COUNTER_REG   (* (reg8 *) blueART_BUART_sRX_RxBitCounter__COUNT_REG )
    #define blueART_RXBITCTR_COUNTER_PTR   (  (reg8 *) blueART_BUART_sRX_RxBitCounter__COUNT_REG )

    #define blueART_RXSTATUS_REG           (* (reg8 *) blueART_BUART_sRX_RxSts__STATUS_REG )
    #define blueART_RXSTATUS_PTR           (  (reg8 *) blueART_BUART_sRX_RxSts__STATUS_REG )
    #define blueART_RXSTATUS_MASK_REG      (* (reg8 *) blueART_BUART_sRX_RxSts__MASK_REG )
    #define blueART_RXSTATUS_MASK_PTR      (  (reg8 *) blueART_BUART_sRX_RxSts__MASK_REG )
    #define blueART_RXSTATUS_ACTL_REG      (* (reg8 *) blueART_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
    #define blueART_RXSTATUS_ACTL_PTR      (  (reg8 *) blueART_BUART_sRX_RxSts__STATUS_AUX_CTL_REG )
#endif /* End  (blueART_RX_ENABLED) || (blueART_HD_ENABLED) */

#if(blueART_INTERNAL_CLOCK_USED)
    /* Register to enable or disable the digital clocks */
    #define blueART_INTCLOCK_CLKEN_REG     (* (reg8 *) blueART_IntClock__PM_ACT_CFG)
    #define blueART_INTCLOCK_CLKEN_PTR     (  (reg8 *) blueART_IntClock__PM_ACT_CFG)

    /* Clock mask for this clock. */
    #define blueART_INTCLOCK_CLKEN_MASK    blueART_IntClock__PM_ACT_MSK
#endif /* End blueART_INTERNAL_CLOCK_USED */


/***************************************
*       Register Constants
***************************************/

#if(blueART_TX_ENABLED)
    #define blueART_TX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End blueART_TX_ENABLED */

#if(blueART_HD_ENABLED)
    #define blueART_TX_FIFO_CLR            (0x02u) /* FIFO1 CLR */
#endif /* End blueART_HD_ENABLED */

#if( (blueART_RX_ENABLED) || (blueART_HD_ENABLED) )
    #define blueART_RX_FIFO_CLR            (0x01u) /* FIFO0 CLR */
#endif /* End  (blueART_RX_ENABLED) || (blueART_HD_ENABLED) */


/***************************************
* The following code is DEPRECATED and
* should not be used in new projects.
***************************************/

/* UART v2_40 obsolete definitions */
#define blueART_WAIT_1_MS      blueART_BL_CHK_DELAY_MS   

#define blueART_TXBUFFERSIZE   blueART_TX_BUFFER_SIZE
#define blueART_RXBUFFERSIZE   blueART_RX_BUFFER_SIZE

#if (blueART_RXHW_ADDRESS_ENABLED)
    #define blueART_RXADDRESSMODE  blueART_RX_ADDRESS_MODE
    #define blueART_RXHWADDRESS1   blueART_RX_HW_ADDRESS1
    #define blueART_RXHWADDRESS2   blueART_RX_HW_ADDRESS2
    /* Backward compatible define */
    #define blueART_RXAddressMode  blueART_RXADDRESSMODE
#endif /* (blueART_RXHW_ADDRESS_ENABLED) */

/* UART v2_30 obsolete definitions */
#define blueART_initvar                    blueART_initVar

#define blueART_RX_Enabled                 blueART_RX_ENABLED
#define blueART_TX_Enabled                 blueART_TX_ENABLED
#define blueART_HD_Enabled                 blueART_HD_ENABLED
#define blueART_RX_IntInterruptEnabled     blueART_RX_INTERRUPT_ENABLED
#define blueART_TX_IntInterruptEnabled     blueART_TX_INTERRUPT_ENABLED
#define blueART_InternalClockUsed          blueART_INTERNAL_CLOCK_USED
#define blueART_RXHW_Address_Enabled       blueART_RXHW_ADDRESS_ENABLED
#define blueART_OverSampleCount            blueART_OVER_SAMPLE_COUNT
#define blueART_ParityType                 blueART_PARITY_TYPE

#if( blueART_TX_ENABLED && (blueART_TXBUFFERSIZE > blueART_FIFO_LENGTH))
    #define blueART_TXBUFFER               blueART_txBuffer
    #define blueART_TXBUFFERREAD           blueART_txBufferRead
    #define blueART_TXBUFFERWRITE          blueART_txBufferWrite
#endif /* End blueART_TX_ENABLED */
#if( ( blueART_RX_ENABLED || blueART_HD_ENABLED ) && \
     (blueART_RXBUFFERSIZE > blueART_FIFO_LENGTH) )
    #define blueART_RXBUFFER               blueART_rxBuffer
    #define blueART_RXBUFFERREAD           blueART_rxBufferRead
    #define blueART_RXBUFFERWRITE          blueART_rxBufferWrite
    #define blueART_RXBUFFERLOOPDETECT     blueART_rxBufferLoopDetect
    #define blueART_RXBUFFER_OVERFLOW      blueART_rxBufferOverflow
#endif /* End blueART_RX_ENABLED */

#ifdef blueART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG
    #define blueART_CONTROL                blueART_CONTROL_REG
#endif /* End blueART_BUART_sCR_SyncCtl_CtrlReg__CONTROL_REG */

#if(blueART_TX_ENABLED)
    #define blueART_TXDATA                 blueART_TXDATA_REG
    #define blueART_TXSTATUS               blueART_TXSTATUS_REG
    #define blueART_TXSTATUS_MASK          blueART_TXSTATUS_MASK_REG
    #define blueART_TXSTATUS_ACTL          blueART_TXSTATUS_ACTL_REG
    /* DP clock */
    #if(blueART_TXCLKGEN_DP)
        #define blueART_TXBITCLKGEN_CTR        blueART_TXBITCLKGEN_CTR_REG
        #define blueART_TXBITCLKTX_COMPLETE    blueART_TXBITCLKTX_COMPLETE_REG
    #else     /* Count7 clock*/
        #define blueART_TXBITCTR_PERIOD        blueART_TXBITCTR_PERIOD_REG
        #define blueART_TXBITCTR_CONTROL       blueART_TXBITCTR_CONTROL_REG
        #define blueART_TXBITCTR_COUNTER       blueART_TXBITCTR_COUNTER_REG
    #endif /* blueART_TXCLKGEN_DP */
#endif /* End blueART_TX_ENABLED */

#if(blueART_HD_ENABLED)
    #define blueART_TXDATA                 blueART_TXDATA_REG
    #define blueART_TXSTATUS               blueART_TXSTATUS_REG
    #define blueART_TXSTATUS_MASK          blueART_TXSTATUS_MASK_REG
    #define blueART_TXSTATUS_ACTL          blueART_TXSTATUS_ACTL_REG
#endif /* End blueART_HD_ENABLED */

#if( (blueART_RX_ENABLED) || (blueART_HD_ENABLED) )
    #define blueART_RXDATA                 blueART_RXDATA_REG
    #define blueART_RXADDRESS1             blueART_RXADDRESS1_REG
    #define blueART_RXADDRESS2             blueART_RXADDRESS2_REG
    #define blueART_RXBITCTR_PERIOD        blueART_RXBITCTR_PERIOD_REG
    #define blueART_RXBITCTR_CONTROL       blueART_RXBITCTR_CONTROL_REG
    #define blueART_RXBITCTR_COUNTER       blueART_RXBITCTR_COUNTER_REG
    #define blueART_RXSTATUS               blueART_RXSTATUS_REG
    #define blueART_RXSTATUS_MASK          blueART_RXSTATUS_MASK_REG
    #define blueART_RXSTATUS_ACTL          blueART_RXSTATUS_ACTL_REG
#endif /* End  (blueART_RX_ENABLED) || (blueART_HD_ENABLED) */

#if(blueART_INTERNAL_CLOCK_USED)
    #define blueART_INTCLOCK_CLKEN         blueART_INTCLOCK_CLKEN_REG
#endif /* End blueART_INTERNAL_CLOCK_USED */

#define blueART_WAIT_FOR_COMLETE_REINIT    blueART_WAIT_FOR_COMPLETE_REINIT

#endif  /* CY_UART_blueART_H */


/* [] END OF FILE */
