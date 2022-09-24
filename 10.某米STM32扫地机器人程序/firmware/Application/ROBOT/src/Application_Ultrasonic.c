#include "Application_Ultrasonic.h"

/**
 * @Filename   Application_Ultrasonic.c
 * @Author     : LiuHongChuan       
 * @Version    : 1.0
 * @Date       : 2019/11/9
 * @Discription : 超声波检测
 * @hardware connect:
 *        
 */
 
uint16_t g_usMaxClashDistance=300; //超声波距离上限
uint16_t g_fSoundDistance=0;   //超声波距离
ClashStruct_t tClash={0};

 //private 
float fUltrasonic_Distance_Caculate(uint16_t  width);
void vUltransonicCaculate(int iCrashDis);
 
 
//标志位接口
//true为检测到障碍物
bool bGetClashFlag(void)
{
    return tClash.bClashFlag;
}

bool bGetClashCrashFlag(void)
{
    return tClash.bCrashFlag;
}
//返回超声波距离
uint16_t usGetClashDistance(void)
{
    return tClash.usClashDistance;
}
 
 
 /*******************************************************************************
* Function Name  :vUltrasonicTask
* Description    :超声波总处理函数
                  周期 50ms ；     精度： 3厘米内和17厘米外识别录低，准确度低
* Input          :
* Output         : None
* Return         : 
*******************************************************************************/
 void vUltrasonicTask(void *p)
{
    static portTickType xULTrasonicTime;
   
   //等待陀螺仪初始化完成
    while(GYRO_WORKING != eGyro_State)
    delay_ms(1000);

    // 使用当前时间初始化变量xLastWakeTime
     xULTrasonicTime = xTaskGetTickCount();
    while(1)
    {
        vUltrasonic_Emit();
        vTaskDelay(2); //延时防止读到上次的值或空值
        vUltransonicCaculate(40); //超声波处理
        vTaskDelayUntil(&xULTrasonicTime, 50 );//绝对延时50ms
//      printf("超声波距离 ：%d\r\n",usGetClashDistance()); 
    
       
    }
}

 /*******************************************************************************
* Function Name  :fUltrasonic_Distance_Caculate
* Description    : 根据时间间隔计算距离  
* Input          : width 高电平时间 unit :us
* Output         : None
* Return         : data 距离 unit: mm
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
* Description    : 超声波距离误差校准  频率:同步超声波发送频率
* Input          : iCrashDis 碰撞准备距离 unit:mm    
* Output         : tClash结构体，冲突标志      距离为999时未检测到障碍物  
* Return         : None
*******************************************************************************/
void vUltransonicCaculate(int iCrashDis)
{
    extern TIM7Time_t tULTime;
    uint8_t DISOFFSET=75; //零漂
    uint8_t DISFAROFFSET=80; //远距离误差
	if( tULTime.bClashflag )//完成接收
    {  
        tULTime.bClashflag  = false;
        g_fSoundDistance=fUltrasonic_Distance_Caculate(tULTime.us_interval);  //原始距离设置
        //出错
        if(g_fSoundDistance>g_usMaxClashDistance||g_fSoundDistance<1)
        {
          tClash.bClashFlag=false;
          tClash.usClashDistance=999;
          return;
        }
        
        //检测到障碍物
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
        tClash.usClashDistance=999;//未检测到障碍物
        tClash.bClashFlag=false;  
    }
	
}
/*******************************************************************************
* Function Name  :vPrintfUltrasonicALL
* Description    :超声波调试输出
* Input          : 
* Output         :        
* Return         : None
*******************************************************************************/
void vPrintfUltrasonicALL(void)
{
    printf("超声波距离 ：%d\r\n",usGetClashDistance()); 
    printf("接触到冲突 ：%d\r\n",bGetClashCrashFlag()); 

    
}

