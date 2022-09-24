
/*
    ����ʱ�䣺2019/10/10
    ������  ��pusheng
    �������������ļ��ڵ����к��� ���Ǹ�USMART������
*/

#include "testfun.h"
/*******************************************************************************
* Function Name  : vPrintf_ALL_RobotSta(void)
* Description    : ��ӡ������״̬����Ϣ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
char *BufIndex[11]={"ֹͣ","ǰ��","���˺���ת","���˺���ת","��ת","��ת","ԭ����ת","�Զ�","�س�","���ڳ��","�ر�"};
void vPrintf_ALL_RobotSta(void )
{
    printf("\r\n");
    printf("���� :%s \r\n",BufIndex[g_tRobotState.Robotstate.eDirection]);
    printf("���� %d %%\r\n",g_tRobotState.Robotstate.ucRemainBattery);
    printf("�쳣 %d \r\n",g_tRobotState.Robotstate.unionAbnormal.AbnormalSta);
    printf("���� %d \r\n",g_tRobotState.Robotstate.unionEEROR.ErrorSta);
    
}
/*******************************************************************************
* Function Name  : printf_ALL_wheel_Data(void)
* Description    : ��ӡ�����ٶȵ���Ϣ
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
* Description    : �ı�����˵������Ƕ� �ٶȺͷ�Χ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vChange_EX_Angle(s16 sExp_Ang,s16 sExp_range,s16 sExp_speed)
{
    g_sExpect_Angle=sExp_Ang-1800;  //�����ĽǶ�
    g_sExpect_speed=sExp_speed;//�ٶ� mm/s
    g_sExpect_range=sExp_range; //�Ƕ���Χ +- 1�� ��λ0.1��
}
/*******************************************************************************
* Function Name  : vChange_PID(s16 P,s16 I,s16 D)
* Description    : �ı�����˵�PIDϵ��
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
* Description    : ��ӡ��ǰ������̬��������ֵ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vGyroscope_VoluePrintf(void)
{
    printf("\r\n");
    printf("����� %f\r\n",GetGyroValue_Z());
    printf("״̬ %d\r\n",eGyro_State);
    
}
/*******************************************************************************
* Function Name  : vAll_UltraSonic_VoluePrintf(void)
* Description    : ��ӡ��ǰ���г�����ͨ���ɼ��ľ���ֵ ��λmm
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void vAll_UltraSonic_VoluePrintf(void)
{
    printf("\r\n\r\n");
    printf("���������� %d\r\n",usGetClashDistance());
    printf("��������־ %d\r\n",bGetClashFlag());

}

/*******************************************************************************
* Function Name  : vAll_AD_VoluePrintf(void)
* Description    : ��ӡ��ǰ����ADͨ���ɼ���ֵ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vAll_AD_VoluePrintf(void)
{
    printf("\r\n");
    printf("4ѡ1adc00 %d \r\n\r\n",g_AdcOriginalValue[0][0]);

    printf("��ˢadc03 %d \r\n",g_AdcOriginalValue[0][2]);
    printf("���adc10 %d \r\n",g_AdcOriginalValue[0][7]);
    printf("����adc11 %d \r\n",g_AdcOriginalValue[0][8]);
    printf("����adc12 %d \r\n",g_AdcOriginalValue[0][9]);
    printf("��ˢadc13 %d \r\n\r\n",g_AdcOriginalValue[0][10]);
    
    printf("����adc04 %d \r\n",g_AdcOriginalValue[0][3]);
    printf("����adc07 %d \r\n",g_AdcOriginalValue[0][6]);
    printf("����adc14 %d \r\n",g_AdcOriginalValue[0][11]);
    printf("����adc15 %d \r\n\r\n",g_AdcOriginalValue[0][12]);
    
    printf("adcAlong_Edge %d \r\n\r\n",g_usAnalogSwitch[adcAlong_Edge]);
 
    printf("����¶�adc02        %d \r\n",g_AdcOriginalValue[0][1]);
    printf("�ŵ����adc05        %d \r\n",g_AdcOriginalValue[0][4]);
    printf("����adc06            %d \r\n",g_AdcOriginalValue[0][5]);    
    printf("���ID 3800-3810adc  %d \r\n",g_usAnalogSwitch[adcBatteryID]);
    printf("������             %d \r\n",g_usAnalogSwitch[adcChargeCurrent]);
    printf("adcRechargeStand     %d \r\n",g_usAnalogSwitch[adcRechargeStand]);
    
    printf("     %d \r\n",ALONG_EDGE_DISTANCE());
    printf("     %d \r\n",uiGetFanRotationRate());
}
/*******************************************************************************
* Function Name  : DROP_AD_Printf(void)
* Description    : ��ӡ���䴫����ADֵ
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
    printf("Ƶ�� ��%d \r\n" , 72000/( ( TIM4->PSC)+1 ) );
    printf("ռ�ձ� %d %% \r\n", ( ((TIM_GetCapture4(TIM4)+1)*100)/ (TIM4->ARR+1) ) );
    printf("\r\n");
    printf("��ǰADֵ   %d \r\n",usGetDropLeftFrontAD());
    printf("���ADֵ   %d \r\n",usGetDropLeftBackAD());
    printf("��ǰADֵ   %d \r\n",usGetDropRightFrontAD());
    printf("�Һ�ADֵ   %d \r\n\r\n",usGetDropRightBackAD());
    printf("\r\n");
    
//    printf("ԭ��� %d\r\n",LB_DropADValue);
//    printf("ԭ�Һ� %d\r\n",RB_DropADValue);
//    printf("ԭ��ǰ�� %d \r\n",LF_DropADValue);
//    printf("ԭ��ǰ�� %d \r\n",RF_DropADValue);
//    printf("\r\n");
    
      
//    printf("��ǰ : %d \r\n", g_usDROP_DIFFERENT_AD[adFL]);
//    printf("��� : %d   \r\n", g_usDROP_DIFFERENT_AD[adBL]);
//    printf("��ǰ : %d  \r\n", g_usDROP_DIFFERENT_AD[adFR]);
//    printf("�Һ� : %d \r\n", g_usDROP_DIFFERENT_AD[adBR]);
//    printf("\r\n");  

    printf("��ǰ���ձ�־ : %d \r\n",tDropcheck.bFrontLFall);
    printf("������ձ�־   %d \r\n",tDropcheck.bBackLFall);
    printf("��ǰ���ձ�־   %d \r\n",tDropcheck.bFrontRFall);
    printf("�Һ����ձ�־   %d \r\n",tDropcheck.bBackRFall);
    printf("����һ���ؼ��⵽����    %d \r\n",tDropcheck.bAnyoneFall);
    printf("\r\n");
}

/*******************************************************************************
* Function Name  : vSetDropTime(u16 frequency , u16 cycle )
* Description    : ���õ��䴫����Ƶ��ռ�ձ�
* Input          : frequency Ƶ�� unit��HZ  ��Χ0-65535
                    cycle ռ�ձ� unit��1% 
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
* Description    : ���䲨�����
* Input          : ����
* Output         : ����
* Return         : None
*******************************************************************************/
void vDropWaveformOut(bool enable)
{
    while(enable>0)
    {
        //ͨ��1�� ��ǰ
        DataScope_Get_Channel_Data(usGetDropLeftFrontAD(),1);
        //ͨ��2 ���
        DataScope_Get_Channel_Data(usGetDropLeftBackAD(),2);
        //ͨ��3 ��ǰ
        DataScope_Get_Channel_Data(usGetDropRightFrontAD(),3);
        //ͨ��4 �Һ�
        DataScope_Get_Channel_Data(usGetDropRightBackAD(),4);
        vWaveFromOut();
        vTaskDelay(10);
    }
        
}
/*******************************************************************************
* Function Name  : FAN_Test(u8 CtlPinSta,u16 Cycle0_499)
* Description    : ������Ժ���
* Input          : CtlPinSta �����Դ�������� 1ʹ�� Cycle0_499 PWMռ�ձ� Խ��Խ��
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
* Description    : ��ӡbq24773���мĴ�����ֵ
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
* Description    : дbq24773�Ĵ�����ֵ
* Input          : addr ���Ĵ�����ַ data������
* Output         : None
* Return         : None
*******************************************************************************/
void vWrite_Bq24773_Register(u8 addr,u8 data)
{
    vBp24773_WriteOneByte(addr,data);
}
/*******************************************************************************
* Function Name  : vWrite_Wheel(u8 sta,u16 cycle)
* Description    : д������
* Input          : sta ��ʹ��״̬ cycle������
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
