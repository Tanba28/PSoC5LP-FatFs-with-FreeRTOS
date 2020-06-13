/* ========================================
 *
 * Copyright (c) 2020 Takumi Niwa
 * This software is released under the MIT license.
 *
 * ========================================
*/
#include "project.h"

#include "sd_api.h"

#include "FreeRTOS.h"
#include "task.h"

#include "USBUART_FreeRTOS.h"

uint32_t buffer[4096];
char debug[64];

void vFreeRTOSSetup(){
    extern void xPortPendSVHandler( void );
    extern void xPortSysTickHandler( void );
    extern void vPortSVCHandler( void );
    extern cyisraddress CyRamVectors[];

    CyRamVectors[ 11 ] = ( cyisraddress ) vPortSVCHandler;
    CyRamVectors[ 14 ] = ( cyisraddress ) xPortPendSVHandler;
    CyRamVectors[ 15 ] = ( cyisraddress ) xPortSysTickHandler;
}

void SdTeskTask(){
    for(uint32_t i=0;i<4096;i++){
        buffer[i] = i;
    }
    
    FATFS fatfs;
    FRESULT	fres;
    FIL file;
    UINT byte;
    uint32_t time;
    int size;
    
    sprintf(debug,"SD Card Mounting\r\n");
    USBUARTPutString(debug,strlen(debug));
    
    vTaskDelay(100);
    
    if((fres = f_mount(&fatfs,"",1)) != FR_OK){
        sprintf(debug,"SD Card Mount Failed.\r\n");
        USBUARTPutString(debug,strlen(debug));
        
        switch(fres){
            case FR_INVALID_DRIVE:
                sprintf(debug,"FR_INVALID_DRIVE\r\n");
                break;
            case FR_DISK_ERR:
                sprintf(debug,"FR_DISK_ERR\r\n");
                break;
            case FR_NOT_READY:
                sprintf(debug,"FR_NOT_READY\r\n");
                break;
            case FR_NO_FILESYSTEM:
                sprintf(debug,"FR_NO_FILESYSTEM\r\n");
                break;
            default:
                sprintf(debug,"UNKNOWN ERROR\r\n");
                break;
        }
        USBUARTPutString(debug,strlen(debug));
        LED_1_Write(0);
        for(;;);
    }  
    
    else{
        sprintf(debug,"SD Card Mounted\r\n");
        USBUARTPutString(debug,strlen(debug));
        
        switch(fatfs.fs_type){
        case FS_FAT12:
            sprintf(debug,"FS_FAT12\r\n");
            break;
        case FS_FAT16:
            sprintf(debug,"FS_FAT16\r\n");
            break;
        case FS_FAT32:
            sprintf(debug,"FS_FAT32\r\n");
            break;
        default:
            sprintf(debug,"?\r\n");
            break;
        }
        USBUARTPutString(debug,strlen(debug));
    }
    
    vTaskDelay(100);
    
    sprintf(debug,"Bench Start\r\n");
    USBUARTPutString(debug,strlen(debug));
    
    fres = f_open(&file,"test.bin",FA_CREATE_ALWAYS|FA_WRITE);
    Timer_Start();
    if(fres == FR_OK){
        for(size = 1;size<32769;size = size*2){
            time = 4294967296 - Timer_ReadCounter();
            
            for(int i=0;i<1000;i++){
                f_write(&file,buffer,size,&byte);
            }
            f_sync(&file);
            time = 4294967296 - Timer_ReadCounter() - time;
            CyDelay(10);
            sprintf(debug,"%d,%ums,%ukByte/s\r\n",size,(unsigned int)time,(unsigned int)(1000*size/time));
            USBUARTPutString(debug,strlen(debug));
        }
        f_close(&file);
    }
    for(;;){
        LED_0_Write(~LED_0_Read());
        vTaskDelay(10000);
    }
}

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    LED_0_Write(1);
    
    USBUARTStart();
    vFreeRTOSSetup();
    
    CyDelay(1000);
    
    LED_1_Write(1);
    
    xTaskCreate(SdTeskTask,"test4",1000,NULL,3,NULL);
    
    vTaskStartScheduler();
    for(;;)
    {
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
