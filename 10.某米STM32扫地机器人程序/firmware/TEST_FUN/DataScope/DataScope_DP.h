

#ifndef __DATA_PRTOCOL_H
#define __DATA_PRTOCOL_H
 
#define MAX_CHANNEL  4 //����4��ʾ����ͨ�� 

extern unsigned char DataScope_OutPut_Buffer[42];	   //������֡���ݻ�����
void vWaveFromOut(void);//�����ݻ�����������ͨ�����ڷ���
void DataScope_Get_Channel_Data(float Data,unsigned char Channel);    // дͨ�������� ������֡���ݻ�����




unsigned char DataScope_Data_Generate(unsigned char Channel_Number);  // ����֡�������ɺ��� 
 
 
#endif 



