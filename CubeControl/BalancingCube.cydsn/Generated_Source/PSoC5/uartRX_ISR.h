/*******************************************************************************
* File Name: uartRX_ISR.h
* Version 1.70
*
*  Description:
*   Provides the function definitions for the Interrupt Controller.
*
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/
#if !defined(CY_ISR_uartRX_ISR_H)
#define CY_ISR_uartRX_ISR_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void uartRX_ISR_Start(void);
void uartRX_ISR_StartEx(cyisraddress address);
void uartRX_ISR_Stop(void);

CY_ISR_PROTO(uartRX_ISR_Interrupt);

void uartRX_ISR_SetVector(cyisraddress address);
cyisraddress uartRX_ISR_GetVector(void);

void uartRX_ISR_SetPriority(uint8 priority);
uint8 uartRX_ISR_GetPriority(void);

void uartRX_ISR_Enable(void);
uint8 uartRX_ISR_GetState(void);
void uartRX_ISR_Disable(void);

void uartRX_ISR_SetPending(void);
void uartRX_ISR_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the uartRX_ISR ISR. */
#define uartRX_ISR_INTC_VECTOR            ((reg32 *) uartRX_ISR__INTC_VECT)

/* Address of the uartRX_ISR ISR priority. */
#define uartRX_ISR_INTC_PRIOR             ((reg8 *) uartRX_ISR__INTC_PRIOR_REG)

/* Priority of the uartRX_ISR interrupt. */
#define uartRX_ISR_INTC_PRIOR_NUMBER      uartRX_ISR__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable uartRX_ISR interrupt. */
#define uartRX_ISR_INTC_SET_EN            ((reg32 *) uartRX_ISR__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the uartRX_ISR interrupt. */
#define uartRX_ISR_INTC_CLR_EN            ((reg32 *) uartRX_ISR__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the uartRX_ISR interrupt state to pending. */
#define uartRX_ISR_INTC_SET_PD            ((reg32 *) uartRX_ISR__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the uartRX_ISR interrupt. */
#define uartRX_ISR_INTC_CLR_PD            ((reg32 *) uartRX_ISR__INTC_CLR_PD_REG)


#endif /* CY_ISR_uartRX_ISR_H */


/* [] END OF FILE */
