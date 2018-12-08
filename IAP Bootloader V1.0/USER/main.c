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
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	delay_init(168);  //��ʼ����ʱ����
	uart_init(9600);		//��ʼ�����ڲ�����Ϊ115200

	//��ʾ��ʾ��Ϣ
	while(1)
	{
		if(1)
		{//�й̼�����
			//for()
			{
				printf("�̼����¿�ʼ!\r\n");	
				STMFLASH_Read(FLASH_APP1_ADDR+0x10000,(u32 *)USART_RX_BUF, applenth);
				iap_write_appbin(FLASH_APP1_ADDR,USART_RX_BUF,applenth);//����FLASH����  
				printf("�̼��������!\r\n");	
			}
		}
		{
			printf("��ʼִ��FLASH�û�����!!\r\n");
			if(((*(vu32*)(FLASH_APP1_ADDR+4))&0xFF000000)==0x08000000)//�ж��Ƿ�Ϊ0X08XXXXXX.
			{	 
				iap_load_app(FLASH_APP1_ADDR);//ִ��FLASH APP����
			}else 
			{
				printf("��FLASHӦ�ó���,�޷�ִ��!\r\n");
			}									   
		}			   
	}  
}

