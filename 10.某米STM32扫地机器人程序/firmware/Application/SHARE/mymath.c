/**
 * @Filename   ��mymath.c
 * @Author     : HuangWentao       
 * @Version    : 1.0
 * @Date       : 2019/11/22
 * @Discription : ����һЩ�й���ѧ����ĺ���
 * @hardware connect:
 *          ---------------------------------------
 */
#include "mymath.h"
/**
  * @Description  �������������Ĳ�ֵ
  * @param  data ��Ҫ�������� length����
  * @retval ����ֵ
  */
float fMath_FindMaxDiff(float *data , u8 length)
{
    float max=*data;
    float min=*data;
    u8 i;
    for(i=1;i<length;i++)
    {
        if(*(data+i)<min)//����Сֵ
        {
            min = *(data+i);
        }
        else if(*(data+i)>max)//�����ֵ
        {
            max = *(data+i);
        }
    }
    if(max==min)
    {
        return 10000;
    }
    return max-min;    
}


/**
  * @Description  �������������Ĳ�ֵ
  * @param  data ��Ҫ�������� length����  allowance�������Ĳ�ֵ
  * @retval 
  */
u8 ucMath_Tolerance(float *data,u8 length,float allowance)
{
    float err;
    int i;
    int zeroCNT=0;
    float data2[10];
    for(i=0;i<length;i++)
    {
        data2[i]=data[i];
        if(data[i] == 0.0f)
        {
            zeroCNT++;
        }
    }
    if( zeroCNT > length*0.7)//����̫��0
    {
        return 0;
    }
    err=fMath_FindMaxDiff(data2,length);
    if(err>allowance)
    {
			DEBUG("\r\n���ֶ�����,���¼���\r\n");
        return 0;
			
    }
    return 1;
}





/**
  * @Description �����С��������
  * @param  data ��Ҫ�������� length����  
  * @retval 
  */
void vMath_BubbleSort_f(float *a,int len)//float 
{
	int i;
	int j;
	double mid;
    for(i=0;i<len;i++)
    {
        for(j=0;j<len-i-1;j++)
        {
            if(*(a+j)>*(a+j+1))
            {
                mid=*(a+j);
                *(a+j)=*(a+j+1);
                *(a+j+1)=mid;
            }
        }
    }
}


