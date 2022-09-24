#ifndef __APP_MOTOR_H
#define __APP_MOTOR_H

#include "SystemTime.h"
#include "freertos.h"
#include "task.h"
#include "timepwm.h"
#include "iooutput.h"
#include "stdlib.h"
#include "TimeCapture.h"
#include "DataScope_DP.h"

/*轮子枚举*/
typedef enum
{
    LWheel=0,
    RWheel=1,
}WheelNum_e;    

/*增量PID的上两次误差数据 记录*/
typedef struct 
{
    u8 PreData;//上上次误差数据
    u8 LastData;//上次误差数据
}IncPID_t;

#define OUTMAXV        400              //速度环输出最大值
#define OUTMINV       -400              //速度环输出最小值
#define UIMAXV         400              //速度环积分最大值
#define UIMINV        -400              //速度环积分最小值


#define   WHEEL_PERIMETER           (220)   //轮子周长
#define   WHEEL_ENCODE_COUNT        (263)   //轮子转一圈 编码器计数个数
#define   SINGLE_ENCODE_DISTANCE    (83650) //一个编码器脉冲 轮子行进的距离*100000. TIM1采集频率为100KHz 所以乘以了100000.

#define   FAN_ENCODE_COUNT          (6)     //风机转一圈 编码器计数个数

#define   SPEED_UPDATE_CYCLE        (10)    //速度更新周期  （单位ms）



extern s16 g_sExpect_Angle;  //期望的角度
extern s16 g_sExpect_speed;//速度 mm/s
extern s16 g_sExpect_range; //角度误差范围 +- 1度 单位0.1度

extern IncPID_t g_tIncPID;

void vAllMotorMotionTask(void *p);
void vMotorSpeedUpdateTask(void *p);
void vSoftSpeedAdvanceTask(void *p);
void vSoftSpeedAdjust(void);

s16 iIncPIDCalc(s16 NewData);

u32 uiGetFanRotationRate(void);
void vSetFanSpeed(u16 PWMPulse);
void vSetSideBrushSpeed(u8 Dir,u16 PWMPulse);
void vSetRollBrushSpeed(u8 Dir,u16 PWMPulse);

int16_t sSpeed_R_PID(int16_t RefV,int16_t FdbV);
int16_t sSpeed_L_PID(int16_t RefV,int16_t FdbV);


u8 ucRoute_Adjust(s16 loc_FbAngle,s16 loc_ExpAngle,s16 loc_Speed);
u8 ucTurn_Angle_Round(s16 loc_Current_Angle,s16 loc_Expect_Angle,u16 loc_Speed,u16 loc_AngRange);
u8 ucTurn_Angle_Round_NoCheak(s16 loc_Diff_Angle,u16 loc_Speed,u16 loc_AngRange);
u8 ucRotateAroundAxle(s16 loc_CurrAngle,s16 loc_ExpAngle,u16 loc_Speed,u16 loc_AngRange,WheelNum_e Wnum);

u8 ucRouteAdjustByAlongEdge(u16 loc_FbValue,u16 loc_ExpValue,s16 loc_Speed);
u8 ucRouteAdjustByAlongEdgeDistance(u16 loc_FbValue,u16 loc_ExpValue,s16 loc_Speed);
#endif

