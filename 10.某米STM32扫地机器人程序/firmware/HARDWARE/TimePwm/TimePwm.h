#ifndef __TIMEPWM_H
#define __TIMEPWM_H

#include "sys.h"

/*TIM5 频率为16KHz*/
#define TIM5_ARR (500)
#define TIM5_PSC (9)

/*TIM4 频率为1.2KHz*/
#define TIM4_ARR (1000)
#define TIM4_PSC (60)
#define TIM4_CH4_CCR 500 

/*TIM3 频率为12KHz*/
#define TIM3_ARR (500)
#define TIM3_PSC (12)

/*风机 Cycle 范围0-499 越大转速越快*/
#define FAN_T5CH2_PWM_CYCLE( Cycle )          TIM_SetCompare2( TIM5, Cycle );

/*右驱动轮 Cycle 范围0-499*/
#define RIGHT_WHEEL_T3CH3_PWM_CYCLE( Cycle )  TIM_SetCompare3( TIM3, Cycle );

/*左驱动轮 Cycle 范围0-499*/
#define LEFT_WHEEL_T3CH4_PWM_CYCLE( Cycle )   TIM_SetCompare4( TIM3, Cycle );

/*滚刷 Cycle 范围0-499*/
#define ROLL_BRUSH_T4CH2_PWM_CYCLE( Cycle )   TIM_SetCompare2( TIM4, Cycle );

/*边刷Cycle 范围0-499*/
#define SIDE_BRUSH_T4CH3_PWM_CYCLE( Cycle )   TIM_SetCompare3( TIM4, Cycle );

/*防跌落DROP 范围0-1000*/
#define DROP_T4CH4_PWM_CYCLE( Cycle )   TIM_SetCompare4( TIM4, Cycle-1 );

/*改变定时器4的频率*/
#define TIM4_PWM_FREQUENCY( Frequency )  TIM_PrescalerConfig(TIM4 , (72000/Frequency)-1 , TIM_PSCReloadMode_Immediate )

void vAll_PWM_Init(void );

void TIM5_PWM_Init(void);
void TIM3_PWM_Init(void);
void TIM4_PWM_Init(void);


void TIM3_CH34_Init(u16 arr,u16 psc);
void TIM4_CH234_Init(u16 arr,u16 psc);
void TIM5_CH2_FanInit(u16 arr,u16 psc);

#endif
