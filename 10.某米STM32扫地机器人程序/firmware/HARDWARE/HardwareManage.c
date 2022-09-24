#include "hardwareManage.h"



void vSystemHardwareDriverInit(void)
{
	vIIC_Init();            //IIC PB6 7 bq24773电源管理芯片
    vKey_Init();            //按键初始化 
    delay_init();	        //延时函数初始化
    vUart1_Init();          //串口1初始化
    vUart2_Init();          //串口2初始化    
	vLEDPWM_Init();         //初始化LED
    vAll_PWM_Init();        //所有PWM初始化
	vTIM6_Int_Init();       //陀螺仪定时器
    vAllAdcPinInit();       //adc初始化	
	vBMI160_SPI2_Init();    //陀螺仪SPI初始化
	vAllOutputPinInit();    //所有普通输出IO初始化	
	vAll_EXTI_PinInit();    //外部中断引脚初始化
	vTIM7_Configuration();  //超声波
    vAll_TIM_CaptureInit(); //所有定时器捕获初始化 

}

