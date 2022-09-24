#ifndef __MYTIM_H_
#define __MYTIM_H_

#include <stm32f10x.h>



void TIM2_Int_Init(u16 arr,u16 psc);
void TIM5_Int_Init(u16 arr,u16 psc);


//##############################��API��##############################
void HW_TIM_Init(void);				//��ʱ����ʼ����Ҫ������stepper.c�е�st_init()
//Driver Interrupt
void HW_TIM_DriverInterrupt_Enable(void); 		//������ʱ��
void HW_TIM_DriverInterrupt_Disable(void);		//�رն�ʱ��
void HW_TIM_DriverInterrupt_ValueConfig(u32 Prescaler,u32 Autoreload);		//���ö�ʱ����װֵ
//Port Reset Interrupt
void HW_TIM_PortResetInterrupt_Enable(void);  	//������ʱ��
void HW_TIM_PortResetInterrupt_Disable(void);	//�رն�ʱ��
void HW_TIM_PortResetInterrupt_ValueConfig(u32 Prescaler,u32 Autoreload); 	//���ö�ʱ����װֵ
//Debounce
void HW_Debounce_Init(void);		//ȥ����ʱ��
void HW_Debounce_Enable(void);		//ʹ��ȥ����ʱ��
void HW_Debounce_Disable(void);		//����ȥ����ʱ��



#endif


