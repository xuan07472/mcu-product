#ifndef __FRAMEHANDLE_H
#define __FRAMEHANDLE_H


#include "behaviormanage.h"
#include "SystemTime.h"
#include "task.h"
#include "protocol.h"
#include "usart1.h"	
#include "stmflash.h"
#include "behaviormanage.h"

#define  PRODUCT_APP_PASSWORD (0xFee1Dead)  //产品应用秘钥
#define  PASSWORD_ADDR        (0x08005000)  //存放地址20kb


/*行为控制类型枚举*/
typedef enum
{
    bccStop        =0xD1,
    bccForward     =0xD2,
    bccRetreat     =0xD3,
    bccTurnRotate  =0xD4,
    bccTurnLeft    =0xD5,
    bccTurnRight   =0xD6,
    
}BehaviorControlCommand_e;

/*行为控制结构体*/
__packed typedef struct
{
    BehaviorControlCommand_e Com;//控制类型
    u8   RunSpeed;               //速度
    s16  TargetAngle;            //目标角度
    
}BehaviorControl_t;

/*坐标点结构体*/
__packed typedef struct
{ 
    int cooX;//类型暂时定为int 如果没有负数 则改为u32 单位mm
    int cooY;
}Coord_t;

/*线路坐标结构体*/
__packed typedef struct
{
    Coord_t  CurrentCoo;//当前机器人坐标
    Coord_t  Coo1;//将要前进到的第一个坐标
    Coord_t  Coo2;//将要前进到的第二个坐标
    Coord_t  Coo3;//将要前进到的第三个坐标
    Coord_t  Coo4;//将要前进到的第四个坐标7
    
}RouteCoord_t;


extern RouteCoord_t  g_tRouteCoord;

void vFrameDataHandlerTask(void *p);

void vFrameAutoResendByUsart1(Rx_FrameRecord *R);

void  vCommandFrameHandle(Rx_FrameRecord *R);
void  vDataFrameHandle(Rx_FrameRecord *R);
    
void vBehaviorControlHandle(Rx_FrameRecord *R);
void vRequestStateCommandHandle(Tx_FrameRecord *T);
void vRouteCoordHandle(Rx_FrameRecord *R);

#endif

