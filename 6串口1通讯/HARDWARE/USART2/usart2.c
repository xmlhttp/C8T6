#include <string.h>
#include "usart2.h"	  
#include "crc.h"
/*#define LengModBusMsg 19
#define CRC_pos_Hi 17
#define CRC_pos_Lo 18

extern u32 waatA;
extern u16 waatV;
extern u32 waatW;
*/
#if EN_USART2_RX   //���ʹ���˽���
//����1�жϷ������
//ע��,��ȡUSARTx->SR�ܱ���Ī������Ĵ���   	
u8 USART2_RX_BUF[USART2_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
//u8 ModBus_message[19]={'\0'};
//u16 CRC_result;
//����״̬
//bit15��	������ɱ�־
//bit14��	���յ�0x0d
//bit13~0��	���յ�����Ч�ֽ���Ŀ
u16 USART2_RX_STA=0;       //����״̬���	  
  
void uart2_init(u32 bound){
  //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO,ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	//USART1_TX   GPIOA.9
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.9
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
  //USART1_RX	  GPIOA.10��ʼ��
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA10
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//GPIO_Mode_IN_FLOATING;//��������
  GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

  //Usart1 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
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

  USART_Init(USART2, &USART_InitStructure); //��ʼ������1
  USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
  USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���1 
}

void USART2_IRQHandler(void){                	//����1�жϷ������
	if (USART_GetFlagStatus(USART2,USART_FLAG_ORE)!= RESET){
	
	}
	if (USART_GetFlagStatus(USART2,USART_IT_RXNE)!= RESET){
		USART2_RX_BUF[USART2_RX_STA++] = USART2->DR;
		USART_ITConfig(USART2,USART_IT_IDLE,ENABLE);
	}
	if(USART_GetFlagStatus(USART2,USART_FLAG_IDLE)!= RESET){
	//	u16 *p;
		USART_ITConfig(USART2,USART_IT_IDLE,DISABLE);
		//��������
		/*memset(ModBus_message,'\0',19);
		ModBus_message[0]=57;
		ModBus_message[1]=3;
		ModBus_message[2]=0;
		ModBus_message[3]=(u8)(waatW>>16);
		ModBus_message[4]=(u8)(waatW>>24);
		ModBus_message[5]=(u8)(waatW>>8);
		ModBus_message[6]=(u8)(waatW);
		ModBus_message[7]=(u8)(waatV>>8);
		ModBus_message[8]=(u8)(waatV);
		ModBus_message[9]=(u8)(waatA>>16);
		ModBus_message[10]=(u8)(waatA>>24);
		ModBus_message[11]=(u8)(waatA>>8);
		ModBus_message[12]=(u8)(waatA);
		ModBus_message[13]=0;
		ModBus_message[14]=0;
		ModBus_message[15]=0;
		ModBus_message[16]=0;
		CRC_result = CRC16 (( u8* )ModBus_message,17);
		ModBus_message[CRC_pos_Hi] = CRC_result >> 8;
		ModBus_message[CRC_pos_Lo] =CRC_result & 0x00FF;
	//	printf("����:%.2fKW*H����ѹ��%.2fV��������%.3fA\r\n",(float)((0xff & ModBus_message[6])<<24|(0xff & ModBus_message[5])<<16|(0xff & ModBus_message[3])<<8|(0xff & ModBus_message[4]))/10,(float)((0xff & ModBus_message[7])<<8|(0xff & ModBus_message[8]))/100,(float)((0xff & ModBus_message[11])<<8|(0xff & ModBus_message[12])|(0xff & ModBus_message[9])<<8|(0xff & ModBus_message[10]))/1000);

		p=(u16 *)ModBus_message; 
		USART_SendData(USART2,*p);													//����232���ݻ�ȡ����
		while (USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);	
		
		//printf("���ͱ��ģ�%s\r\n",ModBus_message);
		
		*/
		memset(USART2_RX_BUF,'\0',USART2_REC_LEN);
		USART2_RX_STA=0;
	}
} 
#endif	

