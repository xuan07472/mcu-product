#include "usart1.h"	  
/**********************************************************************************
 * Filename   ��usart1.c
 * Author     : pusheng       
 * Version    : 1.0
 * Date       : 2019.11.7
 * Discription : Usart1 base driver API
 * 
 * USART1->TXD : PA9
 * USART1->RXD : PA10  
 * 
**********************************************************************************/	 
//���ʹ��ucos,����������ͷ�ļ�����.
#if SYSTEM_SUPPORT_OS
#include "FreeRTOS.h"					//FreeRTOSʹ��	 	  
#endif
 
 

//////////////////////////////////////////////////////////////////
//�������´���,֧��printf����,������Ҫѡ��use MicroLIB	  
#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc���� 
int fputc(int ch, FILE *f)
{      
	while((USART1->SR&0X40)==0);//ѭ������,ֱ���������   
    USART1->DR = (u8) ch;      
	return ch;
}
#endif 
 
/****************************USART1�շ���ر���***********************************/
uint8_t ucUSART1TrainsmitBuffer[USART1_TRANSMIT_GROOVE_SIZE][USART1_TRANSMIT_BUFFER_SIZE] = {0}; //USART1�������ݻ�����
uint8_t ucUSART1TrainsmitLength[USART1_TRANSMIT_GROOVE_SIZE];    //USART1����������  
uint8_t ucDMA1_Stream4TransmitGrooveIndex = 0;                   //DMA1_Stream4����ڼ�������
uint8_t ucUSART1_TransmitMessageNumber = 0;                      //USART1��Ҫ�������Ϣ��    
uint8_t ucWtiteDataToUSART1TransmitGrooveIndex = 0;              //����USART1�ڼ�������                                               
uint8_t ucUSART1_TransmitWritingBytePointer = 0;                 //USART1�������ݻ������±�

uint8_t ucUSART1ReceiveBuffer[USART1_RECEIVE_GROOVE_SIZE][USART1_RECEIVE_BUFFER_SIZE] = {0};    //USART1�������ݻ�����
uint8_t ucUSART1ReceiveBufferLength[USART1_RECEIVE_GROOVE_SIZE];                                //USART1�������ݻ��������ݳ���     
uint8_t ucUSART1ReceiveGrooveIndex = 0;                          //USART1���յڼ�������
uint8_t ucUSART1ReadBufferIndex = 0;                             //��ȡUSART1�ڼ�������
uint8_t ucUSART1ReceiveWritingBytePointer = 0;                   //USART1���ջ����±�   
uint8_t ucDMA1_Stream5ReceiveGrooveIndex = 0;                    //DMA1_Stream5���յڼ�������
uint8_t ucUSART1_ReceiveMessageNumber = 0;                       //USART1��δ�������Ϣ��

/*******************************************************************************
* Function Name  : vUart1_Init(void)
* Description    : ����1�Ͷ�ӦIO���䷢��DMA�ĳ�ʼ��
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
* Description    : ����1����Ϣ����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vUsart1Interactive(void)
{ 
    static USART_DMAState DMA1_Stream4Flag = USART_DMA_DONE;

    /*����Ƿ���������Ҫ����*/
    if( ucUSART1_TransmitMessageNumber ) 
    {
        /********************���ģͣ��������*****************/
        if( DMA_GetFlagStatus( DMA1_FLAG_TC4 )== SET )//�ģͣ�������ɱ�־
        {
            DMA_ClearFlag( DMA1_FLAG_TC4 );
            DMA1_Stream4TransmitDone(  );     	
            DMA1_Stream4Flag = USART_DMA_DONE;
        }
        
        /********************������Ϣ����*****************/
        if( DMA1_Stream4Flag == USART_DMA_DONE )
        {   	
            DMA1_Stream4Flag = USART_DMA_UNDONE;
            DMA_Cmd(DMA1_Channel4, DISABLE ); 
            ucUSART1_TransmitMessageNumber--;                                                        //���ڵȴ���Ҫ���͵���Ϣ��
            DMA1_Channel4->CMAR = (uint32_t)ucUSART1TrainsmitBuffer[ucDMA1_Stream4TransmitGrooveIndex]; //Ҫ���͵Ļ���
            DMA1_Channel4->CNDTR = ucUSART1TrainsmitLength[ucDMA1_Stream4TransmitGrooveIndex];           //���ݳ��� 
            DMA_Cmd( DMA1_Channel4, ENABLE );                                                       //ʹ��DMA������6
        } 
         
    }
    
}

/*******************************************************************************
* Function Name  : vUart1PinFunction_Init(u32 bound)
* Description    : ����1�Ͷ�ӦIO�ĳ�ʼ��
* Input          : ������
* Output         : None
* Return         : None
*******************************************************************************/
void vUart1PinFunction_Init(u32 bound)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
  
	//USART1_TX   GPIOA.9
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.9
   
	//USART1_RX	  GPIOA.10��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA.10  

	//Usart1 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
	//USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
    USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);//�������ڿ����ж�
    USART_DMACmd(USART1,USART_DMAReq_Tx, ENABLE);
	USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 
}


/*******************************************************************************
* Function Name  : vDma1_4_Usart1_TX_Init(u8 *MemoryAddr,u32 DataSize)
* Description    : ����1DMA���͵ĳ�ʼ��
* Input          : MemoryAddr�����ݴ�ŵ�ַ DataSize�����ݴ�С
* Output         : None
* Return         : None
*******************************************************************************/
void vDma1_4_Usart1_TX_Init(u8 *MemoryAddr,u32 DataSize)
{
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//ʹ��DMA����

    DMA_DeInit(DMA1_Channel4);   //��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ

    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)( &( USART1->DR ) );  //DMA�������ַ
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)MemoryAddr;  //DMA�ڴ����ַ
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;  //���ݴ��䷽�򣬴��ڴ淢�͵�����
    DMA_InitStructure.DMA_BufferSize = DataSize;  //DMAͨ����DMA����Ĵ�С
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //�����ַ�Ĵ�������
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�ڴ��ַ�Ĵ�������
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //���ݿ��Ϊ8λ
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //���ݿ��Ϊ8λ
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  //����������ģʽ
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; //DMAͨ�� xӵ�������ȼ� 
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
    DMA_Init(DMA1_Channel4, &DMA_InitStructure);  //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��USART1_Tx_DMA_Channel����ʶ�ļĴ��� 
}

void USART1_IRQHandler(void)                	//����1�жϷ������
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


