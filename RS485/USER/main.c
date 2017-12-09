#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "usart2.h"
#include "beep.h"
#include "includes.h"

/************************************************
 ALIENTEKս��STM32������UCOSʵ��
 UCOSII��ֲ
 ����֧�֣�www.openedv.com
 �Ա����̣�http://eboard.taobao.com 
 ��ע΢�Ź���ƽ̨΢�źţ�"����ԭ��"����ѻ�ȡSTM32���ϡ�
 ������������ӿƼ����޹�˾  
 ���ߣ�����ԭ�� @ALIENTEK
************************************************/
 #define countof(a)   (sizeof(a) / sizeof(*(a)))
//START ����
//�����������ȼ�
#define START_TASK_PRIO			10  ///��ʼ��������ȼ�Ϊ���
//���������ջ��С
#define START_STK_SIZE			128
//���������ջ
OS_STK START_TASK_STK[START_STK_SIZE];
//������
void start_task(void *pdata);

//LED0����
//�����������ȼ�
#define HCRS_TASK_PRIO			15
//���������ջ��С
#define HCRS_STK_SIZE			64
//�����ջ
OS_STK HCRS_TASK_STK[HCRS_STK_SIZE];
//������
void hcrs_task(void *pdata);

int main(void)
{
	delay_init();       	//��ʱ��ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //�жϷ�������
	uart_init(115200);
	uart2_init(9600);
	BEEP_Init();			//��������ʼ��
	OSInit();  				//UCOS��ʼ��
	OSTaskCreate(start_task,(void*)0,(OS_STK*)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO); //������ʼ����
	OSStart(); 				//��ʼ����
}

//��ʼ����
void start_task(void *pdata)
{
	OS_CPU_SR cpu_sr=0;
	pdata=pdata;
	OSStatInit();  //����ͳ������
	
	OS_ENTER_CRITICAL();  //�����ٽ���(�ر��ж�)
	OSTaskCreate(hcrs_task,(void*)0,(OS_STK*)&HCRS_TASK_STK[HCRS_STK_SIZE-1],HCRS_TASK_PRIO);//����HCRS04����
	OSTaskSuspend(START_TASK_PRIO);//����ʼ����
	OS_EXIT_CRITICAL();  //�˳��ٽ���(���ж�)
}
 

//��ȡ�������
void hcrs_task(void *pdata){
	char send_buf[]={0x39,0x03,0x00,0x00,0x00,0x07,0x00,0xB0};
	u8 i=0;
	while(1){
		printf("��ȡ���ݷ���!\r\n");
		GPIO_SetBits(GPIOC,GPIO_Pin_5);	  
		delay_ms(1);
		for(i=0;i<countof(send_buf);i++){					 
			USART_SendData(USART2,send_buf[i]);
			while(USART_GetFlagStatus(USART2,USART_FLAG_TXE)==Bit_RESET);
		}
		delay_ms(2);
		GPIO_ResetBits(GPIOC,GPIO_Pin_5);
		delay_ms(1000);	
	}
	
}

