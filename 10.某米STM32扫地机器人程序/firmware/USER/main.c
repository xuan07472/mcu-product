#include "FreeRTOS.h"
#include "task.h"
#include "usmart.h"
#include "hardwareManage.h"
#include "taskmanage.h"
#include "main.h"

/************************************************
C89��׼
1 tab= 4 space

************************************************/


volatile UserData g_tRobotState = {0};//������ȫ��״̬

int main(void)
{
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4);//����ϵͳ�ж����ȼ�����4	 

    vSystemHardwareDriverInit();//Ӳ����ʼ��
    
#ifdef MYDEBUG
    usmart_dev.init(168);//����ᴴ��һ��FREERTOS��������Ϊʱ��vUsmartTimerTask  
#endif
    
    vTaskCreateFunction();//��������
    vTaskStartScheduler();//�����������                
   
}


