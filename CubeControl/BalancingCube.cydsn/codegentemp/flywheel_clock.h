/*******************************************************************************
* File Name: flywheel_clock.h
* Version 2.20
*
*  Description:
*   Provides the function and constant definitions for the clock component.
*
*  Note:
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions, 
* disclaimers, and limitations in the end user license agreement accompanying 
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_CLOCK_flywheel_clock_H)
#define CY_CLOCK_flywheel_clock_H

#include <cytypes.h>
#include <cyfitter.h>


/***************************************
* Conditional Compilation Parameters
***************************************/

/* Check to see if required defines such as CY_PSOC5LP are available */
/* They are defined starting with cy_boot v3.0 */
#if !defined (CY_PSOC5LP)
    #error Component cy_clock_v2_20 requires cy_boot v3.0 or later
#endif /* (CY_PSOC5LP) */


/***************************************
*        Function Prototypes
***************************************/

void flywheel_clock_Start(void) ;
void flywheel_clock_Stop(void) ;

#if(CY_PSOC3 || CY_PSOC5LP)
void flywheel_clock_StopBlock(void) ;
#endif /* (CY_PSOC3 || CY_PSOC5LP) */

void flywheel_clock_StandbyPower(uint8 state) ;
void flywheel_clock_SetDividerRegister(uint16 clkDivider, uint8 restart) 
                                ;
uint16 flywheel_clock_GetDividerRegister(void) ;
void flywheel_clock_SetModeRegister(uint8 modeBitMask) ;
void flywheel_clock_ClearModeRegister(uint8 modeBitMask) ;
uint8 flywheel_clock_GetModeRegister(void) ;
void flywheel_clock_SetSourceRegister(uint8 clkSource) ;
uint8 flywheel_clock_GetSourceRegister(void) ;
#if defined(flywheel_clock__CFG3)
void flywheel_clock_SetPhaseRegister(uint8 clkPhase) ;
uint8 flywheel_clock_GetPhaseRegister(void) ;
#endif /* defined(flywheel_clock__CFG3) */

#define flywheel_clock_Enable()                       flywheel_clock_Start()
#define flywheel_clock_Disable()                      flywheel_clock_Stop()
#define flywheel_clock_SetDivider(clkDivider)         flywheel_clock_SetDividerRegister(clkDivider, 1u)
#define flywheel_clock_SetDividerValue(clkDivider)    flywheel_clock_SetDividerRegister((clkDivider) - 1u, 1u)
#define flywheel_clock_SetMode(clkMode)               flywheel_clock_SetModeRegister(clkMode)
#define flywheel_clock_SetSource(clkSource)           flywheel_clock_SetSourceRegister(clkSource)
#if defined(flywheel_clock__CFG3)
#define flywheel_clock_SetPhase(clkPhase)             flywheel_clock_SetPhaseRegister(clkPhase)
#define flywheel_clock_SetPhaseValue(clkPhase)        flywheel_clock_SetPhaseRegister((clkPhase) + 1u)
#endif /* defined(flywheel_clock__CFG3) */


/***************************************
*             Registers
***************************************/

/* Register to enable or disable the clock */
#define flywheel_clock_CLKEN              (* (reg8 *) flywheel_clock__PM_ACT_CFG)
#define flywheel_clock_CLKEN_PTR          ((reg8 *) flywheel_clock__PM_ACT_CFG)

/* Register to enable or disable the clock */
#define flywheel_clock_CLKSTBY            (* (reg8 *) flywheel_clock__PM_STBY_CFG)
#define flywheel_clock_CLKSTBY_PTR        ((reg8 *) flywheel_clock__PM_STBY_CFG)

/* Clock LSB divider configuration register. */
#define flywheel_clock_DIV_LSB            (* (reg8 *) flywheel_clock__CFG0)
#define flywheel_clock_DIV_LSB_PTR        ((reg8 *) flywheel_clock__CFG0)
#define flywheel_clock_DIV_PTR            ((reg16 *) flywheel_clock__CFG0)

/* Clock MSB divider configuration register. */
#define flywheel_clock_DIV_MSB            (* (reg8 *) flywheel_clock__CFG1)
#define flywheel_clock_DIV_MSB_PTR        ((reg8 *) flywheel_clock__CFG1)

/* Mode and source configuration register */
#define flywheel_clock_MOD_SRC            (* (reg8 *) flywheel_clock__CFG2)
#define flywheel_clock_MOD_SRC_PTR        ((reg8 *) flywheel_clock__CFG2)

#if defined(flywheel_clock__CFG3)
/* Analog clock phase configuration register */
#define flywheel_clock_PHASE              (* (reg8 *) flywheel_clock__CFG3)
#define flywheel_clock_PHASE_PTR          ((reg8 *) flywheel_clock__CFG3)
#endif /* defined(flywheel_clock__CFG3) */


/**************************************
*       Register Constants
**************************************/

/* Power manager register masks */
#define flywheel_clock_CLKEN_MASK         flywheel_clock__PM_ACT_MSK
#define flywheel_clock_CLKSTBY_MASK       flywheel_clock__PM_STBY_MSK

/* CFG2 field masks */
#define flywheel_clock_SRC_SEL_MSK        flywheel_clock__CFG2_SRC_SEL_MASK
#define flywheel_clock_MODE_MASK          (~(flywheel_clock_SRC_SEL_MSK))

#if defined(flywheel_clock__CFG3)
/* CFG3 phase mask */
#define flywheel_clock_PHASE_MASK         flywheel_clock__CFG3_PHASE_DLY_MASK
#endif /* defined(flywheel_clock__CFG3) */

#endif /* CY_CLOCK_flywheel_clock_H */


/* [] END OF FILE */
