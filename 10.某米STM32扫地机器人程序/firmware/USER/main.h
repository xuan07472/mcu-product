#ifndef __MAIN_H_
#define __MAIN_H_

#include "sys.h"

#define  MYDEBUG 

#ifdef   MYDEBUG     //是否进入串口调试 进入会有串口1信息输出 并可以用usmart调试
#define  DEBUG       printf
#else
#define  DEBUG(...)  (void)(0)
#endif

/* 按键命令 */
typedef enum {
    keyStop      =0,//停止
    keyStart     =1,//启动 
    keyBackCharg =2,//回充
    
}KeyCommand_t;

/* 机器人运动状态 */
typedef enum {
    zStop      =0,           //停止
    zForward   =1,           //前进
    zRetreatL  =2,           //后退后左转
    zRetreatR  =3,           //后退后右转
    zTurnLeft  =4,           //左转
    zTurnRight =5,           //右转
    zRotate    =6,           //原地旋转
    zAuto      =7,           //自动
    zChargeBack=8,           //回充
    zCharging  =9,           //正在充电
    zAlongEdge =10,          //沿边模式
    zRetreat   =11,          //后退
}RobotMotionState_t;

__packed typedef struct{
    
    u8     ucRemainBattery;                //电量 0~100
    RobotMotionState_t  eDirection;        //方向
    RobotMotionState_t  PreviousDirection; //上次方向
    /*系统错误*/
    __packed union 
    {
        u32 ErrorSta;
        __packed struct 
        {
            u32 BatteryErr    : 1;//电池错误
            u32 LeftWheelErr  : 1;//左轮错误
            u32 RightWheelErr : 1;//右轮错误
            u32 DropLFErr  : 1;//左前悬空传感器错误
            u32 DropLBErr  : 1;//左前悬空传感器错误
            u32 DropRFErr  : 1;//左前悬空传感器错误
            u32 DropRBErr  : 1;//左前悬空传感器错误
            u32 UltraSonicErr  : 1;//超声波错误
            u32 CrashErr: 1;//碰撞传感器卡住等错误
            u32 RollBrushErr: 1;//滚刷（缠绕和模组异常）
            u32 SideBrushErr: 1;//边刷（缠绕和模组异常）
            u32 StrainerExistence: 1;//滤网在不在
            u32 StrainerDirty: 1;//滤网脏不脏
            u32 EdgeErr: 1;//沿边传感器
            u32 RobotTilt: 1;//启动时机器人倾斜
            u32 FanErr: 1;//风机损坏

        }ErrIden;
    }unionEEROR;
    
    /*系统异常*/
    __packed union 
    {
        u32 AbnormalSta;
        __packed struct 
        {
            u32 BatteryAbn    : 1;//电池低电量异常
            u32 LeftWheelAbn  : 1;//左轮悬空异常
            u32 RightWheelAbn : 1;//右轮悬空异常
            u32 DropLFAbn  : 1;//左前悬空传感器悬空异常
            u32 DropLBAbn  : 1;//左前悬空传感器悬空异常
            u32 DropRFAbn  : 1;//左前悬空传感器悬空异常
            u32 DropRBAbn  : 1;//左前悬空传感器悬空异常
            u32 UltraSonicAbn     : 1;//超声波检测到障碍物异常
            u32 LeftCollisionAbn  : 1;//左碰撞
            u32 RightCollisionAbn : 1;//右碰撞
            u32 DustBoxAbn        : 1;//尘盒不在 
            u32 InRechargeStand   : 1;//是否在充电座
        }AbnIden;
    }unionAbnormal;

}RobotState_t;

/***************通信协议用户数据结构**************/                  
__packed typedef struct{    
    uint8_t     ucUserNumber;        //客户编号
    uint8_t     ucFirmVersion;       //固件版本号
    RobotState_t  Robotstate;        //机器人状态
    KeyCommand_t  KeyCommand;        //按键命令
}UserData;  


extern volatile UserData g_tRobotState;



#endif

