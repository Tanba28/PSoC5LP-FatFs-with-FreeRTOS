/*******************************************************************************
* File Name: SPIM_SD.h
* Version 2.50
*
* Description:
*  Contains the function prototypes, constants and register definition
*  of the SPI Master Component.
*
* Note:
*  None
*
********************************************************************************
* Copyright 2008-2015, Cypress Semiconductor Corporation.  All rights reserved.
* You may use this file only in accordance with the license, terms, conditions,
* disclaimers, and limitations in the end user license agreement accompanying
* the software package with which this file was provided.
*******************************************************************************/

#if !defined(CY_SPIM_SPIM_SD_H)
#define CY_SPIM_SPIM_SD_H

#include "cyfitter.h"
#include "cytypes.h"
#include "CyLib.h" /* For CyEnterCriticalSection() and CyExitCriticalSection() functions */


/***************************************
*   Conditional Compilation Parameters
***************************************/

#define SPIM_SD_INTERNAL_CLOCK             (0u)

#if(0u != SPIM_SD_INTERNAL_CLOCK)
    #include "SPIM_SD_IntClock.h"
#endif /* (0u != SPIM_SD_INTERNAL_CLOCK) */

#define SPIM_SD_MODE                       (1u)
#define SPIM_SD_DATA_WIDTH                 (8u)
#define SPIM_SD_MODE_USE_ZERO              (1u)
#define SPIM_SD_BIDIRECTIONAL_MODE         (0u)

/* Internal interrupt handling */
#define SPIM_SD_TX_BUFFER_SIZE             (4u)
#define SPIM_SD_RX_BUFFER_SIZE             (4u)
#define SPIM_SD_INTERNAL_TX_INT_ENABLED    (0u)
#define SPIM_SD_INTERNAL_RX_INT_ENABLED    (0u)

#define SPIM_SD_SINGLE_REG_SIZE            (8u)
#define SPIM_SD_USE_SECOND_DATAPATH        (SPIM_SD_DATA_WIDTH > SPIM_SD_SINGLE_REG_SIZE)

#define SPIM_SD_FIFO_SIZE                  (4u)
#define SPIM_SD_TX_SOFTWARE_BUF_ENABLED    ((0u != SPIM_SD_INTERNAL_TX_INT_ENABLED) && \
                                                     (SPIM_SD_TX_BUFFER_SIZE > SPIM_SD_FIFO_SIZE))

#define SPIM_SD_RX_SOFTWARE_BUF_ENABLED    ((0u != SPIM_SD_INTERNAL_RX_INT_ENABLED) && \
                                                     (SPIM_SD_RX_BUFFER_SIZE > SPIM_SD_FIFO_SIZE))


/***************************************
*        Data Struct Definition
***************************************/

/* Sleep Mode API Support */
typedef struct
{
    uint8 enableState;
    uint8 cntrPeriod;
} SPIM_SD_BACKUP_STRUCT;


/***************************************
*        Function Prototypes
***************************************/

void  SPIM_SD_Init(void)                           ;
void  SPIM_SD_Enable(void)                         ;
void  SPIM_SD_Start(void)                          ;
void  SPIM_SD_Stop(void)                           ;

void  SPIM_SD_EnableTxInt(void)                    ;
void  SPIM_SD_EnableRxInt(void)                    ;
void  SPIM_SD_DisableTxInt(void)                   ;
void  SPIM_SD_DisableRxInt(void)                   ;

void  SPIM_SD_Sleep(void)                          ;
void  SPIM_SD_Wakeup(void)                         ;
void  SPIM_SD_SaveConfig(void)                     ;
void  SPIM_SD_RestoreConfig(void)                  ;

void  SPIM_SD_SetTxInterruptMode(uint8 intSrc)     ;
void  SPIM_SD_SetRxInterruptMode(uint8 intSrc)     ;
uint8 SPIM_SD_ReadTxStatus(void)                   ;
uint8 SPIM_SD_ReadRxStatus(void)                   ;
void  SPIM_SD_WriteTxData(uint8 txData)  \
                                                            ;
