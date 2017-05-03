/*******************************************************************************
* File Name: fw2_PWM_PM.c
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

#include "fw2_PWM.h"

static fw2_PWM_backupStruct fw2_PWM_backup;


/*******************************************************************************
* Function Name: fw2_PWM_SaveConfig
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
*  fw2_PWM_backup:  Variables of this global structure are modified to
*  store the values of non retention configuration registers when Sleep() API is
*  called.
*
*******************************************************************************/
void fw2_PWM_SaveConfig(void) 
{

    #if(!fw2_PWM_UsingFixedFunction)
        #if(!fw2_PWM_PWMModeIsCenterAligned)
            fw2_PWM_backup.PWMPeriod = fw2_PWM_ReadPeriod();
        #endif /* (!fw2_PWM_PWMModeIsCenterAligned) */
        fw2_PWM_backup.PWMUdb = fw2_PWM_ReadCounter();
        #if (fw2_PWM_UseStatus)
            fw2_PWM_backup.InterruptMaskValue = fw2_PWM_STATUS_MASK;
        #endif /* (fw2_PWM_UseStatus) */

        #if(fw2_PWM_DeadBandMode == fw2_PWM__B_PWM__DBM_256_CLOCKS || \
            fw2_PWM_DeadBandMode == fw2_PWM__B_PWM__DBM_2_4_CLOCKS)
            fw2_PWM_backup.PWMdeadBandValue = fw2_PWM_ReadDeadTime();
        #endif /*  deadband count is either 2-4 clocks or 256 clocks */

        #if(fw2_PWM_KillModeMinTime)
             fw2_PWM_backup.PWMKillCounterPeriod = fw2_PWM_ReadKillTime();
        #endif /* (fw2_PWM_KillModeMinTime) */

        #if(fw2_PWM_UseControl)
            fw2_PWM_backup.PWMControlRegister = fw2_PWM_ReadControlRegister();
        #endif /* (fw2_PWM_UseControl) */
    #endif  /* (!fw2_PWM_UsingFixedFunction) */
}


/*******************************************************************************
* Function Name: fw2_PWM_RestoreConfig
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
*  fw2_PWM_backup:  Variables of this global structure are used to
*  restore the values of non retention registers on wakeup from sleep mode.
*
*******************************************************************************/
void fw2_PWM_RestoreConfig(void) 
{
        #if(!fw2_PWM_UsingFixedFunction)
            #if(!fw2_PWM_PWMModeIsCenterAligned)
                fw2_PWM_WritePeriod(fw2_PWM_backup.PWMPeriod);
            #endif /* (!fw2_PWM_PWMModeIsCenterAligned) */

            fw2_PWM_WriteCounter(fw2_PWM_backup.PWMUdb);

            #if (fw2_PWM_UseStatus)
                fw2_PWM_STATUS_MASK = fw2_PWM_backup.InterruptMaskValue;
            #endif /* (fw2_PWM_UseStatus) */

            #if(fw2_PWM_DeadBandMode == fw2_PWM__B_PWM__DBM_256_CLOCKS || \
                fw2_PWM_DeadBandMode == fw2_PWM__B_PWM__DBM_2_4_CLOCKS)
                fw2_PWM_WriteDeadTime(fw2_PWM_backup.PWMdeadBandValue);
            #endif /* deadband count is either 2-4 clocks or 256 clocks */

            #if(fw2_PWM_KillModeMinTime)
                fw2_PWM_WriteKillTime(fw2_PWM_backup.PWMKillCounterPeriod);
            #endif /* (fw2_PWM_KillModeMinTime) */

            #if(fw2_PWM_UseControl)
                fw2_PWM_WriteControlRegister(fw2_PWM_backup.PWMControlRegister);
            #endif /* (fw2_PWM_UseControl) */
        #endif  /* (!fw2_PWM_UsingFixedFunction) */
    }


/*******************************************************************************
* Function Name: fw2_PWM_Sleep
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
*  fw2_PWM_backup.PWMEnableState:  Is modified depending on the enable
*  state of the block before entering sleep mode.
*
*******************************************************************************/
void fw2_PWM_Sleep(void) 
{
    #if(fw2_PWM_UseControl)
        if(fw2_PWM_CTRL_ENABLE == (fw2_PWM_CONTROL & fw2_PWM_CTRL_ENABLE))
        {
            /*Component is enabled */
            fw2_PWM_backup.PWMEnableState = 1u;
        }
        else
        {
            /* Component is disabled */
            fw2_PWM_backup.PWMEnableState = 0u;
        }
    #endif /* (fw2_PWM_UseControl) */

    /* Stop component */
    fw2_PWM_Stop();

    /* Save registers configuration */
    fw2_PWM_SaveConfig();
}


/*******************************************************************************
* Function Name: fw2_PWM_Wakeup
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
*  fw2_PWM_backup.pwmEnable:  Is used to restore the enable state of
*  block on wakeup from sleep mode.
*
*******************************************************************************/
void fw2_PWM_Wakeup(void) 
{
     /* Restore registers values */
    fw2_PWM_RestoreConfig();

    if(fw2_PWM_backup.PWMEnableState != 0u)
    {
        /* Enable component's operation */
        fw2_PWM_Enable();
    } /* Do nothing if component's block was disabled before */

}


/* [] END OF FILE */
