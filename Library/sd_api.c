/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "sd_api.h"

static void delay_ms(UINT time){
    #if FREERTOS_ENABLE == 1
        vTaskDelay(time*10);
    #else
        CyDelay(1);
    #endif
}
static uint8_t dummy[1] = { 0xff };

#if FREERTOS_ENABLE == 1      
    SemaphoreHandle_t rx_binary_semaphor;
#else
    volatile CYBIT transfer_flag = 0;
#endif

#if DMA_ENABLE ==1 
    static uint8_t SD_RX_DMA_Channel = 0;
    static uint8_t SD_RX_DMA_TD[1];

    uint8_t SD_TX_DMA_Channel = 0;
    uint8_t SD_TX_DMA_TD[1];
#endif

/* DMA Initialize */
CY_ISR(SD_RX_ISR){
    #if FREERTOS_ENABLE == 1
        BaseType_t xHigherPriorityTaskWoken;
    
        xHigherPriorityTaskWoken = pdFALSE;
    
        xSemaphoreGiveFromISR(rx_binary_semaphor, &xHigherPriorityTaskWoken);
    
        portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
    #else
        transfer_flag = 0;
    #endif
}

/* DMA Initialize */
void init_dma(){
    SD_RX_ISR_StartEx(SD_RX_ISR);
    
    SD_RX_DMA_Channel = SD_RX_DMA_DmaInitialize(SD_RX_DMA_BYTES_PER_BURST, SD_RX_DMA_REQUEST_PER_BURST, HI16(SD_RX_DMA_SRC_BASE), HI16(SD_RX_DMA_DST_BASE));
    SD_RX_DMA_TD[0] = CyDmaTdAllocate();
    CyDmaClearPendingDrq(SD_RX_DMA_Channel);
    
    SD_TX_DMA_Channel = SD_TX_DMA_DmaInitialize(SD_TX_DMA_BYTES_PER_BURST, SD_TX_DMA_REQUEST_PER_BURST, HI16(SD_TX_DMA_SRC_BASE), HI16(SD_TX_DMA_DST_BASE));
    SD_TX_DMA_TD[0] = CyDmaTdAllocate();
    CyDmaClearPendingDrq(SD_TX_DMA_Channel);        
}

/* SPI Function */
void init_spi(){
    FCLK_SLOW();                //set to slow clock
    CS_HI();
    SPIM_SD_Start();
    
    #if DMA_ENABLE == 1
    init_dma();
    #endif
    
    #if FREERTOS_ENABLE == 1
    rx_binary_semaphor = xSemaphoreCreateBinary();
    #endif
    
	delay_ms(10);   
}

BYTE xchg_spi(BYTE dat){
    SPIM_SD_WriteTxData(dat);
    while(!(SPIM_SD_ReadTxStatus() & SPIM_SD_STS_SPI_DONE));
    
    return SPIM_SD_ReadRxData();    
}

void rcvr_spi_multi(BYTE *buff, UINT btr){
    #if DMA_ENABLE == 0
        for(UINT i=0;i<btr;i++){
            buff[i] = xchg_spi(0xFF);
        }
    
    #elif DMA_ENABLE == 1
        CyDmaTdSetConfiguration(SD_TX_DMA_TD[0],btr,CY_DMA_DISABLE_TD,SD_TX_DMA__TD_TERMOUT_EN);
        CyDmaTdSetAddress(SD_TX_DMA_TD[0],LO16((uint32)(dummy)),LO16((uint32)SPIM_SD_TXDATA_PTR));
        CyDmaChSetInitialTd(SD_TX_DMA_Channel,SD_TX_DMA_TD[0]);
        
        CyDmaTdSetConfiguration(SD_RX_DMA_TD[0],btr,CY_DMA_DISABLE_TD,SD_RX_DMA__TD_TERMOUT_EN|TD_INC_DST_ADR);
        CyDmaTdSetAddress(SD_RX_DMA_TD[0],LO16((uint32)(SPIM_SD_RXDATA_PTR)),LO16((uint32)buff));
        CyDmaChSetInitialTd(SD_RX_DMA_Channel,SD_RX_DMA_TD[0]); 
        
        #if FREERTOS_ENABLE == 0
            transfer_flag = 1;
        #endif
        
        CyDmaClearPendingDrq(SD_TX_DMA_Channel);
        CyDmaClearPendingDrq(SD_RX_DMA_Channel);
        
        CyDmaChEnable(SD_TX_DMA_Channel, 1);
        CyDmaChEnable(SD_RX_DMA_Channel, 1);
        
        CyDmaChSetRequest(SD_TX_DMA_Channel,CPU_REQ);
        
        #if FREERTOS_ENABLE == 0
            while(transfer_flag); 
        #elif FREERTOS_ENABLE == 1
            xSemaphoreTake(rx_binary_semaphor,portMAX_DELAY); 
        #endif
        
        CyDmaChSetRequest(SD_TX_DMA_Channel,CPU_TERM_CHAIN);
        CyDmaChSetRequest(SD_RX_DMA_Channel,CPU_TERM_CHAIN);
    #endif
}

