#ifndef __USART_H
#define __USART_H

#include "main.h" 

//���� USE_PRINTF ���ض���printf����
#define USE_PRINTF
#define PRINTF_UART	USART1






void usart1_init(u32 bound);
void usart2_init(u32 bound);
void usart3_init(u32 bound);



void UartSendByte(USART_TypeDef *USART, u8 dat);
void UartSendMultByte(USART_TypeDef *UART, u8 *pStr, u16 len);
void UartSendStr(USART_TypeDef *UART, uc8 *pStr);


#endif


