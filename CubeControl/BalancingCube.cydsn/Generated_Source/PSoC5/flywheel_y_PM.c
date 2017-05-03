/*******************************************************************************
* File Name: flywheel_y_PM.c
* Version 3.30
*
* Description:
*  This file provides the power management source code to API for the
*  PWM.
*
* Note:
*
********************************************************************************
* Copyright 2008-2014, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "flywheel_y.h"

static flywheel_y_backupStruct flywheel_y_backup;


/*******************************************************************************
* Function Name: flywheel_y_SaveConfig
********************************************************************************
*
* Summary:
*  Saves the current user configuration of the component.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  flywheel_y_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void flywheel_y_SaveConfig(void) 
{

    #if(!flywheel_y_UsingFixedFunction)
        #if(!flywheel_y_PWMModeIsCenterAligned)
            flywheel_y_backup.PWMPeriod = flywheel_y_ReadPeriod();
        #endif /* (!flywheel_y_PWMModeIsCenterAligned) */
        flywheel_y_backup.PWMUdb = flywheel_y_ReadCounter();
        #if (flywheel_y_UseStatus)
            flywheel_y_backup.InterruptMaskValue = flywheel_y_STATUS_MASK;
        #endif /* (flywheel_y_UseStatus) */

        #if(flywheel_y_DeadBandMode == flywheel_y__B_PWM__DBM_256_CLOCKS || \
            flywheel_y_DeadBandMode == flywheel_y__B_PWM__DBM_2_4_CLOCKS)
            flywheel_y_backup.PWMdeadBandValue = flywheel_y_ReadDeadTime();
        #endif /*  deadband count is either 2-4 clocks or 256 clocks */

        #if(flywheel_y_KillModeMinTime)
             flywheel_y_backup.PWMKillCounterPeriod = flywheel_y_ReadKillTime();
        #endif /* (flywheel_y_KillModeMinTime) */

        #if(flywheel_y_UseControl)
            flywheel_y_backup.PWMControlRegister = flywheel_y_ReadControlRegister();
        #endif /* (flywheel_y_UseControl) */
    #endif  /* (!flywheel_y_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: flywheel_y_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the current user configuration of the component.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  flywheel_y_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void flywheel_y_RestoreConfig(void) 
{
        #if(!flywheel_y_UsingFixedFunction)
            #if(!flywheel_y_PWMModeIsCenterAligned)
                flywheel_y_WritePeriod(flywheel_y_backup.PWMPeriod);
            #endif /* (!flywheel_y_PWMModeIsCenterAligned) */

            flywheel_y_WriteCounter(flywheel_y_backup.PWMUdb);

            #if (flywheel_y_UseStatus)
                flywheel_y_STATUS_MASK = flywheel_y_backup.InterruptMaskValue;
            #endif /* (flywheel_y_UseStatus) */

            #if(flywheel_y_DeadBandMode == flywheel_y__B_PWM__DBM_256_CLOCKS || \
                flywheel_y_DeadBandMode == flywheel_y__B_PWM__DBM_2_4_CLOCKS)
                flywheel_y_WriteDeadTime(flywheel_y_backup.PWMdeadBandValue);
            #endif /* deadband count is either 2-4 clocks or 256 clocks */

            #if(flywheel_y_KillModeMinTime)
                flywheel_y_WriteKillTime(flywheel_y_backup.PWMKillCounterPeriod);
            #endif /* (flywheel_y_KillModeMinTime) */

            #if(flywheel_y_UseControl)
                flywheel_y_WriteControlRegister(flywheel_y_backup.PWMControlRegister);
            #endif /* (flywheel_y_UseControl) */
        #endif  /* (!flywheel_y_UsingFixedFunction) */
    }


/*******************************************************************************
* Function Name: flywheel_y_Sleep
********************************************************************************
*
* Summary:
*  Disables block's operation and saves the user configuration. Should be called
*  just prior to entering sleep.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  flywheel_y_backup.PWMEnableState:  Is modified depending on the enable
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void flywheel_y_Sleep(void) 
{
    #if(flywheel_y_UseControl)
        if(flywheel_y_CTRL_ENABLE == (flywheel_y_CONTROL & flywheel_y_CTRL_ENABLE))
        {
            /*Component is enabled */
            flywheel_y_backup.PWMEnableState = 1u;
        }
        else
        {
            /* Component is disabled */
            flywheel_y_backup.PWMEnableState = 0u;
        }
    #endif /* (flywheel_y_UseControl) */

    /* Stop component */
    flywheel_y_Stop();

    /* Save registers configuration */
    flywheel_y_SaveConfig();
}


/*******************************************************************************
* Function Name: flywheel_y_Wakeup
********************************************************************************
*
* Summary:
*  Restores and enables the user configuration. Should be called just after
*  awaking from sleep.
*
* Parameters:
*  None
*
* Return:
*  None
*
* Global variables:
*  flywheel_y_backup.pwmEnable:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void flywheel_y_Wakeup(void) 
{
     /* Restore registers values */
    flywheel_y_RestoreConfig();

    if(flywheel_y_backup.PWMEnableState != 0u)
    {
        /* Enable component's operation */
        flywheel_y_Enable();
    } /* Do nothing if component's block was disabled before */

}


/* [] END OF FILE */
