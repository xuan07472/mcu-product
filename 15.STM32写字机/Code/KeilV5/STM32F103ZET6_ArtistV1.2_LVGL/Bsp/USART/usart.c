/***********************************************
* 	FileName:		
*	Description:	UART 
*	Author: 		LXG@firestaradmin
*	Date:			2020/6/19
*	Email:			firestardog@qq.com
************************************************/

#include "main.h"
#include "usart.h"	  
#include <stdio.h>
//#include "../bsp/transfer_BIN_to_EX_FLASH/transfer_BIN_to_EX_FLASH.h"
 
#ifdef USE_PRINTF
	#pragma import(__use_no_semihosting)             
	// �벻Ҫ��ѡUse MicroLib
	#ifdef __MICROLIB
		#error "Please do not use MicroLib!"
	#endif
	
	#ifdef __cplusplus
	extern "C"
	{
		void _sys_exit(int returncode)
		{
			printf("Exited! returncode=%d\n", returncode);
			while (1);
		}
		
		void _ttywrch(int ch)
		{
			if (ch == '\n'){
				while((PRINTF_UART->SR&0X40)==0);
				USART1->DR = (u8) '\r'; 
				while((PRINTF_UART->SR&0X40)==0);
				USART1->DR = (u8) '\n'; 
			} 
			else{
				while((PRINTF_UART->SR&0X40)==0);
				PRINTF_UART->DR = (u8) ch;
			}
			
		}
	}

	namespace std{
		struct __FILE
		{
		int handle;
		/* Whatever you require here. If the only file you are using is */
		/* standard output using printf() for debugging, no file handling */
		/* is required. */
		};
	
		FILE __stdin = {0};
		FILE __stdout = {1};
		FILE __stderr = {2};
			  
		int fgetc(FILE *stream)
		{
			int c = 0;
			if (stream->handle == 0)
			{
				while (USART_GetFlagStatus(PRINTF_UART, USART_FLAG_RXNE) == RESET);
				c = USART_ReceiveData(PRINTF_UART);
				_ttywrch((c == '\r') ? '\n' : c); // ����
				return c;
			}
			return EOF;
		}

		int fputc(int c, FILE *stream)
		{
			if (stream->handle == 1 || stream->handle == 2)
			{
				while((PRINTF_UART->SR&0X40)==0);
				PRINTF_UART->DR = (u8) c;      
				return c;
			}
			return EOF;
		}

		int fclose(FILE * stream)
		{
			return 0;
		}

		int fseek(FILE *stream, long int offset, int whence)
		{
			return -1;
		}
		
		int fflush(FILE *stream)
		{
			if (stream->handle == 1 || stream->handle == 2)
			while (USART_GetFlagStatus(PRINTF_UART, USART_FLAG_TC) == RESET);
			return 0;
		}
	}
	#else
		//��׼����Ҫ��֧�ֺ���                 
		struct __FILE 
		{ 
			int handle; 

		}; 

		FILE __stdout;       
		//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
		void _sys_exit(int returncode)
		{
			printf("Exited! returncode=%d\n", returncode);
			while (1);
		}
		void _ttywrch(int ch)
		{
			if (ch == '\n'){
				while((PRINTF_UART->SR&0X40)==0);
				USART1->DR = (u8) '\r'; 
				while((PRINTF_UART->SR&0X40)==0);
				USART1->DR = (u8) '\n'; 
			} 
			else{
				while((PRINTF_UART->SR&0X40)==0);
				PRINTF_UART->DR = (u8) ch;
			}
			
		}
		int fputc(int c, FILE *stream)
		{
			while((PRINTF_UART->SR&0X40)==0);
			PRINTF_UART->DR = (u8) c;      
			return c;
		}
	#endif
#endif 



	  
/*************************************************************
//USART1
//PA9 	TX->MODULE_RX
//PA10 	RX->MODULE_TX
 *************************************************************/
void usart1_init(u32 bound)
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
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=5 ;//��ռ���ȼ�
	//NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;		//�����ȼ�
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
	USART_Cmd(USART1, ENABLE);                    //ʹ�ܴ���1 

}

