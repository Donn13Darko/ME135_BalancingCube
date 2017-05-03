/*******************************************************************************
* File Name: i2c_COM_PM.c
* Version 3.50
*
* Description:
*  This file provides low power mode APIs for the I2C component.
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation. All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "i2c_COM_PVT.h"

i2c_COM_BACKUP_STRUCT i2c_COM_backup =
{
    i2c_COM_DISABLE,

#if (i2c_COM_FF_IMPLEMENTED)
    i2c_COM_DEFAULT_XCFG,
    i2c_COM_DEFAULT_CFG,
    i2c_COM_DEFAULT_ADDR,
    LO8(i2c_COM_DEFAULT_DIVIDE_FACTOR),
    HI8(i2c_COM_DEFAULT_DIVIDE_FACTOR),
#else  /* (i2c_COM_UDB_IMPLEMENTED) */
    i2c_COM_DEFAULT_CFG,
#endif /* (i2c_COM_FF_IMPLEMENTED) */

#if (i2c_COM_TIMEOUT_ENABLED)
    i2c_COM_DEFAULT_TMOUT_PERIOD,
    i2c_COM_DEFAULT_TMOUT_INTR_MASK,
#endif /* (i2c_COM_TIMEOUT_ENABLED) */
};

#if ((i2c_COM_FF_IMPLEMENTED) && (i2c_COM_WAKEUP_ENABLED))
    volatile uint8 i2c_COM_wakeupSource;
#endif /* ((i2c_COM_FF_IMPLEMENTED) && (i2c_COM_WAKEUP_ENABLED)) */


/*******************************************************************************
* Function Name: i2c_COM_SaveConfig
********************************************************************************
*
* Summary:
*  The Enable wakeup from Sleep Mode selection influences this function
*  implementation:
*   Unchecked: Stores the component non-retention configuration registers.
*   Checked:   Disables the master, if it was enabled before, and enables
*              backup regulator of the I2C hardware. If a transaction intended
*              for component executes during this function call, it waits until
*              the current transaction is completed and I2C hardware is ready
*              to enter sleep mode. All subsequent I2C traffic is NAKed until
*              the device is put into sleep mode.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  i2c_COM_backup - The global variable used to save the component
*                            configuration and non-retention registers before
*                            entering the sleep mode.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void i2c_COM_SaveConfig(void) 
{
#if (i2c_COM_FF_IMPLEMENTED)
    #if (i2c_COM_WAKEUP_ENABLED)
        uint8 intState;
    #endif /* (i2c_COM_WAKEUP_ENABLED) */

    /* Store registers before enter low power mode */
    i2c_COM_backup.cfg     = i2c_COM_CFG_REG;
    i2c_COM_backup.xcfg    = i2c_COM_XCFG_REG;
    i2c_COM_backup.addr    = i2c_COM_ADDR_REG;
    i2c_COM_backup.clkDiv1 = i2c_COM_CLKDIV1_REG;
    i2c_COM_backup.clkDiv2 = i2c_COM_CLKDIV2_REG;

#if (i2c_COM_WAKEUP_ENABLED)
    /* Disable master */
    i2c_COM_CFG_REG &= (uint8) ~i2c_COM_ENABLE_MASTER;

    /* Enable backup regulator to keep block powered in low power mode */
    intState = CyEnterCriticalSection();
    i2c_COM_PWRSYS_CR1_REG |= i2c_COM_PWRSYS_CR1_I2C_REG_BACKUP;
    CyExitCriticalSection(intState);

    /* 1) Set force NACK to ignore I2C transactions;
    *  2) Wait unti I2C is ready go to Sleep; !!
    *  3) These bits are cleared on wake up.
    */
    /* Wait when block is ready for sleep */
    i2c_COM_XCFG_REG |= i2c_COM_XCFG_FORCE_NACK;
    while (0u == (i2c_COM_XCFG_REG & i2c_COM_XCFG_RDY_TO_SLEEP))
    {
    }

    /* Setup wakeup interrupt */
    i2c_COM_DisableInt();
    (void) CyIntSetVector(i2c_COM_ISR_NUMBER, &i2c_COM_WAKEUP_ISR);
    i2c_COM_wakeupSource = 0u;
    i2c_COM_EnableInt();
#endif /* (i2c_COM_WAKEUP_ENABLED) */

#else
    /* Store only address match bit */
    i2c_COM_backup.control = (i2c_COM_CFG_REG & i2c_COM_CTRL_ANY_ADDRESS_MASK);
#endif /* (i2c_COM_FF_IMPLEMENTED) */

#if (i2c_COM_TIMEOUT_ENABLED)
    i2c_COM_TimeoutSaveConfig();
#endif /* (i2c_COM_TIMEOUT_ENABLED) */
}


/*******************************************************************************
* Function Name: i2c_COM_Sleep
********************************************************************************
*
* Summary:
*  This is the preferred method to prepare the component before device enters
*  sleep mode. The Enable wakeup from Sleep Mode selection influences this
*  function implementation:
*   Unchecked: Checks current I2C component state, saves it, and disables the
*              component by calling I2C_Stop() if it is currently enabled.
*              I2C_SaveConfig() is then called to save the component
*              non-retention configuration registers.
*   Checked:   If a transaction intended for component executes during this
*              function call, it waits until the current transaction is
*              completed. All subsequent I2C traffic intended for component
*              is NAKed until the device is put to sleep mode. The address
*              match event wakes up the device.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void i2c_COM_Sleep(void) 
{
#if (i2c_COM_WAKEUP_ENABLED)
    /* Do not enable block after exit low power mode if it is wakeup source */
    i2c_COM_backup.enableState = i2c_COM_DISABLE;

    #if (i2c_COM_TIMEOUT_ENABLED)
        i2c_COM_TimeoutStop();
    #endif /* (i2c_COM_TIMEOUT_ENABLED) */

