#include "usart.h"	  
/**********************************************************************************
 * Filename   ��usart.c
 * Author     : pusheng       
 * Version    : 1.0
 * Date       : 2020.1.2
 * Discription : ��������������
 * 
 * USART2->TXD : PD5
 * USART2->RXD : PD6 
 * 
**********************************************************************************/	 


/****************************USART2�շ���ر���***********************************/
uint8_t ucUSART2TrainsmitBuffer[USART2_TRANSMIT_GROOVE_SIZE][USART2_TRANSMIT_BUFFER_SIZE] = {0}; //USART2�������ݻ�����
uint8_t ucUSART2TrainsmitLength[USART2_TRANSMIT_GROOVE_SIZE];    //USART2����������  
uint8_t ucDMA1_Stream7TransmitGrooveIndex = 0;                   //DMA1_Stream7����ڼ�������
uint8_t ucUSART2_TransmitMessageNumber = 0;                      //USART2��Ҫ�������Ϣ��    
uint8_t ucWtiteDataToUSART2TransmitGrooveIndex = 0;              //����USART2�ڼ�������                                               
uint8_t ucUSART2_TransmitWritingBytePointer = 0;                 //USART2�������ݻ������±�

uint8_t ucUSART2ReceiveBuffer[USART2_RECEIVE_GROOVE_SIZE][USART2_RECEIVE_BUFFER_SIZE] = {0};    //USART2�������ݻ�����
uint8_t ucUSART2ReceiveBufferLength[USART2_RECEIVE_GROOVE_SIZE];                                //USART2�������ݻ��������ݳ���     
uint8_t ucUSART2ReceiveGrooveIndex = 0;                          //USART2���յڼ�������
uint8_t ucUSART2ReadBufferIndex = 0;                             //��ȡUSART2�ڼ�������
uint8_t ucUSART2ReceiveWritingBytePointer = 0;                   //USART2���ջ����±�   
uint8_t ucDMA1_Stream6ReceiveGrooveIndex = 0;                    //DMA1_Stream6���յڼ�������
uint8_t ucUSART2_ReceiveMessageNumber = 0;                       //USART2��δ�������Ϣ��

/*******************************************************************************
* Function Name  : vUsart2DmaInitForTest(void)
* Description    : ����2�Ͷ�ӦIO������շ���DMA�ĳ�ʼ��
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
* Description    : Usart2����Ϣ����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vUsart2Interactive( void )
{  
    static USART_DMAState DMA1_Stream7Flag = USART_DMA_DONE;    
    
    /*����Ƿ�������*/
    if( USART_GetFlagStatus( USART2, USART_FLAG_IDLE ) == SET )                //USART2���տ���
    {
        DMA_Cmd( DMA1_Channel6, DISABLE ); 
        DMA_ClearFlag( DMA1_FLAG_TC6 );                         //ʧ��DMA������2
        /******�ȶ�SR�Ĵ����ٶ�DR�Ĵ������USART_IT_IDLE��־*****/
        USART2->SR;
        USART2->DR;
        ucUSART2ReceiveBufferLength[ucDMA1_Stream6ReceiveGrooveIndex] = USART2_RECEIVE_BUFFER_SIZE - DMA_GetCurrDataCounter( DMA1_Channel6 );     
        DMA1_Stream6ReceiveDone(  );
        DMA1_Channel6->CMAR = (uint32_t)ucUSART2ReceiveBuffer[ucDMA1_Stream6ReceiveGrooveIndex];
        DMA1_Channel6->CNDTR = USART2_RECEIVE_BUFFER_SIZE;                        //���ô��䳤�ȣ����ý��յ�ַ��0��ʼ     
        DMA_Cmd( DMA1_Channel6, ENABLE );                                       //ʹ��DMA������6                           
    }    
    
    /*����Ƿ���������Ҫ����*/
    if( ucUSART2_TransmitMessageNumber ) 
    {
        /********************���ģͣ��������*****************/
        if( DMA_GetFlagStatus( DMA1_FLAG_TC7 )== SET )//�ģͣ�������ɱ�־
        {
            DMA_ClearFlag( DMA1_FLAG_TC7 );
            DMA1_Stream7TransmitDone(  );     	
            DMA1_Stream7Flag = USART_DMA_DONE;
        }
        
        /********************������Ϣ����*****************/
        if( DMA1_Stream7Flag == USART_DMA_DONE )
        {   	
            DMA1_Stream7Flag = USART_DMA_UNDONE;
            DMA_Cmd( DMA1_Channel7, DISABLE );
            ucUSART2_TransmitMessageNumber--;                                                          //���ڵȴ���Ҫ���͵���Ϣ��
            DMA1_Channel7->CMAR = (uint32_t)ucUSART2TrainsmitBuffer[ucDMA1_Stream7TransmitGrooveIndex]; //Ҫ���͵Ļ���
            DMA1_Channel7->CNDTR = ucUSART2TrainsmitLength[ucDMA1_Stream7TransmitGrooveIndex];           //���ݳ��� 
            DMA_Cmd( DMA1_Channel7, ENABLE );                                                           //ʹ��DMA������6
        }   
    }                    
}

