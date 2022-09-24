#include "taskmanage.h"
/**********************************************************************************
 * Filename   ：taskmanage.c
 * Author     : pusheng       
 * Version    : 1.0
 * Date       : 2019.11.7
 * Discription : freeRTOS 任务管理
 *
**********************************************************************************/	

/*******************************************************************************
* Function Name  : vTaskCreateFunction(void)
* Description    : 系统所有任务创建函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vTaskCreateFunction(void)
{   
    xTaskCreate( vSystemHardwareWorkTask, "SystemHardwareWorkTask",  310, 0, 1,0);//系统硬件工作任务 优先级1
    xTaskCreate( vADCDataProcessingTask,  "vADCDataProcessingTask",  310, 0, 1,&HandleADCDataProcessing);//adc数据处理任务优先级1
    xTaskCreate( vGarbage_box_check_task, "vGarbage_box_check_task", 310, 0, 1,0);//垃圾盒检测任务 优先级1
    xTaskCreate( vGyroscopse_task,        "vGyroscopse_task",        310, 0, 2,0);//陀螺仪任务     优先级2
    xTaskCreate( vMotorSpeedUpdateTask,   "vMotorSpeedUpdateTask",   320, 0, 1,0);//速度转速更新   优先级1
    xTaskCreate( vAllMotorMotionTask,     "vAllMotorMotionTask",     320, 0, 3,0);//电机驱动       优先级3
    xTaskCreate( vSoftSpeedAdvanceTask,   "vSoftSpeedAdvanceTask",   120, 0, 1,0);//软加速任务     优先级1
    xTaskCreate( vPowerChipTask,          "vPowerChipTask",          120, 0, 1,0);//电池管理充电任务 优先级1
    xTaskCreate( vUltrasonicTask,         "vUltrasonicTask",         120, 0, 2,0);//超声波任务     优先级1
    xTaskCreate( vDropCheckTask,          "vDropCheckTask",          120, 0, 2,0);//防跌落任务     优先级2
    xTaskCreate( vBehaviorManageTask,     "vBehaviorManageTask",     240, 0, 2,0);//行为任务       优先级1
    xTaskCreate( vErrorAbnormalCheckTask, "vErrorAbnormalCheckTask", 240, 0, 2,0);//警告检测任务   优先级2
    //xTaskCreate( vFrameDataHandlerTask,   "vFrameDataHandlerTask",   240, 0, 2,0);//串口帧数据处理任务 优先级2
}

/*******************************************************************************
* Function Name  : vSystemHardwareWorkTask(void)
* Description    : 系统硬件任务函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vSystemHardwareWorkTask(void *p)
{   
    while(1)
    {
        vButtonCheckOperation(); //按键应用
        vLedStateDisplay();      //LED应用
        vUsart1Interactive();// 串口1发送处理函数
        vUsart2Interactive();// 串口2发送和接受处理函数
        
        vTaskDelay(5);
    }
}

