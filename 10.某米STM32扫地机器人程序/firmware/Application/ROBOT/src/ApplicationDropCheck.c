#include "ApplicationDropCheck.h"

/**
 * @Filename   ：ApplicationDropCheck.c
 * @Author     : HuangWentao LiuHongChuan    
 * @Version    : 1.0
 * @Date       : 2019/12/10
 * @Discription : 四个防跌落传感器
 * @hardware connect:
 *          --------------TIM4-CH4-----------------------
 */

#ifndef DROPHONGCHUAN
#define FALLING_THRESHOLD 15 //跌落阈值
DropCheckData_t tDropcheck={0};//四个跌落传感器标志位
//
uint16_t g_usDROP_AD[4]={0};  //地感AD值 

void GetDropAdValue(void);

//防跌落任务
void vDropCheckTask(void *p)
{
  
    while(1)
    {
        #if DROPEXTERNALTEST==0
        GetDropAdValue();
        #endif
        
        vTaskDelay(10);
    }

}

/**
  * @Description 根据四个跌落传感器AD值，来设置跌落判断标志位
  * @param   无
  * @retval  无
  */
void GetDropAdValue(void)
{
    
    int i = 0;
    int Drop_AD_Value[4] = {0};//用于存放四个传感器的AD值
    
    for(i = 0; i<10; i++)//取10次的平均值
    {
        
        vTaskDelay(1);
        Drop_AD_Value[adFL]+=abs(LF_DropADValue-2048)/10;//完全悬空AD值为2048 离地越近越趋向于两边，（LF_DropADValue-2048）取绝对值得到0-2048范围的AD值（数值越大离地越近）
        Drop_AD_Value[adFR]+=abs(RF_DropADValue-2048)/10;
        Drop_AD_Value[adBL]+=abs(LB_DropADValue-2048)/10;
        Drop_AD_Value[adBR]+=abs(RB_DropADValue-2048)/10;
    }
    
    tDropcheck.bFrontLFall = Drop_AD_Value[adFL]>FALLING_THRESHOLD?0:1;//左前传感器 判断是否大于跌落阈值，小于阈值标记为1，表示有悬崖
    tDropcheck.bFrontRFall = Drop_AD_Value[adFR]>FALLING_THRESHOLD?0:1;//右前传感器
    tDropcheck.bBackLFall = Drop_AD_Value[adBL]>FALLING_THRESHOLD?0:1;//左后传感器
    tDropcheck.bBackRFall = Drop_AD_Value[adBR]>FALLING_THRESHOLD?0:1;//右后传感器
    tDropcheck.bAnyoneFall = (tDropcheck.bFrontLFall | tDropcheck.bFrontRFall | tDropcheck.bBackLFall | tDropcheck.bBackRFall);//任意一个感应到掉落时为1

    //全局变量设置
    usSetDropRightFrontAD(  Drop_AD_Value[adFR] ) ;      
    usSetDropRightBackAD(  Drop_AD_Value[adBR] ) ;      
    usSetDropLeftFrontAD(  Drop_AD_Value[adFL] ) ;     
    usSetDropLeftBackAD(  Drop_AD_Value[adBL] ) ;
    
    memset(&Drop_AD_Value[0],0,4*sizeof(Drop_AD_Value[0]));
     
}
#endif

#ifdef  DROPHONGCHUAN
DropCheckData_t tDropcheck={0};//四个跌落传感器标志位
uint16_t g_usDROP_AD[4]={0};  //地感AD值 
uint16_t g_usDROP_DIFFERENT_AD[4]={0};  //地感前后差值

