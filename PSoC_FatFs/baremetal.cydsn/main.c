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
#include "ff.h"

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    LED_0_Write(1);
    SPIM_SD_Start();
    CyDelay(1000);
    LED_1_Write(1);
    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    FATFS fatfs;
    FRESULT		fres;
	volatile UINT		written;
	UINT		i;
    
    
    fres = f_mount(&fatfs,"",1);
    
    if(fres != FR_OK){
        LED_1_Write(0);
        for(;;);
    }
    FIL file;
    
    fres = f_open(&file,"Test.txt",FA_CREATE_ALWAYS|FA_WRITE);
    
    if(fres != FR_OK){
        LED_1_Write(0);
        for(;;);
    }
    
    for(;;)
    {
        /* Place your application code here. */
        LED_0_Write(~LED_0_Read());
        //written = f_printf(&file,"Hello World\r\n");
        written = f_puts("Hello World\r\n",&file);
        f_sync(&file);
        CyDelay(2000);
    }
}

/* [] END OF FILE */
