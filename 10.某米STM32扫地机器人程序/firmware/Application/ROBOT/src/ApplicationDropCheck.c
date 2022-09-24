#include "ApplicationDropCheck.h"

/**
 * @Filename   ��ApplicationDropCheck.c
 * @Author     : HuangWentao LiuHongChuan    
 * @Version    : 1.0
 * @Date       : 2019/12/10
 * @Discription : �ĸ������䴫����
 * @hardware connect:
 *          --------------TIM4-CH4-----------------------
 */

#ifndef DROPHONGCHUAN
#define FALLING_THRESHOLD 15 //������ֵ
DropCheckData_t tDropcheck={0};//�ĸ����䴫������־λ
//
uint16_t g_usDROP_AD[4]={0};  //�ظ�ADֵ 

void GetDropAdValue(void);

//����������
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
  * @Description �����ĸ����䴫����ADֵ�������õ����жϱ�־λ
  * @param   ��
  * @retval  ��
  */
void GetDropAdValue(void)
{
    
    int i = 0;
    int Drop_AD_Value[4] = {0};//���ڴ���ĸ���������ADֵ
    
    for(i = 0; i<10; i++)//ȡ10�ε�ƽ��ֵ
    {
        
        vTaskDelay(1);
        Drop_AD_Value[adFL]+=abs(LF_DropADValue-2048)/10;//��ȫ����ADֵΪ2048 ���Խ��Խ���������ߣ���LF_DropADValue-2048��ȡ����ֵ�õ�0-2048��Χ��ADֵ����ֵԽ�����Խ����
        Drop_AD_Value[adFR]+=abs(RF_DropADValue-2048)/10;
        Drop_AD_Value[adBL]+=abs(LB_DropADValue-2048)/10;
        Drop_AD_Value[adBR]+=abs(RB_DropADValue-2048)/10;
    }
    
    tDropcheck.bFrontLFall = Drop_AD_Value[adFL]>FALLING_THRESHOLD?0:1;//��ǰ������ �ж��Ƿ���ڵ�����ֵ��С����ֵ���Ϊ1����ʾ������
    tDropcheck.bFrontRFall = Drop_AD_Value[adFR]>FALLING_THRESHOLD?0:1;//��ǰ������
    tDropcheck.bBackLFall = Drop_AD_Value[adBL]>FALLING_THRESHOLD?0:1;//��󴫸���
    tDropcheck.bBackRFall = Drop_AD_Value[adBR]>FALLING_THRESHOLD?0:1;//�Һ󴫸���
    tDropcheck.bAnyoneFall = (tDropcheck.bFrontLFall | tDropcheck.bFrontRFall | tDropcheck.bBackLFall | tDropcheck.bBackRFall);//����һ����Ӧ������ʱΪ1

    //ȫ�ֱ�������
    usSetDropRightFrontAD(  Drop_AD_Value[adFR] ) ;      
    usSetDropRightBackAD(  Drop_AD_Value[adBR] ) ;      
    usSetDropLeftFrontAD(  Drop_AD_Value[adFL] ) ;     
    usSetDropLeftBackAD(  Drop_AD_Value[adBL] ) ;
    
    memset(&Drop_AD_Value[0],0,4*sizeof(Drop_AD_Value[0]));
     
}
#endif

#ifdef  DROPHONGCHUAN
DropCheckData_t tDropcheck={0};//�ĸ����䴫������־λ
uint16_t g_usDROP_AD[4]={0};  //�ظ�ADֵ 
uint16_t g_usDROP_DIFFERENT_AD[4]={0};  //�ظ�ǰ���ֵ

//����������
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
* Description    : ���ձ�־λ����
                    ���ڣ�10ms����һ��
                   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//void vFallFlagCheck(void)
//{


//    //������ؾ�����ǰ��ad��ֵ�ж�����
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
* Description    : �ظ�ADֵ���� ���ٲ���
                   
* Input          : None
* Output         : ÿ10ms���һ��ADֵ
* Return         : None
*******************************************************************************/

