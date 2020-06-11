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
#include "project.h"
#include "stdio.h"
#include "ff.h"

void USBUART_Start_Wrapper();
void USBUART_Connection_Check();
void USBUART_PutString_Wrapper(const char8 string[]);
uint32_t buffer[4096];
char debug[64];
int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    LED_0_Write(1);
    SPIM_SD_Start();
    USBUART_Start_Wrapper();
    CyDelay(1000);
    LED_1_Write(1);
    
    
    
    for(uint32_t i=0;i<4096;i++){
        buffer[i] = i;
    }
    
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    FATFS fatfs;
    FRESULT	fres;
    FIL file;
    UINT byte;
    uint32_t time;
    int size = 32;
    sprintf(debug,"SD Card Mounting\r\n");
    USBUART_PutString_Wrapper(debug);
    CyDelay(10);
    
    if((fres = f_mount(&fatfs,"",1)) != FR_OK){
        sprintf(debug,"SD Card Mount Failed.\r\n");
        USBUART_PutString_Wrapper(debug);
        /*
        switch(fres){
            case FR_INVALID_DRIVE:
                USBUART_PutString_Wrapper("FR_INVALID_DRIVE\n");
                break;
            case FR_DISK_ERR:
                USBUART_PutString_Wrapper("FR_DISK_ERR\n");
                break;
            case FR_NOT_READY:
                USBUART_PutString_Wrapper("FR_NOT_READY\n");
                break;
            case FR_NO_FILESYSTEM:
                USBUART_PutString_Wrapper("FR_NO_FILESYSTEM\n");
                break;
            default:
                break;
        }
        */
        LED_1_Write(0);
        for(;;);
    }  
    else{
        sprintf(debug,"SD Card Mounted\r\n");
        USBUART_PutString_Wrapper(debug);
        /*
        switch(fatfs.fs_type){
        case FS_FAT12:
            USBUART_PutString_Wrapper("FS_FAT12\r\n");
            break;
        case FS_FAT16:
            USBUART_PutString_Wrapper("FS_FAT16\r\n");
            break;
        case FS_FAT32:
            USBUART_PutString_Wrapper("FS_FAT32\r\n");
            break;
        default:
            USBUART_PutString_Wrapper("?\r\n");
            break;
        }
        */
    }
    CyDelay(10);
    sprintf(debug,"Bench Start\r\n");
    USBUART_PutString_Wrapper(debug);
    
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
            sprintf(debug,"%d,%ums,%ukByte/s\r\n",size,time,1000*size/time);
            USBUART_PutString_Wrapper(debug);
        }
        f_close(&file);
    }

        
    for(;;)
    {
        /* Place your application code here. */
        LED_0_Write(~LED_0_Read());
        //USBUART_PutString_Wrapper("SD Card Mounting\r\n");
        //written = f_printf(&file,"Hello World\r\n");
        //f_puts("Hello World\r\n",&file);
        //f_sync(&file);
        CyDelay(2000);
    }
}

void USBUART_Start_Wrapper(){
    USBUART_Start(0, USBUART_DWR_VDDD_OPERATION);
    //500ms TimeOut
    for(uint8_t i=0;i<100;i++){
        if(0u != USBUART_bGetConfiguration()){
            USBUART_CDC_Init(); 
            break;
        } 
        CyDelay(5);
    }
}
void USBUART_Connection_Check(){
    if(0u != USBUART_IsConfigurationChanged()){
        if(0u != USBUART_GetConfiguration()){
            USBUART_CDC_Init();
        }
    }
}

void USBUART_PutString_Wrapper(const char8 string[]){
    if(0u != USBUART_GetConfiguration()){
        while(!USBUART_CDCIsReady());
        USBUART_PutString(string);
    }
}

/* [] END OF FILE */
