#include "TimeCapture.h"

/******************************************************************************
* �ļ���  ��TimeCapture.c
* �ļ�������xiaomi����PWM����IO����
* ������  ��Pu Sheng
* ��    �ڣ�2019��11��8��
* 
* Ӳ�����ӣ�
    PC9  - TIM8_CH4 ���������
    PE9  - TIM1_CH1 ���ֱ�����
    PE11 - TIM1_CH2 ���ֱ�����
******************************************************************************/

/*���������� һ����������1*/
u32 g_FanEncodeCount;

WheelSruct_t g_tLeftWheel = {0};  //���ӽṹ
WheelSruct_t g_tRightWheel = {0};  //���ӽṹ

/*
* Function Name  : vAll_TIM_CaptureInit
* Description    : �������ж�ʱ������������ʼ��
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
* Description    : ��������TIM8���� ������ʼ��
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
* Description    : ��������TIM1���� ������ʼ��
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
* Description    : ����TIM8 CH4������벶��������ʼ��
* Input          : arr ��װ��ֵ psc ��Ƶϵ��
* Output         : None
* Return         : None
*******************************************************************************/
void TIM8_CH4_FanCaptureInit(u16 arr,u16 psc)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    TIM_ICInitTypeDef  TIM_ICInitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//ʹ��PC�˿�ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);	//ʹ�ܶ�ʱ��8ʱ��
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 		 //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO���ٶ�Ϊ50MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    TIM_TimeBaseStructure.TIM_Period = arr-1; 					//��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc-1; 					//����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM���ϼ���ģʽ
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
* Description    : ����TIM1 CH1 CH2�����ֱ��벶��������ʼ��
* Input          : arr ��װ��ֵ psc ��Ƶϵ��
* Output         : None
* Return         : None
*******************************************************************************/
void TIM1_CH12WheelCaptureInit(u16 arr,u16 psc)
{
    GPIO_InitTypeDef         GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef         NVIC_InitStructure;
    TIM_ICInitTypeDef        TIM_ICInitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE, ENABLE);	//ʹ��PE�˿�ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);	//ʹ�ܶ�ʱ��1ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);	//ʹ�ܶ�ʱ��1ʱ��
  
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM1, ENABLE);      //Timer1��ȫ��ӳ��   
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9|GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; 		 //�������
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    
    
    TIM_TimeBaseStructure.TIM_Period = arr-1; 					//��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc-1; 					//����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 	
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM���ϼ���ģʽ
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
        TIM_ClearITPendingBit( TIM1,TIM_IT_CC1 ); //����жϱ�־λ    
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
        TIM_ClearITPendingBit( TIM1,TIM_IT_CC2 ); //����жϱ�־λ
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
        TIM_ClearITPendingBit( TIM8,TIM_IT_CC4 ); //����жϱ�־λ         

        g_FanEncodeCount++;
    }
}







