#include "can.h"

u8 CAN_Mode_Init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp,u8 mode){
	GPIO_InitTypeDef GPIO_InitStructure;
	CAN_InitTypeDef CAN_InitStructure;
	CAN_FilterInitTypeDef CAN_FilterInitStructure;
#if CAN_RX0_INT_ENABLE
	NVIC_InitTypeDef NVIC_InitStructure;
#endif
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1,ENABLE);
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_IPU;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//参数复位
	CAN_DeInit(CAN1);
	CAN_StructInit(&CAN_InitStructure);
	
	//CAN单元设置
	CAN_InitStructure.CAN_TTCM=DISABLE;		//时间出触发模式
	CAN_InitStructure.CAN_ABOM=DISABLE;		//是否使用离线管理
	CAN_InitStructure.CAN_AWUM=DISABLE;		//睡眠模式是否唤醒
	CAN_InitStructure.CAN_NART=ENABLE;		//是否使用自动重传输
	CAN_InitStructure.CAN_RFLM=DISABLE;		//新数据覆盖旧数据
	CAN_InitStructure.CAN_TXFP=DISABLE;		//由标识符决定重传顺序
	CAN_InitStructure.CAN_Mode=mode;			//初始化模式
	//设置率
	CAN_InitStructure.CAN_SJW=tsjw;
	CAN_InitStructure.CAN_BS1=tbs1;
	CAN_InitStructure.CAN_BS2=tbs2;
	CAN_InitStructure.CAN_Prescaler=brp;
	CAN_Init(CAN1,&CAN_InitStructure);
	
	CAN_FilterInitStructure.CAN_FilterNumber=0;
	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;
	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;
	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;	//32位ID
	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;//32位MASK
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;
	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;//过滤器0关联到FIFO0
	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;//激活过滤器0

	CAN_FilterInit(&CAN_FilterInitStructure);			//滤波器初始化
	
#if CAN_RX0_INT_ENABLE
	CAN_ITConfig(CAN1,CAN_IO_FMP0,ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;     // 主优先级为1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;            // 次优先级为0
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
#endif
	return 0;
}
#if CAN_RX0_INT_ENABLE
void USB_LP_CAN1_RX0_IRQHandler(void){
	CanRxMsg RxMessage;
	int i=0;
	CAN_Receive(CAN1,0,&RxMessage);
	for(i=0;i<8;i++){
		printf("rxbuf[%d]:%d\r\n",i,RxMessage.Data[i]);
	}
}
#endif
//发送函数
u8 Can_Send_Msg(u8* msg,u8 len){	
	u8 mbox;
	u16 i=0;
	CanTxMsg TxMessage;
	TxMessage.StdId=0x12;						// 标准标识符 
	TxMessage.ExtId=0x12;						// 设置扩展标示符 
	TxMessage.IDE=CAN_Id_Standard; 	// 标准帧
	TxMessage.RTR=CAN_RTR_Data;			// 数据帧
	TxMessage.DLC=len;							// 要发送的数据长度
	for(i=0;i<len;i++)
	TxMessage.Data[i]=msg[i];			          
	mbox= CAN_Transmit(CAN1, &TxMessage);   
	i=0; 
	while((CAN_TransmitStatus(CAN1, mbox)==CAN_TxStatus_Failed)&&(i<0XFFF))i++;	//等待发送结束
	if(i>=0XFFF)return 1;
	return 0;	 
}
//接收函数
u8 Can_Receive_Msg(u8 *buf){		   		   
	u32 i;
	CanRxMsg RxMessage;
	if( CAN_MessagePending(CAN1,CAN_FIFO0)==0)return 0;		//没有接收到数据,直接退出 
	CAN_Receive(CAN1, CAN_FIFO0, &RxMessage);//读取数据	
	for(i=0;i<8;i++)
	buf[i]=RxMessage.Data[i];  
	return RxMessage.DLC;	
}







