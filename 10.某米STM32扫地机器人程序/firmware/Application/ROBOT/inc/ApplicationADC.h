#ifndef __APP_ADC_H
#define __APP_ADC_H

#include "SystemTime.h"
#include "freertos.h"
#include "task.h"
#include "adc.h"

enum  eInChipAdcChannel
{
    adcAnalogChannel       =0,
    adcBatteryTemperature  =1,
    adcSideBrush           =2,
    adcDropFrontR          =3,
    adcDischargeCurrent    =4,
    adcElectricQuantity    =5,
    adcDropBackR           =6,
    adcFanCurrent          =7,
    adcLeftWheel           =8,
    adcRightWheel          =9,
    adcRollBrush           =10,
    adcDropBackL           =11,
    adcDropFrontL           =12,
};
enum  eAnalogAdcChannel
{
    adcAlong_Edge     = 0,
    adcBatteryID      = 1,
    adcChargeCurrent  = 2,
    adcRechargeStand  = 3,
    
    adcAnalog_Switch_Num,
};

#define ALONG_EDGE_VALUE() g_usAnalogSwitch[adcAlong_Edge]
#define AEV         (ALONG_EDGE_VALUE()>2350 ? 2350: ALONG_EDGE_VALUE())//范围限制

/* 将沿边传感器的AD值 转换成距离mm
2450 1900 1550 1320 1150 1000 900 830 740 660  590   ad值y
 0    10   20   30   40   50   60  70  80  90  100   距离x（mm）

y=-35x+2350   0<x<30
y=-15x+1750  30<x<50
y= -8x+1410  50<x<100

*/
#define ALONG_EDGE_DISTANCE()  ( (int) (AEV>1300)?((AEV-2350)/-35):\
                                      ((AEV>1000)?((AEV-1750)/-15):\
                                      ((AEV-1410)/-8)) )

#define ALONG_EDGE_DISTANCE_TRANSF_VALUE( x )  ( (int) (x<30)?((-35*x)+2350):\
                                               ((x<50)?((-15*x)+1750):\
                                               ((-8*x)+1410) ) )

extern TaskHandle_t HandleADCDataProcessing;
extern uint16_t g_usInChipAdcChannelAverageValue[ADC_CHANNEL_NUM]; //ADC 各通道平均值   
extern uint16_t g_usAnalogSwitch[adcAnalog_Switch_Num];            //模拟开关ADC采样值

/*public*/
void vADCDataProcessingTask(void *p);

/*private*/
void vGetADCChannelAverageNumber( uint16_t *pusBuffer, uint16_t *pusSwitch );

#endif

