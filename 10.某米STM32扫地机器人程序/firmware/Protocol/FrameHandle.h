#ifndef __FRAMEHANDLE_H
#define __FRAMEHANDLE_H


#include "behaviormanage.h"
#include "SystemTime.h"
#include "task.h"
#include "protocol.h"
#include "usart1.h"	
#include "stmflash.h"
#include "behaviormanage.h"

#define  PRODUCT_APP_PASSWORD (0xFee1Dead)  //��ƷӦ����Կ
#define  PASSWORD_ADDR        (0x08005000)  //��ŵ�ַ20kb


/*��Ϊ��������ö��*/
typedef enum
{
    bccStop        =0xD1,
    bccForward     =0xD2,
    bccRetreat     =0xD3,
    bccTurnRotate  =0xD4,
    bccTurnLeft    =0xD5,
    bccTurnRight   =0xD6,
    
}BehaviorControlCommand_e;

/*��Ϊ���ƽṹ��*/
__packed typedef struct
{
    BehaviorControlCommand_e Com;//��������
    u8   RunSpeed;               //�ٶ�
    s16  TargetAngle;            //Ŀ��Ƕ�
    
}BehaviorControl_t;

/*�����ṹ��*/
__packed typedef struct
{ 
    int cooX;//������ʱ��Ϊint ���û�и��� ���Ϊu32 ��λmm
    int cooY;
}Coord_t;

/*��·����ṹ��*/
__packed typedef struct
{
    Coord_t  CurrentCoo;//��ǰ����������
    Coord_t  Coo1;//��Ҫǰ�����ĵ�һ������
    Coord_t  Coo2;//��Ҫǰ�����ĵڶ�������
    Coord_t  Coo3;//��Ҫǰ�����ĵ���������
    Coord_t  Coo4;//��Ҫǰ�����ĵ��ĸ�����7
    
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

