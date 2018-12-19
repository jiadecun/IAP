#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "lcd.h"
#include "key.h"   
#include "stmflash.h" 
#include "iap.h"   
#include "sdio_sdcard.h"    
#include "malloc.h" 
#include "w25qxx.h"    
#include "ff.h"  
#include "exfuns.h" 

//flag定义
//31~16 保留
//15~8  try         --- 范围为0~10启动次数，每次启动都会++，在APP中应该讲其清0，否认会认为该次启动失败，失败10次后，跳转到以前的app
//7~5   保留   
//4     update flag ---1:更新 0 : 不更新。该标志在app中接受完固件以后，设置此标志为1，下次重启的时候会更新固件，
//					     更新成功后清除此标志位0，并切换jump，启动新更新的app
//						 如果更新失败，不会清除次标记，也不会切换jump，依然启动旧的app，在app中检测次标记，如果标记为1，则表明更新失败。
//3~1   保留
//0	    jump ---       0：启动app1   1:启动app2
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
	u32 try=10;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	delay_init(168);  //初始化延时函数
	uart_init(115200);		//初始化串口波特率为115200

	while((SD_OK != SD_Init())&& try-->0)
	{
		delay_us(10);
	}
 	exfuns_init();
  	if(f_mount(fs[0],"0:",1)!=FR_OK)
	{
		printf("\r\n FAT Error !\r\n");
	}else{
		printf("\r\n FAT OK !\r\n");
	}
	
	//显示提示信息
	while(1)
	{
		CHECK_FLAG=0;
		flag = read_flag();
		
		if(flag==0xFFFFFFFF)
		{//如果全FFFFFFFF则清零，因为这是第一次erase 后启动
			flag=0;
			STMFLASH_Write(FLASH_FLAG_ADDR,&flag,1);
			flag = read_flag();
		}
		
		printf("flag=0x%x !\r\n",flag);
		//flag |=0x10;//debug
		if((flag>>4) & 0x01)
		{
			//update 
			if((flag&0x01) == 0x00)//根据当前的跳转情况更新
			{
				update_addr = FLASH_APP2_ADDR;
			}else{
				update_addr = FLASH_APP1_ADDR;
			}
			//do update
			printf("update app%d !\r\n",update_addr == FLASH_APP1_ADDR? 1:2);
			if(do_upddate_firm(update_addr)==0)
			{//update success更新成功后，切换jump
				if((flag&0x01)==0)
				{
					CHECK_FLAG |= 0x00000001;//切换到app2
				}else{
					CHECK_FLAG &= 0xFFFFFFF0;//切换到app1
				}
			}else{
				//更新失败，不做jump切换，并设置update flag为1，表明更新失败
				CHECK_FLAG |= 0x00000010;//clear the update ,如果系统APP启动后检查到update仍为1，说明更行失败
			}
			
		}else{
			//no update不需要更新，检查一下启动失败的次数，
			//check the try
			if(((flag>>8) & 0xFF)>BOOT_TRY_MAX_TIMES)
			{
				//连续10启动失败，表明最近一次更新的固件有问题，要切换到以前的固件。切换jump
				if((flag&0x01)==0)
				{
					CHECK_FLAG |= 0x00000001;
				}else{
					CHECK_FLAG &= 0xFFFFFFF0;
				}
			}
		}
		//try ++每次启动都++。该try有app正常启动后清除。
		CHECK_FLAG |= ((u32)((flag>>8 & 0xFF) + 1)<<8);
		
		//根据最新的jump 启动app1/2
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

