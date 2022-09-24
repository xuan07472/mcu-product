#include "usart.h"	  
/**********************************************************************************
 * Filename   ：usart.c
 * Author     : pusheng       
 * Version    : 1.0
 * Date       : 2020.1.2
 * Discription : 串口驱动及其框架
 * 
 * USART2->TXD : PD5
 * USART2->RXD : PD6 
 * 
**********************************************************************************/	 


/****************************USART2收发相关变量***********************************/
uint8_t ucUSART2TrainsmitBuffer[USART2_TRANSMIT_GROOVE_SIZE][USART2_TRANSMIT_BUFFER_SIZE] = {0}; //USART2发送数据缓冲区
uint8_t ucUSART2TrainsmitLength[USART2_TRANSMIT_GROOVE_SIZE];    //USART2缓冲区长度  
uint8_t ucDMA1_Stream7TransmitGrooveIndex = 0;                   //DMA1_Stream7传输第几个缓冲
uint8_t ucUSART2_TransmitMessageNumber = 0;                      //USART2需要传输的消息数    
uint8_t ucWtiteDataToUSART2TransmitGrooveIndex = 0;              //发送USART2第几个缓冲                                               
uint8_t ucUSART2_TransmitWritingBytePointer = 0;                 //USART2发送数据缓冲区下标

uint8_t ucUSART2ReceiveBuffer[USART2_RECEIVE_GROOVE_SIZE][USART2_RECEIVE_BUFFER_SIZE] = {0};    //USART2接收数据缓冲区
uint8_t ucUSART2ReceiveBufferLength[USART2_RECEIVE_GROOVE_SIZE];                                //USART2接收数据缓冲区数据长度     
uint8_t ucUSART2ReceiveGrooveIndex = 0;                          //USART2接收第几个缓冲
uint8_t ucUSART2ReadBufferIndex = 0;                             //读取USART2第几个缓冲
uint8_t ucUSART2ReceiveWritingBytePointer = 0;                   //USART2接收缓冲下标   
uint8_t ucDMA1_Stream6ReceiveGrooveIndex = 0;                    //DMA1_Stream6接收第几个缓冲
uint8_t ucUSART2_ReceiveMessageNumber = 0;                       //USART2还未处理的消息数

/*******************************************************************************
* Function Name  : vUsart2DmaInitForTest(void)
* Description    : 串口2和对应IO及其接收发送DMA的初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vUart2_Init(void)
{
    vUart2PinFunction_Init(USART2_BOUND);
    vDma1_7_Usart2_TX_Init(ucUSART2TrainsmitBuffer[ucDMA1_Stream7TransmitGrooveIndex],USART2_TRANSMIT_BUFFER_SIZE);
    vDma1_6_Usart2_RX_Init(ucUSART2ReceiveBuffer[ucUSART2ReceiveGrooveIndex],USART2_RECEIVE_BUFFER_SIZE);
}

/*******************************************************************************
* Function Name  : vUsart2DataInteraction
* Description    : Usart2的信息处理
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vUsart2Interactive( void )
{  
    static USART_DMAState DMA1_Stream7Flag = USART_DMA_DONE;    
    
    /*检测是否接收完成*/
    if( USART_GetFlagStatus( USART2, USART_FLAG_IDLE ) == SET )                //USART2接收空闲
    {
        DMA_Cmd( DMA1_Channel6, DISABLE ); 
        DMA_ClearFlag( DMA1_FLAG_TC6 );                         //失能DMA数据流2
        /******先读SR寄存器再读DR寄存器清除USART_IT_IDLE标志*****/
        USART2->SR;
        USART2->DR;
        ucUSART2ReceiveBufferLength[ucDMA1_Stream6ReceiveGrooveIndex] = USART2_RECEIVE_BUFFER_SIZE - DMA_GetCurrDataCounter( DMA1_Channel6 );     
        DMA1_Stream6ReceiveDone(  );
        DMA1_Channel6->CMAR = (uint32_t)ucUSART2ReceiveBuffer[ucDMA1_Stream6ReceiveGrooveIndex];
        DMA1_Channel6->CNDTR = USART2_RECEIVE_BUFFER_SIZE;                        //设置传输长度，并让接收地址从0开始     
        DMA_Cmd( DMA1_Channel6, ENABLE );                                       //使能DMA数据流6                           
    }    
    
    /*检测是否有数据需要发送*/
    if( ucUSART2_TransmitMessageNumber ) 
    {
        /********************检测ＤＭＡ传输完成*****************/
        if( DMA_GetFlagStatus( DMA1_FLAG_TC7 )== SET )//ＤＭＡ发送完成标志
        {
            DMA_ClearFlag( DMA1_FLAG_TC7 );
            DMA1_Stream7TransmitDone(  );     	
            DMA1_Stream7Flag = USART_DMA_DONE;
        }
        
        /********************发送消息设置*****************/
        if( DMA1_Stream7Flag == USART_DMA_DONE )
        {   	
            DMA1_Stream7Flag = USART_DMA_UNDONE;
            DMA_Cmd( DMA1_Channel7, DISABLE );
            ucUSART2_TransmitMessageNumber--;                                                          //正在等待需要发送的消息数
            DMA1_Channel7->CMAR = (uint32_t)ucUSART2TrainsmitBuffer[ucDMA1_Stream7TransmitGrooveIndex]; //要发送的缓冲
            DMA1_Channel7->CNDTR = ucUSART2TrainsmitLength[ucDMA1_Stream7TransmitGrooveIndex];           //数据长度 
            DMA_Cmd( DMA1_Channel7, ENABLE );                                                           //使能DMA数据流6
        }   
    }                    
}