#ifdef __cplusplus
extern "C"{
#endif
	
/*
void USART1_IRQHandler(void)                	//����1�жϷ������
{
	u8 res;
	// ��Ӵ˾䣬��ֹ����һֱ������ж�
	if (USART_GetFlagStatus(USART1, USART_FLAG_ORE) != RESET)
	{
		USART_ReceiveData(USART1);
	}
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  
	{
		USART_ClearITPendingBit(USART1,  USART_IT_RXNE);	//����жϱ�־
		res = USART_ReceiveData(USART1);
		//TODO: �������
		//userCallBack(res);
		TBEF_uart_receive_process(res);
		res = res;
	} 

} 
*/
#ifdef __cplusplus
}
#endif


/*************************************************************
//USART2
//PA2 TX->MODULE_RX
//PA3 RX->MODULE_TX
 *************************************************************/
void usart2_init(u32 bound)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��GPIOAʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//ʹ��USART2ʱ��
	
	//USART2_TX   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA2

	//USART2_RX	  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);//��ʼ��GPIOA3

	//Usart3 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=9 ;//��ռ���ȼ�
	//NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	//USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(USART2, &USART_InitStructure); //��ʼ������2
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
	USART_Cmd(USART2, ENABLE);                    //ʹ�ܴ���2

}


void USART2_IRQHandler(void)                	//����2�жϷ������
{
	u8 res;
	// ��Ӵ˾䣬��ֹ����һֱ������ж�
	if (USART_GetFlagStatus(USART2, USART_FLAG_ORE) != RESET)
	{
		USART_ReceiveData(USART2);
	}

	if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)	//�жϷ��������ж�
	{
		
		USART_ClearITPendingBit(USART2,  USART_IT_RXNE);	//����жϱ�־
		res = USART_ReceiveData(USART2);
		//TODO: �������
		//userCallBack(res);
		res = res;
	}
} 

/*************************************************************
//USART3
PB10 USART3_TX
PB11 USART3_RX 
 *************************************************************/
void usart3_init(u32 bound)
{
	//GPIO�˿�����
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//ʹ��GPIOBʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);	//ʹ��USART3ʱ��
	
	//USART3_TX   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIO

	//USART3_RX	  
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��GPIO

	//Usart3 NVIC ����
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=9 ;//��ռ���ȼ�
	//NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���

	//USART ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//���ڲ�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

	USART_Init(USART3, &USART_InitStructure); //��ʼ������
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//�������ڽ����ж�
	USART_Cmd(USART3, ENABLE);                    //ʹ�ܴ���

}


void USART3_IRQHandler(void)                	//�����жϷ������
{
	u8 res;
	// ��Ӵ˾䣬��ֹ����һֱ������ж�
	if (USART_GetFlagStatus(USART3, USART_FLAG_ORE) != RESET)
	{
		USART_ReceiveData(USART3);
	}

	if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)	//�жϷ��������ж�
	{
		
		USART_ClearITPendingBit(USART3,  USART_IT_RXNE);	//����жϱ�־
		res = USART_ReceiveData(USART3);
		//TODO: �������
		//userCallBack(res);
		res = res;
	}
} 


/**************************************************************************************
* FunctionName   : UartSendByte()
* Description    : ����һ�ֽ�����
* EntryParameter : dat- ����ֵ
* ReturnValue    : None
**************************************************************************************/
void UartSendByte(USART_TypeDef *USART, u8 dat)
{

    USART_SendData(USART, dat);					// ��������
    while (USART_GetFlagStatus(USART, USART_FLAG_TC) == RESET);      // ��ȡ����״̬

}

/**************************************************************************************
* FunctionName   : UartSendMultByte()
* Description    : ���Ͷ��ֽ�����
* EntryParameter : str - ���ݻ���; len - ���ݳ���
* ReturnValue    : None
**************************************************************************************/
void UartSendMultByte(USART_TypeDef *UART, u8 *pStr, u16 len)
{
    while (len--)
    {
        USART_SendData(UART, *pStr++);				// ��������

        while (USART_GetFlagStatus(UART, USART_FLAG_TC) == RESET)  // ��ȡ����״̬
        {
            ;
        }
    }
}

/**************************************************************************************
* FunctionName   : UartSendStr()
* Description    : ���Ͷ��ֽ�����
* EntryParameter : str - ���ݻ���
* ReturnValue    : None
**************************************************************************************/
void UartSendStr(USART_TypeDef *UART, uc8 *pStr)
{
    while (*pStr != '\0')
    {
        USART_SendData(UART, *pStr++);				// ��������
        while (USART_GetFlagStatus(UART, USART_FLAG_TC) == RESET)  // ��ȡ����״̬
        {
            ;
        }
    }
}


















