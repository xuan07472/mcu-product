

#ifndef MAIN_H
#define MAIN_H

#include <stm32f10x.h> 
#include "stm32f10x_conf.h"
#include "../BSP/delay/delay.h"
#include "../BSP/usart/usart.h"
#include "../BSP/TIM/myTIM.h"
#include "../BSP/LED/LED.h"
#include "../bsp/w25qxx/w25qxx.h" 
#include "../bsp/24cxx/24cxx.h"
#include "LCD_ILI9486.h"
#include "HW_EXTI.h"
#include "HW_PWM.h"
#include "lvgl.h"
#include "lv_port_disp.h"
//λ������,ʵ��51���Ƶ�GPIO���ƹ���
//����ʵ��˼��,�ο�<<CM3Ȩ��ָ��>>������(87ҳ~92ҳ).
//IO�ڲ����궨��
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO�ڵ�ַӳ��
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 
 
//IO�ڲ���,ֻ�Ե�һ��IO��!
//ȷ��n��ֵС��16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //��� 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //���� 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //��� 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //���� 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //��� 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //���� 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //��� 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //���� 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //��� 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //����

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //��� 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //����

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //��� 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //����


#define F_CPU 72000000
#define M_PI  3.14159265358979323846

//#####################���꡿#####################
#define cli()      __set_PRIMASK(1)   			//���жϹر�
#define sei()      __set_PRIMASK(0) 			//���жϿ���


//�˴����뽫ʵ��GPIO�Ĺ��ܿ��ƣ��������������ʼ�����������ŵ�ƽ�Ͷ�ȡ���ŵ�ƽ��

//##############################��API��##############################
void HW_GPIO_Init_Out(u32 gpio_clk,GPIO_TypeDef * gpio,u16 gpio_pin);	
void HW_GPIO_Init_In(u32 gpio_clk,GPIO_TypeDef * gpio,u16 gpio_pin);
void HW_GPIO_Write(GPIO_TypeDef * gpio,u16 gpio_pin,u8 val);
u8   HW_GPIO_Read(GPIO_TypeDef * gpio,u16 gpio_pin);

/******************************* ȫ�ֱ������� ************************************/
extern u8 g_Off_Line_Flag;
#endif // MAIN_H