/*******************************************************************************
* Function Name  : vUart2PinFunction_Init(u32 bound)
* Description    : 串口2和对应IO的初始化
* Input          : 波特率
* Output         : None
* Return         : None
*******************************************************************************/
void vUart2PinFunction_Init(u32 bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    
    GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
    
	//USART2_TX   GPIOD5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOD, &GPIO_InitStructure);
   
	//USART2_RX	  GPIOD6初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;//PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOD, &GPIO_InitStructure);//初始化GPIOA.10  
  
	//USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART2, &USART_InitStructure); //初始化串口2

    USART_DMACmd(USART2,USART_DMAReq_Tx, ENABLE);
    USART_DMACmd(USART2,USART_DMAReq_Rx, ENABLE);
	USART_Cmd(USART2, ENABLE);                    //使能串口2 
}

/*******************************************************************************
* Function Name  : vDma1_7_Usart2_TX_Init(u8 *MemoryAddr,u32 DataSize)
* Description    : 串口2DMA发送的初始化
* Input          : MemoryAddr：数据存放地址 DataSize：数据大小
* Output         : None
* Return         : None
*******************************************************************************/
void vDma1_7_Usart2_TX_Init(u8 *MemoryAddr,u32 DataSize)
{
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA传输

    DMA_DeInit(DMA1_Channel7);   //将DMA的通道1寄存器重设为缺省值

    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)( &( USART2->DR ) );  //DMA外设基地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)MemoryAddr;  //DMA内存基地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  //数据传输方向，从内存发送到外设
    DMA_InitStructure.DMA_BufferSize = DataSize;  //DMA通道的DMA缓存的大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址寄存器不变
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址寄存器递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //数据宽度为8位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //数据宽度为8位
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  //工作在正常模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; //DMA通道 x拥有中优先级 
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输
    DMA_Init(DMA1_Channel7, &DMA_InitStructure);  //根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Tx_DMA_Channel所标识的寄存器 
    DMA_ClearFlag( DMA1_FLAG_TC7 );
}
/*******************************************************************************
* Function Name  : vDma1_6_Usart2_RX_Init(u8 *MemoryAddr,u32 DataSize)
* Description    : 串口2DMA接收的初始化
* Input          : MemoryAddr：数据存放地址 DataSize：数据大小
* Output         : None
* Return         : None
*******************************************************************************/
void vDma1_6_Usart2_RX_Init(u8 *MemoryAddr,u32 DataSize)
{
    DMA_InitTypeDef DMA_InitStructure;
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA传输

    DMA_DeInit(DMA1_Channel6);   //将DMA的通道1寄存器重设为缺省值

    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)( &( USART2->DR ) );  //DMA外设基地址
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)MemoryAddr;  //DMA内存基地址
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //数据传输方向，从外设发送到内存
    DMA_InitStructure.DMA_BufferSize = DataSize;  //DMA通道的DMA缓存的大小
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //外设地址寄存器不变
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //内存地址寄存器递增
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //数据宽度为8位
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //数据宽度为8位
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  //工作在正常模式
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; //DMA通道 x拥有中优先级 
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMA通道x没有设置为内存到内存传输
    DMA_Init(DMA1_Channel6, &DMA_InitStructure);  //根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Tx_DMA_Channel所标识的寄存器 
    
    DMA_Cmd(DMA1_Channel6, ENABLE);  //正式驱动DMA传输
}


