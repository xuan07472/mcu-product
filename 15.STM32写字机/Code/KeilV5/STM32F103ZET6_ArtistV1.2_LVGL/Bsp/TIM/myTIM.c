#include "../BSP/TIM/myTIM.h"
#include "../BSP/LED/LED.h"
//#include "../bsp/transfer_BIN_to_EX_FLASH/transfer_BIN_to_EX_FLASH.h"
#include "main.h"


//ͨ�ö�ʱ����ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
void TIM2_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM3��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM�ж�,��������ж�
 
	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;  //TIM�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;  //��ռ���ȼ�
	//NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
 
 
	TIM_Cmd(TIM2, ENABLE);  //ʹ��TIMx					 
}

#ifdef __cplusplus
extern "C"{
#endif


	
//��ʱ���жϷ������
void TIM2_IRQHandler(void)   //TIM�ж� 	TIM2_Int_Init(50 - 1, 7200 - 1); //5MS
{
	static u16 tim_cnt = 0;
	if (TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)  //���TIM�����жϷ������
		{
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
			
			
			tim_cnt++;
			if(tim_cnt >= 100)
			{
				LED_TOGGLE();
				tim_cnt = 0;
			}
			
			lv_tick_inc(5);
		}
}


#ifdef __cplusplus
}
#endif

//ͨ�ö�ʱ����ʼ��
//����ʱ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��
void TIM5_Int_Init(u16 arr,u16 psc)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM5��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler =psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE ); //ʹ��ָ����TIM�ж�,��������ж�
 
	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM5_IRQn;  //TIM�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 10;  //��ռ���ȼ�
	//NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  //�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
 
 
	TIM_Cmd(TIM5, ENABLE);  //ʹ��TIMx					 
}

#ifdef __cplusplus
extern "C"{
#endif


	
//��ʱ���жϷ������
void TIM5_IRQHandler(void)   //TIM�ж� 	TIM5_Int_Init(50 - 1, 7200 - 1); //5MS
{
	static u16 tim_cnt = 0;
	if (TIM_GetITStatus(TIM5, TIM_IT_Update) != RESET)  //���TIM�����жϷ������
		{
		
			
			
			tim_cnt++;
			if(tim_cnt >= 6)
			{
				TIM_Cmd(TIM5, DISABLE);  //ʧ��TIMx		
				lv_task_handler();	/* LVGL Task */
				tim_cnt = 0;
				TIM_Cmd(TIM5, ENABLE);  //ʹ��TIMx		
			}
			TIM_ClearITPendingBit(TIM5, TIM_IT_Update  );  //���TIMx�����жϱ�־ 
		}
}


#ifdef __cplusplus
}
#endif

void HW_TIM_Init(void)		  
{	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3|RCC_APB1Periph_TIM4, ENABLE); //ʱ��ʹ��
	
	//��ʱ��TIM3��ʼ��:Driver Interrupt	400 * per = 72000
	TIM_TimeBaseStructure.TIM_Prescaler=2;					 					
	TIM_TimeBaseStructure.TIM_Period=1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //ʹ��ָ����TIM3�ж�,��������ж�
	TIM_ClearITPendingBit(TIM3, TIM_IT_Update);  //���TIM3�����жϱ�־
	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;  //TIM3�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 4;  //��ռ���ȼ�1��
	//NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;  //�����ȼ�2��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���


	//��ʱ��TIM4��ʼ��:Port Reset Interrupt
	TIM_TimeBaseStructure.TIM_Prescaler=2;					 					
	TIM_TimeBaseStructure.TIM_Period=1;
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure); //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE); //ʹ��ָ����TIM3�ж�,��������ж�
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update);  //���TIM4�����жϱ�־ 
	//�ж����ȼ�NVIC����
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;  //TIM4�ж�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3;  //��ռ���ȼ�0��
	//NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  //�����ȼ�1��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; //IRQͨ����ʹ��
	NVIC_Init(&NVIC_InitStructure);  //��ʼ��NVIC�Ĵ���
}

void HW_Debounce_Init(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;	 //�����ʼ������
	NVIC_InitTypeDef NVIC_InitStructure;  

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1,ENABLE);

	TIM_TimeBaseStructure.TIM_Prescaler =123;    	     //Լ��ʱ30ms����ȥ�� 
	TIM_TimeBaseStructure.TIM_Period = 123; 				
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 	 
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseStructure);
	TIM_ITConfig(TIM1, TIM_IT_Update, ENABLE); 	//TIM1����ж�����

	NVIC_InitStructure.NVIC_IRQChannel=TIM1_UP_IRQn;		//����TIM1������
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;		//��ռ���ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;			//�����ȼ�0
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;		  		//NVICʹ��
	NVIC_Init(&NVIC_InitStructure);						  		//�����жϷ����������ȼ�
}

//##############################��Driver Interrupt��##############################
void HW_TIM_DriverInterrupt_Enable(void)
{
	TIM_Cmd(TIM3,ENABLE);	
}

void HW_TIM_DriverInterrupt_Disable(void)
{
	TIM_Cmd(TIM3,DISABLE);	
}

void HW_TIM_DriverInterrupt_ValueConfig(u32 Prescaler,u32 Autoreload)
{
	TIM_PrescalerConfig(TIM3,(uint16_t)Prescaler,TIM_PSCReloadMode_Immediate);
	TIM_SetAutoreload(TIM3,(uint16_t)Autoreload);			
}


//##############################��Port Reset Interrupt��##############################
void HW_TIM_PortResetInterrupt_Enable(void)
{
	TIM_Cmd(TIM4,ENABLE);
}

void HW_TIM_PortResetInterrupt_Disable(void)
{
	TIM_Cmd(TIM4,DISABLE);
}

void HW_TIM_PortResetInterrupt_ValueConfig(u32 Prescaler,u32 Autoreload)
{
	TIM_PrescalerConfig(TIM4,(uint16_t)Prescaler,TIM_PSCReloadMode_Immediate);
	//TIM_PrescalerConfig(TIM4,(uint16_t)Prescaler,Autoreload);
	TIM_SetAutoreload(TIM4,(uint16_t)Autoreload);
}

//##############################��Debounce��##############################

void HW_Debounce_Enable(void)
{
	TIM_Cmd(TIM1, ENABLE); 
}	 				

void HW_Debounce_Disable(void)
{
	TIM_Cmd(TIM1, DISABLE); 
}	 




