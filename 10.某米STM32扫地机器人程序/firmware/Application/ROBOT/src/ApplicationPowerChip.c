#include "applicationPowerChip.h"

/*
 * Filename   ：applicationPowerChip.c
 * Author     : pusheng       
 * Version    : 2.1 
  增加了电池任务
  增加了充电功能         
 * Date       : 2019.11.11  2020.1.14
 * Discription : 电源管理芯片bp24773应用函数
    v2.1 ：增加ad电量获取限制
*/	

/*电池电量 0-100*/
u8 ucBattery_Level;

/*是否在充电座上 0=没有 */
bool bInRechargeStand;

/*是否需要充电 0不需要 */
bool bNeedCharge;

/*******************************************************************************
* Function Name  : vPowerChipTask
* Description    : bp24773电源管理芯片任务
* Input          : p 入口参数
* Output         : None
* Return         : None
*******************************************************************************/
void vPowerChipTask(void *p)
{
    OS_TimeStruct PowerChipTime;
    SetNeedCharge(true);//设置需要充电
    while(1)
    {       
        vBatteryLevelUpdate();//电池电量数据更新
        vRechargeStandUpdate();//充电座信息更新
        g_tRobotState.Robotstate.ucRemainBattery = ucGetBatteryLevel();//上传电量
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.InRechargeStand=isInRechargeStand();//上传是否在充电座上
        if(isInRechargeStand() && isNeedCharge())//在充电座上且需要充电
        {
            /*可以直接标记充电状态 然后具体操作交给behaviorManage*/
            if(g_tRobotState.Robotstate.PreviousDirection != zCharging)//如果上次状态不为充电中
            {
                g_tRobotState.Robotstate.eDirection=zCharging;//标记方向为充电
                g_tRobotState.Robotstate.PreviousDirection = zCharging;
            }
            if(TimeOut == MultipleTimeoutCheck(&PowerChipTime,0,30,0))//30s延时
            {
                vBq24773_Set_MAX_Charge_Voltage(Battery4S_16_8v);//设置充电电压为16.8
                vBq24773_Set_Charge_Current(Current1A);//设置充电电流1A
            }
        }else
        {
            //不在充电座上或者不需要充电
            if(g_tRobotState.Robotstate.PreviousDirection == zCharging)//如果上次状态为充电中
            {
                //g_tRobotState.Robotstate.eDirection=zAuto;//去到Auto
                g_tRobotState.Robotstate.PreviousDirection = zCharging;
            }
        }
        vTaskDelay(100);
    }
}    

/*******************************************************************************
* Function Name  : ucGetBatteryLevel
* Description    : 获取电池电量
* Input          : None
* Output         : None
* Return         : 电量 0% ~ 100%
*******************************************************************************/
u8 ucGetBatteryLevel(void)
{
    return ucBattery_Level;
}

