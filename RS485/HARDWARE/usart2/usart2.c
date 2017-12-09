#include "sys.h"
#include "delay.h"
#include "usart2.h"	 

////////////////////////////////////////////////////////////////////////////////// 	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos ʹ��	  
#endif

int BUFLENGTH=0; //��ǰ�ַ�������
u8 read_buf[19] = {'\0'};
u8 tep_buf[10]= {'\0'}; 
extern char tcp_client_sendbuf[100];
//�Ƿ��������������
extern u8 IsSend;
//�Ƿ�����
extern u8 islink;
void uart2_init(u32 bound){
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE);	//ʹ��USART2��GPIOAʱ��
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	//USART2_TX   GPIOA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.2
   
	//USART2_RX	  GPIOA.3��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.3  
	//��ʼ����ͣ����GPIOC.1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;//PC1
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;//PD7
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	  //��˫������
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	//���ܽ�
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//PC0
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	//Usart1 NVIC ����							 
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//��ռ���ȼ�3
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
	USART_Init(USART2, &USART_InitStructure); //��ʼ������2
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
	USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���2
	USART_ClearFlag(USART2,USART_FLAG_TC);
	GPIO_ResetBits(GPIOB,GPIO_Pin_5); 
}

//����2�ж�
/*
�жϽ������ݵĵ�һλ�Ƿ�Ϊ��ַλ0x01���ڶ�λ�Ƿ�Ϊ������0x03,����λΪ���յ����ݳ���0x06,�����λCRCУ��λ������11λ
*/
void USART2_IRQHandler(void){
	OSIntEnter();
	if(USART_GetFlagStatus(USART2, USART_FLAG_ORE) != Bit_RESET){
		USART_ClearFlag(USART2, USART_FLAG_ORE);
		if(BUFLENGTH==0){
			u8 temp=USART_ReceiveData(USART2);
			if(temp==57){
				read_buf[BUFLENGTH]=temp;
				BUFLENGTH++;	
			}
		}else if(BUFLENGTH==1){
			u8 temp=USART_ReceiveData(USART2);
			if(temp==3){
				read_buf[BUFLENGTH]=temp;
				BUFLENGTH++;	
			}
		}else{
			read_buf[BUFLENGTH]=USART_ReceiveData(USART2);
			BUFLENGTH++;
		}
	}
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != Bit_RESET){
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		if(BUFLENGTH==0){
			u8 temp=USART_ReceiveData(USART2);
			if(temp==57){
				read_buf[BUFLENGTH]=temp;
				BUFLENGTH++;	
			}
		}else if(BUFLENGTH==1){
			u8 temp=USART_ReceiveData(USART2);
			if(temp==3){
				read_buf[BUFLENGTH]=temp;
				BUFLENGTH++;	
			}
		}else{
			read_buf[BUFLENGTH]=USART_ReceiveData(USART2);
			BUFLENGTH++;
		}
  	}
	if(BUFLENGTH==19&&CHK_CRC16(read_buf,19)==1){
		
		printf("����:%.2fKW*H����ѹ��%.2fV��������%.3fA\r\n",(float)((0xff & read_buf[6])<<24|(0xff & read_buf[5])<<16|(0xff & read_buf[3])<<8|(0xff & read_buf[4]))/10,(float)((0xff & read_buf[7])<<8|(0xff & read_buf[8]))/100,(float)((0xff & read_buf[11])<<8|(0xff & read_buf[12])|(0xff & read_buf[9])<<8|(0xff & read_buf[10]))/1000);

		BUFLENGTH=0;		
  	}
	OSIntExit(); 	   
}

