#ifndef __SYSTEMTIME_H
#define __SYSTEMTIME_H

#include "sys.h"
#include "FreeRTOS.h"
#include <stdbool.h>


typedef enum { NotTimeOut = 0, TimeOut = !NotTimeOut } SystemTimeOutState;

typedef struct timesdelay{    
    uint16_t Time_mSec;
    uint32_t Time_Sec;
    uint32_t Time_Min;    
}OS_TimeStruct;





void vRecordOSRunTime( void );
static void prvClearTimeValue( OS_TimeStruct *TimeStrct );
OS_TimeStruct GetOSRunTimeNow( void );
uint32_t GetOSTimeInterval(OS_TimeStruct *LastTime);
void ResetSingleTimeoutCheck(OS_TimeStruct *TimeStruct);
SystemTimeOutState SingleTimeoutCheck( OS_TimeStruct TimeStruct, uint32_t Min, uint32_t Sec, uint16_t mS );
SystemTimeOutState MultipleTimeoutCheck( OS_TimeStruct *TimeStruct, uint32_t Min, uint32_t Sec, uint16_t mS );

//us level timer
//void vDelayTimer_TIM7_Configuration(void);
//void delay_us(uint16_t timeUs);
//bool ResetTimeInterval(struct timegetstruct *time);
//uint16_t GetTimeInterval(struct timegetstruct *time);

#endif
