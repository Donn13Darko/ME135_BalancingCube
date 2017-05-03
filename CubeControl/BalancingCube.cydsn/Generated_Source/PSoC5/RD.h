/*******************************************************************************
* File Name: RD.h
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
#if !defined(CY_ISR_RD_H)
#define CY_ISR_RD_H


#include <cytypes.h>
#include <cyfitter.h>

/* Interrupt Controller API. */
void RD_Start(void);
void RD_StartEx(cyisraddress address);
void RD_Stop(void);

CY_ISR_PROTO(RD_Interrupt);

void RD_SetVector(cyisraddress address);
cyisraddress RD_GetVector(void);

void RD_SetPriority(uint8 priority);
uint8 RD_GetPriority(void);

void RD_Enable(void);
uint8 RD_GetState(void);
void RD_Disable(void);

void RD_SetPending(void);
void RD_ClearPending(void);


/* Interrupt Controller Constants */

/* Address of the INTC.VECT[x] register that contains the Address of the RD ISR. */
#define RD_INTC_VECTOR            ((reg32 *) RD__INTC_VECT)

/* Address of the RD ISR priority. */
#define RD_INTC_PRIOR             ((reg8 *) RD__INTC_PRIOR_REG)

/* Priority of the RD interrupt. */
#define RD_INTC_PRIOR_NUMBER      RD__INTC_PRIOR_NUM

/* Address of the INTC.SET_EN[x] byte to bit enable RD interrupt. */
#define RD_INTC_SET_EN            ((reg32 *) RD__INTC_SET_EN_REG)

/* Address of the INTC.CLR_EN[x] register to bit clear the RD interrupt. */
#define RD_INTC_CLR_EN            ((reg32 *) RD__INTC_CLR_EN_REG)

/* Address of the INTC.SET_PD[x] register to set the RD interrupt state to pending. */
#define RD_INTC_SET_PD            ((reg32 *) RD__INTC_SET_PD_REG)

/* Address of the INTC.CLR_PD[x] register to clear the RD interrupt. */
#define RD_INTC_CLR_PD            ((reg32 *) RD__INTC_CLR_PD_REG)


#endif /* CY_ISR_RD_H */


/* [] END OF FILE */
