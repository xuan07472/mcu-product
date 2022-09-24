#include "usart1.h"	  
/**********************************************************************************
 * Filename   ：usart1.c
 * Author     : pusheng       
 * Version    : 1.0
 * Date       : 2019.11.7
 * Discription : Usart1 base driver API
 * 
 * USART1->TXD : PA9
 * USART1->RXD : PA10  
 * 
**********************************************************************************/	 
//如果使用ucos,则包括下面的头文件即可.
#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"					//FreeRTOS使用	 	  
#endif
 
 

//////////////////////////////////////////////////////////////////
//加入以下代码,支持printf函数,而不需要选择use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//循环发送,直到发送完毕   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 
 
/****************************USART1收发相关变量***********************************/
uint8_t ucUSART1TrainsmitBuffer[USART1_TRANSMIT_GROOVE_SIZE][USART1_TRANSMIT_BUFFER_SIZE] = {0}; //USART1发送数据缓冲区
uint8_t ucUSART1TrainsmitLength[USART1_TRANSMIT_GROOVE_SIZE];    //USART1缓冲区长度  
uint8_t ucDMA1_Stream4TransmitGrooveIndex = 0;                   //DMA1_Stream4传输第几个缓冲
uint8_t ucUSART1_TransmitMessageNumber = 0;                      //USART1需要传输的消息数    
uint8_t ucWtiteDataToUSART1TransmitGrooveIndex = 0;              //发送USART1第几个缓冲                                               
uint8_t ucUSART1_TransmitWritingBytePointer = 0;                 //USART1发送数据缓冲区下标

uint8_t ucUSART1ReceiveBuffer[USART1_RECEIVE_GROOVE_SIZE][USART1_RECEIVE_BUFFER_SIZE] = {0};    //USART1接收数据缓冲区
uint8_t ucUSART1ReceiveBufferLength[USART1_RECEIVE_GROOVE_SIZE];                                //USART1接收数据缓冲区数据长度     
uint8_t ucUSART1ReceiveGrooveIndex = 0;                          //USART1接收第几个缓冲
uint8_t ucUSART1ReadBufferIndex = 0;                             //读取USART1第几个缓冲
uint8_t ucUSART1ReceiveWritingBytePointer = 0;                   //USART1接收缓冲下标   
uint8_t ucDMA1_Stream5ReceiveGrooveIndex = 0;                    //DMA1_Stream5接收第几个缓冲
uint8_t ucUSART1_ReceiveMessageNumber = 0;                       //USART1还未处理的消息数

/*******************************************************************************
* Function Name  : vUart1_Init(void)
* Description    : 串口1和对应IO及其发送DMA的初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vUart1_Init(void)
{
    vUart1PinFunction_Init(USART1_BOUND);
    vDma1_4_Usart1_TX_Init(ucUSART1TrainsmitBuffer[ucDMA1_Stream4TransmitGrooveIndex],USART1_TRANSMIT_BUFFER_SIZE);
}

/*******************************************************************************
* Function Name  : vUsart1Interactive(void)
* Description    : 串口1的信息处理
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vUsart1Interactive(void)
{ 
    static USART_DMAState DMA1_Stream4Flag = USART_DMA_DONE;

    /*检测是否有数据需要发送*/
    if( ucUSART1_TransmitMessageNumber ) 
    {
        /********************检测ＤＭＡ传输完成*****************/
        if( DMA_GetFlagStatus( DMA1_FLAG_TC4 )== SET )//ＤＭＡ发送完成标志
        {
            DMA_ClearFlag( DMA1_FLAG_TC4 );
            DMA1_Stream4TransmitDone(  );     	
            DMA1_Stream4Flag = USART_DMA_DONE;
        }
        
        /********************发送消息设置*****************/
        if( DMA1_Stream4Flag == USART_DMA_DONE )
        {   	
            DMA1_Stream4Flag = USART_DMA_UNDONE;
            DMA_Cmd(DMA1_Channel4, DISABLE ); 
            ucUSART1_TransmitMessageNumber--;                                                        //正在等待需要发送的消息数
            DMA1_Channel4->CMAR = (uint32_t)ucUSART1TrainsmitBuffer[ucDMA1_Stream4TransmitGrooveIndex]; //要发送的缓冲
            DMA1_Channel4->CNDTR = ucUSART1TrainsmitLength[ucDMA1_Stream4TransmitGrooveIndex];           //数据长度 
            DMA_Cmd( DMA1_Channel4, ENABLE );                                                       //使能DMA数据流6
        } 
         
    }
    
}

/*******************************************************************************
* Function Name  : vUart1PinFunction_Init(u32 bound)
* Description    : 串口1和对应IO的初始化
* Input          : 波特率
* Output         : None
* Return         : None
*******************************************************************************/
void vUart1PinFunction_Init(u32 bound)
{
	//GPIO端口设置
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//使能USART1，GPIOA时钟
  
	//USART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//复用推挽输出
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.9
   
	//USART1_RX	  GPIOA.10初始化
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//浮空输入
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化GPIOA.10  

	//Usart1 NVIC 配置
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
  
	//USART 初始化设置

	USART_InitStructure.USART_BaudRate = bound;//串口波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

	USART_Init(USART1, &USART_InitStructure); //初始化串口1
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//开启串口接受中断
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//开启串口空闲中断
    USART_DMACmd(USART1,USART_DMAReq_Tx, ENABLE);
	USART_Cmd(USART1, ENABLE);                    //使能串口1 
}


/*******************************************************************************
* Function Name  : vDma1_4_Usart1_TX_Init(u8 *MemoryAddr,u32 DataSize)
* Description    : 串口1DMA发送的初始化
* Input          : MemoryAddr：数据存放地址 DataSize：数据大小
* Output         : None
* Return         : None
*******************************************************************************/
void vDma1_4_Usart1_TX_Init(u8 *MemoryAddr,u32 DataSize)
{
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//使能DMA传输

    DMA_DeInit(DMA1_Channel4);   //将DMA的通道1寄存器重设为缺省值

    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)( &( USART1->DR ) );  //DMA外设基地址
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
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);  //根据DMA_InitStruct中指定的参数初始化DMA的通道USART1_Tx_DMA_Channel所标识的寄存器 
}

void USART1_IRQHandler(void)                	//串口1中断服务程序
{
    static uint8_t index=0;
    if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) 
    {
        if(index<USART1_RECEIVE_BUFFER_SIZE)
        {
            ucUSART1ReceiveBuffer[ucDMA1_Stream5ReceiveGrooveIndex][index++] = USART_ReceiveData(USART1);
        }
    }
    else if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
    {
        USART1->SR;
        USART1->DR;
        ucUSART1ReceiveBufferLength[ucDMA1_Stream5ReceiveGrooveIndex] = index;     
        DMA1_Stream5ReceiveDone(  );
        index=0;
    }
} 


