/*******************************************************************************
* File Name: blueART_PM.c
* Version 2.50
*
* Description:
*  This file provides Sleep/WakeUp APIs functionality.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "blueART.h"


/***************************************
* Local data allocation
***************************************/

static blueART_BACKUP_STRUCT  blueART_backup =
{
    /* enableState - disabled */
    0u,
};



/*******************************************************************************
* Function Name: blueART_SaveConfig
********************************************************************************
*
* Summary:
*  This function saves the component nonretention control register.
*  Does not save the FIFO which is a set of nonretention registers.
*  This function is called by the blueART_Sleep() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  blueART_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void blueART_SaveConfig(void)
{
    #if(blueART_CONTROL_REG_REMOVED == 0u)
        blueART_backup.cr = blueART_CONTROL_REG;
    #endif /* End blueART_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: blueART_RestoreConfig
********************************************************************************
*
* Summary:
*  Restores the nonretention control register except FIFO.
*  Does not restore the FIFO which is a set of nonretention registers.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  blueART_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
* Notes:
*  If this function is called without calling blueART_SaveConfig() 
*  first, the data loaded may be incorrect.
*
*******************************************************************************/
void blueART_RestoreConfig(void)
{
    #if(blueART_CONTROL_REG_REMOVED == 0u)
        blueART_CONTROL_REG = blueART_backup.cr;
    #endif /* End blueART_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: blueART_Sleep
********************************************************************************
*
* Summary:
*  This is the preferred API to prepare the component for sleep. 
*  The blueART_Sleep() API saves the current component state. Then it
*  calls the blueART_Stop() function and calls 
*  blueART_SaveConfig() to save the hardware configuration.
*  Call the blueART_Sleep() function before calling the CyPmSleep() 
*  or the CyPmHibernate() function. 
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  blueART_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void blueART_Sleep(void)
{
    #if(blueART_RX_ENABLED || blueART_HD_ENABLED)
        if((blueART_RXSTATUS_ACTL_REG  & blueART_INT_ENABLE) != 0u)
        {
            blueART_backup.enableState = 1u;
        }
        else
        {
            blueART_backup.enableState = 0u;
        }
    #else
        if((blueART_TXSTATUS_ACTL_REG  & blueART_INT_ENABLE) !=0u)
        {
            blueART_backup.enableState = 1u;
        }
        else
        {
            blueART_backup.enableState = 0u;
        }
    #endif /* End blueART_RX_ENABLED || blueART_HD_ENABLED*/

    blueART_Stop();
    blueART_SaveConfig();
}


/*******************************************************************************
* Function Name: blueART_Wakeup
********************************************************************************
*
* Summary:
*  This is the preferred API to restore the component to the state when 
*  blueART_Sleep() was called. The blueART_Wakeup() function
*  calls the blueART_RestoreConfig() function to restore the 
*  configuration. If the component was enabled before the 
*  blueART_Sleep() function was called, the blueART_Wakeup()
*  function will also re-enable the component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  blueART_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void blueART_Wakeup(void)
{
    blueART_RestoreConfig();
    #if( (blueART_RX_ENABLED) || (blueART_HD_ENABLED) )
        blueART_ClearRxBuffer();
    #endif /* End (blueART_RX_ENABLED) || (blueART_HD_ENABLED) */
    #if(blueART_TX_ENABLED || blueART_HD_ENABLED)
        blueART_ClearTxBuffer();
    #endif /* End blueART_TX_ENABLED || blueART_HD_ENABLED */

    if(blueART_backup.enableState != 0u)
    {
        blueART_Enable();
    }
}


/* [] END OF FILE */
