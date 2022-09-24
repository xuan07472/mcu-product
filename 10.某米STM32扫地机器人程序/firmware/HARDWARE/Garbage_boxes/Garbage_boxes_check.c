/**
 * @Filename   ：Garbage_boxes_check.c
 * @Author     : HuangWentao       
 * @Version    : 1.0
 * @Date       : 2019/11/9
 * @Discription : 垃圾盒检测
 * @hardware connect:
 *          -------------PC13(高电平垃圾盒被取出)----------------------
 */

#include "Garbage_boxes_check.h"

static bool bGarbageBoxState=true;//垃圾盒状态 true 表示合上 false 取出
/**
  * @Description  垃圾盒检测任务
  * @param  无
  * @retval 无
  */
void vGarbage_box_check_task(void *pvParameters)
{
	vGarbage_box_check_Init();

	while(1)
	{
		
		vGarbageBoxChecking();
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.DustBoxAbn=!isGarbageBoxClose();//上报尘盒状态
		delay_ms(100);
	}
	
}


/**
  * @Description  判断垃圾盒是否合上
  * @param  无
  * @retval bGarbageBoxState：true合上 false 取出
  */
bool isGarbageBoxClose(void)
{
  return   bGarbageBoxState;
}


/**
  * @Description  垃圾盒状态检测
  * @param  无
  * @retval 无
  */
void vGarbageBoxChecking(void)
{
   if(GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13))//垃圾盒检测
    {
        //printf("垃圾盒被取出了\r\n");
        bGarbageBoxState = false;
    }

    else
    {
        //printf("垃圾盒合上了\r\n");

        bGarbageBoxState = true;
    }
}

/**
  * @Description  垃圾盒检测引脚IO初始化 
  * @param  无
  * @retval 无
  */
void vGarbage_box_check_Init(void)
{
 	
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);	 //使能PB端口时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
}
 