/*******************************************************************************
* Function Name  : vUart2PinFunction_Init(u32 bound)
* Description    : ����2�Ͷ�ӦIO�ĳ�ʼ��
* Input          : ������
* Output         : None
* Return         : None
*******************************************************************************/
void vUart2PinFunction_Init(u32 bound)
{
	//GPIO�˿�����
	GPIO_InitTypeDef  GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);	
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
    
    GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);
    
	//USART2_TX   GPIOD5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOD, &GPIO_InitStructure);
   
	//USART2_RX	  GPIOD6��ʼ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;//PA10
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOD, &GPIO_InitStructure);//��ʼ��GPIOA.10  
  
	//USART ��ʼ������

	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(USART2, &USART_InitStructure); //��ʼ������2

    USART_DMACmd(USART2,USART_DMAReq_Tx, ENABLE);
    USART_DMACmd(USART2,USART_DMAReq_Rx, ENABLE);
	USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���2 
}

/*******************************************************************************
* Function Name  : vDma1_7_Usart2_TX_Init(u8 *MemoryAddr,u32 DataSize)
* Description    : ����2DMA���͵ĳ�ʼ��
* Input          : MemoryAddr�����ݴ�ŵ�ַ DataSize�����ݴ�С
* Output         : None
* Return         : None
*******************************************************************************/
void vDma1_7_Usart2_TX_Init(u8 *MemoryAddr,u32 DataSize)
{
    DMA_InitTypeDef DMA_InitStructure;
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//ʹ��DMA����

    DMA_DeInit(DMA1_Channel7);   //��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ

    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)( &( USART2->DR ) );  //DMA�������ַ
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
    DMA_Init(DMA1_Channel7, &DMA_InitStructure);  //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��USART1_Tx_DMA_Channel����ʶ�ļĴ��� 
    DMA_ClearFlag( DMA1_FLAG_TC7 );
}
/*******************************************************************************
* Function Name  : vDma1_6_Usart2_RX_Init(u8 *MemoryAddr,u32 DataSize)
* Description    : ����2DMA���յĳ�ʼ��
* Input          : MemoryAddr�����ݴ�ŵ�ַ DataSize�����ݴ�С
* Output         : None
* Return         : None
*******************************************************************************/
void vDma1_6_Usart2_RX_Init(u8 *MemoryAddr,u32 DataSize)
{
    DMA_InitTypeDef DMA_InitStructure;
    
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);	//ʹ��DMA����

    DMA_DeInit(DMA1_Channel6);   //��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ

    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)( &( USART2->DR ) );  //DMA�������ַ
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)MemoryAddr;  //DMA�ڴ����ַ
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;  //���ݴ��䷽�򣬴����跢�͵��ڴ�
    DMA_InitStructure.DMA_BufferSize = DataSize;  //DMAͨ����DMA����Ĵ�С
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  //�����ַ�Ĵ�������
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;  //�ڴ��ַ�Ĵ�������
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  //���ݿ��Ϊ8λ
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte; //���ݿ��Ϊ8λ
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;  //����������ģʽ
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium; //DMAͨ�� xӵ�������ȼ� 
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;  //DMAͨ��xû������Ϊ�ڴ浽�ڴ洫��
    DMA_Init(DMA1_Channel6, &DMA_InitStructure);  //����DMA_InitStruct��ָ���Ĳ�����ʼ��DMA��ͨ��USART1_Tx_DMA_Channel����ʶ�ļĴ��� 
    
    DMA_Cmd(DMA1_Channel6, ENABLE);  //��ʽ����DMA����
}


