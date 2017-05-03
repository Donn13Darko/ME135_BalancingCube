/*******************************************************************************
* File Name: flywheel_x_PM.c
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

#include "flywheel_x.h"

static flywheel_x_backupStruct flywheel_x_backup;


/*******************************************************************************
* Function Name: flywheel_x_SaveConfig
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
*  flywheel_x_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void flywheel_x_SaveConfig(void) 
{

    #if(!flywheel_x_UsingFixedFunction)
        #if(!flywheel_x_PWMModeIsCenterAligned)
            flywheel_x_backup.PWMPeriod = flywheel_x_ReadPeriod();
        #endif /* (!flywheel_x_PWMModeIsCenterAligned) */
        flywheel_x_backup.PWMUdb = flywheel_x_ReadCounter();
        #if (flywheel_x_UseStatus)
            flywheel_x_backup.InterruptMaskValue = flywheel_x_STATUS_MASK;
        #endif /* (flywheel_x_UseStatus) */

        #if(flywheel_x_DeadBandMode == flywheel_x__B_PWM__DBM_256_CLOCKS || \
            flywheel_x_DeadBandMode == flywheel_x__B_PWM__DBM_2_4_CLOCKS)
            flywheel_x_backup.PWMdeadBandValue = flywheel_x_ReadDeadTime();
        #endif /*  deadband count is either 2-4 clocks or 256 clocks */

        #if(flywheel_x_KillModeMinTime)
             flywheel_x_backup.PWMKillCounterPeriod = flywheel_x_ReadKillTime();
        #endif /* (flywheel_x_KillModeMinTime) */

        #if(flywheel_x_UseControl)
            flywheel_x_backup.PWMControlRegister = flywheel_x_ReadControlRegister();
        #endif /* (flywheel_x_UseControl) */
    #endif  /* (!flywheel_x_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: flywheel_x_RestoreConfig
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
*  flywheel_x_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void flywheel_x_RestoreConfig(void) 
{
        #if(!flywheel_x_UsingFixedFunction)
            #if(!flywheel_x_PWMModeIsCenterAligned)
                flywheel_x_WritePeriod(flywheel_x_backup.PWMPeriod);
            #endif /* (!flywheel_x_PWMModeIsCenterAligned) */

            flywheel_x_WriteCounter(flywheel_x_backup.PWMUdb);

            #if (flywheel_x_UseStatus)
                flywheel_x_STATUS_MASK = flywheel_x_backup.InterruptMaskValue;
            #endif /* (flywheel_x_UseStatus) */

            #if(flywheel_x_DeadBandMode == flywheel_x__B_PWM__DBM_256_CLOCKS || \
                flywheel_x_DeadBandMode == flywheel_x__B_PWM__DBM_2_4_CLOCKS)
                flywheel_x_WriteDeadTime(flywheel_x_backup.PWMdeadBandValue);
            #endif /* deadband count is either 2-4 clocks or 256 clocks */

            #if(flywheel_x_KillModeMinTime)
                flywheel_x_WriteKillTime(flywheel_x_backup.PWMKillCounterPeriod);
            #endif /* (flywheel_x_KillModeMinTime) */

            #if(flywheel_x_UseControl)
                flywheel_x_WriteControlRegister(flywheel_x_backup.PWMControlRegister);
            #endif /* (flywheel_x_UseControl) */
        #endif  /* (!flywheel_x_UsingFixedFunction) */
    }


/*******************************************************************************
* Function Name: flywheel_x_Sleep
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
*  flywheel_x_backup.PWMEnableState:  Is modified depending on the enable
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void flywheel_x_Sleep(void) 
{
    #if(flywheel_x_UseControl)
        if(flywheel_x_CTRL_ENABLE == (flywheel_x_CONTROL & flywheel_x_CTRL_ENABLE))
        {
            /*Component is enabled */
            flywheel_x_backup.PWMEnableState = 1u;
        }
        else
        {
            /* Component is disabled */
            flywheel_x_backup.PWMEnableState = 0u;
        }
    #endif /* (flywheel_x_UseControl) */

    /* Stop component */
    flywheel_x_Stop();

    /* Save registers configuration */
    flywheel_x_SaveConfig();
}


/*******************************************************************************
* Function Name: flywheel_x_Wakeup
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
*  flywheel_x_backup.pwmEnable:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void flywheel_x_Wakeup(void) 
{
     /* Restore registers values */
    flywheel_x_RestoreConfig();

    if(flywheel_x_backup.PWMEnableState != 0u)
    {
        /* Enable component's operation */
        flywheel_x_Enable();
    } /* Do nothing if component's block was disabled before */

}


/* [] END OF FILE */
