#ifndef __APPLICATIONDROPCHECK_H
#define __APPLICATIONDROPCHECK_H

#include "sys.h"
#include "adc.h"
#include "TimePwm.h"
#include "applicationadc.h"
#include "mymath.h"
#include "stdlib.h "
#include "DataScope_DP.h"
#include "string.h"

#define DROPEXTERNALTEST 0  //宏定义 置1时为外部测试，由外部写值
//#define DROPHONGCHUAN 

#define  LF_DropADValue     g_usInChipAdcChannelAverageValue[adcDropFrontL]
#define  RF_DropADValue     g_usInChipAdcChannelAverageValue[adcDropFrontR]
#define  LB_DropADValue     g_usInChipAdcChannelAverageValue[adcDropBackL]
#define  RB_DropADValue     g_usInChipAdcChannelAverageValue[adcDropBackR]


#define g_FALL_DIFFERENT_THRESHOLD (100) //前后差值的跌落阈值
#define g_DROP_THRESHOLD 950  //离地距离 越大离地越高 (设置悬崖值为1000)

typedef struct DropCheck
{
    bool bAnyoneFall;       //检测到悬空
    bool bFrontLFall;       //左前
    bool bFrontRFall;       //右前
    bool bBackLFall;        //左后
    bool bBackRFall;        //右后
    
}DropCheckData_t;

//跌落ad值数组使用
enum eDropAD
{
    adBL    =0,
    adBR    =1,
    adFL    =2,
    adFR    =3,
};

extern DropCheckData_t tDropcheck;

extern uint16_t g_usDROP_AD[4];

//接口
//获取ad值
#define usGetDropRightFrontAD()       g_usDROP_AD[adFR]
#define usGetDropRightBackAD()        g_usDROP_AD[adBR]
#define usGetDropLeftFrontAD()        g_usDROP_AD[adFL]
#define usGetDropLeftBackAD()         g_usDROP_AD[adBL]
//设置ad值
#define usSetDropRightFrontAD(  value )       g_usDROP_AD[adFR]= value ;
#define usSetDropRightBackAD(  value )        g_usDROP_AD[adBR]= value ;
#define usSetDropLeftFrontAD(  value )        g_usDROP_AD[adFL]= value ;
#define usSetDropLeftBackAD(  value )         g_usDROP_AD[adBL]= value ;
//悬空标志获取
#define IsDropFrontL()    tDropcheck.bFrontLFall;
#define IsDropFrontR()    tDropcheck.bFrontRFall;
#define IsDropBackL()     tDropcheck.bBackLFall;
#define IsDropBackR()     tDropcheck.bBackRFall;
#define IsDropAnyone()    tDropcheck.bBackRFall;
//悬空标志设置
#define SetDropFrontL( boolValue )    tDropcheck.bFrontLFall = boolValue ;
#define SetDropFrontR( boolValue )    tDropcheck.bFrontRFall = boolValue ;
#define SetDropBackL( boolValue )     tDropcheck.bBackLFall = boolValue ;
#define SetDropBackR( boolValue )     tDropcheck.bAnyoneFall = boolValue ;

#ifdef  DROPHONGCHUAN
void vSetDropADAverage(void);
void vPrintfDropALl(void);
uint16_t usGetDropMiddleAD(uint16_t *pusSource, uint8_t ucLength, uint16_t LimitMIN,uint16_t LimitMAX);
uint16_t usGetDropDifferentAD(uint16_t *pusSource, uint8_t ucLength, uint16_t LimitMIN,uint16_t LimitMAX);
#endif

void vDropCheckTask(void *p);
#endif 
