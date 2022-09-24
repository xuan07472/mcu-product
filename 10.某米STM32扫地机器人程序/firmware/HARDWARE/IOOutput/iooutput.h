#ifndef __IOOUTPUT_H
#define __IOOUTPUT_H

#include "sys.h"
#include "delay.h"
#include "timer.h"
#include "FreeRTOS.h"
#include "task.h"

/*主板电源控制引脚*/
#define  POWER_DOWN   PEout(6)=1; //PE6拉高 系统掉电  
#define  POWER_UP     PEout(6)=0; //拉高IO保持系统上电

/*风机电源控制引脚*/
#define  FAN_POWER_DOWN   PEout(2)=0; //PE2拉低 风机掉电  
#define  FAN_POWER_UP     PEout(2)=1; //拉高IO  风机上电

/*4选1模拟通道控制引脚*/
#define  SGM4878_A      PDout(11) 
#define  SGM4878_B      PDout(9) 

/*掉落传感器控制引脚*/
#define  DROP_CTRL      PDout(15) 

/*驱动轮控制引脚 0 正转*/
#define  CTRL_WHEEL_L      PEout(7) 
#define  CTRL_WHEEL_R      PEout(4) 

/*边刷控制引脚 1 正转*/
#define  SIDE_BRUSH_CTRL     PCout(12) 

/*滚刷控制引脚 0 正转*/
#define  ROLL_BRUSH_CTRL     PAout(8) 

/*超声波*/
//发送引脚
#define TRIG_CLOCK RCC_APB2Periph_GPIOC
#define TRIG_PORT GPIOC
#define TRIG_PIN  GPIO_Pin_6
//接收引脚
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