uint8 SPIM_SD_ReadRxData(void) \
                                                            ;
uint8 SPIM_SD_GetRxBufferSize(void)                ;
uint8 SPIM_SD_GetTxBufferSize(void)                ;
void  SPIM_SD_ClearRxBuffer(void)                  ;
void  SPIM_SD_ClearTxBuffer(void)                  ;
void  SPIM_SD_ClearFIFO(void)                              ;
void  SPIM_SD_PutArray(const uint8 buffer[], uint8 byteCount) \
                                                            ;

#if(0u != SPIM_SD_BIDIRECTIONAL_MODE)
    void  SPIM_SD_TxEnable(void)                   ;
    void  SPIM_SD_TxDisable(void)                  ;
#endif /* (0u != SPIM_SD_BIDIRECTIONAL_MODE) */

CY_ISR_PROTO(SPIM_SD_TX_ISR);
CY_ISR_PROTO(SPIM_SD_RX_ISR);


/***************************************
*   Variable with external linkage
***************************************/

extern uint8 SPIM_SD_initVar;


/***************************************
*           API Constants
***************************************/

#define SPIM_SD_TX_ISR_NUMBER     ((uint8) (SPIM_SD_TxInternalInterrupt__INTC_NUMBER))
#define SPIM_SD_RX_ISR_NUMBER     ((uint8) (SPIM_SD_RxInternalInterrupt__INTC_NUMBER))

#define SPIM_SD_TX_ISR_PRIORITY   ((uint8) (SPIM_SD_TxInternalInterrupt__INTC_PRIOR_NUM))
#define SPIM_SD_RX_ISR_PRIORITY   ((uint8) (SPIM_SD_RxInternalInterrupt__INTC_PRIOR_NUM))


/***************************************
*    Initial Parameter Constants
***************************************/

#define SPIM_SD_INT_ON_SPI_DONE    ((uint8) (0u   << SPIM_SD_STS_SPI_DONE_SHIFT))
#define SPIM_SD_INT_ON_TX_EMPTY    ((uint8) (0u   << SPIM_SD_STS_TX_FIFO_EMPTY_SHIFT))
#define SPIM_SD_INT_ON_TX_NOT_FULL ((uint8) (0u << \
                                                                           SPIM_SD_STS_TX_FIFO_NOT_FULL_SHIFT))
#define SPIM_SD_INT_ON_BYTE_COMP   ((uint8) (0u  << SPIM_SD_STS_BYTE_COMPLETE_SHIFT))
#define SPIM_SD_INT_ON_SPI_IDLE    ((uint8) (0u   << SPIM_SD_STS_SPI_IDLE_SHIFT))

/* Disable TX_NOT_FULL if software buffer is used */
#define SPIM_SD_INT_ON_TX_NOT_FULL_DEF ((SPIM_SD_TX_SOFTWARE_BUF_ENABLED) ? \
                                                                        (0u) : (SPIM_SD_INT_ON_TX_NOT_FULL))

/* TX interrupt mask */
#define SPIM_SD_TX_INIT_INTERRUPTS_MASK    (SPIM_SD_INT_ON_SPI_DONE  | \
                                                     SPIM_SD_INT_ON_TX_EMPTY  | \
                                                     SPIM_SD_INT_ON_TX_NOT_FULL_DEF | \
                                                     SPIM_SD_INT_ON_BYTE_COMP | \
                                                     SPIM_SD_INT_ON_SPI_IDLE)

#define SPIM_SD_INT_ON_RX_FULL         ((uint8) (0u << \
                                                                          SPIM_SD_STS_RX_FIFO_FULL_SHIFT))
#define SPIM_SD_INT_ON_RX_NOT_EMPTY    ((uint8) (0u << \
                                                                          SPIM_SD_STS_RX_FIFO_NOT_EMPTY_SHIFT))
#define SPIM_SD_INT_ON_RX_OVER         ((uint8) (0u << \
                                                                          SPIM_SD_STS_RX_FIFO_OVERRUN_SHIFT))

