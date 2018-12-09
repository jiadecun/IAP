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
	u32 app_offset=0;
	u32 CHECK_FLAG=0;
	u32 flag=0;
	u32 update_addr=0;
	u32 run_addr=0;
	u32 tmp[2]={0};
	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  //初始化延时函数
	uart_init(115200);		//初始化串口波特率为115200

	//显示提示信息
	while(1)
	{
		CHECK_FLAG=0;
		flag = read_flag();
		
		if(flag==0xFFFFFFFF)
		{
			flag=0;
			STMFLASH_Write(FLASH_FLAG_ADDR,&flag,1);
			flag = read_flag();
		}
		
		//STMFLASH_Read(FLASH_FLAG_ADDR,(u32 *)(&CHECK_FLAG), sizeof(u32));
		printf("flag=0x%x !\r\n",flag);
		if((flag>>4) & 0x01)
		{
			//update 
			if((flag&0x01) == 0x00)
			{
				update_addr = FLASH_APP2_ADDR;
			}else{
				update_addr = FLASH_APP1_ADDR;
			}
			//do update
			if(do_upddate_firm(update_addr)==0)
			{//update success
				if((flag&0x01)==0)
				{
					CHECK_FLAG |= 0x01;
				}else{
					CHECK_FLAG |= 0x00;
				}
			}else{
				CHECK_FLAG |= 0x00000010;//clear the update ,如果系统APP启动后检查到update仍为1，说明更行失败
			}
			
		}else{
			//no update
			//check the try
			if(((flag>>8) & 0xFF)>BOOT_TRY_MAX_TIMES)
			{
				//连续10启动失败，
				if((flag&0x01)==0)
				{
					CHECK_FLAG |= 0x01;
				}else{
					CHECK_FLAG |= 0x00;
				}
			}
		}
		//try ++
		CHECK_FLAG |= ((u32)((flag>>8 & 0xFF) + 1)<<8);
		
		//STMFLASH_Write(FLASH_FLAG_ADDR,&CHECK_FLAG,1);
		
		if((CHECK_FLAG&0x01) == 0)
			run_addr=FLASH_APP1_ADDR;
		else
			run_addr=FLASH_APP2_ADDR;
		
		tmp[0]=run_addr;
		tmp[1]=CHECK_FLAG;
		printf("CHECK_FLAG=0x%x !\r\n",CHECK_FLAG);
		STMFLASH_Write(FLASH_RUN_ADDR,tmp,2);
		
		{
			printf("开始执行FLASH用户代码 CHECK_FLAG=0x%08x  addr=0x%08x !!\r\n",CHECK_FLAG, run_addr);
			if(((*(vu32*)(run_addr+4))&0xFF000000)==0x08000000)//判断是否为0X08XXXXXX.
			{	 
				iap_load_app(run_addr);//执行FLASH APP代码
			}else 
			{
				printf("非FLASH应用程序,无法执行!\r\n");
			}									   
		}			   
	}  
}

