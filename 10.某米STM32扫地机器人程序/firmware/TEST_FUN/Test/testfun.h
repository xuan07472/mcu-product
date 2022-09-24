#ifndef __TESTFUN_H
#define __TESTFUN_H

#include "sys.h"
#include "hardwareManage.h"
#include "applicationadc.h"
#include "applicationPowerChip.h"
#include "applicationDrop.h"
#include "applicationmotor.h"
#include "bmi160_data.h"
#include "exti.h"
#include "BMI160.h"
#include "DataScope_DP.h"

void vPrintf_ALL_RobotSta(void );
void vFAN_Test(u8 CtlPinSta,u16 Cycle0_499);
void vAll_AD_VoluePrintf(void);
void vPrintf_ALL_Register_Bp24773(void);
void vWrite_Bq24773_Register(u8 addr,u8 data);    
void vWrite_Wheel(u8 sta,u16 loccycle);

void DROP_AD_Printf(void);
void vSetDropTime(u16 frequency , u16 cycle );
void vDropWaveformOut(bool enable);
void vPrintf_ALL_wheel_Data(void );
void vChange_PID(s16 P,s16 I,s16 D);
void vGyroscope_VoluePrintf(void);
void vChange_EX_Angle(s16 sExp_Ang,s16 sExp_range,s16 sExp_speed);
void vAll_UltraSonic_VoluePrintf(void);
#endif

