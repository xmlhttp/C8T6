#include "sys.h"
#include "delay.h"
#include "usart2.h"	 

////////////////////////////////////////////////////////////////////////////////// 	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "includes.h"					//ucos 使用	  
#endif

int BUFLENGTH=0; //当前字符串长度
u8 read_buf[19] = {'\0'};
u8 tep_buf[10]= {'\0'}; 
extern char tcp_client_sendbuf[100];
//是否允许发送数据完成
extern u8 IsSend;
//是否连接
extern u8 islink;
void uart2_init(u32 bound){
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOC|RCC_APB2Periph_AFIO, ENABLE);	//使能USART2，GPIOA时钟
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);
	//USART2_TX   GPIOA.2
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PA.2
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.2
   
	//USART2_RX	  GPIOA.3初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;//PA3
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.3  
	//初始化急停输入GPIOC.1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;//PC1
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;//PD7
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;	  //半双工引脚
	GPIO_Init(GPIOC,&GPIO_InitStructure);
	//充电管脚
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//PC0
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	//Usart1 NVIC 配置							 
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
   //USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
	USART_Init(USART2, &USART_InitStructure); //初始化串口2
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启串口接受中断
	USART_Cmd(USART2, ENABLE);                    //使能串口2
	USART_ClearFlag(USART2,USART_FLAG_TC);
	GPIO_ResetBits(GPIOB,GPIO_Pin_5); 
}

//串口2中断
/*
判断接收数据的第一位是否为地址位0x01，第二位是否为功能码0x03,第三位为接收的数据长度0x06,最后两位CRC校验位，共计11位
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
		
		printf("电能:%.2fKW*H，电压：%.2fV，电流：%.3fA\r\n",(float)((0xff & read_buf[6])<<24|(0xff & read_buf[5])<<16|(0xff & read_buf[3])<<8|(0xff & read_buf[4]))/10,(float)((0xff & read_buf[7])<<8|(0xff & read_buf[8]))/100,(float)((0xff & read_buf[11])<<8|(0xff & read_buf[12])|(0xff & read_buf[9])<<8|(0xff & read_buf[10]))/1000);

		BUFLENGTH=0;		
  	}
	OSIntExit(); 	   
}