/* RX interrupt mask */
#define SPIM_SD_RX_INIT_INTERRUPTS_MASK    (SPIM_SD_INT_ON_RX_FULL      | \
                                                     SPIM_SD_INT_ON_RX_NOT_EMPTY | \
                                                     SPIM_SD_INT_ON_RX_OVER)
/* Nubmer of bits to receive/transmit */
#define SPIM_SD_BITCTR_INIT            (((uint8) (SPIM_SD_DATA_WIDTH << 1u)) - 1u)


/***************************************
*             Registers
***************************************/
#if(CY_PSOC3 || CY_PSOC5)
    #define SPIM_SD_TXDATA_REG (* (reg8 *) \
                                                SPIM_SD_BSPIM_sR8_Dp_u0__F0_REG)
    #define SPIM_SD_TXDATA_PTR (  (reg8 *) \
                                                SPIM_SD_BSPIM_sR8_Dp_u0__F0_REG)
    #define SPIM_SD_RXDATA_REG (* (reg8 *) \
                                                SPIM_SD_BSPIM_sR8_Dp_u0__F1_REG)
    #define SPIM_SD_RXDATA_PTR (  (reg8 *) \
                                                SPIM_SD_BSPIM_sR8_Dp_u0__F1_REG)
#else   /* PSOC4 */
    #if(SPIM_SD_USE_SECOND_DATAPATH)
        #define SPIM_SD_TXDATA_REG (* (reg16 *) \
                                          SPIM_SD_BSPIM_sR8_Dp_u0__16BIT_F0_REG)
        #define SPIM_SD_TXDATA_PTR (  (reg16 *) \
                                          SPIM_SD_BSPIM_sR8_Dp_u0__16BIT_F0_REG)
        #define SPIM_SD_RXDATA_REG (* (reg16 *) \
                                          SPIM_SD_BSPIM_sR8_Dp_u0__16BIT_F1_REG)
        #define SPIM_SD_RXDATA_PTR (  (reg16 *) \
                                          SPIM_SD_BSPIM_sR8_Dp_u0__16BIT_F1_REG)
    #else
        #define SPIM_SD_TXDATA_REG (* (reg8 *) \
                                                SPIM_SD_BSPIM_sR8_Dp_u0__F0_REG)
        #define SPIM_SD_TXDATA_PTR (  (reg8 *) \
                                                SPIM_SD_BSPIM_sR8_Dp_u0__F0_REG)
        #define SPIM_SD_RXDATA_REG (* (reg8 *) \
                                                SPIM_SD_BSPIM_sR8_Dp_u0__F1_REG)
        #define SPIM_SD_RXDATA_PTR (  (reg8 *) \
                                                SPIM_SD_BSPIM_sR8_Dp_u0__F1_REG)
    #endif /* (SPIM_SD_USE_SECOND_DATAPATH) */
#endif     /* (CY_PSOC3 || CY_PSOC5) */

#define SPIM_SD_AUX_CONTROL_DP0_REG (* (reg8 *) \
                                        SPIM_SD_BSPIM_sR8_Dp_u0__DP_AUX_CTL_REG)
#define SPIM_SD_AUX_CONTROL_DP0_PTR (  (reg8 *) \
                                        SPIM_SD_BSPIM_sR8_Dp_u0__DP_AUX_CTL_REG)

#if(SPIM_SD_USE_SECOND_DATAPATH)
    #define SPIM_SD_AUX_CONTROL_DP1_REG  (* (reg8 *) \
                                        SPIM_SD_BSPIM_sR8_Dp_u1__DP_AUX_CTL_REG)
    #define SPIM_SD_AUX_CONTROL_DP1_PTR  (  (reg8 *) \
                                        SPIM_SD_BSPIM_sR8_Dp_u1__DP_AUX_CTL_REG)
#endif /* (SPIM_SD_USE_SECOND_DATAPATH) */

