#include "sys.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK Mini STM32������
//ϵͳ�жϷ������û�		   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/10
//�汾��V1.4
//��Ȩ���У�����ؾ���
//Copyright(C) ����ԭ�� 2009-2019
//All rights reserved
//********************************************************************************  

//THUMBָ�֧�ֻ������
//�������·���ʵ��ִ�л��ָ��WFI  
__asm void WFI_SET(void){
	WFI;         
}
//�ر������ж�
__asm void INTX_DISABLE(void){
    CPSID I;         
}
//���������ж�
__asm void INTX_ENABLE(void){
    CPSIE I;         
}
//����ջ����ַ
//addr:ջ����ַ
__asm void MSR_MSP(u32 addr){
    MSR MSP, r0             //set Main Stack value
    BX r14
}
