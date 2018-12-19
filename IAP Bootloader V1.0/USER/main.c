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

//flag����
//31~16 ����
//15~8  try         --- ��ΧΪ0~10����������ÿ����������++����APP��Ӧ�ý�����0�����ϻ���Ϊ�ô�����ʧ�ܣ�ʧ��10�κ���ת����ǰ��app
//7~5   ����   
//4     update flag ---1:���� 0 : �����¡��ñ�־��app�н�����̼��Ժ����ô˱�־Ϊ1���´�������ʱ�����¹̼���
//					     ���³ɹ�������˱�־λ0�����л�jump�������¸��µ�app
//						 �������ʧ�ܣ���������α�ǣ�Ҳ�����л�jump����Ȼ�����ɵ�app����app�м��α�ǣ�������Ϊ1�����������ʧ�ܡ�
//3~1   ����
//0	    jump ---       0������app1   1:����app2
int main(void)
{ 
	u8 t;
	u8 key;
	u16 oldcount=0;	//�ϵĴ��ڽ�������ֵ
	u32 applenth=30*1024;	//���յ���app���볤��
	u8 clearflag=0; 
	u32 app_offset=0;
	u32 CHECK_FLAG=0;
	u32 flag=0;
	u32 update_addr=0;
	u32 run_addr=0;
	u32 tmp[2]={0};
	u32 try=10;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);  //��ʼ����ʱ����
	uart_init(115200);		//��ʼ�����ڲ�����Ϊ115200

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
	
	//��ʾ��ʾ��Ϣ
	while(1)
	{
		CHECK_FLAG=0;
		flag = read_flag();
		
		if(flag==0xFFFFFFFF)
		{//���ȫFFFFFFFF�����㣬��Ϊ���ǵ�һ��erase ������
			flag=0;
			STMFLASH_Write(FLASH_FLAG_ADDR,&flag,1);
			flag = read_flag();
		}
		
		printf("flag=0x%x !\r\n",flag);
		//flag |=0x10;//debug
		if((flag>>4) & 0x01)
		{
			//update 
			if((flag&0x01) == 0x00)//���ݵ�ǰ����ת�������
			{
				update_addr = FLASH_APP2_ADDR;
			}else{
				update_addr = FLASH_APP1_ADDR;
			}
			//do update
			printf("update app%d !\r\n",update_addr == FLASH_APP1_ADDR? 1:2);
			if(do_upddate_firm(update_addr)==0)
			{//update success���³ɹ����л�jump
				if((flag&0x01)==0)
				{
					CHECK_FLAG |= 0x00000001;//�л���app2
				}else{
					CHECK_FLAG &= 0xFFFFFFF0;//�л���app1
				}
			}else{
				//����ʧ�ܣ�����jump�л���������update flagΪ1����������ʧ��
				CHECK_FLAG |= 0x00000010;//clear the update ,���ϵͳAPP�������鵽update��Ϊ1��˵������ʧ��
			}
			
		}else{
			//no update����Ҫ���£����һ������ʧ�ܵĴ�����
			//check the try
			if(((flag>>8) & 0xFF)>BOOT_TRY_MAX_TIMES)
			{
				//����10����ʧ�ܣ��������һ�θ��µĹ̼������⣬Ҫ�л�����ǰ�Ĺ̼����л�jump
				if((flag&0x01)==0)
				{
					CHECK_FLAG |= 0x00000001;
				}else{
					CHECK_FLAG &= 0xFFFFFFF0;
				}
			}
		}
		//try ++ÿ��������++����try��app���������������
		CHECK_FLAG |= ((u32)((flag>>8 & 0xFF) + 1)<<8);
		
		//�������µ�jump ����app1/2
		if((CHECK_FLAG&0x01) == 0)
			run_addr=FLASH_APP1_ADDR;
		else
			run_addr=FLASH_APP2_ADDR;
		
		tmp[0]=run_addr;
		tmp[1]=CHECK_FLAG;
		printf("CHECK_FLAG=0x%x !\r\n",CHECK_FLAG);
		STMFLASH_Write(FLASH_RUN_ADDR,tmp,2);
		
		{
			printf("��ʼִ��FLASH�û����� CHECK_FLAG=0x%08x  addr=0x%08x !!\r\n",CHECK_FLAG, run_addr);
			if(((*(vu32*)(run_addr+4))&0xFF000000)==0x08000000)//�ж��Ƿ�Ϊ0X08XXXXXX.
			{	 
				iap_load_app(run_addr);//ִ��FLASH APP����
			}else 
			{
				printf("��FLASHӦ�ó���,�޷�ִ��!\r\n");
			}									   
		}			   
	}  
}

