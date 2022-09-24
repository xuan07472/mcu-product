/**********************************************************************************
 * Filename   ：SystemTime.c
 * Author     : Liu Tusheng /HongYi Liang /pusheng     
 * Version    : 2.0
 * Date       : 2016.5.25  20171016 20171028 2019 11.7
 * Discription : 非阻塞延时 判断延时溢出 用FREERTOS的时基
**********************************************************************************/ 
#include "SystemTime.h"




OS_TimeStruct OS_RunTime = {0}; //记录系统运行时间





/*******************************************************************************
* Function Name  : RecordOSRunTime
* Description    : 记录系统运行时间，被时基函数xPortSysTickHandler调用
* Input          : None                
* Output         : None
* Return         : None
*******************************************************************************/
void vRecordOSRunTime( void )
{
    static uint8_t ucTickTime = (1000/configTICK_RATE_HZ); //系统时间片时间，1s = 1000ms
    
    OS_RunTime.Time_mSec += ucTickTime; 
    if( OS_RunTime.Time_mSec >= 1000 )
    {
        OS_RunTime.Time_mSec = 0;
        OS_RunTime.Time_Sec++;       
        if( OS_RunTime.Time_Sec >= 60 )
        {
            OS_RunTime.Time_Sec = 0;
            OS_RunTime.Time_Min++;
        }
//        printf( "LINE--%d: OS run times %d:%d\r\n", __LINE__, OS_RunTime.Time_Min, OS_RunTime.Time_Sec );
    }
                    
}




/*******************************************************************************
* Function Name  : GetOSRunTimeNow
* Description    : 获取系统运行时间，用于一些计时操作
* Input          : None                
* Output         : None
* Return         : 系统运行时间
*******************************************************************************/
OS_TimeStruct GetOSRunTimeNow( void )
{      
    return OS_RunTime;
}

/*******************************************************************************
* Function Name  : prvClearTimeValue
* Description    : 清除时间变量
* Input          : *TimeStrct ：需要清除的时间变量               
* Output         : None
* Return         : None
*******************************************************************************/
/*static void prvClearTimeValue( OS_TimeStruct *TimeStrct )
{      
    TimeStrct->Time_Min = 0;
    TimeStrct->Time_Sec = 0;
    TimeStrct->Time_mSec = 0;
}*/

/*******************************************************************************
* Function Name  : SingleTimeoutCheck
* Description    : Get times interval by each call
* Input          : None           
* Output         : None
* Return         : Times interval(unit:ms)
*******************************************************************************/
uint32_t GetOSTimeInterval(OS_TimeStruct *LastTime)
{
    uint32_t result;
    OS_TimeStruct GetTime = GetOSRunTimeNow( );
    //caculate interval
    result = ((int32_t)GetTime.Time_mSec - LastTime->Time_mSec) + \
             ((int32_t)GetTime.Time_Sec - LastTime->Time_Sec)*1000 + \
             ((int32_t)GetTime.Time_Min - LastTime->Time_Min)*1000*60;
    
    //update last
    *LastTime = GetTime;
    
    return result;
}
/*******************************************************************************
* Function Name  : ResetSingleTimeoutCheck
* Description    : To reset single time out check sturct
* Input          : *TimeStruct:          
* Output         : TimeStruct
* Return         : None
*******************************************************************************/
void ResetSingleTimeoutCheck(OS_TimeStruct *TimeStruct)
{
    *TimeStruct=GetOSRunTimeNow(  );
}
/*******************************************************************************
* Function Name  : SingleTimeoutCheck
* Description    : 单次超时检测
* Input          : *TimeStruct:需要检测的时间变量，Min、Sec、mS分别为超时时间的分、秒、毫秒             
* Output         : None
* Return         : 超时状态
*******************************************************************************/
SystemTimeOutState SingleTimeoutCheck( OS_TimeStruct TimeStruct, uint32_t Min, uint32_t Sec, uint16_t mS )
{
    OS_TimeStruct GetTime = GetOSRunTimeNow(  );

    uint64_t timeSor_ms =  ((uint64_t)TimeStruct.Time_Min*60+TimeStruct.Time_Sec)*1000+TimeStruct.Time_mSec;
    uint64_t timeGet_ms =  ((uint64_t)GetTime.Time_Min*60+GetTime.Time_Sec)*1000+GetTime.Time_mSec; 
    uint64_t timeSet_ms =  ((uint64_t)Min*60+Sec)*1000+mS;
    if(timeGet_ms - timeSor_ms > timeSet_ms)
    {
        return TimeOut;
    }          

    return NotTimeOut;        

}





