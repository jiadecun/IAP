#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "lcd.h"
#include "key.h"   
#include "stmflash.h" 
#include "iap.h"   

//ALIENTEK 探索者STM32F407开发板 实验50
//串口IAP实验-库函数版本
//技术支持：www.openedv.com
//淘宝店铺：http://eboard.taobao.com  
//广州市星翼电子科技有限公司  
//作者：正点原子 @ALIENTEK

int main(void)
{ 
	u8 t;
	u8 key;
	u16 oldcount=0;	//老的串口接收数据值
	u32 applenth=30*1024;	//接收到的app代码长度
	u8 clearflag=0; 
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  //初始化延时函数
	uart_init(9600);		//初始化串口波特率为115200

	//显示提示信息
	while(1)
	{
		if(1)
		{//有固件更新
			//for()
			{
				printf("固件更新开始!\r\n");	
				STMFLASH_Read(FLASH_APP1_ADDR+0x10000,(u32 *)USART_RX_BUF, applenth);
				iap_write_appbin(FLASH_APP1_ADDR,USART_RX_BUF,applenth);//更新FLASH代码  
				printf("固件更新完成!\r\n");	
			}
		}
		{
			printf("开始执行FLASH用户代码!!\r\n");
			if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
			{	 
				iap_load_app(FLASH_APP1_ADDR);//执行FLASH APP代码
			}else 
			{
				printf("非FLASH应用程序,无法执行!\r\n");
			}									   
		}			   
	}  
}

