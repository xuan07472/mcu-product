#include "adc.h"

/******************************************************************************
* �ļ���  ��adc.c
* �ļ�������xiaomi����ADC���IO����
* ������  ��Pu Sheng
* ��    �ڣ�2019��11��8��
* 
* Ӳ�����ӣ�
            PA0 - 4ѡ1ͨ�����      adc1_ch0
            PA2 - ����¶ȼ��      adc1_ch2  1700~2000
            PA3 - ��ˢ�������      adc1_ch3
            PA4 - ��������ǰ        adc1_ch4
            PA5 - �ŵ����IBAT      adc1_ch5
            PA6 - BAT��ص�ѹ       adc1_ch6   
            PA7 - �����Һ�          adc1_ch7
            
            PC0 - ����������      adc1_ch10  Ҫ���adc��ѹ ��Ҫ����VCC+��Դ ��iooutput.c
            PC1 - �������ֵ������  adc1_ch11
            PC2 - �������ֵ������  adc1_ch12
            PC3 - ��ˢ�������      adc1_ch13  
            PC4 - �������          adc1_ch14
            PC5 - ������ǰ          adc1_ch15 
            
            ģ��ͨ��оƬ
            1���رߴ�����  0-2000
            2�����ID      3790-3820
            3��������    0-1000
            4���������ѹ  2900-3200
            
******************************************************************************/
	   
u16 g_AdcOriginalValue[ADC_TRANSFORM_NUM][ADC_CHANNEL_NUM];//adcԭʼ����

/*******************************************************************************
* Function Name  : vAllAdcPinInit
* Description    : ��������ADC IO  ������ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vAllAdcPinInit(void)
{
    GPIO_InitTypeDef    GPIO_InitStructure;
    ADC_InitTypeDef     ADC_InitStructure;
    DMA_InitTypeDef     DMA_InitStructure;
    NVIC_InitTypeDef    NVIC_InitStructure;
    
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//ʹ��PC�˿�ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��PA�˿�ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);	//ʹ��ADC1ʱ��
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//ʹ��DMA����
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 |GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 	
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    /********************************** ADC1 Init *******************************/
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);
    
    ADC_DeInit(ADC1);
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC����ģʽ:ADC1��ADC2�����ڶ���ģʽ
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;	//ģ��ת�������ڵ�ͨ��ģʽ
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	//ģ��ת�������ڵ���ת��ģʽ
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//ת��������������ⲿ��������
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC�����Ҷ���
	ADC_InitStructure.ADC_NbrOfChannel = ADC_CHANNEL_NUM;	//˳����й���ת����ADCͨ������Ŀ
	ADC_Init(ADC1, &ADC_InitStructure);	 
    
    ADC_RegularChannelConfig(ADC1, ADC_Channel_0,  1, ADC_SampleTime_239Cycles5 );
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2,  2, ADC_SampleTime_239Cycles5 );
    ADC_RegularChannelConfig(ADC1, ADC_Channel_3,  3, ADC_SampleTime_239Cycles5 ); 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_4,  4, ADC_SampleTime_239Cycles5 );
    ADC_RegularChannelConfig(ADC1, ADC_Channel_5,  5, ADC_SampleTime_239Cycles5 ); 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_6,  6, ADC_SampleTime_239Cycles5 );    
    ADC_RegularChannelConfig(ADC1, ADC_Channel_7,  7, ADC_SampleTime_239Cycles5 ); 
    ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 8, ADC_SampleTime_239Cycles5 );
    ADC_RegularChannelConfig(ADC1, ADC_Channel_11, 9, ADC_SampleTime_239Cycles5 );
    ADC_RegularChannelConfig(ADC1, ADC_Channel_12, 10, ADC_SampleTime_239Cycles5 );
    ADC_RegularChannelConfig(ADC1, ADC_Channel_13, 11, ADC_SampleTime_239Cycles5 );
    ADC_RegularChannelConfig(ADC1, ADC_Channel_14, 12, ADC_SampleTime_239Cycles5 );
    ADC_RegularChannelConfig(ADC1, ADC_Channel_15, 13, ADC_SampleTime_239Cycles5 );
    
    ADC_DMACmd(ADC1, ENABLE);       //ʹ��ADC1��DMA���� 
    ADC_Cmd(ADC1, ENABLE);	//ʹ��ָ����ADC1
	
	ADC_ResetCalibration(ADC1);	//ʹ�ܸ�λУ׼  	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//�ȴ���λУ׼����	
	ADC_StartCalibration(ADC1);	 //����ADУ׼
	while(ADC_GetCalibrationStatus(ADC1));	 //�ȴ�У׼����
    
    /********************************** DMA Init *******************************/
    
    DMA_DeInit(DMA1_Channel1);

	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)( &ADC1->DR );  //DMA�������ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&g_AdcOriginalValue;  //DMA�ڴ����ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //���ݴ��䷽�򣬴�����ADC���͵��ڴ��ȡ
	DMA_InitStructure.DMA_BufferSize = ADC_CHANNEL_NUM * ADC_TRANSFORM_NUM;  //DMAͨ����DMA����Ĵ�С
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //�����ַ�Ĵ�������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�ڴ��ַ�Ĵ�������
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  //���ݿ��Ϊ16λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //���ݿ��Ϊ16λ
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  //ѭ������ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMAͨ�� xӵ�и����ȼ� 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
	DMA_Init(DMA1_Channel1, &DMA_InitStructure);

    DMA_Cmd(DMA1_Channel1, ENABLE);
    
    DMA_ClearITPendingBit( DMA1_IT_TC1); 
    DMA_ITConfig( DMA1_Channel1, DMA_IT_TC,ENABLE );     
    NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn; 
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 13; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0; 
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
    NVIC_Init( &NVIC_InitStructure ); 
    
    
}

extern TaskHandle_t HandleADCDataProcessing;
void DMA1_Channel1_IRQHandler()
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    
    if( DMA_GetITStatus( DMA1_IT_TC1 )== SET )//DMA������� 
    {
        DMA_ClearITPendingBit( DMA1_IT_TC1);
        ADC_Cmd( ADC1, DISABLE );
        xTaskNotifyFromISR( HandleADCDataProcessing , 1 , eIncrement , &xHigherPriorityTaskWoken );//������Ϣ��adc��������
    }        
      
}



