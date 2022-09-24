/**
 * @Filename   ：bmi160.c
 * @Author     : HuangWentao       
 * @Version    : 1.0
 * @Date       : 2019/11/14
 * @Discription : 用SPI读取陀螺仪数据
 * @hardware connect:
 *          -------------TIM6------------------------
 */
#include "BMI160.h"

#define  BMI160CHIP_ID 209	//0XD1 BMI160芯片ID

struct anglefloat_t tGyro_Offset;//角速度【XYZ】轴的偏移量
enum GyroState_e eGyro_State = GYRO_INIT;//陀螺仪状态【初始化/计算偏移量/工作中】
extern u8 GyroUpdateCNT;

u8 ucToleranceFlag=RESET;//陀螺计算偏移公差标志【SET表示数据正常】
u8 ucToleranceFailFlag=RESET;//公差超过阈值后处理的标志位【SET表示数据处理完】





//陀螺仪任务
void vGyroscopse_task(void *pvParameters)
{

	eGyro_State = GYRO_INIT;
	while(1)
	{
			
		if(eGyro_State == GYRO_INIT )//初始化BMI160
		{
			BMI160_Init();
		}
		else if(eGyro_State == GYRO_GETOFFSET)//计算零点漂移
		{
			BMI160_SPI_GetOffset();
		}
		else if(eGyro_State == GYRO_WORKING)//进入运行状态，5ms更新一次数据
		{
			BMI160_SPI_GetGyro();//获取陀螺仪角速度数据
			BMI160_SPI_GetACC();//获取陀螺仪加速度数据
            delay_ms(5);
		}

	
	}
}




/**
  * @Description  获取重力加速度计数据,最终可得到扫地机左右前后倾斜的角度（-90至90°）
  * @param  无
  * @retval 无
  */
void BMI160_SPI_GetACC(void)
{
		unsigned short x,y,z;
		signed short ACC_X,ACC_Y,ACC_Z;
	
		signed char Angle_x = 0;
		signed char Angle_y = 0;
		/*设置加速度计为正常模式*/
		BMI160_WriteOneByteToSlave(0x7e,0x11);
		delay_us(5);	
		/*X轴数据*/
		x = BMI160_ReadOneByteFromSlave(0x12);
		x = (BMI160_ReadOneByteFromSlave(0x13)<<8)|x;
		ACC_X = (signed short)x;
		//ACC_X = (ACC_X*9.8/(0X7fff/2));	//转换为1g的公式
	
        /*Y轴数据*/
        y = BMI160_ReadOneByteFromSlave(0x14);
		y = (BMI160_ReadOneByteFromSlave(0x15)<<8)|y;
		ACC_Y =(signed short)y;
		//ACC_Y = (ACC_Y*9.8/(0X7fff/2));
		
        /*Z轴数据*/
        z = BMI160_ReadOneByteFromSlave(0x16);
		z = (BMI160_ReadOneByteFromSlave(0x17)<<8)|z;
		ACC_Z = (signed short)z;
		//ACC_Z = (ACC_Z*9.8/(0X7fff/2));

		/*转换为角度(range：-180°~180°)*/
		Angle_x = atan2(ACC_X,ACC_Z)*180/3.14159;
		Angle_y = atan2(ACC_Y,ACC_Z)*180/3.14159;
		
		/*更新倾斜角*/
		SetAccValue_X(Angle_x);
		SetAccValue_Y(Angle_y);
}



/**
  * @Description  获取角速度数据，得到偏航角度和当前实时角度（-179.9-至180°）
  * @param  无
  * @retval 无
  */
