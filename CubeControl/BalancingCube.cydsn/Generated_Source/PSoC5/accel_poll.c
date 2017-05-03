/*******************************************************************************
* File Name: accel_poll.c
* Version 3.20
*
* Description:
*  This file provides the source code to API for the Sleep Timer.
*
********************************************************************************
* Copyright 2008-2012, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "accel_poll.h"
#include "CyLib.h"

/* Variables were not initialized */
uint8 accel_poll_initVar = 0u;


/*******************************************************************************
* Function Name: accel_poll_Start
********************************************************************************
*
* Summary:
*  This is the preferred method to begin component operation. SleepTimer_Start()
*  sets the initVar variable, calls the SleepTimer_Init() function, and then
*  calls the SleepTimer_Enable() function. It enables the 1-kHz ILO clock and
*  leaves it enabled after the Sleep Timer component is stopped.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  The accel_poll_initVar variable is used to indicate initial
*  configuration of this component.  The variable is initialized to zero and
*  set to 1 the first time accel_poll_Start() is called. This allows for
*  component initialization without re-initialization in all subsequent calls
*  to the accel_poll_Start() routine.
*
* Reentrant:
*  No
*
* Side Effects:
*  If the initVar variable is already set, this function only calls the
*  SleepTimer_Enable() function.
*
*******************************************************************************/
void accel_poll_Start(void) 
{
    /* Execute once in normal flow */
    if (0u == accel_poll_initVar)
    {
        accel_poll_Init();
        accel_poll_initVar = 1u;
    }

    /* Enables the component operation */
    accel_poll_Enable();
}


/*******************************************************************************
* Function Name: accel_poll_Init
********************************************************************************
*
* Summary:
*  This function initializes or restores the component according to the
*  customizer Configure dialog settings. It is not necessary to call
*  SleepTimer_Init() because the SleepTimer_Start() API calls this function and
*  is the preferred method to begin component operation. Sets CTW interval
*  period and enables or disables CTW interrupt (according to the customizer's
*  settings).
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void accel_poll_Init(void)  
{
    /* Sets default (passed from customizer) interval */
    accel_poll_SetInterval(accel_poll_INTERVAL);

    /* Check if user sets to issue an interrupt in customizer */
    #if (1u == accel_poll_ENABLE_INTERRUPT)
    {
        /* Enable interrupt */
        accel_poll_EnableInt();
    }
    #else /* interrupt should be disabled */
    {
        /* Disable interrupt */
        accel_poll_DisableInt();
    }   /* Interrupt is disabled  */
    #endif  /* (1u == accel_poll_ENABLE_INTERRUPT) */
}


/*******************************************************************************
* Function Name: accel_poll_Enable
********************************************************************************
* Summary:
*  This function activates the 1-kHz ILO and the CTW and begins component
*  operation. It is not necessary to call SleepTimer_Enable() because the
*  SleepTimer_Start() API calls this function, which is the preferred method to
*  begin component operation.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*  Enables 1 kHz ILO clocks and leaves it enabled after Sleep Time component
*  is stopped.
*
*******************************************************************************/
void accel_poll_Enable(void) 
{
    uint8 interruptState;

    /* If 1kHz ILO is disabled */
    if(0u == (accel_poll_ILO_CFG_REG & accel_poll_ILO_1KHZ_EN))
    {
        /* Enter critical section */
        interruptState = CyEnterCriticalSection();

        /* Enable 1kHz ILO */
        accel_poll_ILO_CFG_REG |=accel_poll_ILO_1KHZ_EN;

        /* Enable the CTW counter */
        accel_poll_TW_CFG_REG |= accel_poll_CTW_EN;

        /* Exit critical section */
        CyExitCriticalSection(interruptState);
    }
    else /* If 1kHz ILO is enabled - enable CTW counter */
    {
        /* Enter critical section */
        interruptState = CyEnterCriticalSection();

        /* Enable the CTW counter */
        accel_poll_TW_CFG_REG |= accel_poll_CTW_EN;

        /* Exit critical section */
        CyExitCriticalSection(interruptState);
    }
}


