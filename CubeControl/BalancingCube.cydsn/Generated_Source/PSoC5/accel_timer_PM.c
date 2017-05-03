/*******************************************************************************
* File Name: accel_timer_PM.c
* Version 2.70
*
*  Description:
*     This file provides the power management source code to API for the
*     Timer.
*
*   Note:
*     None
*
*******************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
********************************************************************************/

#include "accel_timer.h"

static accel_timer_backupStruct accel_timer_backup;


/*******************************************************************************
* Function Name: accel_timer_SaveConfig
********************************************************************************
*
* Summary:
*     Save the current user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  accel_timer_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void accel_timer_SaveConfig(void) 
{
    #if (!accel_timer_UsingFixedFunction)
        accel_timer_backup.TimerUdb = accel_timer_ReadCounter();
        accel_timer_backup.InterruptMaskValue = accel_timer_STATUS_MASK;
        #if (accel_timer_UsingHWCaptureCounter)
            accel_timer_backup.TimerCaptureCounter = accel_timer_ReadCaptureCount();
        #endif /* Back Up capture counter register  */

        #if(!accel_timer_UDB_CONTROL_REG_REMOVED)
            accel_timer_backup.TimerControlRegister = accel_timer_ReadControlRegister();
        #endif /* Backup the enable state of the Timer component */
    #endif /* Backup non retention registers in UDB implementation. All fixed function registers are retention */
}


/*******************************************************************************
* Function Name: accel_timer_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration.
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  accel_timer_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void accel_timer_RestoreConfig(void) 
{   
    #if (!accel_timer_UsingFixedFunction)

        accel_timer_WriteCounter(accel_timer_backup.TimerUdb);
        accel_timer_STATUS_MASK =accel_timer_backup.InterruptMaskValue;
        #if (accel_timer_UsingHWCaptureCounter)
            accel_timer_SetCaptureCount(accel_timer_backup.TimerCaptureCounter);
        #endif /* Restore Capture counter register*/

        #if(!accel_timer_UDB_CONTROL_REG_REMOVED)
            accel_timer_WriteControlRegister(accel_timer_backup.TimerControlRegister);
        #endif /* Restore the enable state of the Timer component */
    #endif /* Restore non retention registers in the UDB implementation only */
}


/*******************************************************************************
* Function Name: accel_timer_Sleep
********************************************************************************
*
* Summary:
*     Stop and Save the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  accel_timer_backup.TimerEnableState:  Is modified depending on the
*  enable state of the block before entering sleep mode.
*
*******************************************************************************/
void accel_timer_Sleep(void) 
{
    #if(!accel_timer_UDB_CONTROL_REG_REMOVED)
        /* Save Counter's enable state */
        if(accel_timer_CTRL_ENABLE == (accel_timer_CONTROL & accel_timer_CTRL_ENABLE))
        {
            /* Timer is enabled */
            accel_timer_backup.TimerEnableState = 1u;
        }
        else
        {
            /* Timer is disabled */
            accel_timer_backup.TimerEnableState = 0u;
        }
    #endif /* Back up enable state from the Timer control register */
    accel_timer_Stop();
    accel_timer_SaveConfig();
}


/*******************************************************************************
* Function Name: accel_timer_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration
*
* Parameters:
*  void
*
* Return:
*  void
*
* Global variables:
*  accel_timer_backup.enableState:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void accel_timer_Wakeup(void) 
{
    accel_timer_RestoreConfig();
    #if(!accel_timer_UDB_CONTROL_REG_REMOVED)
        if(accel_timer_backup.TimerEnableState == 1u)
        {     /* Enable Timer's operation */
                accel_timer_Enable();
        } /* Do nothing if Timer was disabled before */
    #endif /* Remove this code section if Control register is removed */
}


/* [] END OF FILE */
