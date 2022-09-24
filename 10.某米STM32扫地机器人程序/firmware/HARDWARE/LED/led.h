#ifndef __LED_H
#define __LED_H	 

#include "sys.h"

/*TIM2 Ƶ��Ϊ10KHz*/
#define TIM2_ARR  (100)
#define TIM2_PSC  (72)
	
/*Cycle ��Χ0-100 Խ��ledԽ��*/
#define REDLED_PWM_CYCLE( Cycle )         TIM_SetCompare2( TIM2, Cycle )
#define WHITELED_PWM_CYCLE( Cycle )       TIM_SetCompare3( TIM2, Cycle )
#define YELLOWLED_PWM_CYCLE( Cycle )      TIM_SetCompare4( TIM2, Cycle )

void vLEDPWM_Init(void);//��ʼ��

		 				    
#endif
