#include "timer.h"


/******************************************************************************
* �ļ���  timer.c
* �ļ���������ʱ��ʱ��
* ������  ��
* ��    �ڣ�2019��11��8��
* 
* Ӳ�����ӣ�
        TIM6    -   ������     �����ж�Ϊ5ms
        TIM7    -   ������     ����Ϊ1us
    
******************************************************************************/

/****************************************************************************************
 *������: TIM6_Int_Init
 *����:   TIM6��ʼ������,����Ϊ5ms���������ж�
 *����:   ��
 *�����  ��
 *����ֵ����
****************************************************************************************/
void vTIM6_Int_Init(void)
{
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM6��ʼ��
	TIM_TimeBaseStructure.TIM_Period = 4999; 					//��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ 1M/(4999+1) = 200HZ	
	TIM_TimeBaseStructure.TIM_Prescaler =71; 					//����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 72M/(71+1) = 1M
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 	//����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM6, &TIM_TimeBaseStructure); 			//����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM6,TIM_IT_Update,ENABLE ); 					//ʹ��ָ����TIM6�ж�,��������ж�

	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;  			//TIM6�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;  	//��ռ���ȼ�5��
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  		//�����ȼ�0��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 			//IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  							//��ʼ��NVIC�Ĵ���

	TIM_Cmd(TIM6, ENABLE);  									//ʹ��TIM6					 
}

/****************************************************************************************
 *������: GyroUpdata_IRQHander
 *����:   TIM6�жϷ��������ۼ������Ǹ��´���
 *����:   ��
 *�����  ��
 *����ֵ����
****************************************************************************************/
u8 GyroUpdateCNT = 0;//�����Ǹ��¼���
void GyroUpdata_IRQHander(void)
{
	if(TIM_GetITStatus(TIM6,TIM_IT_Update)==SET) //����ж�
	{
        GyroUpdateCNT++;//ÿ5ms�ۼ�
	}
    TIM_ClearITPendingBit(TIM6,TIM_IT_Update);  //����жϱ�־λ
}


/*******************************************************************************
* Function Name  : vTIM7_Configuration
* Description    : ��������ʱ�� ������ʹ��ʱ�����������
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
    TIM_ClearFlag(TIM7, TIM_FLAG_Update);	 //��������жϣ����һ���ж����������ж�  

    TIM_Cmd(TIM7, ENABLE);													
}

