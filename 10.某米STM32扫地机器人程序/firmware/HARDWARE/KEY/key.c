#include "key.h"

/******************************************************************************
* �ļ���  ��key.c
* �ļ�������xiaomi���尴������
* ������  ��Pu Sheng
* ��    �ڣ�2019��9��27��
* 
* Ӳ�����ӣ�PB4 - KEYI2 (��ƽ0Ϊ����)
* Ӳ�����ӣ�PD7 - KEYI1 (��ƽ0Ϊ����)
******************************************************************************/
	    


/*******************************************************************************
* Function Name  : Key_Init
* Description    : ���� IO  ������ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vKey_Init(void)
{    
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PB�˿�ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	 //ʹ��PD�˿�ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	 //ʹ�ܸ���ʱ�Ӷ˿�ʱ��
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		     //��������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_Init(GPIOD, &GPIO_InitStructure);//�����趨������ʼ��GPIOD
    
    GPIO_SetBits(GPIOB,GPIO_Pin_4);	
    GPIO_SetBits(GPIOD,GPIO_Pin_7);	
    
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//�ر�PB4��jTAG���ع��� ������SW�ӿڣ� 
}



