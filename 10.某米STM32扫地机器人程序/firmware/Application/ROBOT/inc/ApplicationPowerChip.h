#ifndef __APP_POWER_CHIP_H
#define __APP_POWER_CHIP_H


#include "SystemTime.h"
#include "myiic.h"
#include "applicationadc.h"
#include "main.h"


#define W_BP24773_DEVICE_ID  0Xd4 //bp24773 IIC д��ַ
#define R_BP24773_DEVICE_ID  0Xd5 //bp24773 IIC ����ַ


/* BQ24733�Ĵ�����ַ*/
#define CHIP_ID_REGISTER  0X09  //be should 0x41 only read

#define CHARGE_OPTION_0_L  0X00  //
#define CHARGE_OPTION_0_H  0X01  //
#define CHARGE_OPTION_1_L  0X02  //
#define CHARGE_OPTION_1_H  0X03  //
#define CHARGE_OPTION_2_L  0X10  //
#define CHARGE_OPTION_2_H  0X11  //

#define PROCHOT_OPTION_0_L  0X04  //
#define PROCHOT_OPTION_0_H  0X05  //
#define PROCHOT_OPTION_1_L  0X06  //
#define PROCHOT_OPTION_1_H  0X07  //

#define CHARGE_CURRENT_L   0X0A
#define CHARGE_CURRENT_H   0X0B

#define MAX_CHARGE_VOLTAGE_L   0X0C
#define MAX_CHARGE_VOLTAGE_H   0X0D

#define MIN_SYTEM_VOLTAGE   0X0E

#define INPUT_CURRENT   0X0F

/* ��ص����궨�� */
#define MAX_BATTER_LEVEL (3800)//������adcֵ
#define MIN_BATTER_LEVEL (2800)
#define ELECTRIC_QUANTITY_RANGE (MAX_BATTER_LEVEL-MIN_BATTER_LEVEL)

/*����ADֵ��Χ����*/
#define ELECTR_QUANTITY  (g_usInChipAdcChannelAverageValue[adcElectricQuantity]>MAX_BATTER_LEVEL?MAX_BATTER_LEVEL:\
                         (g_usInChipAdcChannelAverageValue[adcElectricQuantity]<MIN_BATTER_LEVEL?MIN_BATTER_LEVEL:\
                          g_usInChipAdcChannelAverageValue[adcElectricQuantity]) )
/*����ö��*/

//������ѹ
typedef enum 
{
    Battery1S_4_2v    =0,//��Ƭ﮵�� ����4.2V
    Battery2S_8_4v    =1,
    Battery3S_12_6v   =2,
    Battery4S_16_8v   =3,
    Battery_15v       =4,
    Battery_14v       =5,
}eMax_Charge_Voltage;
//������
typedef enum 
{
    Current5A       =0,//������ 
    Current4A       =1,
    Current3A       =2,
    Current2A       =3,
    Current1A       =4,
    Current500MA    =5,
    Current256MA    =6,   
    Current128MA    =7,
    Current64MA     =8,    
}eCharge_Current;

/*public*/
void vPowerChipTask(void *p);
u8 ucGetBatteryLevel(void);//�õ���ص���
u16 usGetChargeCurrent(void);//�õ�������
bool isNeedCharge(void);//�ж��Ƿ���
bool isInRechargeStand(void);//�ж��Ƿ��ڳ������
void SetNeedCharge(bool loc_NeedCharge);//�����Ƿ���Ҫ���
void vRechargeStandUpdate(void);//�������Ϣ����

/*private*/
void vBatteryLevelUpdate(void);
void vBq24773_Set_Charge_Current(eCharge_Current loc_ChargeCurrent);
void vBq24773_Set_MAX_Charge_Voltage(eMax_Charge_Voltage loc_ChargrVoltage);

void vBp24773_WriteOneByte(u8 WriteAddr,u8 DataToWrite);
u8 ucBp24773_ReadOneByte(u8 ReadAddr);



#endif

