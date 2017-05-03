/*******************************************************************************
* File Name: .h
* Version 3.50
*
* Description:
*  This file provides private constants and parameter values for the I2C
*  component.
*
* Note:
*
********************************************************************************
* Copyright 2012-2015, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_I2C_PVT_i2c_COM_H)
#define CY_I2C_PVT_i2c_COM_H

#include "i2c_COM.h"

#define i2c_COM_TIMEOUT_ENABLED_INC    (0u)
#if (0u != i2c_COM_TIMEOUT_ENABLED_INC)
    #include "i2c_COM_TMOUT.h"
#endif /* (0u != i2c_COM_TIMEOUT_ENABLED_INC) */


/**********************************
*   Variables with external linkage
**********************************/

extern i2c_COM_BACKUP_STRUCT i2c_COM_backup;

extern volatile uint8 i2c_COM_state;   /* Current state of I2C FSM */

/* Master variables */
#if (i2c_COM_MODE_MASTER_ENABLED)
    extern volatile uint8 i2c_COM_mstrStatus;   /* Master Status byte  */
    extern volatile uint8 i2c_COM_mstrControl;  /* Master Control byte */

    /* Transmit buffer variables */
    extern volatile uint8 * i2c_COM_mstrRdBufPtr;   /* Pointer to Master Read buffer */
    extern volatile uint8   i2c_COM_mstrRdBufSize;  /* Master Read buffer size       */
    extern volatile uint8   i2c_COM_mstrRdBufIndex; /* Master Read buffer Index      */

    /* Receive buffer variables */
    extern volatile uint8 * i2c_COM_mstrWrBufPtr;   /* Pointer to Master Write buffer */
    extern volatile uint8   i2c_COM_mstrWrBufSize;  /* Master Write buffer size       */
    extern volatile uint8   i2c_COM_mstrWrBufIndex; /* Master Write buffer Index      */

#endif /* (i2c_COM_MODE_MASTER_ENABLED) */

/* Slave variables */
#if (i2c_COM_MODE_SLAVE_ENABLED)
    extern volatile uint8 i2c_COM_slStatus;         /* Slave Status  */

    /* Transmit buffer variables */
    extern volatile uint8 * i2c_COM_slRdBufPtr;     /* Pointer to Transmit buffer  */
    extern volatile uint8   i2c_COM_slRdBufSize;    /* Slave Transmit buffer size  */
    extern volatile uint8   i2c_COM_slRdBufIndex;   /* Slave Transmit buffer Index */

    /* Receive buffer variables */
    extern volatile uint8 * i2c_COM_slWrBufPtr;     /* Pointer to Receive buffer  */
    extern volatile uint8   i2c_COM_slWrBufSize;    /* Slave Receive buffer size  */
    extern volatile uint8   i2c_COM_slWrBufIndex;   /* Slave Receive buffer Index */

    #if (i2c_COM_SW_ADRR_DECODE)
        extern volatile uint8 i2c_COM_slAddress;     /* Software address variable */
    #endif   /* (i2c_COM_SW_ADRR_DECODE) */

#endif /* (i2c_COM_MODE_SLAVE_ENABLED) */

#if ((i2c_COM_FF_IMPLEMENTED) && (i2c_COM_WAKEUP_ENABLED))
    extern volatile uint8 i2c_COM_wakeupSource;
#endif /* ((i2c_COM_FF_IMPLEMENTED) && (i2c_COM_WAKEUP_ENABLED)) */


#endif /* CY_I2C_PVT_i2c_COM_H */


/* [] END OF FILE */
