#ifndef __USARTALL_H
#define __USARTALL_H
#include "stdio.h"	
#include "sys.h" 
#include "usart1.h"

#define  USART2_BOUND 115200//����2������

#define  USART2_TRANSMIT_GROOVE_SIZE    30   //���ڷ��͵��������
#define  USART2_TRANSMIT_BUFFER_SIZE    90   //����һ�η�������ֽ���
#define  USART2_RECEIVE_GROOVE_SIZE     5    //���ڽ��յ��������
#define  USART2_RECEIVE_BUFFER_SIZE     90   //����һ�η���������ֽ���


/******************************************** USART2 ************************************************/
//дUSART2���ͻ�����
#define WriteDataToUSART2TraismitBufferDone(  ) ucUSART2_TransmitWritingBytePointer = 0; \
        ucWtiteDataToUSART2TransmitGrooveIndex = ( (ucWtiteDataToUSART2TransmitGrooveIndex+1) == USART2_TRANSMIT_GROOVE_SIZE ) ? 0 : (ucWtiteDataToUSART2TransmitGrooveIndex+1);\
        ucUSART2_TransmitMessageNumber++

//USART2���� DMA������� 
#define DMA1_Stream7TransmitDone(  ) \
        ucDMA1_Stream7TransmitGrooveIndex = ( (ucDMA1_Stream7TransmitGrooveIndex+1) == USART2_TRANSMIT_GROOVE_SIZE ) ? 0 : (ucDMA1_Stream7TransmitGrooveIndex+1)

//USART2���� DMA�������  
#define DMA1_Stream6ReceiveDone(  ) \
        ucDMA1_Stream6ReceiveGrooveIndex = ( (ucDMA1_Stream6ReceiveGrooveIndex+1) == USART2_RECEIVE_GROOVE_SIZE ) ? 0 : (ucDMA1_Stream6ReceiveGrooveIndex+1);\
        ucUSART2_ReceiveMessageNumber++

//USART2 ���ջ������ݶ�ȡ��� 
#define USART2ReceiveDataReadDone(  ) \
        ucUSART2ReadBufferIndex = ( (ucUSART2ReadBufferIndex+1) == USART2_RECEIVE_GROOVE_SIZE ) ? 0 : (ucUSART2ReadBufferIndex+1);\
        ucUSART2_ReceiveMessageNumber--


        /********  usart2 ******/
extern uint8_t ucUSART2TrainsmitBuffer[USART2_TRANSMIT_GROOVE_SIZE][USART2_TRANSMIT_BUFFER_SIZE]; //USART2�������ݻ�����
extern uint8_t ucUSART2TrainsmitLength[USART2_TRANSMIT_GROOVE_SIZE];    //USART2����������  
extern uint8_t ucDMA1_Stream7TransmitGrooveIndex ;                   //DMA1_Stream7����ڼ�������
extern uint8_t ucUSART2_TransmitMessageNumber ;                      //USART2��Ҫ�������Ϣ��    
extern uint8_t ucWtiteDataToUSART2TransmitGrooveIndex ;              //����USART2�ڼ�������                                               
extern uint8_t ucUSART2_TransmitWritingBytePointer ;                 //USART2�������ݻ������±�

extern uint8_t ucUSART2ReceiveBuffer[USART2_RECEIVE_GROOVE_SIZE][USART2_RECEIVE_BUFFER_SIZE];    //USART2�������ݻ�����
extern uint8_t ucUSART2ReceiveBufferLength[USART2_RECEIVE_GROOVE_SIZE];                                //USART2�������ݻ��������ݳ���     
extern uint8_t ucUSART2ReceiveGrooveIndex ;                          //USART2���յڼ�������
extern uint8_t ucUSART2ReadBufferIndex;                             //��ȡUSART2�ڼ�������
extern uint8_t ucUSART2ReceiveWritingBytePointer ;                   //USART2���ջ����±�   
extern uint8_t ucDMA1_Stream6ReceiveGrooveIndex;                    //DMA1_Stream6���յڼ�������
extern uint8_t ucUSART2_ReceiveMessageNumber;                       //USART2��δ�������Ϣ��


void vUsart2Interactive(void);

void vUart2_Init(void);
void vUart2PinFunction_Init(u32 bound);

void vDma1_7_Usart2_TX_Init(u8 *MemoryAddr,u32 DataSize);
void vDma1_6_Usart2_RX_Init(u8 *MemoryAddr,u32 DataSize);

#endif


