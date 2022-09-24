#include "timer.h"


/******************************************************************************
* 文件名  timer.c
* 文件描述：计时定时器
* 创建人  ：
* 日    期：2019年11月8日
* 
* 硬件连接：
        TIM6    -   陀螺仪     更新中断为5ms
        TIM7    -   超声波     计数为1us
    
******************************************************************************/

/****************************************************************************************
 *函数名: TIM6_Int_Init
 *描述:   TIM6初始化函数,配置为5ms产生更新中断
 *输入:   无
 *输出：  无
 *返回值：无
****************************************************************************************/
void vTIM6_Int_Init(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); //时钟使能
	
	//定时器TIM6初始化
	TIM_TimeBaseStructure.TIM_Period = 4999; 					//设置在下一个更新事件装入活动的自动重装载寄存器周期的值 1M/(4999+1) = 200HZ	
	TIM_TimeBaseStructure.TIM_Prescaler =71; 					//设置用来作为TIMx时钟频率除数的预分频值 72M/(71+1) = 1M
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 	//设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM向上计数模式
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure); 			//根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE ); 					//使能指定的TIM6中断,允许更新中断

	//中断优先级NVIC设置
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;  			//TIM6中断
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  	//先占优先级5级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  		//从优先级0级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 			//IRQ通道被使能
	NVIC_Init(&NVIC_InitStructure);  							//初始化NVIC寄存器

	TIM_Cmd(TIM6, ENABLE);  									//使能TIM6					 
}

/****************************************************************************************
 *函数名: GyroUpdata_IRQHander
 *描述:   TIM6中断服务函数，累加陀螺仪更新次数
 *输入:   无
 *输出：  无
 *返回值：无
****************************************************************************************/
u8 GyroUpdateCNT = 0;//陀螺仪更新计数
void GyroUpdata_IRQHander(void)
{
	if(TIM_GetITStatus(TIM6,TIM_IT_Update)==SET) //溢出中断
	{
        GyroUpdateCNT++;//每5ms累加
	}
    TIM_ClearITPendingBit(TIM6,TIM_IT_Update);  //清除中断标志位
}


/*******************************************************************************
* Function Name  : vTIM7_Configuration
* Description    : 超声波定时器 超声波使用时会清零计数器
* Input          : 
* Output         : None
* Return         : None
*******************************************************************************/
void vTIM7_Configuration(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure; 
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7 , ENABLE);

    TIM_DeInit(TIM7);
    TIM_TimeBaseStructure.TIM_Period=TIM7_PEROID;		 	
    TIM_TimeBaseStructure.TIM_Prescaler= (72 - 1);				
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 		
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
    TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM7, TIM_FLAG_Update);	 //清除更新中断，免得一打开中断立即产生中断  

    TIM_Cmd(TIM7, ENABLE);													
}