void BMI160_SPI_GetGyro(void)
{

	unsigned short x,y,z;	
	signed short gyr_x,gyr_y,gyr_z;

	float Angle_Z = 0;//暂时存放当前的偏航角
	Angle_Z = GetGyroValue_Z();//获取偏航角
 
	BMI160_WriteOneByteToSlave(0x7e,0x15);//陀螺仪正常工作模式  
	delay_us(10); 
	

	//陀螺仪角速度X轴
	x =( BMI160_ReadOneByteFromSlave(0x0c) &0xff);  
	x = x|(( BMI160_ReadOneByteFromSlave(0x0d) &0xff)<<8);   
	gyr_x = (signed short)x;    
	gyr_x = (gyr_x*250)/0x7fff; //测量范围为250度，转换为角速度°/S的公式 

	//陀螺仪角速度Y轴
	y =( BMI160_ReadOneByteFromSlave(0x0e) &0xff);  
	y = y|(( BMI160_ReadOneByteFromSlave(0x0f) &0xff)<<8);   
	gyr_y = (signed short)y;
	gyr_y = (gyr_y*250)/0x7fff;//测量范围为250度，转换为角速度°/S的公式   
	
	//陀螺仪角速度Z轴 
	z =( BMI160_ReadOneByteFromSlave(0x10));  
	z = z|(( BMI160_ReadOneByteFromSlave(0x11))<<8);  
	gyr_z = (signed short)z;
	
	Angle_Z+=(float)(gyr_z-tGyro_Offset.z)*250.0/0x7fff*0.005*GyroUpdateCNT;//加上5ms时间内所转的角度
    GyroUpdateCNT = 0;
     /*范围限制 左0至180° 右0至-179.99999°*/
    if(Angle_Z>180)
    {
        Angle_Z-=360;    
    }
    else if(Angle_Z<=-180)
    {
        Angle_Z+=360;
    }
    
    
	SetGyroValue_Z(Angle_Z);//更新偏航角

}




/**
  * @Description  获取陀螺仪Z轴角速度零飘
  * @param  无
  * @retval 无
  */
void BMI160_SPI_GetOffset(void)
{

		signed short gyr_x,gyr_y,gyr_z;
		unsigned short x,y,z;

		/*角速度计进入正常电源模式*/
		BMI160_WriteOneByteToSlave(0x7e,0x15);  
		delay_us(100);

		/*读取角速度原始数据*/
		//X轴
		x =( BMI160_ReadOneByteFromSlave(0x0c) &0xff)  ;  
		x = x|(( BMI160_ReadOneByteFromSlave(0x0d) &0xff)<<8);  
		gyr_x = (signed short)x;

		//Y轴
		y =( BMI160_ReadOneByteFromSlave(0x0e) &0xff);  
		y = y|(( BMI160_ReadOneByteFromSlave(0x0f) &0xff)<<8);    
		gyr_y = (signed short)y;	

		//Z轴
		z =( BMI160_ReadOneByteFromSlave(0x10))  ;  
		z = z|(( BMI160_ReadOneByteFromSlave(0x11))<<8);
		gyr_z = (signed short)z;

		//计算零飘
		vBMI160CalcOffset_Gyro(gyr_x,gyr_y,gyr_z);

}



/**
  * @Description  计算角速度零飘（为防止抖动产生错误数据，分三组采集，如果三组数据的最大差大于某个阀值，则认为采集失败，重新采集）
  * @param  data_add_[XYZ]  角速度【XYZ】轴的原始数据
  * @retval 无
  */
