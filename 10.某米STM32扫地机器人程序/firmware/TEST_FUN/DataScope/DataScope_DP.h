

#ifndef __DATA_PRTOCOL_H
#define __DATA_PRTOCOL_H
 
#define MAX_CHANNEL  4 //启用4个示波器通道 

extern unsigned char DataScope_OutPut_Buffer[42];	   //待发送帧数据缓存区
void vWaveFromOut(void);//将数据缓存区的数据通过串口发送
void DataScope_Get_Channel_Data(float Data,unsigned char Channel);    // 写通道数据至 待发送帧数据缓存区




unsigned char DataScope_Data_Generate(unsigned char Channel_Number);  // 发送帧数据生成函数 
 
 
#endif 



