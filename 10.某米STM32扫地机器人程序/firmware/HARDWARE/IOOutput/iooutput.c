#include "iooutput.h"
#
/******************************************************************************
* 文件名  ：iooutput.c
* 文件描述：xiaomi主板普通输出IO配置
* 创建人  ：Pu Sheng
* 日    期：2019年11月8日
* 
* 硬件连接：PE6  - 电源控制IO       高电平关机
* 硬件连接：PE2  - 风机电源控制IO   高电平使能
* 硬件连接：PE8  - VA+电源控制IO    高电平使能
* 硬件连接：PC8  - VCC+电源控制IO   高电平使能
* 硬件连接：PA11 - V+电源控制IO     高电平使能
            PC6  - 超声波发送IO     高电平使能
            PC12 - 边刷控制引脚     1 正转
            PA8  - 滚刷控制引脚     0 正转
            PE4  - 右轮控制引脚     0 正转
            PE7  - 左轮控制引脚     0 正转
            
            4选1模拟通道控制引脚
            PD9
            PD11

******************************************************************************/
	    

/*******************************************************************************
* Function Name  : vAllOutputPinInit
* Description    : 主板所有输出IO  驱动初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vAllOutputPinInit(void)
{
    vBoradPowerCtlPinInit();
    vFanPowerCtlPinInit();
    vVccPowerCtlPinInit();
    vVAPowerCtlPinInit();
    vVPowerCtlPinInit();
    vSGM4878CtlPinInit();
    vWheelCtlPinInit();
    vSideBrushCtlPinInit();
    vRollBrushCtlPinInit();
    vUltrasonic_Init();
}

/*******************************************************************************
* Function Name  : vRollBrushCtlPinInit
* Description    : 滚刷控制引脚初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vRollBrushCtlPinInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能PA端口时钟
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //复用输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}
/*******************************************************************************
* Function Name  : vSideBrushCtlPinInit
* Description    : 边刷控制引脚初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vSideBrushCtlPinInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//使能PC端口时钟
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //复用输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);
}
/*******************************************************************************
* Function Name  : vWheelCtlPinInit
* Description    : 驱动轮控制引脚初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vWheelCtlPinInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);	//使能PE端口时钟
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //复用输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOE, &GPIO_InitStructure);
}

/*******************************************************************************
* Function Name  : vBoradPowerCtlPinInit
* Description    : 主板电源控制引脚  驱动初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vBoradPowerCtlPinInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);	//使能PE端口时钟
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //复用输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOE, GPIO_Pin_6);   //拉低 维持系统供电
}
/*******************************************************************************
* Function Name  : vFanPowerCtlPinInit
* Description    : 风机电源控制引脚  驱动初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vFanPowerCtlPinInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);	//使能PE端口时钟
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //复用输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOE, GPIO_Pin_2);  

}

/*******************************************************************************
* Function Name  : vVccPowerCtlPinInit
* Description    : VA+ （5v）控制引脚 高电平使能 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vVAPowerCtlPinInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);	//使能PE端口时钟
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //复用输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    GPIO_SetBits(GPIOE, GPIO_Pin_8);  

}

/*******************************************************************************
* Function Name  : vVccPowerCtlPinInit
* Description    : VCC+ （3.3v）控制引脚 高电平使能 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vVccPowerCtlPinInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//使能PC端口时钟
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_SetBits(GPIOC, GPIO_Pin_8);  

}

/*******************************************************************************
* Function Name  : vVccPowerCtlPinInit
* Description    : V+ （5v）控制引脚 高电平使能 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vVPowerCtlPinInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能PA端口时钟
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA, GPIO_Pin_11);  

}
/*******************************************************************************
* Function Name  : vSGM4878CtlPinInit
* Description    : 4选1模拟通道芯片SGM4878的控制引脚 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vSGM4878CtlPinInit(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	//使能PD端口时钟
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
    GPIO_Init(GPIOD, &GPIO_InitStructure);  
}


/*******************************************************************************
* Function Name  : Ultransonic_Init
* Description    :  超声波初始化  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vUltrasonic_Init(void)
{	
	vUltrasonic_GPIO_config();

}
/*******************************************************************************
* Function Name  : vvUltrasonic_GPIO_config
* Description    :  超声波发送引脚初始化  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vUltrasonic_GPIO_config(void)
{
	GPIO_InitTypeDef    GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(TRIG_CLOCK, ENABLE);	 //使能PC端口时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	 //使能复用时钟端口时钟	
	//发送初始化

	GPIO_InitStructure.GPIO_Pin = TRIG_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(TRIG_PORT, &GPIO_InitStructure);					 //根据设定参数初始化GPIO
	GPIO_ResetBits(TRIG_PORT,TRIG_PIN);
 

	
}
/*******************************************************************************
* Function Name  :vUltrasonic_Emit
* Description    : 超声波触发
* Input          : 
* Output         : None
* Return         : None
*******************************************************************************/
void vUltrasonic_Emit(void)
{
	//发送超声波
	vTaskSuspendAll();  //挂起任务调度器 
    TIM7_CNT_SET( 0 );  //定时器7计数清零
	GPIO_SetBits( TRIG_PORT, TRIG_PIN );   
	delay_us(30);
	GPIO_ResetBits( TRIG_PORT ,TRIG_PIN );    
	xTaskResumeAll();  //恢复任务调度器 lhc
    

	
}

