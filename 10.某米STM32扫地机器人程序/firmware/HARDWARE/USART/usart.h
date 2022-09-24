#ifndef __USARTALL_H
#define __USARTALL_H
#include "stdio.h"	
#include "sys.h" 
#include "usart1.h"

#define  USART2_BOUND 115200//串口2波特率

#define  USART2_TRANSMIT_GROOVE_SIZE    30   //串口发送的最大条数
#define  USART2_TRANSMIT_BUFFER_SIZE    90   //串口一次发送最大字节数
#define  USART2_RECEIVE_GROOVE_SIZE     5    //串口接收的最大条数
#define  USART2_RECEIVE_BUFFER_SIZE     90   //串口一次发接收最大字节数


/******************************************** USART2 ************************************************/
//写USART2发送缓冲区
#define WriteDataToUSART2TraismitBufferDone(  ) ucUSART2_TransmitWritingBytePointer = 0; \
        ucWtiteDataToUSART2TransmitGrooveIndex = ( (ucWtiteDataToUSART2TransmitGrooveIndex+1) == USART2_TRANSMIT_GROOVE_SIZE ) ? 0 : (ucWtiteDataToUSART2TransmitGrooveIndex+1);\
        ucUSART2_TransmitMessageNumber++

//USART2发送 DMA传输完成 
#define DMA1_Stream7TransmitDone(  ) \
        ucDMA1_Stream7TransmitGrooveIndex = ( (ucDMA1_Stream7TransmitGrooveIndex+1) == USART2_TRANSMIT_GROOVE_SIZE ) ? 0 : (ucDMA1_Stream7TransmitGrooveIndex+1)

//USART2接收 DMA接收完成  
#define DMA1_Stream6ReceiveDone(  ) \
        ucDMA1_Stream6ReceiveGrooveIndex = ( (ucDMA1_Stream6ReceiveGrooveIndex+1) == USART2_RECEIVE_GROOVE_SIZE ) ? 0 : (ucDMA1_Stream6ReceiveGrooveIndex+1);\
        ucUSART2_ReceiveMessageNumber++

//USART2 接收缓冲数据读取完成 
#define USART2ReceiveDataReadDone(  ) \
        ucUSART2ReadBufferIndex = ( (ucUSART2ReadBufferIndex+1) == USART2_RECEIVE_GROOVE_SIZE ) ? 0 : (ucUSART2ReadBufferIndex+1);\
        ucUSART2_ReceiveMessageNumber--


        /********  usart2 ******/
extern uint8_t ucUSART2TrainsmitBuffer[USART2_TRANSMIT_GROOVE_SIZE][USART2_TRANSMIT_BUFFER_SIZE]; //USART2发送数据缓冲区
extern uint8_t ucUSART2TrainsmitLength[USART2_TRANSMIT_GROOVE_SIZE];    //USART2缓冲区长度  
extern uint8_t ucDMA1_Stream7TransmitGrooveIndex ;                   //DMA1_Stream7传输第几个缓冲
extern uint8_t ucUSART2_TransmitMessageNumber ;                      //USART2需要传输的消息数    
extern uint8_t ucWtiteDataToUSART2TransmitGrooveIndex ;              //发送USART2第几个缓冲                                               
extern uint8_t ucUSART2_TransmitWritingBytePointer ;                 //USART2发送数据缓冲区下标

extern uint8_t ucUSART2ReceiveBuffer[USART2_RECEIVE_GROOVE_SIZE][USART2_RECEIVE_BUFFER_SIZE];    //USART2接收数据缓冲区
extern uint8_t ucUSART2ReceiveBufferLength[USART2_RECEIVE_GROOVE_SIZE];                                //USART2接收数据缓冲区数据长度     
extern uint8_t ucUSART2ReceiveGrooveIndex ;                          //USART2接收第几个缓冲
extern uint8_t ucUSART2ReadBufferIndex;                             //读取USART2第几个缓冲
extern uint8_t ucUSART2ReceiveWritingBytePointer ;                   //USART2接收缓冲下标   
extern uint8_t ucDMA1_Stream6ReceiveGrooveIndex;                    //DMA1_Stream6接收第几个缓冲
extern uint8_t ucUSART2_ReceiveMessageNumber;                       //USART2还未处理的消息数


void vUsart2Interactive(void);

void vUart2_Init(void);
void vUart2PinFunction_Init(u32 bound);

void vDma1_7_Usart2_TX_Init(u8 *MemoryAddr,u32 DataSize);
void vDma1_6_Usart2_RX_Init(u8 *MemoryAddr,u32 DataSize);

#endif


