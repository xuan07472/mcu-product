#include "key.h"

/******************************************************************************
* 文件名  ：key.c
* 文件描述：xiaomi主板按键驱动
* 创建人  ：Pu Sheng
* 日    期：2019年9月27日
* 
* 硬件连接：PB4 - KEYI2 (电平0为按下)
* 硬件连接：PD7 - KEYI1 (电平0为按下)
******************************************************************************/
	    


/*******************************************************************************
* Function Name  : Key_Init
* Description    : 按键 IO  驱动初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vKey_Init(void)
{    
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //使能PB端口时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	 //使能PD端口时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	 //使能复用时钟端口时钟
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 		     //上拉输入
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_Init(GPIOD, &GPIO_InitStructure);//根据设定参数初始化GPIOD
    
    GPIO_SetBits(GPIOB,GPIO_Pin_4);	
    GPIO_SetBits(GPIOD,GPIO_Pin_7);	
    
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//关闭PB4的jTAG下载功能 但保持SW接口， 
}



