#include "FreeRTOS.h"
#include "task.h"
#include "usmart.h"
#include "hardwareManage.h"
#include "taskmanage.h"
#include "main.h"

/************************************************
C89标准
1 tab= 4 space

************************************************/


volatile UserData g_tRobotState = {0};//机器人全局状态

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//设置系统中断优先级分组4	 

    vSystemHardwareDriverInit();//硬件初始化
    
#ifdef MYDEBUG
    usmart_dev.init(168);//里面会创建一个FREERTOS任务来作为时基vUsmartTimerTask  
#endif
    
    vTaskCreateFunction();//创建任务
    vTaskStartScheduler();//开启任务调度                
   
}