/*******************************************************************************
* Function Name  : usGetChargeCurrent
* Description    : 获取充电电流
* Input          : None
* Output         : None
* Return         : 电流adc 0 ~ 1100 一般大于50判断为充电
*******************************************************************************/
u16 usGetChargeCurrent(void)
{
    return g_usAnalogSwitch[adcChargeCurrent];
}
/*******************************************************************************
* Function Name  : isInRechargeStand
* Description    : 是否在充电座上
* Input          : None
* Output         : None
* Return         : 0=没有 1在充电座上
*******************************************************************************/
bool isInRechargeStand(void)
{
    return bInRechargeStand;
}
/*******************************************************************************
* Function Name  : isNeedCharge
* Description    : 是否需要充电
* Input          : None
* Output         : None
* Return         : 0 不需要  1需要
*******************************************************************************/
bool isNeedCharge(void)
{
    return bNeedCharge;
}
/*******************************************************************************
* Function Name  : SetNeedCharge(bool loc_NeedCharge)
* Description    : 设置是否需要充电
* Input          : 0 不需要  1需要
* Output         : None
* Return         : None
*******************************************************************************/
void SetNeedCharge(bool loc_NeedCharge)
{
    bNeedCharge=loc_NeedCharge;
}
/*******************************************************************************
* Function Name  : vBatteryLevelUpdate
* Description    : 更新电池电量
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vBatteryLevelUpdate(void)
{
    /* （当前电量adc值 - 最小电量adc值）*100/ 电量adc范围  */
    ucBattery_Level=\
    (ELECTR_QUANTITY-MIN_BATTER_LEVEL)*100\
    /ELECTRIC_QUANTITY_RANGE;
}
/*******************************************************************************
* Function Name  : vRechargeStandUpdate
* Description    : 更新充电座信息
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vRechargeStandUpdate(void)
{
    if(g_usAnalogSwitch[adcRechargeStand] > 2000)//充电座电流adc检测
        bInRechargeStand=true;
    else
        bInRechargeStand=false;
}
/*******************************************************************************
* Function Name  : vBq24773_Set_MAX_Charge_Voltage(eMax_Charge_Voltage loc_ChargrVoltage)
* Description    : 电池最大充电电压
* Input          : ChargrVoltage 电压枚举类型
* Output         : None
* Return         : None
*******************************************************************************/
void vBq24773_Set_MAX_Charge_Voltage(eMax_Charge_Voltage loc_ChargrVoltage)
{
    /*bit |  function
      0   |  Adds 256mV of charger voltage.
      1   |  512mV
      2   |  1024mV
      3   |  2048mV
      4   |  4096mV
      5   |  8192mV
      6   |  16384mV
      7   |  Not use
    */
    u8 ChargeVol_H;
    
    /*bit |  function
    [0:3] |  Not use
      4   |  Adds 16mV of charger voltage.
      5   |  32mV
      6   |  64mV
      7   |  128mV
    */
    u8 ChargeVol_L;
    
    switch(loc_ChargrVoltage)
    {
        case Battery1S_4_2v://实际电压4096+128=4.224v
            ChargeVol_H=(u8)(1<<4);
            ChargeVol_L=(u8)(1<<7);
            vBp24773_WriteOneByte(MAX_CHARGE_VOLTAGE_L,ChargeVol_L);
            vBp24773_WriteOneByte(MAX_CHARGE_VOLTAGE_H,ChargeVol_H);
            break;
        case Battery2S_8_4v://实际电压8192+256=8.448v
            ChargeVol_H=(u8)(1<<5) |(1<<0);
            ChargeVol_L=(u8)(0);
            vBp24773_WriteOneByte(MAX_CHARGE_VOLTAGE_L,ChargeVol_L);
            vBp24773_WriteOneByte(MAX_CHARGE_VOLTAGE_H,ChargeVol_H);
            break;
        case Battery3S_12_6v://实际电压8192+4096+256+64=12.608v
            ChargeVol_H=(u8)(1<<5)|(1<<4)|(1<<0);
            ChargeVol_L=(u8)(1<<6);
            vBp24773_WriteOneByte(MAX_CHARGE_VOLTAGE_L,ChargeVol_L);
            vBp24773_WriteOneByte(MAX_CHARGE_VOLTAGE_H,ChargeVol_H);
            break;
        case Battery4S_16_8v://实际电压16384+256+128+32=16.8v
            ChargeVol_H=(u8)(1<<6)|(1<<0);
            ChargeVol_L=(u8)(1<<7)|(1<<5);
            vBp24773_WriteOneByte(MAX_CHARGE_VOLTAGE_L,ChargeVol_L);
            vBp24773_WriteOneByte(MAX_CHARGE_VOLTAGE_H,ChargeVol_H);
            break;
        case Battery_15v://实际电压8192+4096+2048+512+128=14.976v
            ChargeVol_H=(u8)(1<<5)|(1<<4)|(1<<3)|(1<<1);
            ChargeVol_L=(u8)(1<<7);
            vBp24773_WriteOneByte(MAX_CHARGE_VOLTAGE_L,ChargeVol_L);
            vBp24773_WriteOneByte(MAX_CHARGE_VOLTAGE_H,ChargeVol_H);
            break;
        case Battery_14v://实际电压8192+4096+1024+512+128+32=13.984V
            ChargeVol_H=(u8)(1<<5)|(1<<4)|(1<<2)|(1<<1);
            ChargeVol_L=(u8)(1<<7)|(1<<5);
            vBp24773_WriteOneByte(MAX_CHARGE_VOLTAGE_L,ChargeVol_L);
            vBp24773_WriteOneByte(MAX_CHARGE_VOLTAGE_H,ChargeVol_H);
            break;
    }
    
}

