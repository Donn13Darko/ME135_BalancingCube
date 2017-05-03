/*******************************************************************************
* File Name: blueartRX_ISR.h
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
#if !defined(CY_ISR_blueartRX_ISR_H)
#define CY_ISR_blueartRX_ISR_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void blueartRX_ISR_Start(void);
void blueartRX_ISR_StartEx(cyisraddress address);
void blueartRX_ISR_Stop(void);

CY_ISR_PROTO(blueartRX_ISR_Interrupt);

void blueartRX_ISR_SetVector(cyisraddress address);
cyisraddress blueartRX_ISR_GetVector(void);

void blueartRX_ISR_SetPriority(uint8 priority);
uint8 blueartRX_ISR_GetPriority(void);

void blueartRX_ISR_Enable(void);
uint8 blueartRX_ISR_GetState(void);
void blueartRX_ISR_Disable(void);

void blueartRX_ISR_SetPending(void);
void blueartRX_ISR_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the blueartRX_ISR ISR. */
#define blueartRX_ISR_INTC_VECTOR            ((reg32 *) blueartRX_ISR__INTC_VECT)

/* Address of the blueartRX_ISR ISR priority. */
#define blueartRX_ISR_INTC_PRIOR             ((reg8 *) blueartRX_ISR__INTC_PRIOR_REG)

/* Priority of the blueartRX_ISR interrupt. */
#define blueartRX_ISR_INTC_PRIOR_NUMBER      blueartRX_ISR__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable blueartRX_ISR interrupt. */
#define blueartRX_ISR_INTC_SET_EN            ((reg32 *) blueartRX_ISR__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the blueartRX_ISR interrupt. */
#define blueartRX_ISR_INTC_CLR_EN            ((reg32 *) blueartRX_ISR__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the blueartRX_ISR interrupt state to pending. */
#define blueartRX_ISR_INTC_SET_PD            ((reg32 *) blueartRX_ISR__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the blueartRX_ISR interrupt. */
#define blueartRX_ISR_INTC_CLR_PD            ((reg32 *) blueartRX_ISR__INTC_CLR_PD_REG)


#endif /* CY_ISR_blueartRX_ISR_H */


/* [] END OF FILE */
