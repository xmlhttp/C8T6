#ifndef __USART_H
#define __USART_H
#include "stdio.h"	
#include "stdlib.h"
#include "sys.h" 

#define USART_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART1_RX 			1		//ʹ�ܣ�1��/��ֹ��0������1����
void uart_init(u32 bound);
#endif


