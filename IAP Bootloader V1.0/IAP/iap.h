#ifndef __IAP_H__
#define __IAP_H__
#include "sys.h"  
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//IAP ����	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/7/21
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////    
typedef  void (*iapfun)(void);				//����һ���������͵Ĳ���.   
#define FLASH_BASE_ADDR		0x08000000  	
#define FLASH_IAP_SIZE		0x00010000  	
#define FLASH_FLAG_ADDR		(FLASH_BASE_ADDR+FLASH_IAP_SIZE-4)
#define FLASH_RUN_ADDR		(FLASH_BASE_ADDR+FLASH_IAP_SIZE-8)
#define FLASH_APP1_ADDR		(FLASH_BASE_ADDR+FLASH_IAP_SIZE)
#define FLASH_APP1_SIZE		(0x00072800)
#define FLASH_APP2_ADDR		(FLASH_BASE_ADDR+FLASH_IAP_SIZE+FLASH_APP1_SIZE)
#define FLASH_APP2_SIZE		(0x00080000)
											//����0X08000000~0X0800FFFF�Ŀռ�ΪBootloaderʹ��(��64KB)	   
void iap_load_app(u32 appxaddr);			//��ת��APP����ִ��
void iap_write_appbin(u32 appxaddr,u8 *appbuf,u32 applen);	//��ָ����ַ��ʼ,д��bin
void write_flag(u32 flag);
u32 read_flag();
int do_upddate_firm(u32 update_addr);
#define BOOT_TRY_MAX_TIMES	10
#endif







































