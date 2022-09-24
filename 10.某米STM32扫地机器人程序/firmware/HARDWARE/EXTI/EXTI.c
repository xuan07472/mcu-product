#include "exti.h"

/******************************************************************************
* 文件名  ：exti.c
* 文件描述：xiaomi主板外部中断引脚输入
* 创建人  ：Pu Sheng
* 日    期：2019年11月14日
* 
* 硬件连接：
           
            PD1    EXTI1  -  右轮悬空传感器        悬空后是高电平
            PD2    EXTI2  -  左轮悬空传感器
            PE3    EXTI3  -  电源芯片ACOK事件引脚 有充电座时 变为高电平 无时变低
            PC7    EXTI7  -  超声波距离传感器      距离为低电平时间
            PD8    EXTI8  -  左边碰撞传感器        碰撞后是高电平
            PD10   EXTI10 -  右边碰撞传感器        碰撞后是高电平
            PE13   EXTI13 -  电源芯片事件引脚
            
******************************************************************************/
	   
__IO TIM7Time_t tULTime={0};  //超声波时间

/*******************************************************************************
* Function Name  : vAll_EXTI_PinInit
* Description    : 主板所有外部中断 IO  驱动初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vAll_EXTI_PinInit(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;
    NVIC_InitTypeDef    NVIC_InitStructure;
    EXTI_InitTypeDef    EXTI_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//使能复用功能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOD, ENABLE);	//使能PE端口时钟

    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 	
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_10|GPIO_Pin_1|GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

  	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource13);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOE,GPIO_PinSource3);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD,GPIO_PinSource10);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD,GPIO_PinSource8);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD,GPIO_PinSource2);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOD,GPIO_PinSource1);
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOC,GPIO_PinSource7);
    
    EXTI_InitStructure.EXTI_Line=EXTI_Line13|EXTI_Line3;	
  	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;	
  	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  	EXTI_Init(&EXTI_InitStructure);	
    
    EXTI_InitStructure.EXTI_Line=EXTI_Line8|EXTI_Line10|EXTI_Line2|EXTI_Line1;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_Init(&EXTI_InitStructure);	
    
    EXTI_InitStructure.EXTI_Line=EXTI_Line7;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_Init(&EXTI_InitStructure);		

    NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;			
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;	//抢占优先级2， 
  	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0;
  	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;		
  	NVIC_Init(&NVIC_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;				
  	NVIC_Init(&NVIC_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn;				
  	NVIC_Init(&NVIC_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn;				
  	NVIC_Init(&NVIC_InitStructure);
    
    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;				
  	NVIC_Init(&NVIC_InitStructure);
}


void EXTI3_IRQHandler(void)
{
    if(SET == EXTI_GetITStatus(EXTI_Line3))
    {
        EXTI_ClearITPendingBit(EXTI_Line3);
        DEBUG("EXTI_Line3 充电座离开\r\n");
    }
}
void EXTI1_IRQHandler(void)
{
    if(SET == EXTI_GetITStatus(EXTI_Line1))
    {
        EXTI_ClearITPendingBit(EXTI_Line1);
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.RightWheelAbn=WHEEL_R_VACANT;
        DEBUG("EXTI_Line1 右轮悬空 %d \r\n",g_tRobotState.Robotstate.unionAbnormal.AbnIden.RightWheelAbn);
    }
}
void EXTI2_IRQHandler(void)
{
    if(SET == EXTI_GetITStatus(EXTI_Line2))
    {
        EXTI_ClearITPendingBit(EXTI_Line2);
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.LeftWheelAbn=WHEEL_L_VACANT;//赋值状态
        DEBUG("EXTI_Line2 左轮悬空 %d \r\n",g_tRobotState.Robotstate.unionAbnormal.AbnIden.LeftWheelAbn);

    }
}

/*******************************************************************************
* Function Name  : EXTI9_5_IRQHandler
* Description    : 
                    line7   超声波接收
                    line8   左碰撞
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EXTI9_5_IRQHandler(void)
{
    if(SET == EXTI_GetITStatus(EXTI_Line8))
    {
        EXTI_ClearITPendingBit(EXTI_Line8);
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.LeftCollisionAbn=L_CONLLISION;
        DEBUG("EXTI_Line8 左碰撞 %d \r\n",g_tRobotState.Robotstate.unionAbnormal.AbnIden.LeftCollisionAbn);
        
    }

     /*Ultransonic*/  
    if (EXTI_GetITStatus(EXTI_ECHO_PORT) != RESET)  //中断发送
    {
        EXTI_ClearITPendingBit(EXTI_ECHO_PORT);   //清中断标志
        //计算低电平时间
        if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_7))
        {
            //计算时间间隔
            tULTime.us_UpTime=TIM7->CNT;
            if(tULTime.us_UpTime>tULTime.us_DownTime)
            {
               tULTime.us_interval=tULTime.us_UpTime-tULTime.us_DownTime;
               tULTime.bClashflag=true;
            }
            else
            {
                tULTime.bClashflag=false;
                return ;
            }

        }
        else    //低电平
        {
            tULTime.bClashflag=false;
            tULTime.us_DownTime=TIM7->CNT;             
        }
        
    }
}

void EXTI15_10_IRQHandler(void)
{
    if(SET == EXTI_GetITStatus(EXTI_Line13))
    {
        EXTI_ClearITPendingBit(EXTI_Line13);
        DEBUG("EXTI_Line13  电池错误\r\n");
    }
    if(SET == EXTI_GetITStatus(EXTI_Line10))
    {
        EXTI_ClearITPendingBit(EXTI_Line10);
        
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.RightCollisionAbn=R_CONLLISION;
        DEBUG("EXTI_Line10 右碰撞 %d \r\n",g_tRobotState.Robotstate.unionAbnormal.AbnIden.RightCollisionAbn);
        
    }
}

