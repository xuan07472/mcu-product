/**
 * @Filename   ��bmi160.c
 * @Author     : HuangWentao       
 * @Version    : 1.0
 * @Date       : 2019/11/14
 * @Discription : ��SPI��ȡ����������
 * @hardware connect:
 *          -------------TIM6------------------------
 */
#include "BMI160.h"

#define  BMI160CHIP_ID 209	//0XD1 BMI160оƬID

struct anglefloat_t tGyro_Offset;//���ٶȡ�XYZ�����ƫ����
enum GyroState_e eGyro_State = GYRO_INIT;//������״̬����ʼ��/����ƫ����/�����С�
extern u8 GyroUpdateCNT;

u8 ucToleranceFlag=RESET;//���ݼ���ƫ�ƹ����־��SET��ʾ����������
u8 ucToleranceFailFlag=RESET;//�������ֵ����ı�־λ��SET��ʾ���ݴ����꡿





//����������
void vGyroscopse_task(void *pvParameters)
{

	eGyro_State = GYRO_INIT;
	while(1)
	{
			
		if(eGyro_State == GYRO_INIT )//��ʼ��BMI160
		{
			BMI160_Init();
		}
		else if(eGyro_State == GYRO_GETOFFSET)//�������Ư��
		{
			BMI160_SPI_GetOffset();
		}
		else if(eGyro_State == GYRO_WORKING)//��������״̬��5ms����һ������
		{
			BMI160_SPI_GetGyro();//��ȡ�����ǽ��ٶ�����
			BMI160_SPI_GetACC();//��ȡ�����Ǽ��ٶ�����
            delay_ms(5);
		}

	
	}
}




/**
  * @Description  ��ȡ�������ٶȼ�����,���տɵõ�ɨ�ػ�����ǰ����б�ĽǶȣ�-90��90�㣩
  * @param  ��
  * @retval ��
  */
void BMI160_SPI_GetACC(void)
{
		unsigned short x,y,z;
		signed short ACC_X,ACC_Y,ACC_Z;
	
		signed char Angle_x = 0;
		signed char Angle_y = 0;
		/*���ü��ٶȼ�Ϊ����ģʽ*/
		BMI160_WriteOneByteToSlave(0x7e,0x11);
		delay_us(5);	
		/*X������*/
		x = BMI160_ReadOneByteFromSlave(0x12);
		x = (BMI160_ReadOneByteFromSlave(0x13)<<8)|x;
		ACC_X = (signed short)x;
		//ACC_X = (ACC_X*9.8/(0X7fff/2));	//ת��Ϊ1g�Ĺ�ʽ
	
        /*Y������*/
        y = BMI160_ReadOneByteFromSlave(0x14);
		y = (BMI160_ReadOneByteFromSlave(0x15)<<8)|y;
		ACC_Y =(signed short)y;
		//ACC_Y = (ACC_Y*9.8/(0X7fff/2));
		
        /*Z������*/
        z = BMI160_ReadOneByteFromSlave(0x16);
		z = (BMI160_ReadOneByteFromSlave(0x17)<<8)|z;
		ACC_Z = (signed short)z;
		//ACC_Z = (ACC_Z*9.8/(0X7fff/2));

		/*ת��Ϊ�Ƕ�(range��-180��~180��)*/
		Angle_x = atan2(ACC_X,ACC_Z)*180/3.14159;
		Angle_y = atan2(ACC_Y,ACC_Z)*180/3.14159;
		
		/*������б��*/
		SetAccValue_X(Angle_x);
		SetAccValue_Y(Angle_y);
}



/**
  * @Description  ��ȡ���ٶ����ݣ��õ�ƫ���ǶȺ͵�ǰʵʱ�Ƕȣ�-179.9-��180�㣩
  * @param  ��
  * @retval ��
  */
