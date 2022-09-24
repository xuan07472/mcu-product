#ifndef __TIMEPWM_H
#define __TIMEPWM_H

#include "sys.h"

/*TIM5 Ƶ��Ϊ16KHz*/
#define TIM5_ARR (500)
#define TIM5_PSC (9)

/*TIM4 Ƶ��Ϊ1.2KHz*/
#define TIM4_ARR (1000)
#define TIM4_PSC (60)
#define TIM4_CH4_CCR 500 

/*TIM3 Ƶ��Ϊ12KHz*/
#define TIM3_ARR (500)
#define TIM3_PSC (12)

/*��� Cycle ��Χ0-499 Խ��ת��Խ��*/
#define FAN_T5CH2_PWM_CYCLE( Cycle )          TIM_SetCompare2( TIM5, Cycle );

/*�������� Cycle ��Χ0-499*/
#define RIGHT_WHEEL_T3CH3_PWM_CYCLE( Cycle )  TIM_SetCompare3( TIM3, Cycle );

/*�������� Cycle ��Χ0-499*/
#define LEFT_WHEEL_T3CH4_PWM_CYCLE( Cycle )   TIM_SetCompare4( TIM3, Cycle );

/*��ˢ Cycle ��Χ0-499*/
#define ROLL_BRUSH_T4CH2_PWM_CYCLE( Cycle )   TIM_SetCompare2( TIM4, Cycle );

/*��ˢCycle ��Χ0-499*/
#define SIDE_BRUSH_T4CH3_PWM_CYCLE( Cycle )   TIM_SetCompare3( TIM4, Cycle );

/*������DROP ��Χ0-1000*/
#define DROP_T4CH4_PWM_CYCLE( Cycle )   TIM_SetCompare4( TIM4, Cycle-1 );

/*�ı䶨ʱ��4��Ƶ��*/
#define TIM4_PWM_FREQUENCY( Frequency )  TIM_PrescalerConfig(TIM4 , (72000/Frequency)-1 , TIM_PSCReloadMode_Immediate )

void vAll_PWM_Init(void );

void TIM5_PWM_Init(void);
void TIM3_PWM_Init(void);
void TIM4_PWM_Init(void);


void TIM3_CH34_Init(u16 arr,u16 psc);
void TIM4_CH234_Init(u16 arr,u16 psc);
void TIM5_CH2_FanInit(u16 arr,u16 psc);

#endif
