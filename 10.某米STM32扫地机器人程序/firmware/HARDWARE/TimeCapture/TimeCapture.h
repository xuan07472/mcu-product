#ifndef __TIMECAPTURE_H
#define __TIMECAPTURE_H

#include "sys.h"
#include "stdlib.h"

/*TIM8 ����ֱ���1Mhz*/
#define TIM8_ARR (50000)
#define TIM8_PSC (72)

/*TIM1 ����ֱ���100Khz*/
#define TIM1_ARR (50000)
#define TIM1_PSC (720)

/*���������ݻ������*/   
#define EncodeNumber 5 

/*�����±�ѭ������*/
#define GET_LEFT_ENCODE_DONE(  ) \
    g_tLeftWheel.ucPoint = (g_tLeftWheel.ucPoint+1 ==  EncodeNumber)? 0 : (g_tLeftWheel.ucPoint+1);\
    g_tLeftWheel.usHistory = g_tLeftWheel.usNew

#define GET_RIGHT_ENCODE_DONE(  ) \
    g_tRightWheel.ucPoint = (g_tRightWheel.ucPoint+1 ==  EncodeNumber)? 0 : (g_tRightWheel.ucPoint+1);\
    g_tRightWheel.usHistory = g_tRightWheel.usNew
    


/*�����ַ���ö��*/
typedef enum{
    WheelRetreat = 0,
    WheelForward ,
    WheelStop,
}WheelDirection_e;

/*�����ֽṹ��*/
typedef struct{
    
    uint16_t usEncodeBuffer[EncodeNumber];         //���ӱ������ݻ���      
    uint16_t usHistory;                           //������һ�ζ�ʱ������ֵ                           
    uint16_t usNew;                               //��������һ�ζ�ʱ������ֵ           
    uint8_t ucPoint;                              //���ӱ������ݻ����±�
    WheelDirection_e Direction;                    //�����˶�����
    uint16_t TimeOut;                             //���ӱ�������ʱ    
    int16_t RealSpeed;                            //����ʵ���ٶȣ��ٶȷ���
    int16_t ExpectSpeed;                          //���������ٶ�
    int16_t SoftSpeed;                            //������
    int32_t iSignalCount;                         //���ӱ������
    
}WheelSruct_t;

/*���ӽṹ��*/
extern WheelSruct_t g_tLeftWheel ;
extern WheelSruct_t g_tRightWheel ;

/*���������� һ����������1*/
extern u32 g_FanEncodeCount;

void vAll_TIM_CaptureInit(void);

void TIM8_Capture_Init(void);
void TIM1_Capture_Init(void);


void TIM8_CH4_FanCaptureInit(u16 arr,u16 psc);
void TIM1_CH12WheelCaptureInit(u16 arr,u16 psc);

#endif
