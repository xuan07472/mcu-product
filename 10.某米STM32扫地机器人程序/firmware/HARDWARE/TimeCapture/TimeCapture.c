#include "TimeCapture.h"

/******************************************************************************
* 文件名  ：TimeCapture.c
* 文件描述：xiaomi主板PWM捕获IO配置
* 创建人  ：Pu Sheng
* 日    期：2019年11月8日
* 
* 硬件连接：
    PC9  - TIM8_CH4 风机编码器
    PE9  - TIM1_CH1 左轮编码器
    PE11 - TIM1_CH2 右轮编码器
******************************************************************************/

/*风机编码计数 一个脉冲自增1*/
u32 g_FanEncodeCount;

WheelSruct_t g_tLeftWheel = {0};  //轮子结构
WheelSruct_t g_tRightWheel = {0};  //轮子结构

/*
* Function Name  : vAll_TIM_CaptureInit
* Description    : 主板所有定时器捕获驱动初始化
* Input          : None
* Output         : None
* Return         : None
*/
void vAll_TIM_CaptureInit(void)
{
    TIM8_Capture_Init();
    TIM1_Capture_Init();
}
/*
* Function Name  : TIM8_Capture_Init
* Description    : 主板所有TIM8捕获 驱动初始化
* Input          : None
* Output         : None
* Return         : None
*/
void TIM8_Capture_Init(void)
{
    TIM8_CH4_FanCaptureInit(TIM8_ARR,TIM8_PSC);
}
/*
* Function Name  : TIM1_Capture_Init
* Description    : 主板所有TIM1捕获 驱动初始化
* Input          : None
* Output         : None
* Return         : None
*/
void TIM1_Capture_Init(void)
{
    TIM1_CH12WheelCaptureInit(TIM1_ARR,TIM1_PSC);
}
/*******************************************************************************
* Function Name  : TIM8_CH4_FanCaptureInit
* Description    : 主板TIM8 CH4风机编码捕获驱动初始化
* Input          : arr 重装载值 psc 分频系数
* Output         : None
* Return         : None
*******************************************************************************/
void TIM8_CH4_FanCaptureInit(u16 arr,u16 psc)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_ICInitTypeDef  TIM_ICInitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//使能PC端口时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);	//使能定时器8时钟
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 		 //复用输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    TIM_TimeBaseStructure.TIM_Period = arr-1; 					//设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc-1; 					//设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数模式
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure); 		
    
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_4;                    //CC1S=01 IC1 connect to TI1
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; 
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 
    TIM_ICInitStructure.TIM_ICFilter = 0x05;                            //filter
    TIM_ICInit( TIM8, &TIM_ICInitStructure );
    
    TIM_ClearITPendingBit( TIM8,TIM_IT_CC4 );
    TIM_ITConfig( TIM8,TIM_IT_CC4, ENABLE );  //config IT
    NVIC_InitStructure.NVIC_IRQChannel = TIM8_CC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 13;           
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		            
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			            
    NVIC_Init( &NVIC_InitStructure );	
    
    TIM_Cmd(TIM8, ENABLE);    
}


/*******************************************************************************
* Function Name  : TIM1_CH12WheelCaptureInit
* Description    : 主板TIM1 CH1 CH2左右轮编码捕获驱动初始化
* Input          : arr 重装载值 psc 分频系数
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_CH12WheelCaptureInit(u16 arr,u16 psc)
{
    GPIO_InitTypeDef         GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef         NVIC_InitStructure;
    TIM_ICInitTypeDef        TIM_ICInitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);	//使能PE端口时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);	//使能定时器1时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	//使能定时器1时钟
  
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM1, ENABLE);      //Timer1完全重映射   
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 		 //复用输出
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    
    
    TIM_TimeBaseStructure.TIM_Period = arr-1; 					//设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler =psc-1; 					//设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数模式
	TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure); 		
    
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_1;                    //CC1S=01 IC1 connect to TI1
    TIM_ICInitStructure.TIM_ICPolarity = TIM_ICPolarity_Rising;	
    TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI; 
    TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;	 
    TIM_ICInitStructure.TIM_ICFilter = 0x05;                
    TIM_ICInit( TIM1, &TIM_ICInitStructure );
    
    TIM_ICInitStructure.TIM_Channel = TIM_Channel_2;
    TIM_ICInit( TIM1, &TIM_ICInitStructure );
    
    TIM_ClearITPendingBit( TIM1,TIM_IT_CC1|TIM_IT_CC2 );
    TIM_ITConfig( TIM1,TIM_IT_CC1, ENABLE );  
    TIM_ITConfig( TIM1,TIM_IT_CC2, ENABLE );
    
    NVIC_InitStructure.NVIC_IRQChannel = TIM1_CC_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;           
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		            
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			            
    NVIC_Init( &NVIC_InitStructure );	
    
    TIM_Cmd(TIM1, ENABLE);    
}

void TIM1_CC_IRQHandler( void )
{
    if(SET == TIM_GetITStatus(TIM1,TIM_IT_CC1) )
    {
        TIM_ClearITPendingBit( TIM1,TIM_IT_CC1 ); //清除中断标志位    
        g_tLeftWheel.TimeOut=0;
        g_tLeftWheel.usNew = TIM_GetCapture1( TIM1 );  

        if( g_tLeftWheel.usNew < g_tLeftWheel.usHistory )
        {
            g_tLeftWheel.usEncodeBuffer[g_tLeftWheel.ucPoint] = g_tLeftWheel.usNew+TIM1_ARR-g_tLeftWheel.usHistory+1;
        }
        else
        {
            g_tLeftWheel.usEncodeBuffer[g_tLeftWheel.ucPoint] = g_tLeftWheel.usNew-g_tLeftWheel.usHistory;
        } 
        GET_LEFT_ENCODE_DONE(  ); 
    }

    if(SET == TIM_GetITStatus(TIM1,TIM_IT_CC2) )
    {
        TIM_ClearITPendingBit( TIM1,TIM_IT_CC2 ); //清除中断标志位
        g_tRightWheel.TimeOut=0;        
        g_tRightWheel.usNew = TIM_GetCapture2( TIM1 );  

        if( g_tRightWheel.usNew < g_tRightWheel.usHistory )
        {
            g_tRightWheel.usEncodeBuffer[g_tRightWheel.ucPoint] = g_tRightWheel.usNew+TIM1_ARR-g_tRightWheel.usHistory+1;
        }
        else
        {
            g_tRightWheel.usEncodeBuffer[g_tRightWheel.ucPoint] = g_tRightWheel.usNew-g_tRightWheel.usHistory;
        } 
        GET_RIGHT_ENCODE_DONE(  );    

    }
}


void TIM8_CC_IRQHandler( void )
{
    if(SET == TIM_GetITStatus(TIM8,TIM_IT_CC4) )
    { 
        TIM_ClearITPendingBit( TIM8,TIM_IT_CC4 ); //清除中断标志位         

        g_FanEncodeCount++;
    }
}







