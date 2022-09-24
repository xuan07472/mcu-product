#include "protocol.h"

/*
 * Filename   ��protocol.c
 * Author     : PuSheng 
 * Version    : 2.0
 * Date       : 2019.12.25  2020.1.4
 
 2.0: �汾��д 2019.12.25 
 2.1: ֡���������� �����ֽڶԱ���ɺ� ����֡�����ֽ�д�� �������Է�ֹ�����ȥ��Ӧ ��Ӧ֡
 
 һ��	֡�����
֡Э����� ֡��ʼ�ֽ�+֡�����ֽ�+֡�����ֽ�+���ݳ����ֽ�+�����ֽ�+У���ֽ� ����ɡ�

1.1 ֡��ʼ�ֽ�
��ʼ�ֽڶ���Ϊ�����ֽ�0XAA��

1.2֡�����ֽ�
֡�����ֽ�ҲΪ���ֽڣ����幦�ܼ���1.1
��1.1
�����ֽ�	��ʾ����
0xb0	��Ӧ֡�������յ�һ֡���ݺ󣬽��շ���Ҫ��Ӧһ֡��Ӧ֡��(��Ӧ֡���û�Ӧ)
0xb1	����֡����ʾ��֡�������ֽ���һ��ָ��
0xb2	����֡����ʾ��֡�������ֽ���һ������
0xb3	����֡��Ϊ�˲���ͨ���Ƿ����������ݳ����ֽ�Ϊ0��û�������ֽ�
	
	
	

1.3֡�����ֽ�
֡����Ҳ��ռһ���ֽڣ���ʾ��֡��˳�򣬷��ͷ�ӵ��һ��Ψһ�ķ������У���ֵ��0��255ÿ����һ֡�Լ�1��ѭ���Լӣ���
�������ڱ�ʶ��֡�������շ�����֡У��ʧ�ܻ��߼�⵽©֡�������ù����ֽڵĻ�Ӧ֡��֪���ͷ������·��͸�֡��

1.4 ���ݳ����ֽ�
���ݳ���ռ1���ֽڣ���ʾ��֡�������м����ֽڣ���Χ0-50��������ݳ����ֽ�Ϊ0����֡��û�������ֽ�
1.5 �����ֽ�
��ʾ��֡Я�������ݣ����������ݳ����ֽ��ƶ��������ݳ����ֽ�Ϊ0����û�������ֽڡ����50���ֽڡ�
1.6 У���ֽ�
ͨ��������֡����checksumУ��õ�������д�����ֽڡ�У�鷶Χ����֡��ʼ�ֽڵ������ֽڡ�

����	����֡���
2.1 [��Ӧ֡]
���շ��������յ�һ֡����ʱ�����յ��Ĳ���[��Ӧ֡]�������շ�Ӧ����Ӧһ֡[��Ӧ֡]��[��Ӧ֡]�а�����һ���ֽڵ������ֽڣ�
������ָʾ�˽��շ��Ƿ���յ��������ݡ�[��Ӧ֡]��֡�����ֽ�Ϊ0.
Ex����Ӧ֡ʾ�� AA B0 00 01 XX YY
����AA��һ֡��ʼ��B0��ʾ��֡�ǻ�Ӧ֡��00֡����Ϊ0����Ӧ֡��֡���к�Ϊ0����01��ʾ��һ���ֽڵ����ݡ�
YY��ʾ����������֡��checksumֵ��
XX��01 ֡��ʽ����
	02 У��ֵ����
	03 ֡���д���
	04 ���ճɹ�
����03֡���д��� �����ݳ���Ӧ��Ϊ2����һ���ֽڱ�ʾ03���󣬵ڶ����ֽڱ�ʾӦ�õ����кš�EX��AA B0 00 02 01 XX YY
���ͷ���������һ֡���ݣ��ǻ�Ӧ֡���󣬷��ͷ���Ҫ�ȴ�һ����Ӧ֡�����ܼ�������֡�źš�
��1���ڶ�û�н��յ�[��Ӧ֡]����û�н��յ���ȷ��[��Ӧ֡]�������·��ͱ������ݡ������յ������ֽ�Ϊ03�����ճɹ�����[��Ӧ֡]��
��������Ͽ�ʼ������һ֡�����ݡ������յ������ֽڲ�Ϊ03��[��Ӧ֡]����������Ͽ�ʼ���·��ͱ������ݡ�

2.2 [����֡]
ͨ������֡���������ݣ����߱������������͵�������ʲô���͵ġ�
Ex������֡ʾ�� AA B1 XX 01 YY ZZ
����XX��ʾ��֡��֡�����ֽڣ�YY��ʾ�������ZZ��ʾУ��λ
YY��C0 ���������͵������ǹ̼����򣬱�ʾ���շ���Ҫ�����������ܵ������ݸ��µ��̼�

2.3 [����֡]
����֡��Ҫ����Я�����ݴ��䣬ÿ֡�����ֽ����50�ֽڡ�һ�㶼������һ������֡��ʾ������������֡��ʲô���͵����ݡ�
 
2.4 [����֡]
����֡����Ҫ���þ��Ǽ���Ƿ����ӡ����������̶���ʽΪ
AA B3 XX 00 YY
����AAΪ֡ͷ��B3�ǿ���֡���ܱ�ʶ��XX��֡�����ֽڡ�00��ʾû�и�֡���ݣ�����YY����CheckSum����֡û�������ֽڡ�
��10��û���κ�֡Ҫ���� ����뷢��֡����֡

 
*/



