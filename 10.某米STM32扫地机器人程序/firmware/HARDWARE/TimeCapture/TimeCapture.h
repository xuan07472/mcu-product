#ifndef __TIMECAPTURE_H
#define __TIMECAPTURE_H

#include "sys.h"
#include "stdlib.h"

/*TIM8 捕获分辨率1Mhz*/
#define TIM8_ARR (50000)
#define TIM8_PSC (72)

/*TIM1 捕获分辨率100Khz*/
#define TIM1_ARR (50000)
#define TIM1_PSC (720)

/*编码器数据缓存个数*/   
#define EncodeNumber 5 

/*数据下标循环增加*/
#define GET_LEFT_ENCODE_DONE(  ) \
    g_tLeftWheel.ucPoint = (g_tLeftWheel.ucPoint+1 ==  EncodeNumber)? 0 : (g_tLeftWheel.ucPoint+1);\
    g_tLeftWheel.usHistory = g_tLeftWheel.usNew

#define GET_RIGHT_ENCODE_DONE(  ) \
    g_tRightWheel.ucPoint = (g_tRightWheel.ucPoint+1 ==  EncodeNumber)? 0 : (g_tRightWheel.ucPoint+1);\
    g_tRightWheel.usHistory = g_tRightWheel.usNew
    


/*驱动轮方向枚举*/
typedef enum{
    WheelRetreat = 0,
    WheelForward ,
    WheelStop,
}WheelDirection_e;

/*驱动轮结构体*/
typedef struct{
    
    uint16_t usEncodeBuffer[EncodeNumber];         //轮子编码数据缓冲      
    uint16_t usHistory;                           //轮子上一次定时器捕获值                           
    uint16_t usNew;                               //轮子最新一次定时器捕获值           
    uint8_t ucPoint;                              //轮子编码数据缓冲下标
    WheelDirection_e Direction;                    //轮子运动方向
    uint16_t TimeOut;                             //轮子编码器超时    
    int16_t RealSpeed;                            //轮子实际速度，速度反馈
    int16_t ExpectSpeed;                          //轮子期望速度
    int16_t SoftSpeed;                            //缓加速
    int32_t iSignalCount;                         //轮子编码个数
    
}WheelSruct_t;

/*轮子结构体*/
extern WheelSruct_t g_tLeftWheel ;
extern WheelSruct_t g_tRightWheel ;

/*风机编码计数 一个脉冲自增1*/
extern u32 g_FanEncodeCount;

void vAll_TIM_CaptureInit(void);

void TIM8_Capture_Init(void);
void TIM1_Capture_Init(void);


void TIM8_CH4_FanCaptureInit(u16 arr,u16 psc);
void TIM1_CH12WheelCaptureInit(u16 arr,u16 psc);

#endif
