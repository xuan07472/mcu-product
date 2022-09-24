
/*
    创建时间：2019/10/10
    创建人  ：pusheng
    作用描述：该文件内的所有函数 都是给USMART调试用
*/

#include "testfun.h"
/*******************************************************************************
* Function Name  : vPrintf_ALL_RobotSta(void)
* Description    : 打印机器人状态等信息
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
char *BufIndex[11]={"停止","前进","后退后左转","后退后右转","左转","右转","原地旋转","自动","回充","正在充电","沿边"};
void vPrintf_ALL_RobotSta(void )
{
    printf("\r\n");
    printf("方向 :%s \r\n",BufIndex[g_tRobotState.Robotstate.eDirection]);
    printf("电量 %d %%\r\n",g_tRobotState.Robotstate.ucRemainBattery);
    printf("异常 %d \r\n",g_tRobotState.Robotstate.unionAbnormal.AbnormalSta);
    printf("错误 %d \r\n",g_tRobotState.Robotstate.unionEEROR.ErrorSta);
    
}
/*******************************************************************************
* Function Name  : printf_ALL_wheel_Data(void)
* Description    : 打印轮子速度等信息
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vPrintf_ALL_wheel_Data(void )
{
    printf("\r\n");
    printf("Left ExpectSpeed     %d mm/s\r\n",g_tLeftWheel.ExpectSpeed);
    printf("Right ExpectSpeed    %d mm/s\r\n\r\n",g_tRightWheel.ExpectSpeed);
    
    printf("Left RealSpeed     %d mm/s\r\n",g_tLeftWheel.RealSpeed);
    printf("Right RealSpeed    %d mm/s\r\n",g_tRightWheel.RealSpeed);
    printf("g_FanRotationRate  %d r/s \r\n\r\n",uiGetFanRotationRate());
    
    printf("0    %d \r\n", g_tRightWheel.usEncodeBuffer[0]);
    printf("1    %d \r\n", g_tRightWheel.usEncodeBuffer[1]);
    printf("2    %d \r\n", g_tRightWheel.usEncodeBuffer[2]);
    printf("3    %d \r\n", g_tRightWheel.usEncodeBuffer[3]);
    printf("4    %d \r\n", g_tRightWheel.usEncodeBuffer[4]);
} 
/*******************************************************************************
* Function Name  : change_EX_Angle(s16 sExp_Ang,s16 sExp_range,s16 sExp_speed)
* Description    : 改变机器人的期望角度 速度和范围
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vChange_EX_Angle(s16 sExp_Ang,s16 sExp_range,s16 sExp_speed)
{
    g_sExpect_Angle=sExp_Ang-1800;  //期望的角度
    g_sExpect_speed=sExp_speed;//速度 mm/s
    g_sExpect_range=sExp_range; //角度误差范围 +- 1度 单位0.1度
}
/*******************************************************************************
* Function Name  : vChange_PID(s16 P,s16 I,s16 D)
* Description    : 改变机器人的PID系数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
extern float P_V;
extern float I_V;
extern float D_V;
void vChange_PID(s16 P,s16 I,s16 D)
{
    
 P_V           = P/100.0;
 I_V           = I/100.0;
 D_V           = D/100.0;
}
/*******************************************************************************
* Function Name  : xGyroscope_VoluePrintf(void)
* Description    : 打印当前所有姿态传感器的值
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vGyroscope_VoluePrintf(void)
{
    printf("\r\n");
    printf("航向角 %f\r\n",GetGyroValue_Z());
    printf("状态 %d\r\n",eGyro_State);
    
}
/*******************************************************************************
* Function Name  : vAll_UltraSonic_VoluePrintf(void)
* Description    : 打印当前所有超声波通道采集的距离值 单位mm
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void vAll_UltraSonic_VoluePrintf(void)
{
    printf("\r\n\r\n");
    printf("超声波距离 %d\r\n",usGetClashDistance());
    printf("超声波标志 %d\r\n",bGetClashFlag());

}

/*******************************************************************************
* Function Name  : vAll_AD_VoluePrintf(void)
* Description    : 打印当前所有AD通道采集的值
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vAll_AD_VoluePrintf(void)
{
    printf("\r\n");
    printf("4选1adc00 %d \r\n\r\n",g_AdcOriginalValue[0][0]);

    printf("边刷adc03 %d \r\n",g_AdcOriginalValue[0][2]);
    printf("风机adc10 %d \r\n",g_AdcOriginalValue[0][7]);
    printf("左轮adc11 %d \r\n",g_AdcOriginalValue[0][8]);
    printf("右轮adc12 %d \r\n",g_AdcOriginalValue[0][9]);
    printf("滚刷adc13 %d \r\n\r\n",g_AdcOriginalValue[0][10]);
    
    printf("掉落adc04 %d \r\n",g_AdcOriginalValue[0][3]);
    printf("掉落adc07 %d \r\n",g_AdcOriginalValue[0][6]);
    printf("掉落adc14 %d \r\n",g_AdcOriginalValue[0][11]);
    printf("掉落adc15 %d \r\n\r\n",g_AdcOriginalValue[0][12]);
    
    printf("adcAlong_Edge %d \r\n\r\n",g_usAnalogSwitch[adcAlong_Edge]);
 
    printf("电池温度adc02        %d \r\n",g_AdcOriginalValue[0][1]);
    printf("放电电流adc05        %d \r\n",g_AdcOriginalValue[0][4]);
    printf("电量adc06            %d \r\n",g_AdcOriginalValue[0][5]);    
    printf("电池ID 3800-3810adc  %d \r\n",g_usAnalogSwitch[adcBatteryID]);
    printf("充电电流             %d \r\n",g_usAnalogSwitch[adcChargeCurrent]);
    printf("adcRechargeStand     %d \r\n",g_usAnalogSwitch[adcRechargeStand]);
    
    printf("     %d \r\n",ALONG_EDGE_DISTANCE());
    printf("     %d \r\n",uiGetFanRotationRate());
}
/*******************************************************************************
* Function Name  : DROP_AD_Printf(void)
* Description    : 打印掉落传感器AD值
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void DROP_AD_Printf(void)
{
    printf("\r\n");
    printf("Prescaler: %d\r\n",(TIM4->ARR)+1);
    printf("period %d\r\n",(TIM4->PSC)+1);  
    printf("Pulse: %d \r\n",TIM_GetCapture4(TIM4)+1);
    printf("频率 ：%d \r\n" , 72000/( ( TIM4->PSC)+1 ) );
    printf("占空比 %d %% \r\n", ( ((TIM_GetCapture4(TIM4)+1)*100)/ (TIM4->ARR+1) ) );
    printf("\r\n");
    printf("左前AD值   %d \r\n",usGetDropLeftFrontAD());
    printf("左后AD值   %d \r\n",usGetDropLeftBackAD());
    printf("右前AD值   %d \r\n",usGetDropRightFrontAD());
    printf("右后AD值   %d \r\n\r\n",usGetDropRightBackAD());
    printf("\r\n");
    
//    printf("原左后： %d\r\n",LB_DropADValue);
//    printf("原右后： %d\r\n",RB_DropADValue);
//    printf("原左前： %d \r\n",LF_DropADValue);
//    printf("原右前： %d \r\n",RF_DropADValue);
//    printf("\r\n");
    
      
//    printf("左前 : %d \r\n", g_usDROP_DIFFERENT_AD[adFL]);
//    printf("左后 : %d   \r\n", g_usDROP_DIFFERENT_AD[adBL]);
//    printf("右前 : %d  \r\n", g_usDROP_DIFFERENT_AD[adFR]);
//    printf("右后 : %d \r\n", g_usDROP_DIFFERENT_AD[adBR]);
//    printf("\r\n");  

    printf("左前悬空标志 : %d \r\n",tDropcheck.bFrontLFall);
    printf("左后悬空标志   %d \r\n",tDropcheck.bBackLFall);
    printf("右前悬空标志   %d \r\n",tDropcheck.bFrontRFall);
    printf("右后悬空标志   %d \r\n",tDropcheck.bBackRFall);
    printf("任意一个地检检测到悬空    %d \r\n",tDropcheck.bAnyoneFall);
    printf("\r\n");
}

/*******************************************************************************
* Function Name  : vSetDropTime(u16 frequency , u16 cycle )
* Description    : 设置跌落传感器频率占空比
* Input          : frequency 频率 unit：HZ  范围0-65535
                    cycle 占空比 unit：1% 
* Output         : None
* Return         : None
*******************************************************************************/
void vSetDropTime(u16 frequency , u16 cycle )
{
    uint16_t pulse =0;
    pulse=( cycle * 10) ;
    
    TIM4_PWM_FREQUENCY( frequency );
    DROP_T4CH4_PWM_CYCLE( pulse );
    
}
/*******************************************************************************
* Function Name  : vDropWaveformOut(bool enable)
* Description    : 跌落波形输出
* Input          : 开关
* Output         : 波形
* Return         : None
*******************************************************************************/
void vDropWaveformOut(bool enable)
{
    while(enable>0)
    {
        //通道1： 左前
        DataScope_Get_Channel_Data(usGetDropLeftFrontAD(),1);
        //通道2 左后
        DataScope_Get_Channel_Data(usGetDropLeftBackAD(),2);
        //通道3 右前
        DataScope_Get_Channel_Data(usGetDropRightFrontAD(),3);
        //通道4 右后
        DataScope_Get_Channel_Data(usGetDropRightBackAD(),4);
        vWaveFromOut();
        vTaskDelay(10);
    }
        
}
/*******************************************************************************
* Function Name  : FAN_Test(u8 CtlPinSta,u16 Cycle0_499)
* Description    : 风机测试函数
* Input          : CtlPinSta 风机电源控制引脚 1使能 Cycle0_499 PWM占空比 越大越快
* Output         : None
* Return         : None
*******************************************************************************/
void vFAN_Test(u8 CtlPinSta,u16 Cycle0_499)
{
//    if(CtlPinSta)
//        FAN_POWER_UP
//    else     
//        FAN_POWER_DOWN
//    
//    FAN_T5CH2_PWM_CYCLE(Cycle0_499);  
    if(CtlPinSta)
        vSetFanSpeed(Cycle0_499);
    else
        vSetFanSpeed(0);
}