Rx_FrameRecord FR_Rx={0};
Tx_FrameRecord FR_Tx={0};

/*
* Function Name  : eFrame_Analy
* Description    : ������BUF����Ϊ֡����
* Input          : buf ����ԭʼ���� length ���ݳ��� RFR ����֡�ṹ��
* Output         : None 
* Return         : FrameErr_e ��������
*/
FrameErr_e eFrame_Analy(u8 *buf,u8 length,Rx_FrameRecord *RFR)
{
    u16 i;
    FrameErr_e FErr = FrameFromatErr;
    
    for(i=0;i<length;i++)
    {   
        /*���֡ͷ*/
        if(buf[i] != 0XAA)
        {
            continue;
        }
        /*���֡�����ֽ��Ƿ�Ϸ�*/
        if(buf[i+FRAME_FUNCTION_POS] < ffRespond || buf[i+FRAME_FUNCTION_POS] >= ffMax)
        {
            FErr=FrameFromatErr;continue;
        }
        RFR->tFrame.FrameFunction=(FrameFunction_e)buf[i+FRAME_FUNCTION_POS];//д��֡�����ֽ� ��ֹʶ����һ֡��֡�����ֽ�
        
        /*���֡���ݳ����ֽ��Ƿ�Ϸ�*/
        if(buf[i+FRAME_DATA_LENGTH_POS] > FRAME_DATA_MAX_LENGTH)
        {
             FErr=FrameLengthErr;continue;
        }
        /*���֡У���ֽ��Ƿ�Ϸ�*/
        if( bJudgeChecksum(&buf[i] , 5+buf[i+FRAME_DATA_LENGTH_POS]) == false)
        {
            FErr=FrameCheckErr;continue;
        }
        
        if(false == RFR->IsStartReceive)//�Ƿ�ʼ��¼���к�
        {
            RFR->IsStartReceive = true;
            RFR->NextFrameSeq = buf[i+FRAME_SEQUENCE_POS];
        }
        /*���յ���֡���в���Ӧ�õ�֡���� ��ʾ���Ϸ�����Ӧ֡û��֡����*/
        if(RFR->NextFrameSeq != buf[i+FRAME_SEQUENCE_POS] && buf[i+FRAME_FUNCTION_POS] != ffRespond)//֡���в��Ϸ�
        {
            FErr=FrameSeqErr;continue;
        }
        else
        {
            /*�ɹ����յ�һ֡����*/ 
            RFR->ContinueErrNum=0;//����������
            RFR->FrameErrType=FrameSuccess;//��Ǵ������� Ϊ����ɹ�
            RFR->IsFrameNotProcess=true;//�����������Ҫ����       
            if(buf[i+FRAME_FUNCTION_POS] != ffRespond)//��Ӧ֡ û��֡���� �����Լ����к�
                RFR->NextFrameSeq++;//֡���м�1
            RFR->tFrame.FrameData=RFR->DataBuf;//֡���ݵ�ַ��Ӧ������
            
            RFR->tFrame.FrameHead=buf[i+FRAME_HEAD_POS];//д��֡ͷ�ֽ�
            RFR->tFrame.FrameFunction=(FrameFunction_e)buf[i+FRAME_FUNCTION_POS];//д��֡�����ֽ�
            RFR->tFrame.FrameSequence=buf[i+FRAME_SEQUENCE_POS];//д��֡�����ֽ�
            RFR->tFrame.DataLength=buf[i+FRAME_DATA_LENGTH_POS];//д��֡���ݳ����ֽ�
            RFR->tFrame.FrameChecksum=buf[i+4+buf[i+FRAME_DATA_LENGTH_POS]];//д��У���ֽ�
 
            memcpy(RFR->DataBuf,&buf[i+FRAME_DATA_POS],buf[i+FRAME_DATA_LENGTH_POS]);//д������
            return FrameSuccess;
        }
    }
    
    
    RFR->ContinueErrNum++;
    RFR->FrameErrType=FErr;
    return FErr;

}

