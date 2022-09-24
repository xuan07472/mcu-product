#include "applicationDrop.h"

/******************************************************************************
* 文件名  ：applicationDrop.c
* 文件描述：悬空检测设置
* 创建人  ：liu hongchuan
* 日    期：2019年11月21日

        
******************************************************************************/
	   

/*drop*/
uint16_t g_usDROP_AD[4]={0};  //地感AD值 平常地面为500到800， 地毯为100-300
uint16_t g_usDROP_DIFFERENT_AD[4]={0};  //地感前后差值
DropCheckData_t tDropcheck={0};

//private
uint16_t usGetQueueMiddleValue( uint16_t *pusBuffer, uint8_t ucLength );
uint8_t DataTranstfer(uint16_t *pusTarget, uint16_t *pusSource, uint8_t ucLength);
void vFallFlagCheck(void);
void vSetDropAdaption(uint16_t DropMin,uint16_t Offset);
void vSetDropADAverage(void);
void vSetDropADDifferent(void);
uint16_t usGetQueueMAXValue( uint16_t *pusBuffer, uint8_t ucLength );
/*******************************************************************************
* Function Name  : vDROPTask
* Description    : 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void vDROPTask(void *p)
{
    while(1)
    {
        vSetDropADAverage();
        vSetDropADDifferent();
        vFallFlagCheck();
     
//        vSetDropAdaption(600.,300);   

                vTaskDelay(10);
    }

}
    
/*******************************************************************************
* Function Name  : vFallFlagCheck
* Description    : 悬空标志位设置
                    周期：40ms
                   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vFallFlagCheck(void)
{


    //根据差值判断悬空
     if(usGetDropLeftBackAD()<g_FALL_MIN&&usGetDropLeftBackAD()<g_FALL_DIFFERENT_THRESHOLD)
    {
         
        tDropcheck.bBackLFall=true;
    }
    else
    {
        tDropcheck.bBackLFall=false;
    }
    
    if(usGetDropLeftFrontAD()<g_FALL_MIN&&usGetDropLeftFrontAD()<g_FALL_DIFFERENT_THRESHOLD)
    {
        tDropcheck.bFrontLFall=true;
    }
    else
    {
        tDropcheck.bFrontLFall=false; 
    }
    
    if(usGetDropRightBackAD()<g_FALL_MIN&&usGetDropRightBackAD()<g_FALL_DIFFERENT_THRESHOLD)
    {
        tDropcheck.bBackRFall=true;
    }
    else
    {
        tDropcheck.bBackRFall=false;
    }
    
    if(usGetDropRightFrontAD()<g_FALL_MIN&&usGetDropRightFrontAD()<g_FALL_DIFFERENT_THRESHOLD)
    {
        tDropcheck.bFrontRFall=true;
    }
    else
    {
        tDropcheck.bFrontRFall=false;
    }
    
    if(tDropcheck.bBackLFall|tDropcheck.bBackRFall|tDropcheck.bFrontLFall|tDropcheck.bFrontRFall)
    {
        tDropcheck.bAnyoneFall=true;
    }
    else
    {
       tDropcheck.bAnyoneFall=false;
    }
 
}

/*******************************************************************************
* Function Name  : vSetDropADAverage
* Description    : 地感AD值设置
                   每20ms设置一次
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void vSetDropADAverage(void)
{ 
    
    static uint16_t DROP_CLIFF=2050;     //悬崖值
    static uint16_t DROP_CLIFF_CHECK_MAX=3800;     //悬崖值最大感应值
    
    int j,i; 
    static uint16_t ADFL[10]={0},ADFR[10]={0},ADBL[10]={0},ADBR[10]={0};
    static uint16_t ADFLTemp[10]={0},ADFRTemp[10]={0},ADBLTemp[10]={0},ADBRTemp[10]={0};
    static uint8_t ADFLlen=0,ADFRlen=0,ADBLlen=0,ADBRlen=0;


      //获取原始ad值
    for(j=0;j<10;j++)
    {
        vTaskDelay(2);
        if(BackLFallAD<DROP_CLIFF)
            ADBL[j]= DROP_CLIFF-BackLFallAD;
        else if(BackLFallAD<DROP_CLIFF_CHECK_MAX)
            ADBL[j]=BackLFallAD-DROP_CLIFF;
        else 
            ADBL[j]=0;
        
        if(BackRFallAD<DROP_CLIFF)
            ADBR[j]=DROP_CLIFF-BackRFallAD;
        else if(BackRFallAD<DROP_CLIFF_CHECK_MAX)
            ADBR[j]=BackRFallAD-DROP_CLIFF;
        else 
            ADBR[j]=0;
        
        if(FrontLFallAD<DROP_CLIFF)
            ADFL[j]=DROP_CLIFF-FrontLFallAD;
        else if(FrontLFallAD<DROP_CLIFF_CHECK_MAX)
            ADFL[j]=FrontLFallAD-DROP_CLIFF;
        else 
            ADFL[j]=0;
        
        if(FrontRFallAD<DROP_CLIFF)
            ADFR[j]=DROP_CLIFF-FrontRFallAD;
        else if(FrontRFallAD<DROP_CLIFF_CHECK_MAX)
            ADFR[j]=FrontRFallAD-DROP_CLIFF;
        else
            ADFR[j]=0;
    }
    
   


    
    for(i=0;i<10;i++)
    {
        ADFLTemp[i]=0;
        ADFRTemp[i]=0;
        ADBLTemp[i]=0;
        ADBRTemp[i]=0;
    }
    //清除零值
    ADFLlen=DataTranstfer(ADFLTemp,ADFL,10);
    ADFRlen=DataTranstfer(ADFRTemp,ADFR,10);
    ADBLlen=DataTranstfer(ADBLTemp,ADBL,10);
    ADBRlen=DataTranstfer(ADBRTemp,ADBR,10);
      
    //取中位值
    g_usDROP_AD[adBL]=usGetQueueMiddleValue(ADBLTemp,ADBLlen);
    g_usDROP_AD[adBR]=usGetQueueMiddleValue(ADBRTemp,ADBRlen);
    g_usDROP_AD[adFL]=usGetQueueMiddleValue(ADFLTemp,ADFLlen);
    g_usDROP_AD[adFR]=usGetQueueMiddleValue(ADFRTemp,ADFRlen);


//    printf("\r\n");
//    printf("%d %d %d %d\r\n",ADFLlen,ADFRlen,ADBLlen,ADBRlen);
//    vPrintfDropALl();
  
}
/*******************************************************************************
* Function Name  : vSetDropADDifferent
* Description    : 地感前后AD差值设置
                   每20ms设置一次
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vSetDropADDifferent(void)
{ 
    
    static uint16_t DROP_CLIFF_CHECK_MAX=3800;     //悬崖值最大感应值
    int j,i; 
    static uint16_t ADFL[10]={0},ADFR[10]={0},ADBL[10]={0},ADBR[10]={0};
    static uint16_t ADFLTemp[10]={0},ADFRTemp[10]={0},ADBLTemp[10]={0},ADBRTemp[10]={0};
    static uint8_t ADFLlen=0,ADFRlen=0,ADBLlen=0,ADBRlen=0;


    //获取原始ad值
    for(j=0;j<10;j++)
    {
        vTaskDelay(2);
        if(BackLFallAD<DROP_CLIFF_CHECK_MAX)
            ADBL[j]=BackLFallAD;
        else 
            ADBL[j]=0;
        
        if(BackRFallAD<DROP_CLIFF_CHECK_MAX)
           ADBR[j]=BackRFallAD;
        else 
           ADBR[j]=0;
        
        if(FrontLFallAD<DROP_CLIFF_CHECK_MAX)
           ADFL[j]=FrontLFallAD; 
        else 
            ADFL[j]=0;
        
        if(FrontRFallAD<DROP_CLIFF_CHECK_MAX)
            ADFR[j]=FrontRFallAD;
        else
            ADFR[j]=0;
    }
//    

    for(j=0;j<10-1;j++)
    {
        ADBR[j]=abs(ADBR[j]-ADBR[j+1]);
        ADBL[j]=abs(ADBL[j]-ADBL[j+1]);
        ADFL[j]=abs(ADFL[j]-ADFL[j+1]);
        ADFR[j]=abs(ADFR[j]-ADFR[j+1]);
       
    }  
    for(i=0;i<10;i++)
    {
        ADFLTemp[i]=0;
        ADFRTemp[i]=0;
        ADBLTemp[i]=0;
        ADBRTemp[i]=0;
    }
    //清除零值
    ADFLlen=DataTranstfer(ADFLTemp,ADFL,9);
    ADFRlen=DataTranstfer(ADFRTemp,ADFR,9);
    ADBLlen=DataTranstfer(ADBLTemp,ADBL,9);
    ADBRlen=DataTranstfer(ADBRTemp,ADBR,9);
      
    //取最大值
    g_usDROP_DIFFERENT_AD[adBL]=usGetQueueMAXValue(ADBLTemp,ADBLlen);
    g_usDROP_DIFFERENT_AD[adBR]=usGetQueueMAXValue(ADBRTemp,ADBRlen);
    g_usDROP_DIFFERENT_AD[adFL]=usGetQueueMAXValue(ADFLTemp,ADFLlen);
    g_usDROP_DIFFERENT_AD[adFR]=usGetQueueMAXValue(ADFRTemp,ADFRlen);


//    printf("\r\n");
//    printf("%d %d %d %d\r\n",ADFLlen,ADFRlen,ADBLlen,ADBRlen);
//    vPrintfDropALl();
  
}
/*******************************************************************************
* Function Name  : vSetDropAdaption
* Description    : 悬空自适应  消耗时间长，勿轻易调用
                   Cycle范围： 0-499       悬空默认值为2050
* Input          : DropMin 与标定值最小要求差距  Offset AD值跳动误差范围
* Output         : None
* Return         : None
*******************************************************************************/
void vSetDropAdaption(uint16_t DropMin,uint16_t Offset)
{
    uint16_t usOFF_MAX=200;   //AD跳动范围
    uint16_t usDROP_MIN=10;    //与默认参考值的差值
    uint16_t usAverage_Offset=200;//各个AD值在同一平面的差值
    uint16_t iPeriod,iPulse;
    uint16_t iOldPeriod,iOldPrescaler,iOldPulse;
    int ADFL[5]={0},ADFR[5]={0},ADBL[5]={0},ADBR[5]={0},iaverage[5]={0};
    int i,j;
    int isum=0; 
    
    iOldPrescaler=TIM4->ARR;  //预装载值
    iOldPeriod=TIM4->PSC;       //预分频值
    iOldPulse=TIM_GetCapture4(TIM4);
    usOFF_MAX=Offset;
    usDROP_MIN=DropMin;

    printf("drop测试开始");
//    TIM4->ARR=500;
    for(iPeriod=30;iPeriod<100;iPeriod+=5)
    {
       TIM_PrescalerConfig(TIM4,iPeriod,TIM_PSCReloadMode_Immediate);//立即改变预分频系数
        
       for(iPulse=50;iPulse<200;iPulse+=5)
        {      
            TIM_SetCompare4(TIM4,iPulse); //ccr4
                 
            //获取数据
              for(j=1;j<5;j++)
            {
                vSetDropADAverage();
                ADBL[j]=usGetDropLeftBackAD();
                ADBR[j]=usGetDropRightBackAD();
                ADFL[j]=usGetDropLeftFrontAD();
                ADFR[j]=usGetDropRightFrontAD();
            }
            //与悬崖值的最小差值
            isum=1;
            for(j=1;j<5;j++)
            {
                ADBR[0]=ADBR[j]-2050;
                ADBL[0]=ADBL[j]-2050;
                ADFL[0]=ADFL[j]-2050;
                ADFR[0]=ADFR[j]-2050;
                if(abs(ADFR[0])<usDROP_MIN||abs(ADFL[0])<usDROP_MIN||abs(ADBL[0])<usDROP_MIN||abs(ADBR[0])<usDROP_MIN)
                {   isum=9;
                    break;
                }
            }
            //数据跳动范围
            for(j=2;j<5;j++)
            {
                ADBR[0]=ADBR[j]-ADBR[j-1];
                ADBL[0]=ADBL[j]-ADBL[j-1];
                ADFL[0]=ADFL[j]-ADFL[j-1];
                ADFR[0]=ADFR[j]-ADFR[j-1];
                if(abs(ADFR[0])>usOFF_MAX||abs(ADFL[0])>usOFF_MAX||abs(ADBL[0])>usOFF_MAX||abs(ADBR[0])>usOFF_MAX)
                {
                    isum=9;
                    break;
                }
            }    
           
            //四个AD值差异
            j=5;
            for(i=0;i<5;i++)//清空数组
            {
                iaverage[i]=0;
            }
            while(j>1)
            {
                j--;
                iaverage[1]+=ADBR[j];
                iaverage[2]+=ADBL[j];
                iaverage[3]+=ADFL[j];
                iaverage[4]+=ADFR[j];
                 if(1==j)
                {
                    iaverage[1]=iaverage[1]/4;
                    iaverage[2]=iaverage[2]/4;
                    iaverage[3]=iaverage[3]/4;
                    iaverage[4]/=4;
                }
                
            }
            for(j=2;j<5;j++)
            {
                for(i=1;i<j;i++)
                {
                   if(abs(iaverage[j]-iaverage[i])>usAverage_Offset)
                   isum=9;
                }
                
            }
            
            //找到合适的值，设置定时器
             if(isum!=9)
            {
//                TIM4->ARR=iPrescaler;
                TIM4->PSC=iPeriod;
                TIM4->CCR4=iPulse;
                return;
       
            }
           
        }
      
    }
    //自适应失败
    if(9==isum)
    {
        TIM4->ARR=iOldPrescaler;
        TIM4->PSC=iOldPeriod;
        TIM4->CCR4=iOldPulse;
    }
    printf("\r\n");
    printf("测试结束\r\n");

    
}
    

