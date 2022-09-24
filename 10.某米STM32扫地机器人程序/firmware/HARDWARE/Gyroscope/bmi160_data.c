/**
 * @Filename   ��bmi160_data.c
 * @Author     : huangwentao       
 * @Version    : 1.0
 * @Date       : 2019/11/9
 * @Discription : ������ȫ�����ݣ��ⲿͨ������ĺ�����������������
 * @hardware connect:
 *          -------------------------------------
 */

#include "bmi160_data.h"
gyrovalue_t GyroValue_t;//�����ǽǶ����ݡ�XYZ��
accvalue_t AccValue_t;//���ٶȼ����ݡ�XYZ��


/**
  * @Description  SetValue��������������
  * @param  data  ������
  * @retval ��
  */
void SetGyroValue_X(float data)
{
	GyroValue_t.X = data;
}

void SetGyroValue_Y(float data)
{
	GyroValue_t.Y = data;
}

void SetGyroValue_Z(float data)
{
	GyroValue_t.Z = data;
}

void SetAccValue_X(float data)
{
	AccValue_t.X = data;
}

void SetAccValue_Y(float data)
{
	AccValue_t.Y = data;
}

void SetAccValue_Z(float data)
{
	AccValue_t.Z = data;
}

/**
  * @Description  GetValue��ȡ����������
  * @param  ��
  * @retval ��ת�Ƕ�/��б�Ƕ�����
  */
float GetGyroValue_X(void)
{
	return GyroValue_t.X;
}

float GetGyroValue_Y(void)
{
	return GyroValue_t.Y;
}

float GetGyroValue_Z(void)
{
	return GyroValue_t.Z;
}

signed char GetAccValue_X(void)
{
	return AccValue_t.X;
}

signed char GetAccValue_Y(void)
{
	return AccValue_t.Y;
}

signed char GetAccValue_Z(void)
{
	return AccValue_t.Z;
}