/*
* Function Name  : bJudgeChecksum
* Description    : ���һ�������Ƿ�����У���
* Input          : buf ����ԭʼ���� length ���ݳ��� 
* Output         : None 
* Return         : false У�鲻ͨ��   true У��ͨ��
*/
bool bJudgeChecksum(u8 *buf,u8 length)
{
    u8 check = 0;
	while(length--)
	{
		check += *buf++;//���������ۼ�
	}
    check = check%256;
	if(check == 0xFF)//��ΪFF ��ʾУ����ȷ
        return true;
    else
        return false;
}

/*
* Function Name  : ucCode_RespondFrame
* Description    : ����һ����Ӧ֡����
* Input          : Tx ���ͼ�¼�ṹ�� Rx ���ռ�¼�ṹ�� buf ���úõ�֡����
* Output         : None 
* Return         : 0�ɹ�  ����ʧ��
*/
u8 ucCode_RespondFrame(Tx_FrameRecord *Tx,Rx_FrameRecord *Rx,u8 *buf)
{
    Tx->tFrame.FrameHead=0XAA;
    Tx->tFrame.FrameFunction=ffRespond;//������֡����Ϊ ��Ӧ֡
    Tx->tFrame.FrameSequence=0;//��Ӧ֡ �����к�Ϊ0
    Tx->tFrame.DataLength=1;
    Tx->tFrame.FrameData=Tx->DataBuf;
    
    switch(Rx->FrameErrType)
    {
        case FrameFromatErr:
            Tx->DataBuf[0]=FrameFromatErr;
            break;
        case FrameCheckErr:
            Tx->DataBuf[0]=FrameCheckErr;
            break;
        case FrameSeqErr:
            Tx->tFrame.DataLength=2;
            Tx->DataBuf[0]=FrameSeqErr;
            Tx->DataBuf[1]=Rx->NextFrameSeq;
            break;
        case FrameLengthErr:
            Tx->DataBuf[0]=FrameLengthErr;
            break;
        case FrameSuccess:
            Tx->DataBuf[0]=FrameSuccess;
            break;
    }
    memcpy(buf,&(Tx->tFrame.FrameHead),4);//д��֡ͷ��Ϣ
    memcpy(&buf[4],Tx->DataBuf,Tx->tFrame.DataLength);//д������
    Tx->tFrame.FrameChecksum=ucCodeChecksum(buf,4+Tx->tFrame.DataLength);//��ȡ��֡��У����
    
    buf[4+Tx->tFrame.DataLength]=Tx->tFrame.FrameChecksum;//д��У������Ϣ
    
    return 0;
}

