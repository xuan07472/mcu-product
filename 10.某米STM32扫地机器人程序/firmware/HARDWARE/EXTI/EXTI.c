#include "exti.h"

/******************************************************************************
* �ļ���  ��exti.c
* �ļ�������xiaomi�����ⲿ�ж���������
* ������  ��Pu Sheng
* ��    �ڣ�2019��11��14��
* 
* Ӳ�����ӣ�
           
            PD1    EXTI1  -  �������մ�����        ���պ��Ǹߵ�ƽ
            PD2    EXTI2  -  �������մ�����
            PE3    EXTI3  -  ��ԴоƬACOK�¼����� �г����ʱ ��Ϊ�ߵ�ƽ ��ʱ���
            PC7    EXTI7  -  ���������봫����      ����Ϊ�͵�ƽʱ��
            PD8    EXTI8  -  �����ײ������        ��ײ���Ǹߵ�ƽ
            PD10   EXTI10 -  �ұ���ײ������        ��ײ���Ǹߵ�ƽ
            PE13   EXTI13 -  ��ԴоƬ�¼�����
            
******************************************************************************/
	   
__IO TIM7Time_t tULTime={0};  //������ʱ��

/*******************************************************************************
* Function Name  : vAll_EXTI_PinInit
* Description    : ���������ⲿ�ж� IO  ������ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vAll_EXTI_PinInit(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;
    NVIC_InitTypeDef    NVIC_InitStructure;
    EXTI_InitTypeDef    EXTI_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);	//ʹ�ܸ��ù���ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE|RCC_APB2Periph_GPIOD, ENABLE);	//ʹ��PE�˿�ʱ��

    
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
  	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01;	//��ռ���ȼ�2�� 
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
        DEBUG("EXTI_Line3 ������뿪\r\n");
    }
}
void EXTI1_IRQHandler(void)
{
    if(SET == EXTI_GetITStatus(EXTI_Line1))
    {
        EXTI_ClearITPendingBit(EXTI_Line1);
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.RightWheelAbn=WHEEL_R_VACANT;
        DEBUG("EXTI_Line1 �������� %d \r\n",g_tRobotState.Robotstate.unionAbnormal.AbnIden.RightWheelAbn);
    }
}
void EXTI2_IRQHandler(void)
{
    if(SET == EXTI_GetITStatus(EXTI_Line2))
    {
        EXTI_ClearITPendingBit(EXTI_Line2);
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.LeftWheelAbn=WHEEL_L_VACANT;//��ֵ״̬
        DEBUG("EXTI_Line2 �������� %d \r\n",g_tRobotState.Robotstate.unionAbnormal.AbnIden.LeftWheelAbn);

    }
}

/*******************************************************************************
* Function Name  : EXTI9_5_IRQHandler
* Description    : 
                    line7   ����������
                    line8   ����ײ
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
        DEBUG("EXTI_Line8 ����ײ %d \r\n",g_tRobotState.Robotstate.unionAbnormal.AbnIden.LeftCollisionAbn);
        
    }

     /*Ultransonic*/  
    if (EXTI_GetITStatus(EXTI_ECHO_PORT) != RESET)  //�жϷ���
    {
        EXTI_ClearITPendingBit(EXTI_ECHO_PORT);   //���жϱ�־
        //����͵�ƽʱ��
        if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_7))
        {
            //����ʱ����
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
        else    //�͵�ƽ
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
        DEBUG("EXTI_Line13  ��ش���\r\n");
    }
    if(SET == EXTI_GetITStatus(EXTI_Line10))
    {
        EXTI_ClearITPendingBit(EXTI_Line10);
        
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.RightCollisionAbn=R_CONLLISION;
        DEBUG("EXTI_Line10 ����ײ %d \r\n",g_tRobotState.Robotstate.unionAbnormal.AbnIden.RightCollisionAbn);
        
    }
}