//防跌落任务
void vDropCheckTask(void *p)
{
  
    while(1)
    {
        #if DROPEXTERNALTEST==0
        
        vSetDropADAverage();
        
        #endif
        
        vTaskDelay(10);
    }

}
/*******************************************************************************
* Function Name  : vFallFlagCheck
* Description    : 悬空标志位设置
                    周期：10ms更新一次
                   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//void vFallFlagCheck(void)
//{


//    //根据离地距离与前后ad差值判断悬空
//     if(usGetDropLeftBackAD()>g_DROP_THRESHOLD&& g_usDROP_DIFFERENT_AD[adBL]<g_FALL_DIFFERENT_THRESHOLD)
//    {
//         
//        tDropcheck.bBackLFall=true;
//    }
//    else
//    {
//        tDropcheck.bBackLFall=false;
//    }
//    
//    if(usGetDropLeftFrontAD()>g_DROP_THRESHOLD&&g_usDROP_DIFFERENT_AD[adFL]<g_FALL_DIFFERENT_THRESHOLD)
//    {
//        tDropcheck.bFrontLFall=true;
//    }
//    else
//    {
//        tDropcheck.bFrontLFall=false; 
//    }
//    
//    if(usGetDropRightBackAD()>g_DROP_THRESHOLD&&g_usDROP_DIFFERENT_AD[adBR]<g_FALL_DIFFERENT_THRESHOLD)
//    {
//        tDropcheck.bBackRFall=true;
//    }
//    else
//    {
//        tDropcheck.bBackRFall=false;
//    }
//    
//    if(usGetDropRightFrontAD()>g_DROP_THRESHOLD&&g_usDROP_DIFFERENT_AD[adFR]<g_FALL_DIFFERENT_THRESHOLD)
//    {
//        tDropcheck.bFrontRFall=true;
//    }
//    else
//    {
//        tDropcheck.bFrontRFall=false;
//    }
//    
//    if(tDropcheck.bBackLFall|tDropcheck.bBackRFall|tDropcheck.bFrontLFall|tDropcheck.bFrontRFall)
//    {
//        tDropcheck.bAnyoneFall=true;
//    }
//    else
//    {
//       tDropcheck.bAnyoneFall=false;
//    }
// 
//}

/*******************************************************************************
* Function Name  : vSetDropADAverage
* Description    : 地感AD值设置 减少波动
                   
* Input          : None
* Output         : 每10ms输出一次AD值
* Return         : None
*******************************************************************************/

