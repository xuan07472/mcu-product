#ifndef __LED_H
#define __LED_H	 

#include "sys.h"

/*TIM2 频率为10KHz*/
#define TIM2_ARR  (100)
#define TIM2_PSC  (72)
	
/*Cycle 范围0-100 越大led越亮*/
#define REDLED_PWM_CYCLE( Cycle )         TIM_SetCompare2( TIM2, Cycle )
#define WHITELED_PWM_CYCLE( Cycle )       TIM_SetCompare3( TIM2, Cycle )
#define YELLOWLED_PWM_CYCLE( Cycle )      TIM_SetCompare4( TIM2, Cycle )

void vLEDPWM_Init(void);//初始化

		 				    
#endif
