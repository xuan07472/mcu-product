#ifndef __ADC_H
#define __ADC_H

#include "sys.h"
#include "iooutput.h"
#include "freertos.h"
#include "task.h"

#define  ADC_CHANNEL_NUM    (13) //ADCͨ����Ŀ
#define  ADC_TRANSFORM_NUM   (3) //ADCת������


extern u16 g_AdcOriginalValue[ADC_TRANSFORM_NUM][ADC_CHANNEL_NUM];

void vAllAdcPinInit(void);


#endif