#define SPIM_SD_COUNTER_PERIOD_REG     (* (reg8 *) SPIM_SD_BSPIM_BitCounter__PERIOD_REG)
#define SPIM_SD_COUNTER_PERIOD_PTR     (  (reg8 *) SPIM_SD_BSPIM_BitCounter__PERIOD_REG)
#define SPIM_SD_COUNTER_CONTROL_REG    (* (reg8 *) SPIM_SD_BSPIM_BitCounter__CONTROL_AUX_CTL_REG)
#define SPIM_SD_COUNTER_CONTROL_PTR    (  (reg8 *) SPIM_SD_BSPIM_BitCounter__CONTROL_AUX_CTL_REG)

#define SPIM_SD_TX_STATUS_REG          (* (reg8 *) SPIM_SD_BSPIM_TxStsReg__STATUS_REG)
#define SPIM_SD_TX_STATUS_PTR          (  (reg8 *) SPIM_SD_BSPIM_TxStsReg__STATUS_REG)
#define SPIM_SD_RX_STATUS_REG          (* (reg8 *) SPIM_SD_BSPIM_RxStsReg__STATUS_REG)
#define SPIM_SD_RX_STATUS_PTR          (  (reg8 *) SPIM_SD_BSPIM_RxStsReg__STATUS_REG)

#define SPIM_SD_CONTROL_REG            (* (reg8 *) \
                                      SPIM_SD_BSPIM_BidirMode_CtrlReg__CONTROL_REG)
#define SPIM_SD_CONTROL_PTR            (  (reg8 *) \
                                      SPIM_SD_BSPIM_BidirMode_CtrlReg__CONTROL_REG)

#define SPIM_SD_TX_STATUS_MASK_REG     (* (reg8 *) SPIM_SD_BSPIM_TxStsReg__MASK_REG)
#define SPIM_SD_TX_STATUS_MASK_PTR     (  (reg8 *) SPIM_SD_BSPIM_TxStsReg__MASK_REG)
#define SPIM_SD_RX_STATUS_MASK_REG     (* (reg8 *) SPIM_SD_BSPIM_RxStsReg__MASK_REG)
#define SPIM_SD_RX_STATUS_MASK_PTR     (  (reg8 *) SPIM_SD_BSPIM_RxStsReg__MASK_REG)

#define SPIM_SD_TX_STATUS_ACTL_REG     (* (reg8 *) SPIM_SD_BSPIM_TxStsReg__STATUS_AUX_CTL_REG)
#define SPIM_SD_TX_STATUS_ACTL_PTR     (  (reg8 *) SPIM_SD_BSPIM_TxStsReg__STATUS_AUX_CTL_REG)
#define SPIM_SD_RX_STATUS_ACTL_REG     (* (reg8 *) SPIM_SD_BSPIM_RxStsReg__STATUS_AUX_CTL_REG)
#define SPIM_SD_RX_STATUS_ACTL_PTR     (  (reg8 *) SPIM_SD_BSPIM_RxStsReg__STATUS_AUX_CTL_REG)

#if(SPIM_SD_USE_SECOND_DATAPATH)
    #define SPIM_SD_AUX_CONTROLDP1     (SPIM_SD_AUX_CONTROL_DP1_REG)
#endif /* (SPIM_SD_USE_SECOND_DATAPATH) */


/***************************************
*       Register Constants
***************************************/

/* Status Register Definitions */
#define SPIM_SD_STS_SPI_DONE_SHIFT             (0x00u)
#define SPIM_SD_STS_TX_FIFO_EMPTY_SHIFT        (0x01u)
#define SPIM_SD_STS_TX_FIFO_NOT_FULL_SHIFT     (0x02u)
#define SPIM_SD_STS_BYTE_COMPLETE_SHIFT        (0x03u)
#define SPIM_SD_STS_SPI_IDLE_SHIFT             (0x04u)
#define SPIM_SD_STS_RX_FIFO_FULL_SHIFT         (0x04u)
#define SPIM_SD_STS_RX_FIFO_NOT_EMPTY_SHIFT    (0x05u)
#define SPIM_SD_STS_RX_FIFO_OVERRUN_SHIFT      (0x06u)