/*******************************************************************************
* Function Name  : vPrintf_ALL_Register_Bp24773
* Description    : 打印bq24773所有寄存器的值
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vPrintf_ALL_Register_Bp24773(void)
{
    printf ("\r\n[00H]%#2x\r\n",ucBp24773_ReadOneByte(0));
    printf ("[01H]%#2x\r\n",ucBp24773_ReadOneByte(1));
    printf ("[02H]%#2x\r\n",ucBp24773_ReadOneByte(2));
    printf ("[03H]%#2x\r\n",ucBp24773_ReadOneByte(3));
    printf ("[04H]%#2x\r\n",ucBp24773_ReadOneByte(4));
    printf ("[05H]%#2x\r\n",ucBp24773_ReadOneByte(5));
    printf ("[06H]%#2x\r\n",ucBp24773_ReadOneByte(6));
    printf ("[07H]%#2x\r\n",ucBp24773_ReadOneByte(7));

    printf ("[09H]%#2x\r\n",ucBp24773_ReadOneByte(9));
    printf ("[0AH]%#2x\r\n",ucBp24773_ReadOneByte(10));
    printf ("[0BH]%#2x\r\n",ucBp24773_ReadOneByte(11));
    printf ("[0CH]%#2x\r\n",ucBp24773_ReadOneByte(12));
    printf ("[0DH]%#2x\r\n",ucBp24773_ReadOneByte(13));
    printf ("[0EH]%#2x\r\n",ucBp24773_ReadOneByte(14));
    printf ("[0FH]%#2x\r\n",ucBp24773_ReadOneByte(15));
    printf ("[10H]%#2x\r\n",ucBp24773_ReadOneByte(16));
    printf ("[11H]%#2x\r\n",ucBp24773_ReadOneByte(CHARGE_OPTION_2_H));  

}
/*******************************************************************************
* Function Name  : vWrite_Bq24773_Register(u8 addr,u8 data)
* Description    : 写bq24773寄存器的值
* Input          : addr ：寄存器地址 data：数据
* Output         : None
* Return         : None
*******************************************************************************/
void vWrite_Bq24773_Register(u8 addr,u8 data)
{
    vBp24773_WriteOneByte(addr,data);
}
/*******************************************************************************
* Function Name  : vWrite_Wheel(u8 sta,u16 cycle)
* Description    : 写驱动轮
* Input          : sta ：使能状态 cycle：周期
* Output         : None
* Return         : None
*******************************************************************************/

void vWrite_Wheel(u8 sta,u16 loccycle)
{
    RIGHT_WHEEL_T3CH3_PWM_CYCLE(loccycle);
    LEFT_WHEEL_T3CH4_PWM_CYCLE(loccycle);
    ROLL_BRUSH_T4CH2_PWM_CYCLE(loccycle);
    SIDE_BRUSH_T4CH3_PWM_CYCLE(loccycle);
    
    CTRL_WHEEL_L=sta;
    CTRL_WHEEL_R=sta;
    SIDE_BRUSH_CTRL=sta;
    ROLL_BRUSH_CTRL=sta;
}