/*
* Function Name  : ucCode_CommandFrame
* Description    : ����һ������֡����
* Input          : Tx ���ͼ�¼�ṹ�� Commandbuf ��������  length ���� buf ���úõ�֡����
* Output         : None 
* Return         : 0�ɹ�  ����ʧ��
*/
u8 ucCode_CommandFrame(Tx_FrameRecord *Tx,u8 *Commandbuf,u8 length,u8 *buf)
{
    u8 Err=0;
    if(length > FRAME_DATA_MAX_LENGTH)//���ݳ���һ֡�����
    {
        length=FRAME_DATA_MAX_LENGTH;
        Err=1;
    }
        
    /*���÷��ͽṹ��*/
    Tx->tFrame.FrameHead=0XAA;
    Tx->tFrame.FrameFunction=ffCommand;//������֡����Ϊ ����֡
    Tx->tFrame.FrameSequence=Tx->NextFrameSeq++;//д���к�
    Tx->tFrame.DataLength=length;
    Tx->tFrame.FrameData=Tx->DataBuf;
    memcpy(Tx->DataBuf,Commandbuf,length);//д��������Ϣ
    
    /*�����ͽṹ������ д�� ����buf*/
    memcpy(buf,&(Tx->tFrame.FrameHead),4);//д��֡ͷ��Ϣ
    memcpy(&buf[4],Tx->DataBuf,Tx->tFrame.DataLength);//д������
    Tx->tFrame.FrameChecksum=ucCodeChecksum(buf,4+Tx->tFrame.DataLength);//��ȡ��֡��У����
    
    buf[4+Tx->tFrame.DataLength]=Tx->tFrame.FrameChecksum;//д��У������Ϣ
    
    return Err;
}
/*
* Function Name  : ucCode_DataFrame
* Description    : ����һ������֡����
* Input          : Tx ���ͼ�¼�ṹ�� Databuf ���� length���� buf ���úõ�֡����
* Output         : None 
* Return         : 0�ɹ�  ����ʧ��
*/
u8 ucCode_DataFrame(Tx_FrameRecord *Tx,u8 *Databuf,u8 length,u8 *buf)
{
    u8 Err=0;
    if(length > FRAME_DATA_MAX_LENGTH)//���ݳ���һ֡�����
    {
        length=FRAME_DATA_MAX_LENGTH;
        Err=1;
    }
        
    /*���÷��ͽṹ��*/
    Tx->tFrame.FrameHead=0XAA;
    Tx->tFrame.FrameFunction=ffData;//������֡����Ϊ ����֡
    Tx->tFrame.FrameSequence=Tx->NextFrameSeq++;//д���к�
    Tx->tFrame.DataLength=length;
    Tx->tFrame.FrameData=Tx->DataBuf;
    memcpy(Tx->DataBuf,Databuf,length);//д��������Ϣ
    
    /*�����ͽṹ������ д�� ����buf*/
    memcpy(buf,&(Tx->tFrame.FrameHead),4);//д��֡ͷ��Ϣ
    memcpy(&buf[4],Tx->DataBuf,Tx->tFrame.DataLength);//д������
    Tx->tFrame.FrameChecksum=ucCodeChecksum(buf,4+Tx->tFrame.DataLength);//��ȡ��֡��У����
    
    buf[4+Tx->tFrame.DataLength]=Tx->tFrame.FrameChecksum;//д��У������Ϣ
    
    return Err;
}

/*
* Function Name  : ucCode_IdleFrame
* Description    : ����һ������֡����
* Input          : Tx ���ͼ�¼�ṹ��  buf ���úõ�֡����
* Output         : None 
* Return         : 0�ɹ�  ����ʧ��
*/
u8 ucCode_IdleFrame(Tx_FrameRecord *Tx,u8 *buf)
{ 
    /*���÷��ͽṹ��*/
    Tx->tFrame.FrameHead=0XAA;
    Tx->tFrame.FrameFunction=ffIdle;//������֡����Ϊ����֡
    Tx->tFrame.FrameSequence=Tx->NextFrameSeq++;//д���к�
    Tx->tFrame.DataLength=0;//�����ݳ���
    Tx->tFrame.FrameData=Tx->DataBuf;
    
    /*�����ͽṹ������ д�� ����buf*/
    memcpy(buf,&(Tx->tFrame.FrameHead),4);//д��֡ͷ��Ϣ
    Tx->tFrame.FrameChecksum=ucCodeChecksum(buf,4+Tx->tFrame.DataLength);//��ȡ��֡��У����
    
    buf[4+Tx->tFrame.DataLength]=Tx->tFrame.FrameChecksum;//д��У������Ϣ
    
    return 0;
}

/*
* Function Name  : ucCodeChecksum
* Description    : �õ�һ�������У���ֵ
* Input          : buf ���� length ���ݳ��� 
* Output         : None 
* Return         : u8 У��ֵ
*/
u8 ucCodeChecksum(u8 *buf,u8 length)
{
    u8 check = 0;
	while(length--)
	{
		check += *buf++;
	}
    check = check%256;
	
    return 0xff-check;
}

