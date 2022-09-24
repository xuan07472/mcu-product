#ifndef __USART1_H
#define __USART1_H
#include "stdio.h"	
#include "sys.h" 

#define  USART1_BOUND 115200//����1������

#define  USART1_TRANSMIT_GROOVE_SIZE    5   //���ڷ��͵��������
#define  USART1_TRANSMIT_BUFFER_SIZE    90   //����һ�η�������ֽ���
#define  USART1_RECEIVE_GROOVE_SIZE     5    //���ڽ��յ��������
#define  USART1_RECEIVE_BUFFER_SIZE     90   //����һ�η���������ֽ���

typedef enum {USART_DMA_UNDONE = 0, USART_DMA_DONE = !USART_DMA_UNDONE} USART_DMAState;

/********************************************* USART1 *******************************************/
//дUSART1���ͻ�����
#define WriteDataToUSART1TraismitBufferDone(  ) ucUSART1_TransmitWritingBytePointer = 0; \
        ucWtiteDataToUSART1TransmitGrooveIndex = ( (ucWtiteDataToUSART1TransmitGrooveIndex+1) == USART1_TRANSMIT_GROOVE_SIZE ) ? 0 : (ucWtiteDataToUSART1TransmitGrooveIndex+1);\
        ucUSART1_TransmitMessageNumber++

//USART1���� DMA�������
#define DMA1_Stream4TransmitDone(  ) \
        ucDMA1_Stream4TransmitGrooveIndex = ( (ucDMA1_Stream4TransmitGrooveIndex+1) == USART1_TRANSMIT_GROOVE_SIZE ) ? 0 : (ucDMA1_Stream4TransmitGrooveIndex+1)

//USART1���� DMA�������  
#define DMA1_Stream5ReceiveDone(  ) \
        ucDMA1_Stream5ReceiveGrooveIndex = ( (ucDMA1_Stream5ReceiveGrooveIndex+1) == USART1_RECEIVE_GROOVE_SIZE ) ? 0 : (ucDMA1_Stream5ReceiveGrooveIndex+1);\
        ucUSART1_ReceiveMessageNumber++

//USART1 ���ջ������ݶ�ȡ���  
#define USART1ReceiveDataReadDone(  ) \
        ucUSART1ReadBufferIndex = ( (ucUSART1ReadBufferIndex+1) == USART1_RECEIVE_GROOVE_SIZE ) ? 0 : (ucUSART1ReadBufferIndex+1);\
        ucUSART1_ReceiveMessageNumber--

//��ȡ����δ������Ϣ�� 
#define usartGET_TEST_RECEIVE_MESSAGE_NUMBER(  )  ucUSART1_ReceiveMessageNumber 


            /********  usart1 ******/
extern uint8_t ucUSART1TrainsmitBuffer[USART1_TRANSMIT_GROOVE_SIZE][USART1_TRANSMIT_BUFFER_SIZE]; //USART1�������ݻ�����
extern uint8_t ucUSART1TrainsmitLength[USART1_TRANSMIT_GROOVE_SIZE];    //USART1����������  
extern uint8_t ucDMA1_Stream4TransmitGrooveIndex;                   //DMA1_Stream4����ڼ�������
extern uint8_t ucUSART1_TransmitMessageNumber ;                      //USART1��Ҫ�������Ϣ��    
extern uint8_t ucWtiteDataToUSART1TransmitGrooveIndex ;              //����USART1�ڼ�������                                               
extern uint8_t ucUSART1_TransmitWritingBytePointer ;                 //USART1�������ݻ������±�

extern uint8_t ucUSART1ReceiveBuffer[USART1_RECEIVE_GROOVE_SIZE][USART1_RECEIVE_BUFFER_SIZE];    //USART1�������ݻ�����
extern uint8_t ucUSART1ReceiveBufferLength[USART1_RECEIVE_GROOVE_SIZE];                                //USART1�������ݻ��������ݳ���     
extern uint8_t ucUSART1ReceiveGrooveIndex ;                          //USART1���յڼ�������
extern uint8_t ucUSART1ReadBufferIndex ;                             //��ȡUSART1�ڼ�������
extern uint8_t ucUSART1ReceiveWritingBytePointer ;                   //USART1���ջ����±�   
extern uint8_t ucUSART1_ReceiveMessageNumber ;                       //USART1��δ�������Ϣ��


void vUsart1Interactive(void);

void vUart1_Init(void);
void vUart1PinFunction_Init(u32 bound);
void vDma1_4_Usart1_TX_Init(u8 *MemoryAddr,u32 DataSize);

#endif


