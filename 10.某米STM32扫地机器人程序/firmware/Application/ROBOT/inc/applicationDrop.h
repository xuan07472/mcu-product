
#ifndef __APP_DROP_H
#define __APP_DROP_H

#include "sys.h"
#include "adc.h"
#include "TimePwm.h"
#include "applicationadc.h"
#include "mymath.h"
#include "stdlib.h "


enum eDROPAD
{
    adBL    =0,
    adBR    =1,
    adFL    =2,
    adFR    =3,
};

typedef struct DropCheck
{
    bool bAnyoneFall;      //检测到悬空
    bool bFrontLFall;
    bool bFrontRFall;
    bool bBackLFall;
    bool bBackRFall;
    
}DropCheckData_t;

extern DropCheckData_t tDropcheck;
extern uint16_t g_usDROP_AD[4];
extern uint16_t g_usDROP_DIFFERENT_AD[4];  //地感前后差值

/*dropAD值*/
#define g_FALL_MIN (250)        //   跌落阈值
#define g_FALL_DIFFERENT_THRESHOLD (120) //前后差值

#define FrontLFallAD    g_usInChipAdcChannelAverageValue[adcDropFrontL]
#define BackLFallAD     g_usInChipAdcChannelAverageValue[adcDropBackL]
#define FrontRFallAD    g_usInChipAdcChannelAverageValue[adcDropFrontR]
#define BackRFallAD     g_usInChipAdcChannelAverageValue[adcDropBackR]



//public
//接口
#define usGetDropRightFrontAD()       g_usDROP_AD[adFR]
#define usGetDropRightBackAD()        g_usDROP_AD[adBR]
#define usGetDropLeftFrontAD()        g_usDROP_AD[adFL]
#define usGetDropLeftBackAD()         g_usDROP_AD[adBL]

#define bGetDropFrontL()    tDropcheck.bFrontLFall;
#define bGetDropFrontR()    tDropcheck.bFrontRFall;
#define bGetDropBackL()     tDropcheck.bBackLFall;
#define bGetDropBackR()     tDropcheck.bBackRFall;


void vDROPTask(void *p);
void vPrintfDropALl(void);


#endif

