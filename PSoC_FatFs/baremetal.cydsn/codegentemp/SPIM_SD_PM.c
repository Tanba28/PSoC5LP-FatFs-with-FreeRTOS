/*******************************************************************************
* File Name: SPIM_SD_PM.c
* Version 2.50
*
* Description:
*  This file contains the setup, control and status commands to support
*  component operations in low power mode.
*
* Note:
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "SPIM_SD_PVT.h"

static SPIM_SD_BACKUP_STRUCT SPIM_SD_backup =
{
    SPIM_SD_DISABLED,
    SPIM_SD_BITCTR_INIT,
};


/*******************************************************************************
* Function Name: SPIM_SD_SaveConfig
********************************************************************************
*
* Summary:
*  Empty function. Included for consistency with other components.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void SPIM_SD_SaveConfig(void) 
{

}


/*******************************************************************************
* Function Name: SPIM_SD_RestoreConfig
********************************************************************************
*
* Summary:
*  Empty function. Included for consistency with other components.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
*******************************************************************************/
void SPIM_SD_RestoreConfig(void) 
{

}


/*******************************************************************************
* Function Name: SPIM_SD_Sleep
********************************************************************************
*
* Summary:
*  Prepare SPIM Component goes to sleep.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  SPIM_SD_backup - modified when non-retention registers are saved.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SPIM_SD_Sleep(void) 
{
    /* Save components enable state */
    SPIM_SD_backup.enableState = ((uint8) SPIM_SD_IS_ENABLED);

    SPIM_SD_Stop();
}


/*******************************************************************************
* Function Name: SPIM_SD_Wakeup
********************************************************************************
*
* Summary:
*  Prepare SPIM Component to wake up.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global Variables:
*  SPIM_SD_backup - used when non-retention registers are restored.
*  SPIM_SD_txBufferWrite - modified every function call - resets to
*  zero.
*  SPIM_SD_txBufferRead - modified every function call - resets to
*  zero.
*  SPIM_SD_rxBufferWrite - modified every function call - resets to
*  zero.
*  SPIM_SD_rxBufferRead - modified every function call - resets to
*  zero.
*
* Reentrant:
*  No.
*
*******************************************************************************/
void SPIM_SD_Wakeup(void) 
{
    #if(SPIM_SD_RX_SOFTWARE_BUF_ENABLED)
        SPIM_SD_rxBufferFull  = 0u;
        SPIM_SD_rxBufferRead  = 0u;
        SPIM_SD_rxBufferWrite = 0u;
    #endif /* (SPIM_SD_RX_SOFTWARE_BUF_ENABLED) */

    #if(SPIM_SD_TX_SOFTWARE_BUF_ENABLED)
        SPIM_SD_txBufferFull  = 0u;
        SPIM_SD_txBufferRead  = 0u;
        SPIM_SD_txBufferWrite = 0u;
    #endif /* (SPIM_SD_TX_SOFTWARE_BUF_ENABLED) */

    /* Clear any data from the RX and TX FIFO */
    SPIM_SD_ClearFIFO();

    /* Restore components block enable state */
    if(0u != SPIM_SD_backup.enableState)
    {
        SPIM_SD_Enable();
    }
}


/* [] END OF FILE */