#define SPIM_SD_STS_SPI_DONE           ((uint8) (0x01u << SPIM_SD_STS_SPI_DONE_SHIFT))
#define SPIM_SD_STS_TX_FIFO_EMPTY      ((uint8) (0x01u << SPIM_SD_STS_TX_FIFO_EMPTY_SHIFT))
#define SPIM_SD_STS_TX_FIFO_NOT_FULL   ((uint8) (0x01u << SPIM_SD_STS_TX_FIFO_NOT_FULL_SHIFT))
#define SPIM_SD_STS_BYTE_COMPLETE      ((uint8) (0x01u << SPIM_SD_STS_BYTE_COMPLETE_SHIFT))
#define SPIM_SD_STS_SPI_IDLE           ((uint8) (0x01u << SPIM_SD_STS_SPI_IDLE_SHIFT))
#define SPIM_SD_STS_RX_FIFO_FULL       ((uint8) (0x01u << SPIM_SD_STS_RX_FIFO_FULL_SHIFT))
#define SPIM_SD_STS_RX_FIFO_NOT_EMPTY  ((uint8) (0x01u << SPIM_SD_STS_RX_FIFO_NOT_EMPTY_SHIFT))
#define SPIM_SD_STS_RX_FIFO_OVERRUN    ((uint8) (0x01u << SPIM_SD_STS_RX_FIFO_OVERRUN_SHIFT))

/* TX and RX masks for clear on read bits */
#define SPIM_SD_TX_STS_CLR_ON_RD_BYTES_MASK    (0x09u)
#define SPIM_SD_RX_STS_CLR_ON_RD_BYTES_MASK    (0x40u)

/* StatusI Register Interrupt Enable Control Bits */
/* As defined by the Register map for the AUX Control Register */
#define SPIM_SD_INT_ENABLE     (0x10u) /* Enable interrupt from statusi */
#define SPIM_SD_TX_FIFO_CLR    (0x01u) /* F0 - TX FIFO */
#define SPIM_SD_RX_FIFO_CLR    (0x02u) /* F1 - RX FIFO */
#define SPIM_SD_FIFO_CLR       (SPIM_SD_TX_FIFO_CLR | SPIM_SD_RX_FIFO_CLR)

/* Bit Counter (7-bit) Control Register Bit Definitions */
/* As defined by the Register map for the AUX Control Register */
#define SPIM_SD_CNTR_ENABLE    (0x20u) /* Enable CNT7 */

/* Bi-Directional mode control bit */
#define SPIM_SD_CTRL_TX_SIGNAL_EN  (0x01u)

/* Datapath Auxillary Control Register definitions */
#define SPIM_SD_AUX_CTRL_FIFO0_CLR         (0x01u)
#define SPIM_SD_AUX_CTRL_FIFO1_CLR         (0x02u)
#define SPIM_SD_AUX_CTRL_FIFO0_LVL         (0x04u)
#define SPIM_SD_AUX_CTRL_FIFO1_LVL         (0x08u)
#define SPIM_SD_STATUS_ACTL_INT_EN_MASK    (0x10u)

/* Component disabled */
#define SPIM_SD_DISABLED   (0u)


/***************************************
*       Macros
***************************************/

/* Returns true if componentn enabled */
#define SPIM_SD_IS_ENABLED (0u != (SPIM_SD_TX_STATUS_ACTL_REG & SPIM_SD_INT_ENABLE))

/* Retuns TX status register */
#define SPIM_SD_GET_STATUS_TX(swTxSts) ( (uint8)(SPIM_SD_TX_STATUS_REG | \
                                                          ((swTxSts) & SPIM_SD_TX_STS_CLR_ON_RD_BYTES_MASK)) )
/* Retuns RX status register */
#define SPIM_SD_GET_STATUS_RX(swRxSts) ( (uint8)(SPIM_SD_RX_STATUS_REG | \
                                                          ((swRxSts) & SPIM_SD_RX_STS_CLR_ON_RD_BYTES_MASK)) )


