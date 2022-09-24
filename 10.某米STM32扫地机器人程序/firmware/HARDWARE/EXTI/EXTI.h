#ifndef __EXTI_H
#define __EXTI_H

#include "sys.h"
#include "main.h"
#include "freertos.h"
#include "task.h"
#include "TimeCapture.h"
#include "Application_Ultrasonic.h"

#define  WHEEL_R_VACANT  PDin(1)
#define  WHEEL_L_VACANT  PDin(2)
#define  L_CONLLISION    PDin(8)
#define  R_CONLLISION    PDin(10)

//�ⲿ�ж�����
#define EXTI_ECHO_PORT EXTI_Line7
#define EXTI_GPIO_PinSource GPIO_PinSource7

void vAll_EXTI_PinInit(void);

//�������ṹ��
typedef struct 
{
    uint16_t us_UpTime;
    uint16_t us_DownTime;
    uint16_t us_interval;   //��������������ռ�� lhc
    bool 	bClashflag;
}TIM7Time_t;



#endif