void BMI160_SPI_GetGyro(void)
{

	unsigned short x,y,z;	
	signed short gyr_x,gyr_y,gyr_z;

	float Angle_Z = 0;//��ʱ��ŵ�ǰ��ƫ����
	Angle_Z = GetGyroValue_Z();//��ȡƫ����
 
	BMI160_WriteOneByteToSlave(0x7e,0x15);//��������������ģʽ  
	delay_us(10); 
	

	//�����ǽ��ٶ�X��
	x =( BMI160_ReadOneByteFromSlave(0x0c) &0xff);  
	x = x|(( BMI160_ReadOneByteFromSlave(0x0d) &0xff)<<8);   
	gyr_x = (signed short)x;    
	gyr_x = (gyr_x*250)/0x7fff; //������ΧΪ250�ȣ�ת��Ϊ���ٶȡ�/S�Ĺ�ʽ 

	//�����ǽ��ٶ�Y��
	y =( BMI160_ReadOneByteFromSlave(0x0e) &0xff);  
	y = y|(( BMI160_ReadOneByteFromSlave(0x0f) &0xff)<<8);   
	gyr_y = (signed short)y;
	gyr_y = (gyr_y*250)/0x7fff;//������ΧΪ250�ȣ�ת��Ϊ���ٶȡ�/S�Ĺ�ʽ   
	
	//�����ǽ��ٶ�Z�� 
	z =( BMI160_ReadOneByteFromSlave(0x10));  
	z = z|(( BMI160_ReadOneByteFromSlave(0x11))<<8);  
	gyr_z = (signed short)z;
	
	Angle_Z+=(float)(gyr_z-tGyro_Offset.z)*250.0/0x7fff*0.005*GyroUpdateCNT;//����5msʱ������ת�ĽǶ�
    GyroUpdateCNT = 0;
     /*��Χ���� ��0��180�� ��0��-179.99999��*/
    if(Angle_Z>180)
    {
        Angle_Z-=360;    
    }
    else if(Angle_Z<=-180)
    {
        Angle_Z+=360;
    }
    
    
	SetGyroValue_Z(Angle_Z);//����ƫ����

}




/**
  * @Description  ��ȡ������Z����ٶ���Ʈ
  * @param  ��
  * @retval ��
  */
void BMI160_SPI_GetOffset(void)
{

		signed short gyr_x,gyr_y,gyr_z;
		unsigned short x,y,z;

		/*���ٶȼƽ���������Դģʽ*/
		BMI160_WriteOneByteToSlave(0x7e,0x15);  
		delay_us(100);

		/*��ȡ���ٶ�ԭʼ����*/
		//X��
		x =( BMI160_ReadOneByteFromSlave(0x0c) &0xff)  ;  
		x = x|(( BMI160_ReadOneByteFromSlave(0x0d) &0xff)<<8);  
		gyr_x = (signed short)x;

		//Y��
		y =( BMI160_ReadOneByteFromSlave(0x0e) &0xff);  
		y = y|(( BMI160_ReadOneByteFromSlave(0x0f) &0xff)<<8);    
		gyr_y = (signed short)y;	

		//Z��
		z =( BMI160_ReadOneByteFromSlave(0x10))  ;  
		z = z|(( BMI160_ReadOneByteFromSlave(0x11))<<8);
		gyr_z = (signed short)z;

		//������Ʈ
		vBMI160CalcOffset_Gyro(gyr_x,gyr_y,gyr_z);

}



/**
  * @Description  ������ٶ���Ʈ��Ϊ��ֹ���������������ݣ�������ɼ�������������ݵ��������ĳ����ֵ������Ϊ�ɼ�ʧ�ܣ����²ɼ���
  * @param  data_add_[XYZ]  ���ٶȡ�XYZ�����ԭʼ����
  * @retval ��
  */
