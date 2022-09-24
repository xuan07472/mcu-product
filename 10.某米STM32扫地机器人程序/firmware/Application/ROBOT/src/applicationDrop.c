#include "applicationDrop.h"

/******************************************************************************
* �ļ���  ��applicationDrop.c
* �ļ����������ռ������
* ������  ��liu hongchuan
* ��    �ڣ�2019��11��21��

        
******************************************************************************/
	   

/*drop*/
uint16_t g_usDROP_AD[4]={0};  //�ظ�ADֵ ƽ������Ϊ500��800�� ��̺Ϊ100-300
uint16_t g_usDROP_DIFFERENT_AD[4]={0};  //�ظ�ǰ���ֵ
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
* Description    : ���ձ�־λ����
                    ���ڣ�40ms
                   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vFallFlagCheck(void)
{


    //���ݲ�ֵ�ж�����
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
* Description    : �ظ�ADֵ����
                   ÿ20ms����һ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void vSetDropADAverage(void)
{ 
    
    static uint16_t DROP_CLIFF=2050;     //����ֵ
    static uint16_t DROP_CLIFF_CHECK_MAX=3800;     //����ֵ����Ӧֵ
    
    int j,i; 
    static uint16_t ADFL[10]={0},ADFR[10]={0},ADBL[10]={0},ADBR[10]={0};
    static uint16_t ADFLTemp[10]={0},ADFRTemp[10]={0},ADBLTemp[10]={0},ADBRTemp[10]={0};
    static uint8_t ADFLlen=0,ADFRlen=0,ADBLlen=0,ADBRlen=0;


      //��ȡԭʼadֵ
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
    //�����ֵ
    ADFLlen=DataTranstfer(ADFLTemp,ADFL,10);
    ADFRlen=DataTranstfer(ADFRTemp,ADFR,10);
    ADBLlen=DataTranstfer(ADBLTemp,ADBL,10);
    ADBRlen=DataTranstfer(ADBRTemp,ADBR,10);
      
    //ȡ��λֵ
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
* Description    : �ظ�ǰ��AD��ֵ����
                   ÿ20ms����һ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vSetDropADDifferent(void)
{ 
    
    static uint16_t DROP_CLIFF_CHECK_MAX=3800;     //����ֵ����Ӧֵ
    int j,i; 
    static uint16_t ADFL[10]={0},ADFR[10]={0},ADBL[10]={0},ADBR[10]={0};
    static uint16_t ADFLTemp[10]={0},ADFRTemp[10]={0},ADBLTemp[10]={0},ADBRTemp[10]={0};
    static uint8_t ADFLlen=0,ADFRlen=0,ADBLlen=0,ADBRlen=0;


    //��ȡԭʼadֵ
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
    //�����ֵ
    ADFLlen=DataTranstfer(ADFLTemp,ADFL,9);
    ADFRlen=DataTranstfer(ADFRTemp,ADFR,9);
    ADBLlen=DataTranstfer(ADBLTemp,ADBL,9);
    ADBRlen=DataTranstfer(ADBRTemp,ADBR,9);
      
    //ȡ���ֵ
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
* Description    : ��������Ӧ  ����ʱ�䳤�������׵���
                   Cycle��Χ�� 0-499       ����Ĭ��ֵΪ2050
* Input          : DropMin ��궨ֵ��СҪ����  Offset ADֵ������Χ
* Output         : None
* Return         : None
*******************************************************************************/
void vSetDropAdaption(uint16_t DropMin,uint16_t Offset)
{
    uint16_t usOFF_MAX=200;   //AD������Χ
    uint16_t usDROP_MIN=10;    //��Ĭ�ϲο�ֵ�Ĳ�ֵ
    uint16_t usAverage_Offset=200;//����ADֵ��ͬһƽ��Ĳ�ֵ
    uint16_t iPeriod,iPulse;
    uint16_t iOldPeriod,iOldPrescaler,iOldPulse;
    int ADFL[5]={0},ADFR[5]={0},ADBL[5]={0},ADBR[5]={0},iaverage[5]={0};
    int i,j;
    int isum=0; 
    
    iOldPrescaler=TIM4->ARR;  //Ԥװ��ֵ
    iOldPeriod=TIM4->PSC;       //Ԥ��Ƶֵ
    iOldPulse=TIM_GetCapture4(TIM4);
    usOFF_MAX=Offset;
    usDROP_MIN=DropMin;

    printf("drop���Կ�ʼ");
//    TIM4->ARR=500;
    for(iPeriod=30;iPeriod<100;iPeriod+=5)
    {
       TIM_PrescalerConfig(TIM4,iPeriod,TIM_PSCReloadMode_Immediate);//�����ı�Ԥ��Ƶϵ��
        
       for(iPulse=50;iPulse<200;iPulse+=5)
        {      
            TIM_SetCompare4(TIM4,iPulse); //ccr4
                 
            //��ȡ����
              for(j=1;j<5;j++)
            {
                vSetDropADAverage();
                ADBL[j]=usGetDropLeftBackAD();
                ADBR[j]=usGetDropRightBackAD();
                ADFL[j]=usGetDropLeftFrontAD();
                ADFR[j]=usGetDropRightFrontAD();
            }
            //������ֵ����С��ֵ
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
            //����������Χ
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
           
            //�ĸ�ADֵ����
            j=5;
            for(i=0;i<5;i++)//�������
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
            
            //�ҵ����ʵ�ֵ�����ö�ʱ��
             if(isum!=9)
            {
//                TIM4->ARR=iPrescaler;
                TIM4->PSC=iPeriod;
                TIM4->CCR4=iPulse;
                return;
       
            }
           
        }
      
    }
    //����Ӧʧ��
    if(9==isum)
    {
        TIM4->ARR=iOldPrescaler;
        TIM4->PSC=iOldPeriod;
        TIM4->CCR4=iOldPulse;
    }
    printf("\r\n");
    printf("���Խ���\r\n");

    
}
    

/*******************************************************************************
* Function Name  : usGetMiddleValue
* Description    : ��ȡһά�����С�м�ֵ���� ������һ��
* Input          : *pusBuffer�������׵�ַ��ucLength�����ݳ���
* Output         : None
* Return         : usMiddle���м�ֵ
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
* Description    : ��ȡһά�������ֵ
* Input          : *pusBuffer�������׵�ַ��ucLength�����ݳ���
* Output         : None
* Return         : usMiddle���м�ֵ
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
* Description    : ȥ�����������ֵ
* Input          : *pusBuffer�������׵�ַ��ucLength�����ݳ���
* Output         : None
* Return         : usMiddle���м�ֵ
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
* Description    : ���drop�������
* Input          : 
* Output         : None
* Return         :none
*******************************************************************************/
void vPrintfDropALl(void)
{
    printf("Ƶ�ʣ�%d   ռ�ձȣ�%d \r\n",144000/(TIM4->PSC),(TIM4->CCR4)/5);
    printf("Ԥ��Ƶ�� %d\r\n",TIM4->PSC);
    printf("Pulse: %d\r\n",TIM_GetCapture4(TIM4));
    printf("��ǰ : %d \r\n",usGetDropLeftFrontAD());
    printf("��� : %d   \r\n",usGetDropLeftBackAD());
    printf("��ǰ : %d  \r\n",usGetDropRightFrontAD());
    printf("�Һ� : %d \r\n",usGetDropRightBackAD());
    
    printf("��ǰ : %d \r\n", g_usDROP_DIFFERENT_AD[adFL]);
    printf("��� : %d   \r\n", g_usDROP_DIFFERENT_AD[adBL]);
    printf("��ǰ : %d  \r\n", g_usDROP_DIFFERENT_AD[adFR]);
    printf("�Һ� : %d \r\n", g_usDROP_DIFFERENT_AD[adBR]);
}