void vBMI160CalcOffset_Gyro(float data_add_x , float data_add_y , float data_add_z)
{ 
  
    static float tempGyro[3];//存放累加的值
    static uint16_t cnt_N=0;//累加到了第N个
    static uint16_t cnt_G=0;//采集第G组数据
    static float OffsetTemp[3][3] = {0};//XYZ三个轴九组数据
	/**
		OffsetTemp[3][3]存放的数据位置:
		[x1][y1][z1]
		[x2][y2][z2]
		[x3][y3][z3]
		*/
    static float OffsetTempZz;
    static float OffsetTempZ[3];
    struct anglefloat_t tGyro_Offset_temp;

		if(cnt_N == 0)
		{
			
            //将所有临时变量都置零
				tGyro_Offset_temp.x=0;
				tGyro_Offset_temp.y=0;
				tGyro_Offset_temp.z=0;
				tempGyro[0]=0;
				tempGyro[1]=0;
				tempGyro[2]=0;
				//memset(&OffsetTemp[0][0],0,9*sizeof(OffsetTemp[0][0]));
		}
		cnt_N++;  
		tempGyro[0]+= data_add_x;//累加角速度X轴数据
		tempGyro[1]+= data_add_y;//累加角速度Y轴数据
		tempGyro[2]+= data_add_z;//累加角速度Z轴数据
		 
		
		if(!ucBMI160deal_Shake_Reset(data_add_z))//Z轴数据出现毛刺      
		{
				cnt_N=0;//重新开始
            DEBUG("重新开始cnt_N = %d\r\n",cnt_N);  
		}
		else if(cnt_N >= 1000) //采集次数到达1万次
		 {
					OffsetTemp[cnt_G][0] = tempGyro[0]/cnt_N;//取X轴平均
					OffsetTemp[cnt_G][1] = tempGyro[1]/cnt_N;//取Y轴平均
					OffsetTemp[cnt_G][2] = tempGyro[2]/cnt_N;//取Z轴平均
					OffsetTempZz = OffsetTemp[cnt_G][2];
					cnt_G++;
					if(cnt_G>=3)//采集完3组
					{
						cnt_G=0;
						OffsetTempZ[0]=OffsetTemp[0][2];//Z轴第一组数据均值
						OffsetTempZ[1]=OffsetTemp[1][2];//Z轴第二组数据均值
						OffsetTempZ[2]=OffsetTemp[2][2];//Z轴第三组数据均值
						DEBUG("OffsetTempZ = %f %f %f\r\n",OffsetTempZ[0],OffsetTempZ[1],OffsetTempZ[2]);

						/*--Tolerance deal--*/            
						if(ucMath_Tolerance(OffsetTempZ,3,4) && ucToleranceFlag==RESET)// 判断三组数据最大差值是否小于阈值     
								ucToleranceFlag=SET;//小于阈值，计算完成
//						else//大于阈值，另外处理
//								OffsetTempZz=ucBmi160_ToleranceFail_Deal(OffsetTempZz);
					}
					cnt_N = 0;
					
			}
    
        if( SET == ucToleranceFlag  )
        {
                //三组数据求均值
				tGyro_Offset_temp.x = (OffsetTemp[0][0]+OffsetTemp[1][0]+OffsetTemp[2][0])/3;
                tGyro_Offset_temp.y = (OffsetTemp[0][1]+OffsetTemp[1][1]+OffsetTemp[2][1])/3;
                tGyro_Offset_temp.z = (OffsetTemp[0][2]+OffsetTemp[1][2]+OffsetTemp[2][2])/3;
                //赋值给全局变量，用于陀螺仪减零飘所用
				tGyro_Offset.x = tGyro_Offset_temp.x;
                tGyro_Offset.y = tGyro_Offset_temp.y;
                tGyro_Offset.z = tGyro_Offset_temp.z;
                vBmi160_InitAttAngle();     //初始化姿态角
                #ifdef UseTempertureCompensation
                    SetTempertureB();//设定温度补偿B值
                #endif
                #ifdef UserTemperatureControl
                    SetTempertureF();//设定温度控制目标温度
                #elif defined UserTemperatureControl_Heat
                    SetTempertureF_Heat();
                #endif
                if(ucToleranceFailFlag)
                    tGyro_Offset.z=OffsetTempZz;
                /*重置内部变量*/
 
                cnt_N=0;
                cnt_G=0;
                ucToleranceFlag=RESET;
                /*-------------------------*/
            
			DEBUG("Gyro_Offset_t.z = %f",tGyro_Offset.z);
			eGyro_State = GYRO_WORKING;
            
        }
               
}


/**
  * @Description  计算角速度零飘时，采集的3次数据最大差值超过阈值则到这里作处理
  * @param  offset：最近采集的零飘值
  * @retval result：做处理后，得到的零飘平均值
  */
