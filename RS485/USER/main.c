#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "usart2.h"
#include "beep.h"
#include "includes.h"

/************************************************
 ALIENTEK战舰STM32开发板UCOS实验
 UCOSII移植
 技术支持：www.openedv.com
 淘宝店铺：http://eboard.taobao.com 
 关注微信公众平台微信号："正点原子"，免费获取STM32资料。
 广州市星翼电子科技有限公司  
 作者：正点原子 @ALIENTEK
************************************************/
 #define countof(a)   (sizeof(a) / sizeof(*(a)))
//START 任务
//设置任务优先级
#define START_TASK_PRIO			10  ///开始任务的优先级为最低
//设置任务堆栈大小
#define START_STK_SIZE			128
//任务任务堆栈
OS_STK START_TASK_STK[START_STK_SIZE];
//任务函数
void start_task(void *pdata);

//LED0任务
//设置任务优先级
#define HCRS_TASK_PRIO			15
//设置任务堆栈大小
#define HCRS_STK_SIZE			64
//任务堆栈
OS_STK HCRS_TASK_STK[HCRS_STK_SIZE];
//任务函数
void hcrs_task(void *pdata);

int main(void)
{
	delay_init();       	//延时初始化
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2); //中断分组配置
	uart_init(115200);
	uart2_init(9600);
	BEEP_Init();			//蜂鸣器初始化
	OSInit();  				//UCOS初始化
	OSTaskCreate(start_task,(void*)0,(OS_STK*)&START_TASK_STK[START_STK_SIZE-1],START_TASK_PRIO); //创建开始任务
	OSStart(); 				//开始任务
}

//开始任务
void start_task(void *pdata)
{
	OS_CPU_SR cpu_sr=0;
	pdata=pdata;
	OSStatInit();  //开启统计任务
	
	OS_ENTER_CRITICAL();  //进入临界区(关闭中断)
	OSTaskCreate(hcrs_task,(void*)0,(OS_STK*)&HCRS_TASK_STK[HCRS_STK_SIZE-1],HCRS_TASK_PRIO);//创建HCRS04任务
	OSTaskSuspend(START_TASK_PRIO);//挂起开始任务
	OS_EXIT_CRITICAL();  //退出临界区(开中断)
}
 

//读取电表任务
void hcrs_task(void *pdata){
	char send_buf[]={0x39,0x03,0x00,0x00,0x00,0x07,0x00,0xB0};
	u8 i=0;
	while(1){
		printf("读取数据发送!\r\n");
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