void vBMI160CalcOffset_Gyro(float data_add_x , float data_add_y , float data_add_z)
{ 
  
    static float tempGyro[3];//����ۼӵ�ֵ
    static uint16_t cnt_N=0;//�ۼӵ��˵�N��
    static uint16_t cnt_G=0;//�ɼ���G������
    static float OffsetTemp[3][3] = {0};//XYZ�������������
	/**
		OffsetTemp[3][3]��ŵ�����λ��:
		[x1][y1][z1]
		[x2][y2][z2]
		[x3][y3][z3]
		*/
    static float OffsetTempZz;
    static float OffsetTempZ[3];
    struct anglefloat_t tGyro_Offset_temp;

		if(cnt_N == 0)
		{
			
            //��������ʱ����������
				tGyro_Offset_temp.x=0;
				tGyro_Offset_temp.y=0;
				tGyro_Offset_temp.z=0;
				tempGyro[0]=0;
				tempGyro[1]=0;
				tempGyro[2]=0;
				//memset(&OffsetTemp[0][0],0,9*sizeof(OffsetTemp[0][0]));
		}
		cnt_N++;  
		tempGyro[0]+= data_add_x;//�ۼӽ��ٶ�X������
		tempGyro[1]+= data_add_y;//�ۼӽ��ٶ�Y������
		tempGyro[2]+= data_add_z;//�ۼӽ��ٶ�Z������
		 
		
		if(!ucBMI160deal_Shake_Reset(data_add_z))//Z�����ݳ���ë��      
		{
				cnt_N=0;//���¿�ʼ
            DEBUG("���¿�ʼcnt_N = %d\r\n",cnt_N);  
		}
		else if(cnt_N >= 1000) //�ɼ���������1���
		 {
					OffsetTemp[cnt_G][0] = tempGyro[0]/cnt_N;//ȡX��ƽ��
					OffsetTemp[cnt_G][1] = tempGyro[1]/cnt_N;//ȡY��ƽ��
					OffsetTemp[cnt_G][2] = tempGyro[2]/cnt_N;//ȡZ��ƽ��
					OffsetTempZz = OffsetTemp[cnt_G][2];
					cnt_G++;
					if(cnt_G>=3)//�ɼ���3��
					{
						cnt_G=0;
						OffsetTempZ[0]=OffsetTemp[0][2];//Z���һ�����ݾ�ֵ
						OffsetTempZ[1]=OffsetTemp[1][2];//Z��ڶ������ݾ�ֵ
						OffsetTempZ[2]=OffsetTemp[2][2];//Z����������ݾ�ֵ
						DEBUG("OffsetTempZ = %f %f %f\r\n",OffsetTempZ[0],OffsetTempZ[1],OffsetTempZ[2]);

						/*--Tolerance deal--*/            
						if(ucMath_Tolerance(OffsetTempZ,3,4) && ucToleranceFlag==RESET)// �ж�������������ֵ�Ƿ�С����ֵ     
								ucToleranceFlag=SET;//С����ֵ���������
//						else//������ֵ�����⴦��
//								OffsetTempZz=ucBmi160_ToleranceFail_Deal(OffsetTempZz);
					}
					cnt_N = 0;
					
			}
    
        if( SET == ucToleranceFlag  )
        {
                //�����������ֵ
				tGyro_Offset_temp.x = (OffsetTemp[0][0]+OffsetTemp[1][0]+OffsetTemp[2][0])/3;
                tGyro_Offset_temp.y = (OffsetTemp[0][1]+OffsetTemp[1][1]+OffsetTemp[2][1])/3;
                tGyro_Offset_temp.z = (OffsetTemp[0][2]+OffsetTemp[1][2]+OffsetTemp[2][2])/3;
                //��ֵ��ȫ�ֱ��������������Ǽ���Ʈ����
				tGyro_Offset.x = tGyro_Offset_temp.x;
                tGyro_Offset.y = tGyro_Offset_temp.y;
                tGyro_Offset.z = tGyro_Offset_temp.z;
                vBmi160_InitAttAngle();     //��ʼ����̬��
                #ifdef UseTempertureCompensation
                    SetTempertureB();//�趨�¶Ȳ���Bֵ
                #endif
                #ifdef UserTemperatureControl
                    SetTempertureF();//�趨�¶ȿ���Ŀ���¶�
                #elif defined UserTemperatureControl_Heat
                    SetTempertureF_Heat();
                #endif
                if(ucToleranceFailFlag)
                    tGyro_Offset.z=OffsetTempZz;
                /*�����ڲ�����*/
 
                cnt_N=0;
                cnt_G=0;
                ucToleranceFlag=RESET;
                /*-------------------------*/
            
			DEBUG("Gyro_Offset_t.z = %f",tGyro_Offset.z);
			eGyro_State = GYRO_WORKING;
            
        }
               
}


