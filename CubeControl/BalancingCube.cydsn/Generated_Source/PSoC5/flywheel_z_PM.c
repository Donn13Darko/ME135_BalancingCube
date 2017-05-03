/*******************************************************************************
* File Name: flywheel_z_PM.c
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

#include "flywheel_z.h"

static flywheel_z_backupStruct flywheel_z_backup;


/*******************************************************************************
* Function Name: flywheel_z_SaveConfig
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
*  flywheel_z_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void flywheel_z_SaveConfig(void) 
{

    #if(!flywheel_z_UsingFixedFunction)
        #if(!flywheel_z_PWMModeIsCenterAligned)
            flywheel_z_backup.PWMPeriod = flywheel_z_ReadPeriod();
        #endif /* (!flywheel_z_PWMModeIsCenterAligned) */
        flywheel_z_backup.PWMUdb = flywheel_z_ReadCounter();
        #if (flywheel_z_UseStatus)
            flywheel_z_backup.InterruptMaskValue = flywheel_z_STATUS_MASK;
        #endif /* (flywheel_z_UseStatus) */

        #if(flywheel_z_DeadBandMode == flywheel_z__B_PWM__DBM_256_CLOCKS || \
            flywheel_z_DeadBandMode == flywheel_z__B_PWM__DBM_2_4_CLOCKS)
            flywheel_z_backup.PWMdeadBandValue = flywheel_z_ReadDeadTime();
        #endif /*  deadband count is either 2-4 clocks or 256 clocks */

        #if(flywheel_z_KillModeMinTime)
             flywheel_z_backup.PWMKillCounterPeriod = flywheel_z_ReadKillTime();
        #endif /* (flywheel_z_KillModeMinTime) */

        #if(flywheel_z_UseControl)
            flywheel_z_backup.PWMControlRegister = flywheel_z_ReadControlRegister();
        #endif /* (flywheel_z_UseControl) */
    #endif  /* (!flywheel_z_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: flywheel_z_RestoreConfig
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
*  flywheel_z_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void flywheel_z_RestoreConfig(void) 
{
        #if(!flywheel_z_UsingFixedFunction)
            #if(!flywheel_z_PWMModeIsCenterAligned)
                flywheel_z_WritePeriod(flywheel_z_backup.PWMPeriod);
            #endif /* (!flywheel_z_PWMModeIsCenterAligned) */

            flywheel_z_WriteCounter(flywheel_z_backup.PWMUdb);

            #if (flywheel_z_UseStatus)
                flywheel_z_STATUS_MASK = flywheel_z_backup.InterruptMaskValue;
            #endif /* (flywheel_z_UseStatus) */

            #if(flywheel_z_DeadBandMode == flywheel_z__B_PWM__DBM_256_CLOCKS || \
                flywheel_z_DeadBandMode == flywheel_z__B_PWM__DBM_2_4_CLOCKS)
                flywheel_z_WriteDeadTime(flywheel_z_backup.PWMdeadBandValue);
            #endif /* deadband count is either 2-4 clocks or 256 clocks */

            #if(flywheel_z_KillModeMinTime)
                flywheel_z_WriteKillTime(flywheel_z_backup.PWMKillCounterPeriod);
            #endif /* (flywheel_z_KillModeMinTime) */

            #if(flywheel_z_UseControl)
                flywheel_z_WriteControlRegister(flywheel_z_backup.PWMControlRegister);
            #endif /* (flywheel_z_UseControl) */
        #endif  /* (!flywheel_z_UsingFixedFunction) */
    }


/*******************************************************************************
* Function Name: flywheel_z_Sleep
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
*  flywheel_z_backup.PWMEnableState:  Is modified depending on the enable
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void flywheel_z_Sleep(void) 
{
    #if(flywheel_z_UseControl)
        if(flywheel_z_CTRL_ENABLE == (flywheel_z_CONTROL & flywheel_z_CTRL_ENABLE))
        {
            /*Component is enabled */
            flywheel_z_backup.PWMEnableState = 1u;
        }
        else
        {
            /* Component is disabled */
            flywheel_z_backup.PWMEnableState = 0u;
        }
    #endif /* (flywheel_z_UseControl) */

    /* Stop component */
    flywheel_z_Stop();

    /* Save registers configuration */
    flywheel_z_SaveConfig();
}


/*******************************************************************************
* Function Name: flywheel_z_Wakeup
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
*  flywheel_z_backup.pwmEnable:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void flywheel_z_Wakeup(void) 
{
     /* Restore registers values */
    flywheel_z_RestoreConfig();

    if(flywheel_z_backup.PWMEnableState != 0u)
    {
        /* Enable component's operation */
        flywheel_z_Enable();
    } /* Do nothing if component's block was disabled before */

}


/* [] END OF FILE */