void vSetDropADAverage(void)
{ 
    #define DROP_CLIFF_AD 2050   //����ֵ ��⵽����ʱ��ԭʼAD
    int nNum; 
    static uint16_t DropAD[4][8]={0};      //�ظд�����adֵ
    static uint16_t DropDifferentAD[4][8]={0};      //��ŵظ�ǰ�����ζ�ȡadֵ��ֵ

    //��ȡadԭʼֵ������ֵ�Ĳ�ֵ
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
    
    //��ȡ�м�ֵ
    g_usDROP_AD[adBL] = usGetDropMiddleAD(DropAD[adBL], 8, 0, 2100);
    g_usDROP_AD[adBR] = usGetDropMiddleAD(DropAD[adBR], 8, 0, 2100);
    g_usDROP_AD[adFL] = usGetDropMiddleAD(DropAD[adFL], 8, 0, 2100);
    g_usDROP_AD[adFR] = usGetDropMiddleAD(DropAD[adFR], 8, 0, 2100);
        

    
    //ȡǰ���ֵ
    g_usDROP_DIFFERENT_AD[adBL] = usGetDropDifferentAD(DropDifferentAD[adBL], 8, 0, 2100);
    g_usDROP_DIFFERENT_AD[adBR] = usGetDropDifferentAD(DropDifferentAD[adBR], 8, 0, 2100);
    g_usDROP_DIFFERENT_AD[adFL] = usGetDropDifferentAD(DropDifferentAD[adFL], 8, 0, 2100);
    g_usDROP_DIFFERENT_AD[adFR] = usGetDropDifferentAD(DropDifferentAD[adFR], 8, 0, 2100);
    
    
  
}
/*******************************************************************************
* Function Name  : usGetDropMiddleAD
* Description    : ��ȡ�ظ������м�ֵ����  
* Input          :pusSource ��Դ���飬ucLength ���鳤�ȣ� LimitMAX�� ���ݴ�С��Ч��Χ���� �� 
                    LimitMIN ���ݴ�С��Ч��Χ����
* Output         : None
* Return         : usMiddle���м�ֵ�����ֵ�Ĳ�ֵ ������ֵ��ռ�ձȸı�,1khz 30%ʱΪ1050��
*******************************************************************************/
uint16_t usGetDropMiddleAD(uint16_t *pusSource, uint8_t ucLength, uint16_t LimitMIN,uint16_t LimitMAX)
{
    uint8_t i=0, ucLen=0; //ucLen ��Ч��ֵ������
    uint16_t usMin,usCount = 0,usPoint;
    static uint16_t uslastMiddle, usMiddle=0,IsFirst=1;
    
    
    //���������Ĵ���ֵ
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
    //��Ч���ȼ�� 
    if(ucLen<1)
    {
        //���ش����־
        return 5000;
    }
    
    
//    for(i=0;i<ucLen;i++)
//    {
//        usMiddle+=pusSource[i];
//    }
//    usMiddle/=10;
    
    //ȡ��ֵ�� ƽ�����ڲ��ε��°벿��
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
* Description    : ��ȡ�ظ�����ǰ�����ζ�ȡ�Ĳ�ֵ
* Input          :pusSource �ظ�ad���飬ucLength ���鳤�ȣ� LimitMAX�� ���ݴ�С��Ч��Χ���� �� 
                    LimitMIN ���ݴ�С��Ч��Χ����
* Output         : None
* Return         : usMax ǰ������ֵ
*******************************************************************************/
uint16_t usGetDropDifferentAD(uint16_t *pusSource, uint8_t ucLength, uint16_t LimitMIN,uint16_t LimitMAX)
{
    uint8_t i=0, ucLen=0; //ucLen ��Ч��ֵ������
    uint16_t usMax;
    
    //���������Ĵ���ֵ
    for(i=0; i<ucLength; i++)
    {
        if(pusSource[i] >LimitMIN && pusSource[i] < LimitMAX)
        {
            pusSource[ucLen] = pusSource[i];
            ucLen++;
        }
            
    }
    //��Ч���ȼ�� ���󷵻�5000
    if(ucLen<2)
    {
        return 5000;
    }
   
    //ȡǰ���ֵ
    for(i=0; i<ucLen-1; i++)
    {
        pusSource[i] = abs(pusSource[i+1] - pusSource[i]);  
    }
    ucLen -= 1;
    //ȡ���ֵ
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

#endif


