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

/*����ö��*/
typedef enum
{
    LWheel=0,
    RWheel=1,
}WheelNum_e;    

/*����PID��������������� ��¼*/
typedef struct 
{
    u8 PreData;//���ϴ��������
    u8 LastData;//�ϴ��������
}IncPID_t;

#define OUTMAXV        400              //�ٶȻ�������ֵ
#define OUTMINV       -400              //�ٶȻ������Сֵ
#define UIMAXV         400              //�ٶȻ��������ֵ
#define UIMINV        -400              //�ٶȻ�������Сֵ


#define   WHEEL_PERIMETER           (220)   //�����ܳ�
#define   WHEEL_ENCODE_COUNT        (263)   //����תһȦ ��������������
#define   SINGLE_ENCODE_DISTANCE    (83650) //һ������������ �����н��ľ���*100000. TIM1�ɼ�Ƶ��Ϊ100KHz ���Գ�����100000.

#define   FAN_ENCODE_COUNT          (6)     //���תһȦ ��������������

#define   SPEED_UPDATE_CYCLE        (10)    //�ٶȸ�������  ����λms��



extern s16 g_sExpect_Angle;  //�����ĽǶ�
extern s16 g_sExpect_speed;//�ٶ� mm/s
extern s16 g_sExpect_range; //�Ƕ���Χ +- 1�� ��λ0.1��

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

