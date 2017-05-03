/*******************************************************************************
* File Name: fw1_PWM_PM.c
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

#include "fw1_PWM.h"

static fw1_PWM_backupStruct fw1_PWM_backup;


/*******************************************************************************
* Function Name: fw1_PWM_SaveConfig
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
*  fw1_PWM_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void fw1_PWM_SaveConfig(void) 
{

    #if(!fw1_PWM_UsingFixedFunction)
        #if(!fw1_PWM_PWMModeIsCenterAligned)
            fw1_PWM_backup.PWMPeriod = fw1_PWM_ReadPeriod();
        #endif /* (!fw1_PWM_PWMModeIsCenterAligned) */
        fw1_PWM_backup.PWMUdb = fw1_PWM_ReadCounter();
        #if (fw1_PWM_UseStatus)
            fw1_PWM_backup.InterruptMaskValue = fw1_PWM_STATUS_MASK;
        #endif /* (fw1_PWM_UseStatus) */

        #if(fw1_PWM_DeadBandMode == fw1_PWM__B_PWM__DBM_256_CLOCKS || \
            fw1_PWM_DeadBandMode == fw1_PWM__B_PWM__DBM_2_4_CLOCKS)
            fw1_PWM_backup.PWMdeadBandValue = fw1_PWM_ReadDeadTime();
        #endif /*  deadband count is either 2-4 clocks or 256 clocks */

        #if(fw1_PWM_KillModeMinTime)
             fw1_PWM_backup.PWMKillCounterPeriod = fw1_PWM_ReadKillTime();
        #endif /* (fw1_PWM_KillModeMinTime) */

        #if(fw1_PWM_UseControl)
            fw1_PWM_backup.PWMControlRegister = fw1_PWM_ReadControlRegister();
        #endif /* (fw1_PWM_UseControl) */
    #endif  /* (!fw1_PWM_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: fw1_PWM_RestoreConfig
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
*  fw1_PWM_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void fw1_PWM_RestoreConfig(void) 
{
        #if(!fw1_PWM_UsingFixedFunction)
            #if(!fw1_PWM_PWMModeIsCenterAligned)
                fw1_PWM_WritePeriod(fw1_PWM_backup.PWMPeriod);
            #endif /* (!fw1_PWM_PWMModeIsCenterAligned) */

            fw1_PWM_WriteCounter(fw1_PWM_backup.PWMUdb);

            #if (fw1_PWM_UseStatus)
                fw1_PWM_STATUS_MASK = fw1_PWM_backup.InterruptMaskValue;
            #endif /* (fw1_PWM_UseStatus) */

            #if(fw1_PWM_DeadBandMode == fw1_PWM__B_PWM__DBM_256_CLOCKS || \
                fw1_PWM_DeadBandMode == fw1_PWM__B_PWM__DBM_2_4_CLOCKS)
                fw1_PWM_WriteDeadTime(fw1_PWM_backup.PWMdeadBandValue);
            #endif /* deadband count is either 2-4 clocks or 256 clocks */

            #if(fw1_PWM_KillModeMinTime)
                fw1_PWM_WriteKillTime(fw1_PWM_backup.PWMKillCounterPeriod);
            #endif /* (fw1_PWM_KillModeMinTime) */

            #if(fw1_PWM_UseControl)
                fw1_PWM_WriteControlRegister(fw1_PWM_backup.PWMControlRegister);
            #endif /* (fw1_PWM_UseControl) */
        #endif  /* (!fw1_PWM_UsingFixedFunction) */
    }


/*******************************************************************************
* Function Name: fw1_PWM_Sleep
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
*  fw1_PWM_backup.PWMEnableState:  Is modified depending on the enable
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void fw1_PWM_Sleep(void) 
{
    #if(fw1_PWM_UseControl)
        if(fw1_PWM_CTRL_ENABLE == (fw1_PWM_CONTROL & fw1_PWM_CTRL_ENABLE))
        {
            /*Component is enabled */
            fw1_PWM_backup.PWMEnableState = 1u;
        }
        else
        {
            /* Component is disabled */
            fw1_PWM_backup.PWMEnableState = 0u;
        }
    #endif /* (fw1_PWM_UseControl) */

    /* Stop component */
    fw1_PWM_Stop();

    /* Save registers configuration */
    fw1_PWM_SaveConfig();
}


/*******************************************************************************
* Function Name: fw1_PWM_Wakeup
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
*  fw1_PWM_backup.pwmEnable:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void fw1_PWM_Wakeup(void) 
{
     /* Restore registers values */
    fw1_PWM_RestoreConfig();

    if(fw1_PWM_backup.PWMEnableState != 0u)
    {
        /* Enable component's operation */
        fw1_PWM_Enable();
    } /* Do nothing if component's block was disabled before */

}


/* [] END OF FILE */