/*******************************************************************************
* Function Name: accel_poll_Stop
********************************************************************************
*
* Summary:
*  This function stops Sleep Timer operation and disables wakeup and interrupt.
*  The device does not wake up when the CTW counter reaches terminal count, and
*  an interrupt is not issued.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Side Effects:
*  Leaves the 1 kHz ILO clock enabled after Sleep Time component is stopped.
*
*******************************************************************************/
void accel_poll_Stop(void) 
{
    uint8 interruptState;

    /* Enter critical section */
    interruptState = CyEnterCriticalSection();

    /* Disable CTW counter */
    accel_poll_TW_CFG_REG &= (uint8)(~accel_poll_CTW_EN);

    /* Exit critical section */
    CyExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: accel_poll_EnableInt
********************************************************************************
*
* Summary:
*  This function enables the CTW terminal count interrupt.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void accel_poll_EnableInt(void) 
{
    uint8 interruptState;

    /* Enter critical section */
    interruptState = CyEnterCriticalSection();

     /* Enable interrupt on wake up */
    accel_poll_TW_CFG_REG |= accel_poll_CTW_IE;

    /* Exit critical section */
    CyExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: accel_poll_DisableInt
********************************************************************************
*
* Summary:
*  This function disables the CTW terminal count interrupt.
*
* Parameters:
*  None
*
* Return:
*  None
*
*******************************************************************************/
void accel_poll_DisableInt(void) 
{
    uint8 interruptState;

    /* Enter critical section */
    interruptState = CyEnterCriticalSection();

    /* Disable interrupt on wake up */
    accel_poll_TW_CFG_REG &= (uint8)(~accel_poll_CTW_IE);

    /* Exit critical section */
    CyExitCriticalSection(interruptState);
}


/*******************************************************************************
* Function Name: accel_poll_SetInterval
********************************************************************************
*
* Summary:
*  This function sets the CTW interval period. The first interval can range
*  from 1 to (period + 1) milliseconds. Additional intervals occur at the
*  nominal period. You can only change the interval value when CTW is disabled,
*  which you do by stopping the component.
*
* Parameters:
*  uint8 interval: interval?s value for the CTW.
*
*  PSoC 5:
*  Only the 4-, 8-, 16-, 32-, 64-, 128-, and 256-ms selections are supported.
*
*           Name                    Value        Period
*   accel_poll__CTW_2_MS      4'b0001        2 ms
*   accel_poll__CTW_4_MS      4'b0010        4 ms
*   accel_poll__CTW_8_MS      4'b0011        8 ms
*   accel_poll__CTW_16_MS     4'b0100        16 ms
*   accel_poll__CTW_32_MS     4'b0101        32 ms
*   accel_poll__CTW_64_MS     4'b0110        64 ms
*   accel_poll__CTW_128_MS    4'b0111        128 ms
*   accel_poll__CTW_256_MS    4'b1000        256 ms
*   accel_poll__CTW_512_MS    4'b1001        512 ms
*   accel_poll__CTW_1024_MS   4'b1010        1024 ms
*   accel_poll__CTW_2048_MS   4'b1011        2048 ms
*   accel_poll__CTW_4096_MS   4'b1100        4096 ms
*
* Return:
*  None
*
* Side Effects:
*  Interval value can be only changed when component is stopped (CTW is
*  disabled).
*
*******************************************************************************/
void accel_poll_SetInterval(uint8 interval) 
{
    /* Check if CTW is disabled */
    if (0u == (accel_poll_TW_CFG_REG & accel_poll_CTW_EN))
    {
        #if(CY_PSOC5A)

            CYASSERT((accel_poll__CTW_4_MS   == interval) ||
                     (accel_poll__CTW_8_MS   == interval) ||
                     (accel_poll__CTW_16_MS  == interval) ||
                     (accel_poll__CTW_32_MS  == interval) ||
                     (accel_poll__CTW_64_MS  == interval) ||
                     (accel_poll__CTW_128_MS == interval) ||
                     (accel_poll__CTW_256_MS == interval));

        #endif  /* (CY_PSOC5A) */

        /* Set CTW interval */
        accel_poll_CTW_INTERVAL_REG = \
            (accel_poll_CTW_INTERVAL_REG & ((uint8)~accel_poll_INTERVAL_MASK)) | \
            (interval & accel_poll_INTERVAL_MASK);
    }
    /* Do nothing if CTW is enabled. */
}


/*******************************************************************************
* Function Name: accel_poll_GetStatus
********************************************************************************
*
* Summary:
*  This function returns the state of the Sleep Timer's status register, and
*  clears the pending interrupt status bit. The application code must always
*  call this function after wakeup to clear the ctw_int status bit. The code
*  must call this function whether the Sleep Timer's interrupt is disabled or
*  enabled.
*
* Parameters:
*  None
*
* Return:
*  Returns an 8-bit value (uint8) with bits set if a corresponding event has
*  occurred. The constants shown in the following table describe the two-bit
*  masks for the two events that this return value can contain.
*
*            Name                             Description
*  accel_poll_PM_INT_SR_ONEPPSP  A onepps event has occured
*  accel_poll_PM_INT_SR_CTW      A central timewheel event occured
*  accel_poll_PM_INT_SR_FTW      A fast timewheel event occured
*
* Side Effects:
*  If the SleepTimer_GetStatus() function is not called in an interrupt
*  associated with the SleepTimer, the interrupt is not cleared and as soon as
*  the interrupt is exited it is re-entered.
*
*  After the Sleep Timer has expired, the sleep interval is functionally 0 ms,
*  because the interrupt is called until the ctw_int flag is cleared by the
*  SleepTimer_GetStatus() function.
*
*  If an interrupt is generated at the same time as a register read/clear, the
*  bit remains set (which causes another interrupt).
*
*  Reports and then clears all interrupt status bits in the Power Manager
*  Interrupt Status register. Some of the bits are not relevant to this
*  component's operation.
*
*  The application code must always call this function (when the Sleep Timer's
*  interrupt is disabled or enabled) after wakeup to clear the ctw_int status
*  bit. The code must call SleepTimer_GetStatus() within 1 ms (1 clock cycle of
*  the ILO) after the CTW event occurred.
*
*******************************************************************************/
uint8 accel_poll_GetStatus(void) 
{
    /* Read to clear  */
    return CyPmReadStatus(CY_PM_CTW_INT);
}


/* [] END OF FILE */
