#ifndef __IOOUTPUT_H
#define __IOOUTPUT_H

#include "sys.h"
#include "delay.h"
#include "timer.h"
#include "FreeRTOS.h"
#include "task.h"

/*�����Դ��������*/
#define  POWER_DOWN   PEout(6)=1; //PE6���� ϵͳ����  
#define  POWER_UP     PEout(6)=0; //����IO����ϵͳ�ϵ�

/*�����Դ��������*/
#define  FAN_POWER_DOWN   PEout(2)=0; //PE2���� �������  
#define  FAN_POWER_UP     PEout(2)=1; //����IO  ����ϵ�

/*4ѡ1ģ��ͨ����������*/
#define  SGM4878_A      PDout(11) 
#define  SGM4878_B      PDout(9) 

/*���䴫������������*/
#define  DROP_CTRL      PDout(15) 

/*�����ֿ������� 0 ��ת*/
#define  CTRL_WHEEL_L      PEout(7) 
#define  CTRL_WHEEL_R      PEout(4) 

/*��ˢ�������� 1 ��ת*/
#define  SIDE_BRUSH_CTRL     PCout(12) 

/*��ˢ�������� 0 ��ת*/
#define  ROLL_BRUSH_CTRL     PAout(8) 

/*������*/
//��������
#define TRIG_CLOCK RCC_APB2Periph_GPIOC
#define TRIG_PORT GPIOC
#define TRIG_PIN  GPIO_Pin_6
//��������
#define ECHO_CLOCK RCC_APB2Periph_GPIOC
#define ECHO_PORT GPIOC
#define ECHO_PIN  GPIO_Pin_7



void vAllOutputPinInit(void);

void vBoradPowerCtlPinInit(void);
void vFanPowerCtlPinInit(void);
void vVAPowerCtlPinInit(void);
void vVccPowerCtlPinInit(void);
void vVPowerCtlPinInit(void);
void vSGM4878CtlPinInit(void);
void vWheelCtlPinInit(void);
void vRollBrushCtlPinInit(void);
void vSideBrushCtlPinInit(void);
void vUltrasonic_Init(void);
void vUltrasonic_Emit(void);
void vUltrasonic_GPIO_config(void);
void vECHO_EXTI_Enable(EXTITrigger_TypeDef mode);
void vECHO_EXTI_Disable(void);
void EXTI9_5_IRQHandler(void);

#endif
