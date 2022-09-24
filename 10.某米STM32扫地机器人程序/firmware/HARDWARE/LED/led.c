#include "led.h"

/******************************************************************************
* 文件名  ：led.c
* 文件描述：xiaomi主板LED驱动
* 创建人  ：Pu Sheng
* 日    期：2019年9月27日
* 
* 硬件连接：PB3 - LED_CTL2  - RI1 RI10 RI4 RI17 (红灯)  TIM2 CHANNEL2
*           PB10 - LED_CTL  - RI2 RI11 RI5 RI18 (白灯)  TIM2 CHANNEL3
*           PB11 - LED_CTL1 - RI3 RI12 RI6 RI19 (黄灯)  TIM2 CHANNEL4 都是高电平点亮 
******************************************************************************/
	    


/*******************************************************************************
* Function Name  : vLED_Init
* Description    : LED IO  驱动初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vLEDPWM_Init(void)
{
 	
    GPIO_InitTypeDef  GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//使能PB端口时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);	//使能定时器2时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	//使能复用时钟端口时钟

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_10|GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //复用输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);					 //根据设定参数初始化GPIOB
    
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//关闭PB3的jTAG下载功能 但保持SW接口
	GPIO_PinRemapConfig(GPIO_FullRemap_TIM2, ENABLE);      //Timer2完全重映射   映射到PB3 10 11  
 
 
   //初始化TIM2
	TIM_TimeBaseStructure.TIM_Period = TIM2_ARR-1; //设置在下一个更新事件装入活动的自动重装载寄存器周期的值
	TIM_TimeBaseStructure.TIM_Prescaler =TIM2_PSC-1; //设置用来作为TIMx时钟频率除数的预分频值 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0; //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	//初始化TIM2  PWM模式	 
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式2
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
    TIM_OCInitStructure.TIM_Pulse=0;
	TIM_OC2Init(TIM2, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM2 OC2
    TIM_OC3Init(TIM2, &TIM_OCInitStructure);  
    TIM_OC4Init(TIM2, &TIM_OCInitStructure);  
    
    TIM_OC2PreloadConfig(TIM2, TIM_OCPreload_Enable);  //使能TIM2在CCR2上的预装载寄存器
    TIM_OC3PreloadConfig(TIM2, TIM_OCPreload_Enable);  
    TIM_OC4PreloadConfig(TIM2, TIM_OCPreload_Enable); 
    
	TIM_Cmd(TIM2, ENABLE);  //使能TIM2    
}
 