/*******************************************************************************
* Function Name  : vBq24773_Set_Charge_Current(eCharge_Current loc_ChargeCurrent)
* Description    : 电池充电电流
* Input          : loc_ChargeCurrent 电流枚举类型
* Output         : None
* Return         : None
*******************************************************************************/
void vBq24773_Set_Charge_Current(eCharge_Current loc_ChargeCurrent)
{
    /*bit |  function
      0   |  Adds 256mA of charger Current
      1   |  512mA
      2   |  1024mA
      3   |  2048mA
      4   |  4096mA
    [5:7] |  Not Use
    */
    u8 ChargeCurrent_H;
    
    /*bit |  function
    [0:5] |  Not use
      6   |  64mA
      7   |  128mA
    */
    u8 ChargeCurrent_L;
    
    switch(loc_ChargeCurrent)
    {
        case Current5A://实际电流4096+512+256+128=4992ma
            ChargeCurrent_H=(u8)(1<<4)|(1<<1)|(1<<0);
            ChargeCurrent_L=(u8)(1<<7);
            vBp24773_WriteOneByte(CHARGE_CURRENT_L,ChargeCurrent_L);
            vBp24773_WriteOneByte(CHARGE_CURRENT_H,ChargeCurrent_H);
            break;
        case Current4A://实际电流4096=4096ma
            ChargeCurrent_H=(u8)(1<<4);
            ChargeCurrent_L=(u8)(0);
            vBp24773_WriteOneByte(CHARGE_CURRENT_L,ChargeCurrent_L);
            vBp24773_WriteOneByte(CHARGE_CURRENT_H,ChargeCurrent_H);
            break;
        case Current3A:///实际电流2048+512+256+128+64=3008ma
            ChargeCurrent_H=(u8)(1<<3)|(1<<1)|(1<<0);
            ChargeCurrent_L=(u8)(1<<7)|(1<<6);
            vBp24773_WriteOneByte(CHARGE_CURRENT_L,ChargeCurrent_L);
            vBp24773_WriteOneByte(CHARGE_CURRENT_H,ChargeCurrent_H);
            break;
        case Current2A://实际电流2048=2048ma
            ChargeCurrent_H=(u8)(1<<3);
            ChargeCurrent_L=(u8)(0);
            vBp24773_WriteOneByte(CHARGE_CURRENT_L,ChargeCurrent_L);
            vBp24773_WriteOneByte(CHARGE_CURRENT_H,ChargeCurrent_H);
            break;
        case Current1A://实际电流1024=1024ma
            ChargeCurrent_H=(u8)(1<<2);
            ChargeCurrent_L=(u8)(0);
            vBp24773_WriteOneByte(CHARGE_CURRENT_L,ChargeCurrent_L);
            vBp24773_WriteOneByte(CHARGE_CURRENT_H,ChargeCurrent_H);
            break;
        case Current500MA://实际电流512ma
            ChargeCurrent_H=(u8)(1<<1);
            ChargeCurrent_L=(u8)(0);
            vBp24773_WriteOneByte(CHARGE_CURRENT_L,ChargeCurrent_L);
            vBp24773_WriteOneByte(CHARGE_CURRENT_H,ChargeCurrent_H);
            break;
        case Current256MA://实际电流256ma
            ChargeCurrent_H=(u8)(1<<0);
            ChargeCurrent_L=(u8)(0);
            vBp24773_WriteOneByte(CHARGE_CURRENT_L,ChargeCurrent_L);
            vBp24773_WriteOneByte(CHARGE_CURRENT_H,ChargeCurrent_H);
            break;
        case Current128MA://实际电流128ma
            ChargeCurrent_H=(u8)(0);
            ChargeCurrent_L=(u8)(1<<7);
            vBp24773_WriteOneByte(CHARGE_CURRENT_L,ChargeCurrent_L);
            vBp24773_WriteOneByte(CHARGE_CURRENT_H,ChargeCurrent_H);
            break;
        case Current64MA://实际电流64ma
            ChargeCurrent_H=(u8)(0);
            ChargeCurrent_L=(u8)(1<<6);
            vBp24773_WriteOneByte(CHARGE_CURRENT_L,ChargeCurrent_L);
            vBp24773_WriteOneByte(CHARGE_CURRENT_H,ChargeCurrent_H);
            break;
       
    }
}
/*******************************************************************************
* Function Name  : u8 ucBp24773_ReadOneByte(u8 ReadAddr)
* Description    : bp24773电源管理芯片IIC读取寄存器函数
* Input          : ReadAddr 寄存器地址
* Output         : None
* Return         : 读到的数据
*******************************************************************************/
u8 ucBp24773_ReadOneByte(u8 ReadAddr)
{				  
	u8 temp=0;		  	    																 
    IIC_Start();  
    vIIC_Send_Byte(W_BP24773_DEVICE_ID);   //发送器件地址0XD4,伪写数据 	 

	ucIIC_Wait_Ack(); 
    vIIC_Send_Byte(ReadAddr);   //发送寄存器地址
	ucIIC_Wait_Ack();	    
	IIC_Start();  	 	    
	vIIC_Send_Byte(R_BP24773_DEVICE_ID);           //进入接收模式			   
	ucIIC_Wait_Ack();	 
    temp=ucIIC_Read_Byte(0);		   
    IIC_Stop();//产生一个停止条件	    
	return temp;
}
/*******************************************************************************
* Function Name  : vBp24773_WriteOneByte(u8 WriteAddr,u8 DataToWrite)
* Description    : bp24773电源管理芯片IIC写入寄存器函数
* Input          : WriteAddr 寄存器地址 DataToWrite 写入的数据
* Output         : None
* Return         : None
*******************************************************************************/
void vBp24773_WriteOneByte(u8 WriteAddr,u8 DataToWrite)
{				   	  	    																 
    IIC_Start();  

	vIIC_Send_Byte(W_BP24773_DEVICE_ID);   //发送器件地址写数据 
	 
	ucIIC_Wait_Ack();	   
    vIIC_Send_Byte(WriteAddr);   //发送低地址
	ucIIC_Wait_Ack(); 	 										  		   
	vIIC_Send_Byte(DataToWrite);     //发送字节							   
	ucIIC_Wait_Ack();  		    	   
    IIC_Stop();//产生一个停止条件 
	delay_ms(10);	 
}

