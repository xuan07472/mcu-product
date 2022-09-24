#include "timepwm.h"

/******************************************************************************
* 文件名  ：timepwm.c
* 文件描述：xiaomi主板PWM输出IO配置
* 创建人  ：Pu Sheng
* 日    期：2019年11月8日
* 
* 硬件连接：
            PA1  - 风机控制IO TIM5_CH2高电平全速
            PB0  -TIM3_CH3  右轮 低电平越多越快
            PB1  -TIM3_CH4  左轮
            PD13 -TIM4_CH2  滚刷 高电平越多越快
            PD14 -TIM4_CH3  边刷 高电平越多越快
            PD15 -TIM4_CH4  防跌落  50%时
******************************************************************************/


/*******************************************************************************
* Function Name  : All_PWM_Init
* Description    : 主板所有PWM输出初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vAll_PWM_Init(void )
{
    TIM3_PWM_Init();
    TIM4_PWM_Init();
    TIM5_PWM_Init();
}
/*******************************************************************************
* Function Name  : TIM5_PWM_Init
* Description    : 主板TIM5驱动初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM5_PWM_Init(void)
{
    TIM5_CH2_FanInit(TIM5_ARR,TIM5_PSC);
}

/*******************************************************************************
* Function Name  : TIM3_PWM_Init
* Description    : 主板TIM3驱动初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM3_PWM_Init(void)
{
    TIM3_CH34_Init(TIM3_ARR,TIM3_PSC);
}
/*******************************************************************************
* Function Name  : TIM4_PWM_Init
* Description    : 主板TIM4驱动初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TIM4_PWM_Init(void)
{
    TIM4_CH234_Init(TIM4_ARR,TIM4_PSC);
    
    DROP_T4CH4_PWM_CYCLE(TIM4_CH4_CCR);

}

/*******************************************************************************
* Function Name  : TIM5_CH2_FanInit
* Description    : 主板TIM5 CH2风机驱动初始化
* Input          : arr 重装载值 psc 分频系数
* Output         : None
* Return         : None
*******************************************************************************/
void TIM5_CH2_FanInit(u16 arr,u16 psc)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能PA端口时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);	//使能定时器5时钟
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //复用输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    TIM_TimeBaseStructure.TIM_Period = arr-1; 					//设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
    TIM_TimeBaseStructure.TIM_Prescaler =psc-1; 					//设置用来作为TIMx时钟频率除数的预分频值
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 	
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数模式
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); 		


    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式1
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
    TIM_OCInitStructure.TIM_Pulse=0;
    TIM_OC2Init(TIM5, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM5 OC2

    TIM_OC2PreloadConfig(TIM5, TIM_OCPreload_Enable);  //使能TIM5在CCR2上的预装载寄存器

    TIM_Cmd(TIM5, ENABLE);    
}

/*******************************************************************************
* Function Name  : TIM3_CH34_Init
* Description    : 主板TIM3 CH34驱动初始化
* Input          : arr 重装载值 psc 分频系数
* Output         : None
* Return         : None
*******************************************************************************/
void TIM3_CH34_Init(u16 arr,u16 psc)
{
    GPIO_InitTypeDef         GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//使能PB端口时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);	//使能定时器3时钟
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //复用输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    TIM_TimeBaseStructure.TIM_Period = arr-1; 					//设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc-1; 					//设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数模式
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); 		
    

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式1
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
    TIM_OCInitStructure.TIM_Pulse=0;
	TIM_OC3Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM5 OC2
    TIM_OC4Init(TIM3, &TIM_OCInitStructure);  //根据T指定的参数初始化外设TIM5 OC2
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM5在CCR2上的预装载寄存器
    TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable);  //使能TIM5在CCR2上的预装载寄存器
    
    TIM_Cmd(TIM3, ENABLE);    
}


/*******************************************************************************
* Function Name  : TIM4_CH234_Init
* Description    : 主板TIM4 CH23驱动初始化
* Input          : arr 重装载值 psc 分频系数
* Output         : None
* Return         : None
*******************************************************************************/
void TIM4_CH234_Init(u16 arr,u16 psc)
{
    GPIO_InitTypeDef         GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	//使能PD端口时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);	//使能定时器4时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	//
    
    GPIO_PinRemapConfig(GPIO_Remap_TIM4,ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13|GPIO_Pin_14|GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //复用输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    TIM_TimeBaseStructure.TIM_Period = arr-1; 					//设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc-1; 					//设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); 		
    

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //选择定时器模式:TIM脉冲宽度调制模式1
 	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //输出极性:TIM输出比较极性高
    TIM_OCInitStructure.TIM_Pulse=0;
    TIM_OC2Init(TIM4, &TIM_OCInitStructure);  
	TIM_OC3Init(TIM4, &TIM_OCInitStructure); 
    TIM_OC4Init(TIM4, &TIM_OCInitStructure);  

    TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
    TIM_OC3PreloadConfig(TIM4, TIM_OCPreload_Enable);  
    TIM_OC4PreloadConfig(TIM4, TIM_OCPreload_Enable); 
    
    TIM_Cmd(TIM4, ENABLE);    
}