/*******************************************************************************
* Function Name  : usGetMiddleValue
* Description    : 获取一维数组大小中间值的数 清掉大的一半
* Input          : *pusBuffer：数组首地址，ucLength：数据长度
* Output         : None
* Return         : usMiddle：中间值
*******************************************************************************/
uint16_t usGetQueueMiddleValue( uint16_t *pusBuffer, uint8_t ucLength )
{
    uint8_t i;
    uint16_t usMiddle,usMax,usCount = 0,usPoint;
    
    while( usCount <= (ucLength/2) )
    {
        usPoint = 0;
        usMax = pusBuffer[0];
        for( i=1;i<ucLength;i++ ) 
        {      
            if( pusBuffer[i] > usMax ) 
            {
                usMax = pusBuffer[i];
                usPoint = i;
            }                
            
        }
        usMiddle = usMax;
        pusBuffer[usPoint] = 0;   
        usCount++;       
    }
    
    return usMiddle;
}

/*******************************************************************************
* Function Name  : usGetQueueMAXValue
* Description    : 获取一维数组最大值
* Input          : *pusBuffer：数组首地址，ucLength：数据长度
* Output         : None
* Return         : usMiddle：中间值
*******************************************************************************/
uint16_t usGetQueueMAXValue( uint16_t *pusBuffer, uint8_t ucLength )
{
    uint8_t i;
    uint16_t usMax;
    
    usMax = pusBuffer[0];
    for( i=1;i<ucLength;i++ ) 
    {      
        if( pusBuffer[i] > usMax ) 
        {
            usMax = pusBuffer[i];
           
        }                
        
    } 
    return usMax;
}
/*******************************************************************************
* Function Name  : usGetMiddleValue
* Description    : 去除数组里的零值
* Input          : *pusBuffer：数组首地址，ucLength：数据长度
* Output         : None
* Return         : usMiddle：中间值
*******************************************************************************/
uint8_t DataTranstfer(uint16_t *pusTarget, uint16_t *pusSource, uint8_t ucLength)
{
    uint8_t i=0,num=0;
    
    while(i<ucLength)
    {
        if(pusSource[i]>0)
        {
            pusTarget[num]=pusSource[i];
            num++;
            i++;
        }
        else
            i++;       
    }
     return num;
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


