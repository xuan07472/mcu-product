#include "applicationPowerChip.h"

/*
 * Filename   ��applicationPowerChip.c
 * Author     : pusheng       
 * Version    : 2.1 
  �����˵������
  �����˳�繦��         
 * Date       : 2019.11.11  2020.1.14
 * Discription : ��Դ����оƬbp24773Ӧ�ú���
    v2.1 ������ad������ȡ����
*/	

/*��ص��� 0-100*/
u8 ucBattery_Level;

/*�Ƿ��ڳ������ 0=û�� */
bool bInRechargeStand;

/*�Ƿ���Ҫ��� 0����Ҫ */
bool bNeedCharge;

/*******************************************************************************
* Function Name  : vPowerChipTask
* Description    : bp24773��Դ����оƬ����
* Input          : p ��ڲ���
* Output         : None
* Return         : None
*******************************************************************************/
void vPowerChipTask(void *p)
{
    OS_TimeStruct PowerChipTime;
    SetNeedCharge(true);//������Ҫ���
    while(1)
    {       
        vBatteryLevelUpdate();//��ص������ݸ���
        vRechargeStandUpdate();//�������Ϣ����
        g_tRobotState.Robotstate.ucRemainBattery = ucGetBatteryLevel();//�ϴ�����
        g_tRobotState.Robotstate.unionAbnormal.AbnIden.InRechargeStand=isInRechargeStand();//�ϴ��Ƿ��ڳ������
        if(isInRechargeStand() && isNeedCharge())//�ڳ����������Ҫ���
        {
            /*����ֱ�ӱ�ǳ��״̬ Ȼ������������behaviorManage*/
            if(g_tRobotState.Robotstate.PreviousDirection != zCharging)//����ϴ�״̬��Ϊ�����
            {
                g_tRobotState.Robotstate.eDirection=zCharging;//��Ƿ���Ϊ���
                g_tRobotState.Robotstate.PreviousDirection = zCharging;
            }
            if(TimeOut == MultipleTimeoutCheck(&PowerChipTime,0,30,0))//30s��ʱ
            {
                vBq24773_Set_MAX_Charge_Voltage(Battery4S_16_8v);//���ó���ѹΪ16.8
                vBq24773_Set_Charge_Current(Current1A);//���ó�����1A
            }
        }else
        {
            //���ڳ�����ϻ��߲���Ҫ���
            if(g_tRobotState.Robotstate.PreviousDirection == zCharging)//����ϴ�״̬Ϊ�����
            {
                //g_tRobotState.Robotstate.eDirection=zAuto;//ȥ��Auto
                g_tRobotState.Robotstate.PreviousDirection = zCharging;
            }
        }
        vTaskDelay(100);
    }
}    

/*******************************************************************************
* Function Name  : ucGetBatteryLevel
* Description    : ��ȡ��ص���
* Input          : None
* Output         : None
* Return         : ���� 0% ~ 100%
*******************************************************************************/
u8 ucGetBatteryLevel(void)
{
    return ucBattery_Level;
}