void xmit_spi_multi(const BYTE *buff, UINT btx){
    #if DMA_ENABLE == 0
        for(UINT i=0;i<btx;i++){
            xchg_spi(buff[i]);
        }   
    #elif DMA_ENABLE == 1
        CyDmaTdSetConfiguration(SD_TX_DMA_TD[0],btx,CY_DMA_DISABLE_TD,SD_TX_DMA__TD_TERMOUT_EN|TD_INC_SRC_ADR);
        CyDmaTdSetAddress(SD_TX_DMA_TD[0],LO16((uint32)(buff)),LO16((uint32)SPIM_SD_TXDATA_PTR));
        CyDmaChSetInitialTd(SD_TX_DMA_Channel,SD_TX_DMA_TD[0]);
        
        CyDmaTdSetConfiguration(SD_RX_DMA_TD[0],btx,CY_DMA_DISABLE_TD,SD_RX_DMA__TD_TERMOUT_EN);
        CyDmaTdSetAddress(SD_RX_DMA_TD[0],LO16((uint32)(SPIM_SD_RXDATA_PTR)),LO16((uint32)dummy));
        CyDmaChSetInitialTd(SD_RX_DMA_Channel,SD_RX_DMA_TD[0]);    
        
        #if FREERTOS_ENABLE == 0
            transfer_flag = 1;
        #endif
        
        CyDmaClearPendingDrq(SD_TX_DMA_Channel);
        CyDmaClearPendingDrq(SD_RX_DMA_Channel);
        
        CyDmaChEnable(SD_TX_DMA_Channel, 1);
        CyDmaChEnable(SD_RX_DMA_Channel, 1);
        
        CyDmaChSetRequest(SD_TX_DMA_Channel,CPU_REQ);
        
        #if FREERTOS_ENABLE == 0
            while(transfer_flag);
            
        #elif FREERTOS_ENABLE == 1
            xSemaphoreTake(rx_binary_semaphor,portMAX_DELAY);
        #endif
        
        CyDmaChSetRequest(SD_TX_DMA_Channel,CPU_TERM_CHAIN);
        CyDmaChSetRequest(SD_RX_DMA_Channel,CPU_TERM_CHAIN);
    #endif   
}

/* Wait for card ready */
int wait_ready(UINT wt){
	BYTE d;

	for(UINT i=0;i<wt;i++){
        d = xchg_spi(0xFF);
        if(d == 0xFF){
            return 1; /* Ready */
        }
        delay_ms(1);
    }

	return 0; /* Time Out */    
}

/* Deselect card and release SPI */
void deselect(){
    CS_HI();
	xchg_spi(0xFF);	/* Dummy clock (force DO hi-z for multiple slave SPI) */
}

/* Selec card and wait for ready */
int select(){
	CS_LOW();
	xchg_spi(0xFF);	/* Dummy clock (force DO enabled) */
	if (wait_ready(500)) return 1;	/* Wait for card ready */

	deselect();
	return 0;	/* Timeout */    
}

/* Send a data packet to the MMC */
int rcvr_datablock  (BYTE *buff, UINT btr){
    BYTE token;
    for(UINT i=0;i<200;i++){ /* 200 ms Time Out */
        token = xchg_spi(0xFF);
        if(token != 0xFF){
            break;
        }
        delay_ms(1);
    }
    
    if(token != 0xFE){
        return 0; /* Time Out */
    }

	rcvr_spi_multi(buff, btr);		/* Store trailing data to the buffer */
	xchg_spi(0xFF); xchg_spi(0xFF);			/* Discard CRC */

	return 1;						/* Function succeeded */
}

/* Send a data packet to the MMC */
int xmit_datablock(const BYTE *buff, BYTE token){
	BYTE resp;
	if (!wait_ready(500)) return 0;		/* Wait for card ready */

	xchg_spi(token);					/* Send token */
	if (token != 0xFD) {				/* Send data if token is other than StopTran */
		xmit_spi_multi(buff, 512);		/* Data */
		xchg_spi(0xFF); xchg_spi(0xFF);	/* Dummy CRC */

		resp = xchg_spi(0xFF);				/* Receive data resp */
		if ((resp & 0x1F) != 0x05)		/* Function fails if the data packet was not accepted */
			return 0;
	}
	return 1;  
}

/* Send a command packet to the MMC */
BYTE send_cmd(BYTE cmd, DWORD arg){
	BYTE n, res;


	if (cmd & 0x80) {	/* Send a CMD55 prior to ACMD<n> */
		cmd &= 0x7F;
		res = send_cmd(CMD55, 0);
		if (res > 1) return res;
	}

	/* Select the card and wait for ready except to stop multiple block read */
	if (cmd != CMD12) {
        deselect();
		if (!select()) return 0xFF;
	}

	/* Send command packet */
	xchg_spi(0x40 | cmd);				/* Start + command index */
	xchg_spi((BYTE)(arg >> 24));		/* Argument[31..24] */
	xchg_spi((BYTE)(arg >> 16));		/* Argument[23..16] */
	xchg_spi((BYTE)(arg >> 8));			/* Argument[15..8] */
	xchg_spi((BYTE)arg);				/* Argument[7..0] */
	n = 0x01;							/* Dummy CRC + Stop */
	if (cmd == CMD0) n = 0x95;			/* Valid CRC for CMD0(0) */
	if (cmd == CMD8) n = 0x87;			/* Valid CRC for CMD8(0x1AA) */
	xchg_spi(n);

	/* Receive command resp */
	if (cmd == CMD12) xchg_spi(0xFF);	/* Diacard following one byte when CMD12 */
	n = 10;								/* Wait for response (10 bytes max) */
	do
		res = xchg_spi(0xFF);
	while ((res & 0x80) && --n);

	return res;							/* Return received response */    
}

/* [] END OF FILE */