/**
  * @Description  ������ٶ���Ʈʱ���ɼ���3����������ֵ������ֵ������������
  * @param  offset������ɼ�����Ʈֵ
  * @retval result��������󣬵õ�����Ʈƽ��ֵ
  */
float ucBmi160_ToleranceFail_Deal(float offset)
{
	#define ToleranceSize 10
    const u8 BffSize=ToleranceSize;//�ƻ��ﵽ�ĸ���
    static u8 cnt_tolerancefail=0;//��ǰ�ۼƵ����ݸ���
    static float Tempbuf[BffSize];//���ڴ����Ʈ���ݵ�����
    static u8 cnt_o=0;
    float result=0;
    u8 i;
    Tempbuf[cnt_o++] = offset;//����Ʈ���ݴ�Ž�����
    if(cnt_o>=BffSize)
        cnt_o=0;

    if(cnt_tolerancefail>=(ToleranceSize-1))//�ﵽ���Լ���ĸ���
    {
        ucToleranceFlag=SET;
        ucToleranceFailFlag=SET;
//        cnt_tolerancefail=0;
        vMath_BubbleSort_f(Tempbuf,BffSize);//ð������
        if(BffSize>=7)//�������ݸ�������7����ȥ��ͷ������β����
        {
            for(i=2;i<BffSize-2;i++)
                result+=Tempbuf[i];
            result/=(BffSize-4);
        }
        else if(BffSize>=5)//����5����ȥ����һ�������һ��
        {
            for(i=1;i<BffSize-1;i++)
                result+=Tempbuf[i];
            result/=(BffSize-2);  
        }
        else//��������3��������ɾ������
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
  * @Description  �ж��������Ƿ�Ϊë�̻򶶶���������ǰ���ݼ�ȥ�ϴ����ݴ�����ֵ�����ж�Ϊ������
  * @param  fnewdata ���µ�����
  * @retval 1���������� 0�����ݷ�����
  */
u8 ucBMI160deal_Shake_Reset(float fnewdata)
{
    return ucBmi160_Shake_Reset(fnewdata,255);
}



/**
  * @Description  �жϵ�ǰ���ݱ��ϴ������Ƿ񳬹��趨��ֵ
  * @param  fnewdata ���µ����� ucResetthreshold����ֵ
  * @retval 1��������ֵ  0��������ֵ
  */
u8 ucBmi160_Shake_Reset(float fnewdata,u8 ucResetthreshold)
{
    static float flastdata=0;
    if(flastdata==0)//��һ�ν���
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
  * @Description  ������ģ���ʼ��
  * @param  ��
  * @retval ��
  */
void BMI160_Init(void)
{
	BMI160_WriteOneByteToSlave(0x7e,0xb6);//����
	delay_ms(500);
	
	while(BMI160CHIP_ID != BMI160_SPI_GetID())
	{
		delay_ms(1000);
		DEBUG("BMI160 INIT ERR!!!\r\n");
	}

	BMI160_WriteOneByteToSlave(0x41,0x03);//���ٶȼ�����+-4g rangge
	delay_ms(100);
	BMI160_WriteOneByteToSlave(0x42,0x2c);//�������1600Hz
	delay_ms(100);
	BMI160_WriteOneByteToSlave(0x43,0x03);//�����Ƿ�Χ+-250��/S
	delay_ms(100);

	
	DEBUG("BMI160 INIT SUCCESS!!!\r\n");
	eGyro_State = GYRO_GETOFFSET;
	
}


/**
  * @Description  ��ʼ����ZXY�᡿ȫ�ֱ���
  * @param  ��
  * @retval ��
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
  * @Description  ��ȡBMI160��ID��ֱ�Ӷ�0x00�Ĵ�����ַ����
  * @param  ��
  * @retval BMI160 ID
  */
u8 BMI160_SPI_GetID(void)
{
    u8 ID = 0;
    ID = BMI160_ReadOneByteFromSlave(0x00);
    return ID;
}