/*******************************************************************************
* Function Name  : usGetChargeCurrent
* Description    : ��ȡ������
* Input          : None
* Output         : None
* Return         : ����adc 0 ~ 1100 һ�����50�ж�Ϊ���
*******************************************************************************/
u16 usGetChargeCurrent(void)
{
    return g_usAnalogSwitch[adcChargeCurrent];
}
/*******************************************************************************
* Function Name  : isInRechargeStand
* Description    : �Ƿ��ڳ������
* Input          : None
* Output         : None
* Return         : 0=û�� 1�ڳ������
*******************************************************************************/
bool isInRechargeStand(void)
{
    return bInRechargeStand;
}
/*******************************************************************************
* Function Name  : isNeedCharge
* Description    : �Ƿ���Ҫ���
* Input          : None
* Output         : None
* Return         : 0 ����Ҫ  1��Ҫ
*******************************************************************************/
bool isNeedCharge(void)
{
    return bNeedCharge;
}
/*******************************************************************************
* Function Name  : SetNeedCharge(bool loc_NeedCharge)
* Description    : �����Ƿ���Ҫ���
* Input          : 0 ����Ҫ  1��Ҫ
* Output         : None
* Return         : None
*******************************************************************************/
void SetNeedCharge(bool loc_NeedCharge)
{
    bNeedCharge=loc_NeedCharge;
}
/*******************************************************************************
* Function Name  : vBatteryLevelUpdate
* Description    : ���µ�ص���
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vBatteryLevelUpdate(void)
{
    /* ����ǰ����adcֵ - ��С����adcֵ��*100/ ����adc��Χ  */
    ucBattery_Level=\
    (ELECTR_QUANTITY-MIN_BATTER_LEVEL)*100\
    /ELECTRIC_QUANTITY_RANGE;
}
/*******************************************************************************
* Function Name  : vRechargeStandUpdate
* Description    : ���³������Ϣ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vRechargeStandUpdate(void)
{
    if(g_usAnalogSwitch[adcRechargeStand] > 2000)//���������adc���
        bInRechargeStand=true;
    else
        bInRechargeStand=false;
}
/*******************************************************************************
* Function Name  : vBq24773_Set_MAX_Charge_Voltage(eMax_Charge_Voltage loc_ChargrVoltage)
* Description    : ���������ѹ
* Input          : ChargrVoltage ��ѹö������
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
        case Battery1S_4_2v://ʵ�ʵ�ѹ4096+128=4.224v
            ChargeVol_H=(u8)(1<<4);
            ChargeVol_L=(u8)(1<<7);
            vBp24773_WriteOneByte(MAX_CHARGE_VOLTAGE_L,ChargeVol_L);
            vBp24773_WriteOneByte(MAX_CHARGE_VOLTAGE_H,ChargeVol_H);
            break;
        case Battery2S_8_4v://ʵ�ʵ�ѹ8192+256=8.448v
            ChargeVol_H=(u8)(1<<5) |(1<<0);
            ChargeVol_L=(u8)(0);
            vBp24773_WriteOneByte(MAX_CHARGE_VOLTAGE_L,ChargeVol_L);
            vBp24773_WriteOneByte(MAX_CHARGE_VOLTAGE_H,ChargeVol_H);
            break;
        case Battery3S_12_6v://ʵ�ʵ�ѹ8192+4096+256+64=12.608v
            ChargeVol_H=(u8)(1<<5)|(1<<4)|(1<<0);
            ChargeVol_L=(u8)(1<<6);
            vBp24773_WriteOneByte(MAX_CHARGE_VOLTAGE_L,ChargeVol_L);
            vBp24773_WriteOneByte(MAX_CHARGE_VOLTAGE_H,ChargeVol_H);
            break;
        case Battery4S_16_8v://ʵ�ʵ�ѹ16384+256+128+32=16.8v
            ChargeVol_H=(u8)(1<<6)|(1<<0);
            ChargeVol_L=(u8)(1<<7)|(1<<5);
            vBp24773_WriteOneByte(MAX_CHARGE_VOLTAGE_L,ChargeVol_L);
            vBp24773_WriteOneByte(MAX_CHARGE_VOLTAGE_H,ChargeVol_H);
            break;
        case Battery_15v://ʵ�ʵ�ѹ8192+4096+2048+512+128=14.976v
            ChargeVol_H=(u8)(1<<5)|(1<<4)|(1<<3)|(1<<1);
            ChargeVol_L=(u8)(1<<7);
            vBp24773_WriteOneByte(MAX_CHARGE_VOLTAGE_L,ChargeVol_L);
            vBp24773_WriteOneByte(MAX_CHARGE_VOLTAGE_H,ChargeVol_H);
            break;
        case Battery_14v://ʵ�ʵ�ѹ8192+4096+1024+512+128+32=13.984V
            ChargeVol_H=(u8)(1<<5)|(1<<4)|(1<<2)|(1<<1);
            ChargeVol_L=(u8)(1<<7)|(1<<5);
            vBp24773_WriteOneByte(MAX_CHARGE_VOLTAGE_L,ChargeVol_L);
            vBp24773_WriteOneByte(MAX_CHARGE_VOLTAGE_H,ChargeVol_H);
            break;
    }
    
}

/*******************************************************************************
* Function Name  : vBq24773_Set_Charge_Current(eCharge_Current loc_ChargeCurrent)
* Description    : ��س�����
* Input          : loc_ChargeCurrent ����ö������
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
        case Current5A://ʵ�ʵ���4096+512+256+128=4992ma
            ChargeCurrent_H=(u8)(1<<4)|(1<<1)|(1<<0);
            ChargeCurrent_L=(u8)(1<<7);
            vBp24773_WriteOneByte(CHARGE_CURRENT_L,ChargeCurrent_L);
            vBp24773_WriteOneByte(CHARGE_CURRENT_H,ChargeCurrent_H);
            break;
        case Current4A://ʵ�ʵ���4096=4096ma
            ChargeCurrent_H=(u8)(1<<4);
            ChargeCurrent_L=(u8)(0);
            vBp24773_WriteOneByte(CHARGE_CURRENT_L,ChargeCurrent_L);
            vBp24773_WriteOneByte(CHARGE_CURRENT_H,ChargeCurrent_H);
            break;
        case Current3A:///ʵ�ʵ���2048+512+256+128+64=3008ma
            ChargeCurrent_H=(u8)(1<<3)|(1<<1)|(1<<0);
            ChargeCurrent_L=(u8)(1<<7)|(1<<6);
            vBp24773_WriteOneByte(CHARGE_CURRENT_L,ChargeCurrent_L);
            vBp24773_WriteOneByte(CHARGE_CURRENT_H,ChargeCurrent_H);
            break;
        case Current2A://ʵ�ʵ���2048=2048ma
            ChargeCurrent_H=(u8)(1<<3);
            ChargeCurrent_L=(u8)(0);
            vBp24773_WriteOneByte(CHARGE_CURRENT_L,ChargeCurrent_L);
            vBp24773_WriteOneByte(CHARGE_CURRENT_H,ChargeCurrent_H);
            break;
        case Current1A://ʵ�ʵ���1024=1024ma
            ChargeCurrent_H=(u8)(1<<2);
            ChargeCurrent_L=(u8)(0);
            vBp24773_WriteOneByte(CHARGE_CURRENT_L,ChargeCurrent_L);
            vBp24773_WriteOneByte(CHARGE_CURRENT_H,ChargeCurrent_H);
            break;
        case Current500MA://ʵ�ʵ���512ma
            ChargeCurrent_H=(u8)(1<<1);
            ChargeCurrent_L=(u8)(0);
            vBp24773_WriteOneByte(CHARGE_CURRENT_L,ChargeCurrent_L);
            vBp24773_WriteOneByte(CHARGE_CURRENT_H,ChargeCurrent_H);
            break;
        case Current256MA://ʵ�ʵ���256ma
            ChargeCurrent_H=(u8)(1<<0);
            ChargeCurrent_L=(u8)(0);
            vBp24773_WriteOneByte(CHARGE_CURRENT_L,ChargeCurrent_L);
            vBp24773_WriteOneByte(CHARGE_CURRENT_H,ChargeCurrent_H);
            break;
        case Current128MA://ʵ�ʵ���128ma
            ChargeCurrent_H=(u8)(0);
            ChargeCurrent_L=(u8)(1<<7);
            vBp24773_WriteOneByte(CHARGE_CURRENT_L,ChargeCurrent_L);
            vBp24773_WriteOneByte(CHARGE_CURRENT_H,ChargeCurrent_H);
            break;
        case Current64MA://ʵ�ʵ���64ma
            ChargeCurrent_H=(u8)(0);
            ChargeCurrent_L=(u8)(1<<6);
            vBp24773_WriteOneByte(CHARGE_CURRENT_L,ChargeCurrent_L);
            vBp24773_WriteOneByte(CHARGE_CURRENT_H,ChargeCurrent_H);
            break;
       
    }
}
/*******************************************************************************
* Function Name  : u8 ucBp24773_ReadOneByte(u8 ReadAddr)
* Description    : bp24773��Դ����оƬIIC��ȡ�Ĵ�������
* Input          : ReadAddr �Ĵ�����ַ
* Output         : None
* Return         : ����������
*******************************************************************************/
u8 ucBp24773_ReadOneByte(u8 ReadAddr)
{				  
	u8 temp=0;		  	    																 
    IIC_Start();  
    vIIC_Send_Byte(W_BP24773_DEVICE_ID);   //����������ַ0XD4,αд���� 	 

	ucIIC_Wait_Ack(); 
    vIIC_Send_Byte(ReadAddr);   //���ͼĴ�����ַ
	ucIIC_Wait_Ack();	    
	IIC_Start();  	 	    
	vIIC_Send_Byte(R_BP24773_DEVICE_ID);           //�������ģʽ			   
	ucIIC_Wait_Ack();	 
    temp=ucIIC_Read_Byte(0);		   
    IIC_Stop();//����һ��ֹͣ����	    
	return temp;
}
/*******************************************************************************
* Function Name  : vBp24773_WriteOneByte(u8 WriteAddr,u8 DataToWrite)
* Description    : bp24773��Դ����оƬIICд��Ĵ�������
* Input          : WriteAddr �Ĵ�����ַ DataToWrite д�������
* Output         : None
* Return         : None
*******************************************************************************/
void vBp24773_WriteOneByte(u8 WriteAddr,u8 DataToWrite)
{				   	  	    																 
    IIC_Start();  

	vIIC_Send_Byte(W_BP24773_DEVICE_ID);   //����������ַд���� 
	 
	ucIIC_Wait_Ack();	   
    vIIC_Send_Byte(WriteAddr);   //���͵͵�ַ
	ucIIC_Wait_Ack(); 	 										  		   
	vIIC_Send_Byte(DataToWrite);     //�����ֽ�							   
	ucIIC_Wait_Ack();  		    	   
    IIC_Stop();//����һ��ֹͣ���� 
	delay_ms(10);	 
}