#else
    /* Store enable state */
    i2c_COM_backup.enableState = (uint8) i2c_COM_IS_ENABLED;

    if (0u != i2c_COM_backup.enableState)
    {
        i2c_COM_Stop();
    }
#endif /* (i2c_COM_WAKEUP_ENABLED) */

    i2c_COM_SaveConfig();
}


/*******************************************************************************
* Function Name: i2c_COM_RestoreConfig
********************************************************************************
*
* Summary:
*  The Enable wakeup from Sleep Mode selection influences this function
*  implementation:
*   Unchecked: Restores the component non-retention configuration registers
*              to the state they were in before I2C_Sleep() or I2C_SaveConfig()
*              was called.
*   Checked:   Disables the backup regulator of the I2C hardware. Sets up the
*              regular component interrupt handler and generates the component
*              interrupt if it was wake up source to release the bus and
*              continue in-coming I2C transaction.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  i2c_COM_backup - The global variable used to save the component
*                            configuration and non-retention registers before
*                            exiting the sleep mode.
*
* Reentrant:
*  No.
*
* Side Effects:
*  Calling this function before i2c_COM_SaveConfig() or
*  i2c_COM_Sleep() will lead to unpredictable results.
*
*******************************************************************************/
void i2c_COM_RestoreConfig(void) 
{
#if (i2c_COM_FF_IMPLEMENTED)
    uint8 intState;

    if (i2c_COM_CHECK_PWRSYS_I2C_BACKUP)
    /* Low power mode was Sleep - backup regulator is enabled */
    {
        /* Enable backup regulator in active mode */
        intState = CyEnterCriticalSection();
        i2c_COM_PWRSYS_CR1_REG &= (uint8) ~i2c_COM_PWRSYS_CR1_I2C_REG_BACKUP;
        CyExitCriticalSection(intState);

        /* Restore master */
        i2c_COM_CFG_REG = i2c_COM_backup.cfg;
    }
    else
    /* Low power mode was Hibernate - backup regulator is disabled. All registers are cleared */
    {
    #if (i2c_COM_WAKEUP_ENABLED)
        /* Disable power to block before register restore */
        intState = CyEnterCriticalSection();
        i2c_COM_ACT_PWRMGR_REG  &= (uint8) ~i2c_COM_ACT_PWR_EN;
        i2c_COM_STBY_PWRMGR_REG &= (uint8) ~i2c_COM_STBY_PWR_EN;
        CyExitCriticalSection(intState);

        /* Enable component in I2C_Wakeup() after register restore */
        i2c_COM_backup.enableState = i2c_COM_ENABLE;
    #endif /* (i2c_COM_WAKEUP_ENABLED) */

        /* Restore component registers after Hibernate */
        i2c_COM_XCFG_REG    = i2c_COM_backup.xcfg;
        i2c_COM_CFG_REG     = i2c_COM_backup.cfg;
        i2c_COM_ADDR_REG    = i2c_COM_backup.addr;
        i2c_COM_CLKDIV1_REG = i2c_COM_backup.clkDiv1;
        i2c_COM_CLKDIV2_REG = i2c_COM_backup.clkDiv2;
    }

#if (i2c_COM_WAKEUP_ENABLED)
    i2c_COM_DisableInt();
    (void) CyIntSetVector(i2c_COM_ISR_NUMBER, &i2c_COM_ISR);
    if (0u != i2c_COM_wakeupSource)
    {
        /* Generate interrupt to process incoming transaction */
        i2c_COM_SetPendingInt();
    }
    i2c_COM_EnableInt();
#endif /* (i2c_COM_WAKEUP_ENABLED) */

#else
    i2c_COM_CFG_REG = i2c_COM_backup.control;
#endif /* (i2c_COM_FF_IMPLEMENTED) */

#if (i2c_COM_TIMEOUT_ENABLED)
    i2c_COM_TimeoutRestoreConfig();
#endif /* (i2c_COM_TIMEOUT_ENABLED) */
}


/*******************************************************************************
* Function Name: i2c_COM_Wakeup
********************************************************************************
*
* Summary:
*  This is the preferred method to prepare the component for active mode
*  operation (when device exits sleep mode). The Enable wakeup from Sleep Mode
*  selection influences this function implementation:
*   Unchecked: Restores the component non-retention configuration registers
*              by calling I2C_RestoreConfig(). If the component was enabled
*              before the I2C_Sleep() function was called, I2C_Wakeup()
*              re-enables it.
*   Checked:   Enables  master functionality if it was enabled before sleep,
*              and disables the backup regulator of the I2C hardware.
*              The incoming transaction continues as soon as the regular
*              I2C interrupt handler is set up (global interrupts has to be
*              enabled to service I2C component interrupt).
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Reentrant:
*  No.
*
* Side Effects:
*  Calling this function before i2c_COM_SaveConfig() or
*  i2c_COM_Sleep() will lead to unpredictable results.
*
*******************************************************************************/
void i2c_COM_Wakeup(void) 
{
    i2c_COM_RestoreConfig();

    /* Restore component enable state */
    if (0u != i2c_COM_backup.enableState)
    {
        i2c_COM_Enable();
        i2c_COM_EnableInt();
    }
    else
    {
    #if (i2c_COM_TIMEOUT_ENABLED)
        i2c_COM_TimeoutEnable();
    #endif /* (i2c_COM_TIMEOUT_ENABLED) */
    }
}


/* [] END OF FILE */
