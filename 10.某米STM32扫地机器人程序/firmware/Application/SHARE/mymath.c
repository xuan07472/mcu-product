/**
 * @Filename   ：mymath.c
 * @Author     : HuangWentao       
 * @Version    : 1.0
 * @Date       : 2019/11/22
 * @Discription : 包含一些有关数学计算的函数
 * @hardware connect:
 *          ---------------------------------------
 */
#include "mymath.h"
/**
  * @Description  计算数组中最大的差值
  * @param  data ：要计算数组 length长度
  * @retval 最大差值
  */
float fMath_FindMaxDiff(float *data , u8 length)
{
    float max=*data;
    float min=*data;
    u8 i;
    for(i=1;i<length;i++)
    {
        if(*(data+i)<min)//找最小值
        {
            min = *(data+i);
        }
        else if(*(data+i)>max)//找最大值
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
  * @Description  计算数组中最大的差值
  * @param  data ：要计算数组 length长度  allowance允许最大的差值
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
    if( zeroCNT > length*0.7)//出现太多0
    {
        return 0;
    }
    err=fMath_FindMaxDiff(data2,length);
    if(err>allowance)
    {
			DEBUG("\r\n出现抖动了,重新计算\r\n");
        return 0;
			
    }
    return 1;
}





/**
  * @Description 数组从小到大排序
  * @param  data ：要计算数组 length长度  
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


