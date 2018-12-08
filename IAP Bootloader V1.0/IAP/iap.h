#ifndef __IAP_H__
#define __IAP_H__
#include "sys.h"  
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
typedef  void (*iapfun)(void);				//定义一个函数类型的参数.   
#define FLASH_BASE_ADDR		0x08000000  	
#define FLASH_IAP_SIZE		0x00010000  	
#define FLASH_FLAG_ADDR		(FLASH_BASE_ADDR+FLASH_IAP_SIZE-4)
#define FLASH_RUN_ADDR		(FLASH_BASE_ADDR+FLASH_IAP_SIZE-8)
#define FLASH_APP1_ADDR		(FLASH_BASE_ADDR+FLASH_IAP_SIZE)
#define FLASH_APP1_SIZE		(0x00072800)
#define FLASH_APP2_ADDR		(FLASH_BASE_ADDR+FLASH_IAP_SIZE+FLASH_APP1_SIZE)
#define FLASH_APP2_SIZE		(0x00080000)
											//保留0X08000000~0X0800FFFF的空间为Bootloader使用(共64KB)	   
void iap_load_app(u32 appxaddr);			//跳转到APP程序执行
void iap_write_appbin(u32 appxaddr,u8 *appbuf,u32 applen);	//在指定地址开始,写入bin
void write_flag(u32 flag);
u32 read_flag();
int do_upddate_firm(u32 update_addr);
#define BOOT_TRY_MAX_TIMES	10
#endif







