/***************************************
* The following code is DEPRECATED and 
* should not be used in new projects.
***************************************/

#define SPIM_SD_WriteByte   SPIM_SD_WriteTxData
#define SPIM_SD_ReadByte    SPIM_SD_ReadRxData
void  SPIM_SD_SetInterruptMode(uint8 intSrc)       ;
uint8 SPIM_SD_ReadStatus(void)                     ;
void  SPIM_SD_EnableInt(void)                      ;
void  SPIM_SD_DisableInt(void)                     ;

#define SPIM_SD_TXDATA                 (SPIM_SD_TXDATA_REG)
#define SPIM_SD_RXDATA                 (SPIM_SD_RXDATA_REG)
#define SPIM_SD_AUX_CONTROLDP0         (SPIM_SD_AUX_CONTROL_DP0_REG)
#define SPIM_SD_TXBUFFERREAD           (SPIM_SD_txBufferRead)
#define SPIM_SD_TXBUFFERWRITE          (SPIM_SD_txBufferWrite)
#define SPIM_SD_RXBUFFERREAD           (SPIM_SD_rxBufferRead)
#define SPIM_SD_RXBUFFERWRITE          (SPIM_SD_rxBufferWrite)

#define SPIM_SD_COUNTER_PERIOD         (SPIM_SD_COUNTER_PERIOD_REG)
#define SPIM_SD_COUNTER_CONTROL        (SPIM_SD_COUNTER_CONTROL_REG)
#define SPIM_SD_STATUS                 (SPIM_SD_TX_STATUS_REG)
#define SPIM_SD_CONTROL                (SPIM_SD_CONTROL_REG)
#define SPIM_SD_STATUS_MASK            (SPIM_SD_TX_STATUS_MASK_REG)
#define SPIM_SD_STATUS_ACTL            (SPIM_SD_TX_STATUS_ACTL_REG)

#define SPIM_SD_INIT_INTERRUPTS_MASK  (SPIM_SD_INT_ON_SPI_DONE     | \
                                                SPIM_SD_INT_ON_TX_EMPTY     | \
                                                SPIM_SD_INT_ON_TX_NOT_FULL_DEF  | \
                                                SPIM_SD_INT_ON_RX_FULL      | \
                                                SPIM_SD_INT_ON_RX_NOT_EMPTY | \
                                                SPIM_SD_INT_ON_RX_OVER      | \
                                                SPIM_SD_INT_ON_BYTE_COMP)
                                                
#define SPIM_SD_DataWidth                  (SPIM_SD_DATA_WIDTH)
#define SPIM_SD_InternalClockUsed          (SPIM_SD_INTERNAL_CLOCK)
#define SPIM_SD_InternalTxInterruptEnabled (SPIM_SD_INTERNAL_TX_INT_ENABLED)
#define SPIM_SD_InternalRxInterruptEnabled (SPIM_SD_INTERNAL_RX_INT_ENABLED)
#define SPIM_SD_ModeUseZero                (SPIM_SD_MODE_USE_ZERO)
#define SPIM_SD_BidirectionalMode          (SPIM_SD_BIDIRECTIONAL_MODE)
#define SPIM_SD_Mode                       (SPIM_SD_MODE)
#define SPIM_SD_DATAWIDHT                  (SPIM_SD_DATA_WIDTH)
#define SPIM_SD_InternalInterruptEnabled   (0u)

#define SPIM_SD_TXBUFFERSIZE   (SPIM_SD_TX_BUFFER_SIZE)
#define SPIM_SD_RXBUFFERSIZE   (SPIM_SD_RX_BUFFER_SIZE)

#define SPIM_SD_TXBUFFER       SPIM_SD_txBuffer
#define SPIM_SD_RXBUFFER       SPIM_SD_rxBuffer

#endif /* (CY_SPIM_SPIM_SD_H) */


/* [] END OF FILE */