void vSetDropADAverage(void)
{ 
    #define DROP_CLIFF_AD 2050   //悬崖值 检测到悬崖时的原始AD
    int nNum; 
    static uint16_t DropAD[4][8]={0};      //地感传感器ad值
    static uint16_t DropDifferentAD[4][8]={0};      //存放地感前后两次读取ad值差值

    //获取ad原始值与悬崖值的差值
    for(nNum=0; nNum<8; nNum++)
    {
        vTaskDelay(1);
        DropAD[adBL][nNum] = abs(LB_DropADValue - DROP_CLIFF_AD);
        DropAD[adBR][nNum] = abs(RB_DropADValue - DROP_CLIFF_AD);
        DropAD[adFL][nNum] = abs(LF_DropADValue - DROP_CLIFF_AD);
        DropAD[adFR][nNum] = abs(RF_DropADValue - DROP_CLIFF_AD);
        
    }
    for(nNum=0; nNum<8; nNum++)
    {
       DropDifferentAD[adBL][nNum] = DropAD[adBL][nNum];
       DropDifferentAD[adBR][nNum] = DropAD[adBR][nNum];
       DropDifferentAD[adFL][nNum] = DropAD[adFL][nNum];
       DropDifferentAD[adFR][nNum] = DropAD[adFR][nNum];
        
    }
    
    //获取中间值
    g_usDROP_AD[adBL] = usGetDropMiddleAD(DropAD[adBL], 8, 0, 2100);
    g_usDROP_AD[adBR] = usGetDropMiddleAD(DropAD[adBR], 8, 0, 2100);
    g_usDROP_AD[adFL] = usGetDropMiddleAD(DropAD[adFL], 8, 0, 2100);
    g_usDROP_AD[adFR] = usGetDropMiddleAD(DropAD[adFR], 8, 0, 2100);
        

    
    //取前后差值
    g_usDROP_DIFFERENT_AD[adBL] = usGetDropDifferentAD(DropDifferentAD[adBL], 8, 0, 2100);
    g_usDROP_DIFFERENT_AD[adBR] = usGetDropDifferentAD(DropDifferentAD[adBR], 8, 0, 2100);
    g_usDROP_DIFFERENT_AD[adFL] = usGetDropDifferentAD(DropDifferentAD[adFL], 8, 0, 2100);
    g_usDROP_DIFFERENT_AD[adFR] = usGetDropDifferentAD(DropDifferentAD[adFR], 8, 0, 2100);
    
    
  
}
/*******************************************************************************
* Function Name  : usGetDropMiddleAD
* Description    : 获取地感数组中间值的数  
* Input          :pusSource 来源数组，ucLength 数组长度； LimitMAX： 数据大小有效范围上限 ； 
                    LimitMIN 数据大小有效范围下限
* Output         : None
* Return         : usMiddle：中间值与地面值的差值 （地面值随占空比改变,1khz 30%时为1050）
*******************************************************************************/
uint16_t usGetDropMiddleAD(uint16_t *pusSource, uint8_t ucLength, uint16_t LimitMIN,uint16_t LimitMAX)
{
    uint8_t i=0, ucLen=0; //ucLen 有效数值的数量
    uint16_t usMin,usCount = 0,usPoint;
    static uint16_t uslastMiddle, usMiddle=0,IsFirst=1;
    
    
    //清除数组里的错误值
    for(i=0; i<ucLength; i++)
    {
        if(pusSource[i] > LimitMIN && pusSource[i]< LimitMAX)
        {
            pusSource[ucLen]=pusSource[i];
            ucLen++;
        }
            
    }
    for(i=1; i<ucLength; i++)
    {
        if(abs(pusSource[i]-pusSource[i+1])>500)
        {
            pusSource[i]
        }
    }
    //有效长度检测 
    if(ucLen<1)
    {
        //返回错误标志
        return 5000;
    }
    
    
//    for(i=0;i<ucLen;i++)
//    {
//        usMiddle+=pusSource[i];
//    }
//    usMiddle/=10;
    
    //取中值， 平均线在波形的下半部分
    while( usCount <= (ucLen/2) )
    {
        usPoint = 0;
        usMin = pusSource[0];
        for( i=1; i<ucLength; i++ ) 
        {      
            if( pusSource[i] > usMin ) 
            {
                usMin = pusSource[i];
                usPoint = i;
            }                
            
        }
        usMiddle = usMin;
        pusSource[usPoint] = 0;   
        usCount++;       
    }
//    if(abs(usMiddle-uslastMiddle)>800&& (!IsFirst) )
//    {
//        usMiddle = uslastMiddle;
//    }
//    if(IsFirst)
//    {
//        IsFirst=0;
//    }
    uslastMiddle=usMiddle;
    return usMiddle;
}
/*******************************************************************************
* Function Name  : usGetDropDifferentAD
* Description    : 获取地感数组前后两次读取的差值
* Input          :pusSource 地感ad数组，ucLength 数组长度； LimitMAX： 数据大小有效范围上限 ； 
                    LimitMIN 数据大小有效范围下限
* Output         : None
* Return         : usMax 前后最大差值
*******************************************************************************/
uint16_t usGetDropDifferentAD(uint16_t *pusSource, uint8_t ucLength, uint16_t LimitMIN,uint16_t LimitMAX)
{
    uint8_t i=0, ucLen=0; //ucLen 有效数值的数量
    uint16_t usMax;
    
    //清除数组里的错误值
    for(i=0; i<ucLength; i++)
    {
        if(pusSource[i] >LimitMIN && pusSource[i] < LimitMAX)
        {
            pusSource[ucLen] = pusSource[i];
            ucLen++;
        }
            
    }
    //有效长度检测 错误返回5000
    if(ucLen<2)
    {
        return 5000;
    }
   
    //取前后差值
    for(i=0; i<ucLen-1; i++)
    {
        pusSource[i] = abs(pusSource[i+1] - pusSource[i]);  
    }
    ucLen -= 1;
    //取最大值
    usMax = pusSource[0];
    for(i=1; i<ucLen; i++)
    {
        if(pusSource[i] > usMax)
        {
            usMax = pusSource[i];
        }
    }
    return usMax;
    
}
/*******************************************************************************
* Function Name  : vPrintfDropALl
* Description    : 输出drop相关数据
* Input          : 
* Output         : None
* Return         :none
*******************************************************************************/
void vPrintfDropALl(void)
{
    printf("频率：%d   占空比：%d \r\n",144000/(TIM4->PSC),(TIM4->CCR4)/5);
    printf("预分频数 %d\r\n",TIM4->PSC);
    printf("Pulse: %d\r\n",TIM_GetCapture4(TIM4));
    printf("左前 : %d \r\n",usGetDropLeftFrontAD());
    printf("左后 : %d   \r\n",usGetDropLeftBackAD());
    printf("右前 : %d  \r\n",usGetDropRightFrontAD());
    printf("右后 : %d \r\n",usGetDropRightBackAD());
    
    printf("左前 : %d \r\n", g_usDROP_DIFFERENT_AD[adFL]);
    printf("左后 : %d   \r\n", g_usDROP_DIFFERENT_AD[adBL]);
    printf("右前 : %d  \r\n", g_usDROP_DIFFERENT_AD[adFR]);
    printf("右后 : %d \r\n", g_usDROP_DIFFERENT_AD[adBR]);
}

#endif


