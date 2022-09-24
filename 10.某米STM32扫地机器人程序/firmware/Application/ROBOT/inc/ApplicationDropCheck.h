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

#define DROPEXTERNALTEST 0  //�궨�� ��1ʱΪ�ⲿ���ԣ����ⲿдֵ
//#define DROPHONGCHUAN 

#define  LF_DropADValue     g_usInChipAdcChannelAverageValue[adcDropFrontL]
#define  RF_DropADValue     g_usInChipAdcChannelAverageValue[adcDropFrontR]
#define  LB_DropADValue     g_usInChipAdcChannelAverageValue[adcDropBackL]
#define  RB_DropADValue     g_usInChipAdcChannelAverageValue[adcDropBackR]


#define g_FALL_DIFFERENT_THRESHOLD (100) //ǰ���ֵ�ĵ�����ֵ
#define g_DROP_THRESHOLD 950  //��ؾ��� Խ�����Խ�� (��������ֵΪ1000)

typedef struct DropCheck
{
    bool bAnyoneFall;       //��⵽����
    bool bFrontLFall;       //��ǰ
    bool bFrontRFall;       //��ǰ
    bool bBackLFall;        //���
    bool bBackRFall;        //�Һ�
    
}DropCheckData_t;

//����adֵ����ʹ��
enum eDropAD
{
    adBL    =0,
    adBR    =1,
    adFL    =2,
    adFR    =3,
};

extern DropCheckData_t tDropcheck;

extern uint16_t g_usDROP_AD[4];

//�ӿ�
//��ȡadֵ
#define usGetDropRightFrontAD()       g_usDROP_AD[adFR]
#define usGetDropRightBackAD()        g_usDROP_AD[adBR]
#define usGetDropLeftFrontAD()        g_usDROP_AD[adFL]
#define usGetDropLeftBackAD()         g_usDROP_AD[adBL]
//����adֵ
#define usSetDropRightFrontAD(  value )       g_usDROP_AD[adFR]= value ;
#define usSetDropRightBackAD(  value )        g_usDROP_AD[adBR]= value ;
#define usSetDropLeftFrontAD(  value )        g_usDROP_AD[adFL]= value ;
#define usSetDropLeftBackAD(  value )         g_usDROP_AD[adBL]= value ;
//���ձ�־��ȡ
#define IsDropFrontL()    tDropcheck.bFrontLFall;
#define IsDropFrontR()    tDropcheck.bFrontRFall;
#define IsDropBackL()     tDropcheck.bBackLFall;
#define IsDropBackR()     tDropcheck.bBackRFall;
#define IsDropAnyone()    tDropcheck.bBackRFall;
//���ձ�־����
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
