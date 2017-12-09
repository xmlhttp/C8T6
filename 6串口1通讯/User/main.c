#include "delay.h"
#include "sys.h"
#include "usart.h"
#include "usart2.h"
#include <stdio.h>
#include <stdint.h>
#include "crc.h"
#define LengModBusMsg 19
#define CRC_pos_Hi 17
#define CRC_pos_Lo 18

u8 ModBus_message[19];
extern u32 waatA;
extern u16 waatV;
extern u32 waatW;

int main(void){
	u16 CRC_result;
	delay_init();
	uart_init(115200);
	uart2_init(115200);
	
while(1){

		ModBus_message[0]=57;
		ModBus_message[1]=3;
		ModBus_message[2]=0;
		ModBus_message[3]=(u8)(waatW>>8);
		ModBus_message[4]=(u8)(waatW);
		ModBus_message[5]=(u8)(waatW>>16);
		ModBus_message[6]=(u8)(waatW>>24);
		ModBus_message[7]=(u8)(waatV>>8);
		ModBus_message[8]=(u8)(waatV);
		ModBus_message[9]=(u8)(waatA>>8);
		ModBus_message[10]=(u8)(waatA);
		ModBus_message[11]=(u8)(waatA>>24);
		ModBus_message[12]=(u8)(waatA>>16);
		ModBus_message[13]=0;
		ModBus_message[14]=0;
		ModBus_message[15]=0;
		ModBus_message[16]=0;
		CRC_result = CRC16 (( u8* )ModBus_message,17);
		ModBus_message[CRC_pos_Hi] = CRC_result >> 8;
		ModBus_message[CRC_pos_Lo] =CRC_result & 0x00FF;
		printf("原始数据:%s\r\n",ModBus_message);
	printf("电能:%.1fKW*H，电压：%.2fV，电流：%.3fA\r\n",
		(float)((0xff & ModBus_message[6])<<24|(0xff & ModBus_message[5])<<16|(0xff & ModBus_message[3])<<8|(0xff & ModBus_message[4]))/10,
		(float)((0xff & ModBus_message[7])<<8|(0xff & ModBus_message[8]))/100,
		(float)((0xff & ModBus_message[11])<<24|(0xff & ModBus_message[12])<<16|(0xff & ModBus_message[9])<<8|(0xff & ModBus_message[10]))/1000);

		printf("电能:%d，电压：%d，电流：%d\r\n",waatW,waatV,waatA);
	delay_ms(1000);
}
}

