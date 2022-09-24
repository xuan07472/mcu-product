#ifndef __TIMER_H
#define __TIMER_H
#include "sys.h"
#include "exti.h"

#define GyroUpdata_IRQHander TIM6_IRQHandler
#define TIM7_CNT_SET( Counter )  TIM_SetCounter( TIM7 , Counter)

#define TIM7_PEROID 65535
void vTIM6_Int_Init(void);
void vTIM7_Configuration(void);
#endif
