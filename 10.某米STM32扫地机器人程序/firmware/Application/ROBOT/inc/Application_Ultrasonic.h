#ifndef __APP_ULTRA_H
#define __APP_ULTRA_H
#include "sys.h"
#include "exti.h"
#include "SystemTime.h"
#include "iooutput.h"
#include "exti.h"
#include "BMI160.h" 

typedef struct 
{
    bool bClashFlag;
    bool bCrashFlag;    //�������룬δʹ��
    uint16_t usClashDistance; // �ϰ������ unit: mm
    
}ClashStruct_t;


bool bGetClashFlag(void);
bool bGetClashCrashFlag(void);
uint16_t usGetClashDistance(void);
void vPrintfUltrasonicALL(void);
void vUltrasonicTask(void *p);


#endif

