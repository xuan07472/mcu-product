#include "led.h"

/******************************************************************************
* �ļ���  ��led.c
* �ļ�������xiaomi����LED����
* ������  ��Pu Sheng
* ��    �ڣ�2019��9��27��
* 
* Ӳ�����ӣ�PB3 - LED_CTL2  - RI1 RI10 RI4 RI17 (���)  TIM2 CHANNEL2
*           PB10 - LED_CTL  - RI2 RI11 RI5 RI18 (�׵�)  TIM2 CHANNEL3
*           PB11 - LED_CTL1 - RI3 RI12 RI6 RI19 (�Ƶ�)  TIM2 CHANNEL4 ���Ǹߵ�ƽ���� 
******************************************************************************/
	    


/*******************************************************************************
* Function Name  : vLED_Init
* Description    : LED IO  ������ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vLEDPWM_Init(void)
{
 	
    GPIO_InitTypeDef  GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//ʹ��PB�˿�ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//ʹ�ܶ�ʱ��2ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	//ʹ�ܸ���ʱ�Ӷ˿�ʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_10|GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);					 //�����趨������ʼ��GPIOB
    
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//�ر�PB3��jTAG���ع��� ������SW�ӿ�
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE);      //Timer2��ȫ��ӳ��   ӳ�䵽PB3 10 11  
 
 
   //��ʼ��TIM2
	TIM_TimeBaseStructure.TIM_Period = TIM2_ARR-1; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
	TIM_TimeBaseStructure.TIM_Prescaler =TIM2_PSC-1; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	//��ʼ��TIM2  PWMģʽ	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
    TIM_OCInitStructure.TIM_Pulse=0;
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);  //����Tָ���Ĳ�����ʼ������TIM2 OC2
    TIM_OC3Init(TIM2, &TIM_OCInitStructure);  
    TIM_OC4Init(TIM2, &TIM_OCInitStructure);  
    
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);  //ʹ��TIM2��CCR2�ϵ�Ԥװ�ؼĴ���
    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);  
    TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable); 
    
	TIM_Cmd(TIM2, ENABLE);  //ʹ��TIM2    
}
 