/*******************************************************************************
* Function Name  : MultipleTimeoutCheck
* Description    : 连续超时检测
* Input          : *TimeStruct:需要检测的时间变量，Min、Sec、mS分别为超时时间的分、秒、毫秒             
* Output         : None
* Return         : 超时状态
*******************************************************************************/
SystemTimeOutState MultipleTimeoutCheck( OS_TimeStruct *TimeStruct, uint32_t Min, uint32_t Sec, uint16_t mS )
{
    OS_TimeStruct GetTime = GetOSRunTimeNow(  );
    
    uint64_t timeSor_ms =  ((uint64_t)TimeStruct->Time_Min*60+TimeStruct->Time_Sec)*1000+TimeStruct->Time_mSec;
    uint64_t timeGet_ms =  ((uint64_t)GetTime.Time_Min*60+GetTime.Time_Sec)*1000+GetTime.Time_mSec; 
    uint64_t timeSet_ms =  ((uint64_t)Min*60+Sec)*1000+mS;
    if(timeGet_ms - timeSor_ms > timeSet_ms)
    {
        /*updatelast*/
        TimeStruct->Time_Min = GetTime.Time_Min;
        TimeStruct->Time_Sec = GetTime.Time_Sec;
        TimeStruct->Time_mSec = GetTime.Time_mSec;
        return TimeOut;
    }       


    return NotTimeOut;              
}



/*******************************************************************************
* Function Name  : vTIM7_Configuration
* Description    : TIM7 configuration for delay
* Input          : timeUs:delay times (us)  Max:TIM7_PERIODE 
* Output         : None
* Return         : None
*******************************************************************************/
//void vDelayTimer_TIM7_Configuration(void)
//{
//    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
//    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7 , ENABLE);

//    TIM_DeInit(TIM7);
//    TIM_TimeBaseStructure.TIM_Period=TIM7_PEROID;		 	
//    TIM_TimeBaseStructure.TIM_Prescaler= (84 - 1);				
//    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 		
//    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
//    TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);
//    TIM_ClearFlag(TIM7, TIM_FLAG_Update);							    	
//    TIM_Cmd(TIM7, ENABLE);													
//}
/*******************************************************************************
* Function Name  : delay_us
* Description    : delay us 
* Input          : timeUs:delay times (us)  Max:TIM7_PERIODE 
* Output         : None
* Return         : None  
* Attantion      : delay max time equar TIM7_PEROID
*******************************************************************************/
//void delay_us(uint16_t timeUs)
//{
//    u16 now;
//    u16 begin;
//    /*Here is to prevent overflow*/
//    if(timeUs>=TIM7_PEROID)
//        timeUs=TIM7_PEROID;
//    
//    begin=TIM7->CNT;
//    while(1)
//    {
//        now=TIM7->CNT;
//        if(now>=begin)
//        {
//            if((now-begin)>=timeUs)
//                break;
//        }
//        else if(now<begin)
//        {
//            if((TIM7_PEROID-begin+now)>=timeUs)
//                break;
//        }
//    }
//}
/*******************************************************************************
* Function Name  : ResetTimeInterval
* Description    : Reset Time interval
* Input          : struct
* Output         : time->us_interval:us  Max value:TIM7_PERIODE 
* Return         : None
*******************************************************************************/
//bool ResetTimeInterval(struct timegetstruct *time)
//{   
//    if( TIM_CR1_CEN != (TIM7->CR1 | TIM_CR1_CEN) )
//    {
//       //printf("TIM7 hasn't been enabled");
//       return false; 
//    }

//    //update last time
//    time->us_last=TIM7->CNT;
//    
//    return true;
//}
/*******************************************************************************
* Function Name  : GetTimeInterval
* Description    : get time interval
* Input          : struct
* Output         : time->us_interval:us  Max value:TIM7_PERIODE 
* Return         : None
*******************************************************************************/
//uint16_t GetTimeInterval(struct timegetstruct *time)
//{
//    //get new time
//    time -> us_new = TIM7->CNT;   
//    
//    //get time interval
//    if(time->us_new > time->us_last)
//    {
//        time->us_interval = time->us_new - time->us_last;
//    }
//    else
//    {
//        time->us_interval = TIM7_PEROID- time->us_last + time->us_new;
//    }
//    
//    //update last time
//    time->us_last=time->us_new;
//    
//    return time->us_interval;
//}



