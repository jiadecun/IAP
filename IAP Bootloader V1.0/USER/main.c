#include "sys.h"
#include "delay.h"  
#include "usart.h"   
#include "led.h"
#include "lcd.h"
#include "key.h"   
#include "stmflash.h" 
#include "iap.h"   

//ALIENTEK ̽����STM32F407������ ʵ��50
//����IAPʵ��-�⺯���汾
//����֧�֣�www.openedv.com
//�Ա����̣�http://eboard.taobao.com  
//������������ӿƼ����޹�˾  
//���ߣ�����ԭ�� @ALIENTEK

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
	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);  //��ʼ����ʱ����
	uart_init(115200);		//��ʼ�����ڲ�����Ϊ115200

	//��ʾ��ʾ��Ϣ
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
				CHECK_FLAG |= 0x00000010;//clear the update ,���ϵͳAPP�������鵽update��Ϊ1��˵������ʧ��
			}
			
		}else{
			//no update
			//check the try
			if(((flag>>8) & 0xFF)>BOOT_TRY_MAX_TIMES)
			{
				//����10����ʧ�ܣ�
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