float ucBmi160_ToleranceFail_Deal(float offset)
{
	#define ToleranceSize 10
    const u8 BffSize=ToleranceSize;//计划达到的个数
    static u8 cnt_tolerancefail=0;//当前累计的数据个数
    static float Tempbuf[BffSize];//用于存放零飘数据的数组
    static u8 cnt_o=0;
    float result=0;
    u8 i;
    Tempbuf[cnt_o++] = offset;//将零飘数据存放进数组
    if(cnt_o>=BffSize)
        cnt_o=0;

    if(cnt_tolerancefail>=(ToleranceSize-1))//达到可以计算的个数
    {
        ucToleranceFlag=SET;
        ucToleranceFailFlag=SET;
//        cnt_tolerancefail=0;
        vMath_BubbleSort_f(Tempbuf,BffSize);//冒泡排序
        if(BffSize>=7)//数组数据个数超过7个，去掉头两个和尾两个
        {
            for(i=2;i<BffSize-2;i++)
                result+=Tempbuf[i];
            result/=(BffSize-4);
        }
        else if(BffSize>=5)//超过5个，去掉第一个和最后一个
        {
            for(i=1;i<BffSize-1;i++)
                result+=Tempbuf[i];
            result/=(BffSize-2);  
        }
        else//数据少于3个，则不做删除处理
        {
            for(i=0;i<BffSize;i++)
                result+=Tempbuf[i];
            result/=BffSize; 
        }
    }
    else
    {
        cnt_tolerancefail++;
    }
    return result;
}


/**
  * @Description  判断新数据是否为毛刺或抖动产生（当前数据减去上次数据大于阈值，则判断为抖动）
  * @param  fnewdata ：新的数据
  * @retval 1：数据正常 0：数据非正常
  */
u8 ucBMI160deal_Shake_Reset(float fnewdata)
{
    return ucBmi160_Shake_Reset(fnewdata,255);
}



/**
  * @Description  判断当前数据比上次数据是否超过设定阈值
  * @param  fnewdata ：新的数据 ucResetthreshold：阈值
  * @retval 1：低于阈值  0：超过阈值
  */
u8 ucBmi160_Shake_Reset(float fnewdata,u8 ucResetthreshold)
{
    static float flastdata=0;
    if(flastdata==0)//第一次进入
    {
        flastdata=fnewdata;
    }
			if(fabs(fnewdata-flastdata)<500)
    {
        flastdata=fnewdata;
        return 1;
    }
    flastdata=0;
    return 0;
}


/**
  * @Description  陀螺仪模块初始化
  * @param  无
  * @retval 无
  */
void BMI160_Init(void)
{
	BMI160_WriteOneByteToSlave(0x7e,0xb6);//重置
	delay_ms(500);
	
	while(BMI160CHIP_ID != BMI160_SPI_GetID())
	{
		delay_ms(1000);
		DEBUG("BMI160 INIT ERR!!!\r\n");
	}

	BMI160_WriteOneByteToSlave(0x41,0x03);//加速度计设置+-4g rangge
	delay_ms(100);
	BMI160_WriteOneByteToSlave(0x42,0x2c);//输出带宽1600Hz
	delay_ms(100);
	BMI160_WriteOneByteToSlave(0x43,0x03);//陀螺仪范围+-250°/S
	delay_ms(100);

	
	DEBUG("BMI160 INIT SUCCESS!!!\r\n");
	eGyro_State = GYRO_GETOFFSET;
	
}


/**
  * @Description  初始化【ZXY轴】全局变量
  * @param  无
  * @retval 无
  */
void vBmi160_InitAttAngle(void)
{
	SetGyroValue_X(0);
	SetGyroValue_Y(0);
	SetGyroValue_Z(0);
	SetAccValue_X(0);
	SetAccValue_Y(0);
	SetAccValue_Z(0);
}


/**
  * @Description  获取BMI160的ID，直接读0x00寄存器地址即可
  * @param  无
  * @retval BMI160 ID
  */
u8 BMI160_SPI_GetID(void)
{
    u8 ID = 0;
    ID = BMI160_ReadOneByteFromSlave(0x00);
    return ID;
}













