#include "usmart.h"
#include "usmart_str.h"
////////////////////////////用户配置区///////////////////////////////////////////////
//这下面要包含所用到的函数所申明的头文件(用户自己添加) 

#include "testfun.h"

#include "applicationPowerChip.h"

//函数名列表初始化(用户自己添加)
//用户直接在这里输入要执行的函数名及其查找串
struct _m_usmart_nametab usmart_nametab[]=
{
#if USMART_USE_WRFUNS==1 	//如果使能了读写操作
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
//函数控制管理器初始化
//得到各个受控函数的名字
//得到函数总数量
struct _m_usmart_dev usmart_dev=
{
	usmart_nametab,
	usmart_init,
	usmart_cmd_rec,
	usmart_exe,
	usmart_scan,
	sizeof(usmart_nametab)/sizeof(struct _m_usmart_nametab),//函数数量
	0,	  	//参数数量
	0,	 	//函数ID
	1,		//参数显示类型,0,10进制;1,16进制
	0,		//参数类型.bitx:,0,数字;1,字符串	    
	0,	  	//每个参数的长度暂存表,需要MAX_PARM个0初始化
	0,		//函数的参数,需要PARM_LEN个0初始化
};   



















