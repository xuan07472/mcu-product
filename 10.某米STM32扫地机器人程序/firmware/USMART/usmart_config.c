#include "usmart.h"
#include "usmart_str.h"
////////////////////////////�û�������///////////////////////////////////////////////
//������Ҫ�������õ��ĺ�����������ͷ�ļ�(�û��Լ����) 

#include "testfun.h"

#include "applicationPowerChip.h"

//�������б��ʼ��(�û��Լ����)
//�û�ֱ������������Ҫִ�еĺ�����������Ҵ�
struct _m_usmart_nametab usmart_nametab[]=
{
#if USMART_USE_WRFUNS==1 	//���ʹ���˶�д����
	(void*)read_addr,"u32 read_addr(u32 addr)",
	(void*)write_addr,"void write_addr(u32 addr,u32 val)",	 
#endif		
    (void*)vFAN_Test,"void vFAN_Test(u8 CtlPinSta,u16 Cycle0_499)",
    (void*)vAll_AD_VoluePrintf,"void vAll_AD_VoluePrintf(void)",
    (void*)vPrintf_ALL_Register_Bp24773,"void vPrintf_ALL_Register_Bp24773(void)",
    (void*)vWrite_Bq24773_Register,"void vWrite_Bq24773_Register(u8 addr,u8 data)",   
        
    (void*)vBq24773_Set_Charge_Current,"void vBq24773_Set_Charge_Current(eCharge_Current loc_ChargeCurrent)",
    (void*)vBq24773_Set_MAX_Charge_Voltage,"void vBq24773_Set_MAX_Charge_Voltage(eMax_Charge_Voltage loc_ChargrVoltage)",
    (void*)vWrite_Wheel,"void vWrite_Wheel(u8 sta,u16 cycle)",
    (void*)vPrintf_ALL_wheel_Data,"void vPrintf_ALL_wheel_Data(void )",      
    (void*)vChange_PID,"void vChange_PID(s16 P,s16 I,s16 D)",   
    (void*)vChange_PID,"void vChange_PID(s16 P,s16 I,s16 D)",    
    (void*)vGyroscope_VoluePrintf,"void vGyroscope_VoluePrintf(void)", 
    (void*)DROP_AD_Printf,"void DROP_AD_Printf(void)",  
    (void*)vDropWaveformOut,"void vDropWaveformOut(bool enable)",
    (void*)vSetDropTime,"void vSetDropTime(u16 frequency , u16 cycle )",
    (void*)vChange_EX_Angle,"void vChange_EX_Angle(s16 sExp_Ang,s16 sExp_range,s16 sExp_speed)",
    (void*)vAll_UltraSonic_VoluePrintf,"void vAll_UltraSonic_VoluePrintf(void)",       
    (void*)vPrintf_ALL_RobotSta,"void vPrintf_ALL_RobotSta(void ) ", 
 
};						  
///////////////////////////////////END///////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//�������ƹ�������ʼ��
//�õ������ܿغ���������
//�õ�����������
struct _m_usmart_dev usmart_dev=
{
	usmart_nametab,
	usmart_init,
	usmart_cmd_rec,
	usmart_exe,
	usmart_scan,
	sizeof(usmart_nametab)/sizeof(struct _m_usmart_nametab),//��������
	0,	  	//��������
	0,	 	//����ID
	1,		//������ʾ����,0,10����;1,16����
	0,		//��������.bitx:,0,����;1,�ַ���	    
	0,	  	//ÿ�������ĳ����ݴ��,��ҪMAX_PARM��0��ʼ��
	0,		//�����Ĳ���,��ҪPARM_LEN��0��ʼ��
};   



















