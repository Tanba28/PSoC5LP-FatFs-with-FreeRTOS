/*******************************************************************************
* File Name: SPIM_SD_INT.c
* Version 2.50
*
* Description:
*  This file provides all Interrupt Service Routine (ISR) for the SPI Master
*  component.
*
* Note:
*  None.
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#include "SPIM_SD_PVT.h"
#include "cyapicallbacks.h"

/* User code required at start of ISR */
/* `#START SPIM_SD_ISR_START_DEF` */

/* `#END` */


/*******************************************************************************
* Function Name: SPIM_SD_TX_ISR
********************************************************************************
*
* Summary:
*  Interrupt Service Routine for TX portion of the SPI Master.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  SPIM_SD_txBufferWrite - used for the account of the bytes which
*  have been written down in the TX software buffer.
*  SPIM_SD_txBufferRead - used for the account of the bytes which
*  have been read from the TX software buffer, modified when exist data to
*  sending and FIFO Not Full.
*  SPIM_SD_txBuffer[SPIM_SD_TX_BUFFER_SIZE] - used to store
*  data to sending.
*  All described above Global variables are used when Software Buffer is used.
*
*******************************************************************************/
CY_ISR(SPIM_SD_TX_ISR)
{
    #if(SPIM_SD_TX_SOFTWARE_BUF_ENABLED)
        uint8 tmpStatus;
    #endif /* (SPIM_SD_TX_SOFTWARE_BUF_ENABLED) */

    #ifdef SPIM_SD_TX_ISR_ENTRY_CALLBACK
        SPIM_SD_TX_ISR_EntryCallback();
    #endif /* SPIM_SD_TX_ISR_ENTRY_CALLBACK */

    /* User code required at start of ISR */
    /* `#START SPIM_SD_TX_ISR_START` */

    /* `#END` */
    
    #if(SPIM_SD_TX_SOFTWARE_BUF_ENABLED)
        /* Check if TX data buffer is not empty and there is space in TX FIFO */
        while(SPIM_SD_txBufferRead != SPIM_SD_txBufferWrite)
        {
            tmpStatus = SPIM_SD_GET_STATUS_TX(SPIM_SD_swStatusTx);
            SPIM_SD_swStatusTx = tmpStatus;

            if(0u != (SPIM_SD_swStatusTx & SPIM_SD_STS_TX_FIFO_NOT_FULL))
            {
                if(0u == SPIM_SD_txBufferFull)
                {
                   SPIM_SD_txBufferRead++;

                    if(SPIM_SD_txBufferRead >= SPIM_SD_TX_BUFFER_SIZE)
                    {
                        SPIM_SD_txBufferRead = 0u;
                    }
                }
                else
                {
                    SPIM_SD_txBufferFull = 0u;
                }

                /* Put data element into the TX FIFO */
                CY_SET_REG8(SPIM_SD_TXDATA_PTR, 
                                             SPIM_SD_txBuffer[SPIM_SD_txBufferRead]);
            }
            else
            {
                break;
            }
        }

        if(SPIM_SD_txBufferRead == SPIM_SD_txBufferWrite)
        {
            /* TX Buffer is EMPTY: disable interrupt on TX NOT FULL */
            SPIM_SD_TX_STATUS_MASK_REG &= ((uint8) ~SPIM_SD_STS_TX_FIFO_NOT_FULL);
        }

    #endif /* (SPIM_SD_TX_SOFTWARE_BUF_ENABLED) */

    /* User code required at end of ISR (Optional) */
    /* `#START SPIM_SD_TX_ISR_END` */

    /* `#END` */
    
    #ifdef SPIM_SD_TX_ISR_EXIT_CALLBACK
        SPIM_SD_TX_ISR_ExitCallback();
    #endif /* SPIM_SD_TX_ISR_EXIT_CALLBACK */
}


/*******************************************************************************
* Function Name: SPIM_SD_RX_ISR
********************************************************************************
*
* Summary:
*  Interrupt Service Routine for RX portion of the SPI Master.
*
* Parameters:
*  None.
*
* Return:
*  None.
*
* Global variables:
*  SPIM_SD_rxBufferWrite - used for the account of the bytes which
*  have been written down in the RX software buffer modified when FIFO contains
*  new data.
*  SPIM_SD_rxBufferRead - used for the account of the bytes which
*  have been read from the RX software buffer, modified when overflow occurred.
*  SPIM_SD_rxBuffer[SPIM_SD_RX_BUFFER_SIZE] - used to store
*  received data, modified when FIFO contains new data.
*  All described above Global variables are used when Software Buffer is used.
*
*******************************************************************************/
CY_ISR(SPIM_SD_RX_ISR)
{
    #if(SPIM_SD_RX_SOFTWARE_BUF_ENABLED)
        uint8 tmpStatus;
        uint8 rxData;
    #endif /* (SPIM_SD_RX_SOFTWARE_BUF_ENABLED) */

    #ifdef SPIM_SD_RX_ISR_ENTRY_CALLBACK
        SPIM_SD_RX_ISR_EntryCallback();
    #endif /* SPIM_SD_RX_ISR_ENTRY_CALLBACK */

    /* User code required at start of ISR */
    /* `#START SPIM_SD_RX_ISR_START` */

    /* `#END` */
    
    #if(SPIM_SD_RX_SOFTWARE_BUF_ENABLED)

        tmpStatus = SPIM_SD_GET_STATUS_RX(SPIM_SD_swStatusRx);
        SPIM_SD_swStatusRx = tmpStatus;

        /* Check if RX data FIFO has some data to be moved into the RX Buffer */
        while(0u != (SPIM_SD_swStatusRx & SPIM_SD_STS_RX_FIFO_NOT_EMPTY))
        {
            rxData = CY_GET_REG8(SPIM_SD_RXDATA_PTR);

            /* Set next pointer. */
            SPIM_SD_rxBufferWrite++;
            if(SPIM_SD_rxBufferWrite >= SPIM_SD_RX_BUFFER_SIZE)
            {
                SPIM_SD_rxBufferWrite = 0u;
            }

            if(SPIM_SD_rxBufferWrite == SPIM_SD_rxBufferRead)
            {
                SPIM_SD_rxBufferRead++;
                if(SPIM_SD_rxBufferRead >= SPIM_SD_RX_BUFFER_SIZE)
                {
                    SPIM_SD_rxBufferRead = 0u;
                }

                SPIM_SD_rxBufferFull = 1u;
            }

            /* Move data from the FIFO to the Buffer */
            SPIM_SD_rxBuffer[SPIM_SD_rxBufferWrite] = rxData;

            tmpStatus = SPIM_SD_GET_STATUS_RX(SPIM_SD_swStatusRx);
            SPIM_SD_swStatusRx = tmpStatus;
        }

    #endif /* (SPIM_SD_RX_SOFTWARE_BUF_ENABLED) */

    /* User code required at end of ISR (Optional) */
    /* `#START SPIM_SD_RX_ISR_END` */

    /* `#END` */
    
    #ifdef SPIM_SD_RX_ISR_EXIT_CALLBACK
        SPIM_SD_RX_ISR_ExitCallback();
    #endif /* SPIM_SD_RX_ISR_EXIT_CALLBACK */
}

/* [] END OF FILE */
