/*******************************************************************************
* File Name: bluetooth_COM_PM.c
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

#include "bluetooth_COM.h"


/***************************************
* Local data allocation
***************************************/

static bluetooth_COM_BACKUP_STRUCT  bluetooth_COM_backup =
{
    /* enableState - disabled */
    0u,
};



/*******************************************************************************
* Function Name: bluetooth_COM_SaveConfig
********************************************************************************
*
* Summary:
*  This function saves the component nonretention control register.
*  Does not save the FIFO which is a set of nonretention registers.
*  This function is called by the bluetooth_COM_Sleep() function.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  bluetooth_COM_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void bluetooth_COM_SaveConfig(void)
{
    #if(bluetooth_COM_CONTROL_REG_REMOVED == 0u)
        bluetooth_COM_backup.cr = bluetooth_COM_CONTROL_REG;
    #endif /* End bluetooth_COM_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: bluetooth_COM_RestoreConfig
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
*  bluetooth_COM_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
* Notes:
*  If this function is called without calling bluetooth_COM_SaveConfig() 
*  first, the data loaded may be incorrect.
*
*******************************************************************************/
void bluetooth_COM_RestoreConfig(void)
{
    #if(bluetooth_COM_CONTROL_REG_REMOVED == 0u)
        bluetooth_COM_CONTROL_REG = bluetooth_COM_backup.cr;
    #endif /* End bluetooth_COM_CONTROL_REG_REMOVED */
}


/*******************************************************************************
* Function Name: bluetooth_COM_Sleep
********************************************************************************
*
* Summary:
*  This is the preferred API to prepare the component for sleep. 
*  The bluetooth_COM_Sleep() API saves the current component state. Then it
*  calls the bluetooth_COM_Stop() function and calls 
*  bluetooth_COM_SaveConfig() to save the hardware configuration.
*  Call the bluetooth_COM_Sleep() function before calling the CyPmSleep() 
*  or the CyPmHibernate() function. 
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  bluetooth_COM_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void bluetooth_COM_Sleep(void)
{
    #if(bluetooth_COM_RX_ENABLED || bluetooth_COM_HD_ENABLED)
        if((bluetooth_COM_RXSTATUS_ACTL_REG  & bluetooth_COM_INT_ENABLE) != 0u)
        {
            bluetooth_COM_backup.enableState = 1u;
        }
        else
        {
            bluetooth_COM_backup.enableState = 0u;
        }
    #else
        if((bluetooth_COM_TXSTATUS_ACTL_REG  & bluetooth_COM_INT_ENABLE) !=0u)
        {
            bluetooth_COM_backup.enableState = 1u;
        }
        else
        {
            bluetooth_COM_backup.enableState = 0u;
        }
    #endif /* End bluetooth_COM_RX_ENABLED || bluetooth_COM_HD_ENABLED*/

    bluetooth_COM_Stop();
    bluetooth_COM_SaveConfig();
}


/*******************************************************************************
* Function Name: bluetooth_COM_Wakeup
********************************************************************************
*
* Summary:
*  This is the preferred API to restore the component to the state when 
*  bluetooth_COM_Sleep() was called. The bluetooth_COM_Wakeup() function
*  calls the bluetooth_COM_RestoreConfig() function to restore the 
*  configuration. If the component was enabled before the 
*  bluetooth_COM_Sleep() function was called, the bluetooth_COM_Wakeup()
*  function will also re-enable the component.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  bluetooth_COM_backup - used when non-retention registers are restored.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void bluetooth_COM_Wakeup(void)
{
    bluetooth_COM_RestoreConfig();
    #if( (bluetooth_COM_RX_ENABLED) || (bluetooth_COM_HD_ENABLED) )
        bluetooth_COM_ClearRxBuffer();
    #endif /* End (bluetooth_COM_RX_ENABLED) || (bluetooth_COM_HD_ENABLED) */
    #if(bluetooth_COM_TX_ENABLED || bluetooth_COM_HD_ENABLED)
        bluetooth_COM_ClearTxBuffer();
    #endif /* End bluetooth_COM_TX_ENABLED || bluetooth_COM_HD_ENABLED */

    if(bluetooth_COM_backup.enableState != 0u)
    {
        bluetooth_COM_Enable();
    }
}


/* [] END OF FILE */
