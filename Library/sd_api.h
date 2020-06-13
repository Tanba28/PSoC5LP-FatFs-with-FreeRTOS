/* ========================================
 *
 * Copyright (c) 2020 Takumi Niwa
 * This software is released under the MIT license.
 *
 * ========================================
*/
#ifndef _SD_API_H
#define _SD_API_H

#include "project.h"
#include "ff.h"

#define DMA_ENABLE 1
#define FREERTOS_ENABLE 1

#define FCLK_SLOW() {SD_Clock_SetDividerValue(160);}
#define FCLK_FAST() {SD_Clock_SetDividerValue(2);}
#define CS_HI() {SD_CS_Write(1);}
#define CS_LOW() {SD_CS_Write(0);}

#if FREERTOS_ENABLE == 1    
    #include "FreeRTOS.h"
    #include "task.h"
    #include "semphr.h"
#endif

#if DMA_ENABLE ==1     
    #define SD_RX_DMA_BYTES_PER_BURST 1
    #define SD_RX_DMA_REQUEST_PER_BURST 1
    #define SD_RX_DMA_SRC_BASE (CYDEV_PERIPH_BASE)
    #define SD_RX_DMA_DST_BASE (CYDEV_SRAM_BASE)

    #define SD_TX_DMA_BYTES_PER_BURST 1
    #define SD_TX_DMA_REQUEST_PER_BURST 1
    #define SD_TX_DMA_SRC_BASE (CYDEV_SRAM_BASE)
    #define SD_TX_DMA_DST_BASE (CYDEV_PERIPH_BASE)
#endif

/* MMC/SD command */
#define CMD0	(0)			/* GO_IDLE_STATE */
#define CMD1	(1)			/* SEND_OP_COND (MMC) */
#define	ACMD41	(0x80+41)	/* SEND_OP_COND (SDC) */
#define CMD8	(8)			/* SEND_IF_COND */
#define CMD9	(9)			/* SEND_CSD */
#define CMD10	(10)		/* SEND_CID */
#define CMD12	(12)		/* STOP_TRANSMISSION */
#define ACMD13	(0x80+13)	/* SD_STATUS (SDC) */
#define CMD16	(16)		/* SET_BLOCKLEN */
#define CMD17	(17)		/* READ_SINGLE_BLOCK */
#define CMD18	(18)		/* READ_MULTIPLE_BLOCK */
#define CMD23	(23)		/* SET_BLOCK_COUNT (MMC) */
#define	ACMD23	(0x80+23)	/* SET_WR_BLK_ERASE_COUNT (SDC) */
#define CMD24	(24)		/* WRITE_BLOCK */
#define CMD25	(25)		/* WRITE_MULTIPLE_BLOCK */
#define CMD32	(32)		/* ERASE_ER_BLK_START */
#define CMD33	(33)		/* ERASE_ER_BLK_END */
#define CMD38	(38)		/* ERASE */
#define CMD55	(55)		/* APP_CMD */
#define CMD58	(58)		/* READ_OCR */

/* DMA Initialize */
CY_ISR_PROTO(SD_RX_ISR);

/* DMA Initialize */
void init_dma();

/* SPI Function */
void init_spi();

BYTE xchg_spi(BYTE dat);

void rcvr_spi_multi(BYTE *buff, UINT btr);
void xmit_spi_multi(const BYTE *buff, UINT btx);

/* Wait for card ready */
int wait_ready(UINT wt);

/* Deselect card and release SPI */
void deselect();

/* Selec card and wait for ready */
int select();

/* Send a data packet to the MMC */
int rcvr_datablock  (BYTE *buff, UINT btr);

/* Send a data packet to the MMC */
int xmit_datablock(const BYTE *buff, BYTE token);

/* Send a command packet to the MMC */
BYTE send_cmd(BYTE cmd, DWORD arg);

#endif
/* [] END OF FILE */
