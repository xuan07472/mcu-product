#include "taskmanage.h"
/**********************************************************************************
 * Filename   ��taskmanage.c
 * Author     : pusheng       
 * Version    : 1.0
 * Date       : 2019.11.7
 * Discription : freeRTOS �������
 *
**********************************************************************************/	

/*******************************************************************************
* Function Name  : vTaskCreateFunction(void)
* Description    : ϵͳ�������񴴽�����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vTaskCreateFunction(void)
{   
    xTaskCreate( vSystemHardwareWorkTask, "SystemHardwareWorkTask",  310, 0, 1,0);//ϵͳӲ���������� ���ȼ�1
    xTaskCreate( vADCDataProcessingTask,  "vADCDataProcessingTask",  310, 0, 1,&HandleADCDataProcessing);//adc���ݴ����������ȼ�1
    xTaskCreate( vGarbage_box_check_task, "vGarbage_box_check_task", 310, 0, 1,0);//�����м������ ���ȼ�1
    xTaskCreate( vGyroscopse_task,        "vGyroscopse_task",        310, 0, 2,0);//����������     ���ȼ�2
    xTaskCreate( vMotorSpeedUpdateTask,   "vMotorSpeedUpdateTask",   320, 0, 1,0);//�ٶ�ת�ٸ���   ���ȼ�1
    xTaskCreate( vAllMotorMotionTask,     "vAllMotorMotionTask",     320, 0, 3,0);//�������       ���ȼ�3
    xTaskCreate( vSoftSpeedAdvanceTask,   "vSoftSpeedAdvanceTask",   120, 0, 1,0);//���������     ���ȼ�1
    xTaskCreate( vPowerChipTask,          "vPowerChipTask",          120, 0, 1,0);//��ع��������� ���ȼ�1
    xTaskCreate( vUltrasonicTask,         "vUltrasonicTask",         120, 0, 2,0);//����������     ���ȼ�1
    xTaskCreate( vDropCheckTask,          "vDropCheckTask",          120, 0, 2,0);//����������     ���ȼ�2
    xTaskCreate( vBehaviorManageTask,     "vBehaviorManageTask",     240, 0, 2,0);//��Ϊ����       ���ȼ�1
    xTaskCreate( vErrorAbnormalCheckTask, "vErrorAbnormalCheckTask", 240, 0, 2,0);//����������   ���ȼ�2
    //xTaskCreate( vFrameDataHandlerTask,   "vFrameDataHandlerTask",   240, 0, 2,0);//����֡���ݴ������� ���ȼ�2
}

/*******************************************************************************
* Function Name  : vSystemHardwareWorkTask(void)
* Description    : ϵͳӲ��������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vSystemHardwareWorkTask(void *p)
{   
    while(1)
    {
        vButtonCheckOperation(); //����Ӧ��
        vLedStateDisplay();      //LEDӦ��
        vUsart1Interactive();// ����1���ʹ�����
        vUsart2Interactive();// ����2���ͺͽ��ܴ�����
        
        vTaskDelay(5);
    }
}

