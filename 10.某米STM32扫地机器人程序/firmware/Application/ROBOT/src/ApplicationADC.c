#include "applicationadc.h"

/*
 * Filename   ��applicationadc.c
 * Author     : pusheng       
 * Version    : 1.0
 * Date       : 2019.11.12
 * Discription : ADC���ݴ���Ӧ�ú���
*/	

TaskHandle_t HandleADCDataProcessing = NULL;//adc���ݴ���������

uint16_t g_usInChipAdcChannelAverageValue[ADC_CHANNEL_NUM]; //ADC ��ͨ��ƽ��ֵ   
uint16_t g_usAnalogSwitch[adcAnalog_Switch_Num];            //ģ�⿪��ADC����ֵ

/*******************************************************************************
* Function Name  : vADCDataProcessingTask
* Description    : ADC���ݴ�������
* Input          : p �������
* Output         : None
* Return         : None
*******************************************************************************/
void vADCDataProcessingTask(void *p)
{
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);	  //ʹ��ָ����ADC1�����ת����������
    while(1)
    {
        if( ulTaskNotifyTake( pdFALSE , portMAX_DELAY) )//�����ȴ�adcת�����
        {
            vGetADCChannelAverageNumber( g_usInChipAdcChannelAverageValue, g_usAnalogSwitch );//���ݴ���
            ADC_Cmd( ADC1, ENABLE );
            ADC_SoftwareStartConvCmd(ADC1, ENABLE);	  //ʹ��ָ����ADC1�����ת����������
        }
    }
}


/*******************************************************************************
* Function Name  : vGetADCChannelAverageNumber
* Description    : ��ȡADCÿ��ͨ���ɼ�ֵ
* Input          : None
* Output         : *pucBuffer������װ��ADCÿ��ͨ��ƽ��ֵ����,pusSwitch��ģ�⿪��ADCֵ
* Return         : None
*******************************************************************************/
void vGetADCChannelAverageNumber( uint16_t *pusBuffer, uint16_t *pusSwitch ) 
{
    #define SELECT_ALONG_EDGE_ADC       SGM4878_A=0;SGM4878_B=0; ucAnalogPoint=adcAlong_Edge;
    #define SELECT_BATTERY_ID_ADC       SGM4878_A=1;SGM4878_B=0; ucAnalogPoint=adcBatteryID;
    #define SELECT_CHARGE_CURRENT_ADC   SGM4878_A=0;SGM4878_B=1; ucAnalogPoint=adcChargeCurrent;
    #define SELECT_RECHARGES_STAND_ADC  SGM4878_A=1;SGM4878_B=1; ucAnalogPoint=adcRechargeStand;

    static uint8_t ucAnalogPoint = 0;   //usAnalogSwitch[]�±�
    int8_t i,j,k;
    uint32_t uiSum[ADC_CHANNEL_NUM] = {0};
    uint16_t AxtractData[ADC_CHANNEL_NUM][ADC_TRANSFORM_NUM] = {0};
    uint16_t usTemp;
    
    
    /*****************ȡ��ÿ��ADCͨ����ֵ ***************/
    for(i=0;i<ADC_TRANSFORM_NUM;i++)        //
    {
        for(j=0;j<ADC_CHANNEL_NUM;j++)  //
        {
            AxtractData[j][i] = g_AdcOriginalValue[i][j];
        }        
    }
    
    /***************************����****************************/
    for( k=0;k<ADC_CHANNEL_NUM;k++ )
    {
        for( i=1;i<ADC_TRANSFORM_NUM;i++ ) 
        { 
            usTemp=AxtractData[k][i];                         //usTempΪ�����Ԫ��
            j=i-1; 
            while( ( j>=0 ) && ( usTemp<AxtractData[k][j] ) ) //����array[i-1] �����С����
            { 
                AxtractData[k][j+1]=AxtractData[k][j]; 
                j--; 
            } 
            AxtractData[k][j+1]=usTemp;                       //��������
        } 
    }
      
    /******************����ÿ��ADCͨ��ƽ��ֵ******************/
    for( i=0;i<ADC_CHANNEL_NUM;i++ )        
    {
        for( j=1;j<ADC_TRANSFORM_NUM-1;j++ )//ȥ��һ�����ֵ����Сֵ
        {
            uiSum[i] += AxtractData[i][j];
        }         
        pusBuffer[i] = uiSum[i]/(ADC_TRANSFORM_NUM-2);
        
    }

    /**************ȡ��ģ�⿪�ض�Ӧͨ����ADCֵ***************/
    switch( ucAnalogPoint )
    {
        case adcAlong_Edge :
            pusSwitch[adcAlong_Edge] = pusBuffer[adcAnalogChannel];
            SELECT_BATTERY_ID_ADC;       //ѡ����һ��ͨ��                
            break;                                    
        case adcBatteryID :
            pusSwitch[adcBatteryID] = pusBuffer[adcAnalogChannel];
            SELECT_CHARGE_CURRENT_ADC; //ѡ����һ��ͨ��
           break;            
        case adcChargeCurrent :
            pusSwitch[adcChargeCurrent] = pusBuffer[adcAnalogChannel];
            SELECT_RECHARGES_STAND_ADC; //ѡ����һ��ͨ��               
            break;            
        case adcRechargeStand :
            pusSwitch[adcRechargeStand] = pusBuffer[adcAnalogChannel];
            SELECT_ALONG_EDGE_ADC; //ѡ����һ��ͨ��               
            break; 
        default: break;
    }           
    
}


