#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "stmflash.h"
#include "iap.h"
#include "sdio_sdcard.h"    
#include "malloc.h" 
#include "w25qxx.h"    
#include "ff.h"  
#include "exfuns.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//IAP 代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/7/21
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 

iapfun jump2app; 
u32 iapbuf[2048]; 	//2K字节缓存  
//appxaddr:应用程序的起始地址
//appbuf:应用程序CODE.
//appsize:应用程序大小(字节).
void iap_write_appbin(u32 appxaddr,u8 *appbuf,u32 appsize)
{
	u32 t;
	u16 i=0;
	u32 temp;
	u32 fwaddr=appxaddr;//当前写入的地址
	u8 *dfu=appbuf;
	for(t=0;t<appsize;t+=4)
	{						   
		temp=(u32)dfu[3]<<24;   
		temp|=(u32)dfu[2]<<16;    
		temp|=(u32)dfu[1]<<8;
		temp|=(u32)dfu[0];	  
		dfu+=4;//偏移4个字节
		iapbuf[i++]=temp;	    
		if(i==512)
		{
			i=0; 
			STMFLASH_Write(fwaddr,iapbuf,512);
			fwaddr+=2048;//偏移2048  512*4=2048
		}
	} 
	if(i)STMFLASH_Write(fwaddr,iapbuf,i);//将最后的一些内容字节写进去.  
}

//跳转到应用程序段
//appxaddr:用户代码起始地址.
void iap_load_app(u32 appxaddr)
{
	if(((*(vu32*)appxaddr)&0x2FFE0000)==0x20000000)	//检查栈顶地址是否合法.
	{ 
		jump2app=(iapfun)*(vu32*)(appxaddr+4);		//用户代码区第二个字为程序开始地址(复位地址)		
		MSR_MSP(*(vu32*)appxaddr);					//初始化APP堆栈指针(用户代码区的第一个字用于存放栈顶地址)
		jump2app();									//跳转到APP.
	}
}		 

void write_flag(u32 flag)
{
	FLASH_Status status = FLASH_COMPLETE;
	u32 addr=FLASH_FLAG_ADDR;
	
	while(1){
		status=FLASH_EraseSector(FLASH_Sector_3,VoltageRange_3);//VCC=2.7~3.6V之间!!
		if(status==FLASH_COMPLETE)
			break;
	}
	while(1){
		if(FLASH_ProgramWord(addr,flag)==FLASH_COMPLETE)
			break;
	}
}
u32 read_flag()
{
	u32 addr=FLASH_FLAG_ADDR;
	return STMFLASH_ReadWord(addr);
}

#define MAX_BUFF_SIZE   512
int do_upddate_firm(u32 update_addr)
{
	FRESULT fr;
    FATFS fs;
    FIL fp;
	u32 br=0;
	u32 file_size=0;
	u32 once=0;
	unsigned char buf[MAX_BUFF_SIZE];
	
    /* Opens an existing file. If not exist, creates a new file. */
    fr = f_open(&fp, "TEST.bin", FA_READ | FA_OPEN_ALWAYS);
    if (fr != FR_OK) {
		return -1;
    }
	printf("file_size=0x%x ",file_size);
	file_size = fp.fsize;
	printf("file_size=0x%x ",file_size);
	do{
		printf("file_size=0x%x ",file_size);
		if(file_size > MAX_BUFF_SIZE)
			once = MAX_BUFF_SIZE;
		else
			once = file_size;
		fr = f_read (&fp, buf,	once, &br);
		if(once != br)
		{
			printf("Read file failed ! \r\n");
			break;
		}else{
			iap_write_appbin(update_addr + (fp.fsize - file_size), buf, once);
			file_size -= once;
		}
		
	}while(file_size>0);

    f_close(&fp);

	return 0;
}









