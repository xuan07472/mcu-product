#include "Application_Ultrasonic.h"

/**
 * @Filename   Application_Ultrasonic.c
 * @Author     : LiuHongChuan       
 * @Version    : 1.0
 * @Date       : 2019/11/9
 * @Discription : ���������
 * @hardware connect:
 *        
 */
 
uint16_t g_usMaxClashDistance=300; //��������������
uint16_t g_fSoundDistance=0;   //����������
ClashStruct_t tClash={0};

 //private 
float fUltrasonic_Distance_Caculate(uint16_t  width);
void vUltransonicCaculate(int iCrashDis);
 
 
//��־λ�ӿ�
//trueΪ��⵽�ϰ���
bool bGetClashFlag(void)
{
    return tClash.bClashFlag;
}

bool bGetClashCrashFlag(void)
{
    return tClash.bCrashFlag;
}
//���س���������
uint16_t usGetClashDistance(void)
{
    return tClash.usClashDistance;
}
 
 
 /*******************************************************************************
* Function Name  :vUltrasonicTask
* Description    :�������ܴ�����
                  ���� 50ms ��     ���ȣ� 3�����ں�17������ʶ��¼�ͣ�׼ȷ�ȵ�
* Input          :
* Output         : None
* Return         : 
*******************************************************************************/
 void vUltrasonicTask(void *p)
{
    static portTickType xULTrasonicTime;
   
   //�ȴ������ǳ�ʼ�����
    while(GYRO_WORKING != eGyro_State)
    delay_ms(1000);

    // ʹ�õ�ǰʱ���ʼ������xLastWakeTime
     xULTrasonicTime = xTaskGetTickCount();
    while(1)
    {
        vUltrasonic_Emit();
        vTaskDelay(2); //��ʱ��ֹ�����ϴε�ֵ���ֵ
        vUltransonicCaculate(40); //����������
        vTaskDelayUntil(&xULTrasonicTime, 50 );//������ʱ50ms
//      printf("���������� ��%d\r\n",usGetClashDistance()); 
    
       
    }
}

 /*******************************************************************************
* Function Name  :fUltrasonic_Distance_Caculate
* Description    : ����ʱ�����������  
* Input          : width �ߵ�ƽʱ�� unit :us
* Output         : None
* Return         : data ���� unit: mm
*******************************************************************************/
float fUltrasonic_Distance_Caculate(uint16_t  width)
{

    float data=1 ;
    const int SoundSpeed=340;
    data=width/2.0f/1000*SoundSpeed;

    if(data==0)
    {
        data = 1;
    }
    if(data>g_usMaxClashDistance)
    {
        data = g_usMaxClashDistance;
    }
    return data;
}


/*******************************************************************************
* Function Name  :vUltransonicCaculate
* Description    : �������������У׼  Ƶ��:ͬ������������Ƶ��
* Input          : iCrashDis ��ײ׼������ unit:mm    
* Output         : tClash�ṹ�壬��ͻ��־      ����Ϊ999ʱδ��⵽�ϰ���  
* Return         : None
*******************************************************************************/
void vUltransonicCaculate(int iCrashDis)
{
    extern TIM7Time_t tULTime;
    uint8_t DISOFFSET=75; //��Ư
    uint8_t DISFAROFFSET=80; //Զ�������
	if( tULTime.bClashflag )//��ɽ���
    {  
        tULTime.bClashflag  = false;
        g_fSoundDistance=fUltrasonic_Distance_Caculate(tULTime.us_interval);  //ԭʼ��������
        //����
        if(g_fSoundDistance>g_usMaxClashDistance||g_fSoundDistance<1)
        {
          tClash.bClashFlag=false;
          tClash.usClashDistance=999;
          return;
        }
        
        //��⵽�ϰ���
        if(g_fSoundDistance<g_usMaxClashDistance)
        {
            tClash.bClashFlag=true;
          
        }
        
        if(g_fSoundDistance>DISOFFSET&&g_fSoundDistance<150)
            tClash.usClashDistance=(int)g_fSoundDistance-DISOFFSET;  
        
        if(g_fSoundDistance>150&&g_fSoundDistance<g_usMaxClashDistance)
            tClash.usClashDistance=(int)g_fSoundDistance-DISFAROFFSET; 
    }
    else
    {
        tClash.usClashDistance=999;//δ��⵽�ϰ���
        tClash.bClashFlag=false;  
    }
	
}
/*******************************************************************************
* Function Name  :vPrintfUltrasonicALL
* Description    :�������������
* Input          : 
* Output         :        
* Return         : None
*******************************************************************************/
void vPrintfUltrasonicALL(void)
{
    printf("���������� ��%d\r\n",usGetClashDistance()); 
    printf("�Ӵ�����ͻ ��%d\r\n",bGetClashCrashFlag()); 

    
}

