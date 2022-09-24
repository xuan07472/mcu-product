#include "adc.h"

/******************************************************************************
* 文件名  ：adc.c
* 文件描述：xiaomi主板ADC输出IO配置
* 创建人  ：Pu Sheng
* 日    期：2019年11月8日
* 
* 硬件连接：
            PA0 - 4选1通道检测      adc1_ch0
            PA2 - 电池温度检测      adc1_ch2  1700~2000
            PA3 - 边刷电流检测      adc1_ch3
            PA4 - 防跌落右前        adc1_ch4
            PA5 - 放电电流IBAT      adc1_ch5
            PA6 - BAT电池电压       adc1_ch6   
            PA7 - 防跌右后          adc1_ch7
            
            PC0 - 风机电流检测      adc1_ch10  要检测adc电压 需要开启VCC+电源 见iooutput.c
            PC1 - 左驱动轮电流检测  adc1_ch11
            PC2 - 右驱动轮电流检测  adc1_ch12
            PC3 - 滚刷电流检测      adc1_ch13  
            PC4 - 防跌左后          adc1_ch14
            PC5 - 防跌左前          adc1_ch15 
            
            模拟通道芯片
            1：沿边传感器  0-2000
            2：电池ID      3790-3820
            3：充电电流    0-1000
            4：充电座电压  2900-3200
            
******************************************************************************/
	   
u16 g_AdcOriginalValue[ADC_TRANSFORM_NUM][ADC_CHANNEL_NUM];//adc原始数据

/*******************************************************************************
* Function Name  : vAllAdcPinInit
* Description    : 主板所有ADC IO  驱动初始化
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
    
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	//使能PC端口时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//使能PA端口时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);	//使能ADC1时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA传输
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 |GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN; 	
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_2|GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    /********************************** ADC1 Init *******************************/
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);
    
    ADC_DeInit(ADC1);
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;	//ADC工作模式:ADC1和ADC2工作在独立模式
	ADC_InitStructure.ADC_ScanConvMode = ENABLE;	//模数转换工作在单通道模式
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;	//模数转换工作在单次转换模式
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;	//转换由软件而不是外部触发启动
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	//ADC数据右对齐
	ADC_InitStructure.ADC_NbrOfChannel = ADC_CHANNEL_NUM;	//顺序进行规则转换的ADC通道的数目
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
    
    ADC_DMACmd(ADC1, ENABLE);       //使能ADC1的DMA传输 
    ADC_Cmd(ADC1, ENABLE);	//使能指定的ADC1
	
	ADC_ResetCalibration(ADC1);	//使能复位校准  	 
	while(ADC_GetResetCalibrationStatus(ADC1));	//等待复位校准结束	
	ADC_StartCalibration(ADC1);	 //开启AD校准
	while(ADC_GetCalibrationStatus(ADC1));	 //等待校准结束
    
    /********************************** DMA Init *******************************/
    
    DMA_DeInit(DMA1_Channel1);

	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)( &ADC1->DR );  //DMA外设基地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&g_AdcOriginalValue;  //DMA内存基地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //数据传输方向，从外设ADC发送到内存读取
	DMA_InitStructure.DMA_BufferSize = ADC_CHANNEL_NUM * ADC_TRANSFORM_NUM;  //DMA通道的DMA缓存的大小
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址寄存器不变
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址寄存器递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;  //数据宽度为16位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord; //数据宽度为16位
	DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;  //循环工作模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_High; //DMA通道 x拥有高优先级 
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输
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
    
    if( DMA_GetITStatus( DMA1_IT_TC1 )== SET )//DMA传输完成 
    {
        DMA_ClearITPendingBit( DMA1_IT_TC1);
        ADC_Cmd( ADC1, DISABLE );
        xTaskNotifyFromISR( HandleADCDataProcessing , 1 , eIncrement , &xHigherPriorityTaskWoken );//发送消息给adc处理任务
    }        
      
}



