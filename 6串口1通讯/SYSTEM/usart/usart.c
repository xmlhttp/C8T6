#include <string.h>
#include "usart.h"	  
u32 waatA;
u16 waatV;
u32 waatW;
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
_sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
 
#if EN_USART1_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART_RX_BUF[USART_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART_RX_STA=0;       //����״̬���	  
  
void uart_init(u32 bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
  
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
  //USART1_RX	  GPIOA.10��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
   //USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 

}

void USART1_IRQHandler(void){                							//����1�жϷ������

	if (USART_GetFlagStatus(USART1,USART_FLAG_ORE)!= RESET){
	}
	if (USART_GetFlagStatus(USART1,USART_IT_RXNE)!= RESET){
		USART_RX_BUF[USART_RX_STA++] = USART1->DR;
		USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);
		//printf("���ͱ��ģ�%s",USART_RX_BUF);
	}
	if(USART_GetFlagStatus(USART1,USART_FLAG_IDLE)!= RESET){
		USART_ITConfig(USART1,USART_IT_IDLE,DISABLE);
		
		printf("���ͱ��ģ�%s�����ȣ�%d\r\n",USART_RX_BUF,USART_RX_STA);
		if (!strncmp((const char *)USART_RX_BUF,"AT+setA",7)&&USART_RX_STA==12){
			char str[4];
			memset(str,'\0',4);
			str[0]=USART_RX_BUF[7];
			str[1]=USART_RX_BUF[8];
			str[2]=USART_RX_BUF[9];
			waatA=atoi(str);
		}
		if (!strncmp((const char *)USART_RX_BUF,"AT+setV",7)&&USART_RX_STA==14){
			char str[6];
			memset(str,'\0',6);
			str[0]=USART_RX_BUF[7];
			str[1]=USART_RX_BUF[8];
			str[2]=USART_RX_BUF[9];
			str[3]=USART_RX_BUF[10];
			str[4]=USART_RX_BUF[11];
			waatV=atoi(str);
		}
		
		if (!strncmp((const char *)USART_RX_BUF,"AT+setW",7)&&USART_RX_STA==14){
			char str[6];
			memset(str,'\0',6);
			str[0]=USART_RX_BUF[7];
			str[1]=USART_RX_BUF[8];
			str[2]=USART_RX_BUF[9];
			str[3]=USART_RX_BUF[10];
			str[4]=USART_RX_BUF[11];
			waatW=atoi(str);
		}
		
		memset(USART_RX_BUF,'\0',USART_REC_LEN);
		USART_RX_STA=0;
	}
	
} 
#endif	

