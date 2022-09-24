#include "protocol.h"

/*
 * Filename   ：protocol.c
 * Author     : PuSheng 
 * Version    : 2.0
 * Date       : 2019.12.25  2020.1.4
 
 2.0: 版本重写 2019.12.25 
 2.1: 帧分析加入了 功能字节对比完成后 立马将帧功能字节写入 这样可以防止错误的去回应 回应帧
 
 一．	帧的组成
帧协议采用 帧起始字节+帧功能字节+帧序列字节+数据长度字节+数据字节+校验字节 来组成。

1.1 帧起始字节
起始字节定义为单个字节0XAA。

1.2帧功能字节
帧功能字节也为单字节，具体功能见表1.1
表1.1
功能字节	表示含义
0xb0	回应帧：当接收到一帧数据后，接收方需要回应一帧回应帧。(回应帧不用回应)
0xb1	命令帧：表示该帧的数据字节是一个指令
0xb2	数据帧：表示该帧的数据字节是一个数据
0xb3	空闲帧：为了测试通信是否正常，数据长度字节为0。没有数据字节
	
	
	

1.3帧序列字节
帧序列也是占一个字节，表示该帧的顺序，发送方拥有一个唯一的发送序列，数值从0到255每发送一帧自加1（循环自加）。
作用在于标识该帧，当接收方发现帧校验失败或者检测到漏帧，可以用功能字节的回应帧告知发送方，重新发送该帧。

1.4 数据长度字节
数据长度占1个字节，表示该帧的数据有几个字节，范围0-50。如果数据长度字节为0，该帧则没有数据字节
1.5 数据字节
表示该帧携带的数据，长度由数据长度字节制定，若数据长度字节为0，则没有数据字节。最大50个字节。
1.6 校验字节
通过对整个帧进行checksum校验得到的数据写到该字节。校验范围包括帧起始字节到数据字节。

二．	功能帧详解
2.1 [回应帧]
接收方：当接收到一帧数据时（接收到的不是[回应帧]），接收方应当回应一帧[回应帧]。[回应帧]中包括了一个字节的数据字节，
该数据指示了接收方是否接收到完整数据。[回应帧]的帧序列字节为0.
Ex：回应帧示例 AA B0 00 01 XX YY
其中AA是一帧开始，B0表示该帧是回应帧，00帧序列为0（回应帧的帧序列恒为0），01表示有一个字节的数据。
YY表示对整个数据帧的checksum值。
XX：01 帧格式错误
	02 校验值错误
	03 帧序列错误
	04 接收成功
其中03帧序列错误 的数据长度应该为2，第一个字节表示03错误，第二个字节表示应该的序列号。EX：AA B0 00 02 01 XX YY
发送方：当发送一帧数据（非回应帧）后，发送方需要等待一个回应帧，才能继续发送帧信号。
若1秒内都没有接收到[回应帧]或者没有接收到正确的[回应帧]。则重新发送本次数据。若接收到数据字节为03（接收成功）的[回应帧]，
则可以马上开始发送下一帧的数据。若接收到数据字节不为03的[回应帧]，则可以马上开始重新发送本次数据。

2.2 [命令帧]
通过命令帧来请求数据，或者表明接下来发送的数据是什么类型的。
Ex：命令帧示例 AA B1 XX 01 YY ZZ
其中XX表示该帧的帧序列字节，YY表示具体命令。ZZ表示校验位
YY：C0 接下来发送的数据是固件程序，表示接收方需要将接下来接受到的数据更新到固件

2.3 [数据帧]
数据帧主要就是携带数据传输，每帧数据字节最多50字节。一般都是先有一个命令帧表示接下来的数据帧是什么类型的数据。
 
2.4 [空闲帧]
空闲帧的主要作用就是检测是否连接、心跳包。固定格式为
AA B3 XX 00 YY
其中AA为帧头，B3是空闲帧功能标识，XX是帧序列字节。00表示没有该帧数据，所以YY就是CheckSum。该帧没有数据字节。
若10秒没有任何帧要发送 则必须发送帧空闲帧

 
*/



Rx_FrameRecord FR_Rx={0};
Tx_FrameRecord FR_Tx={0};

/*
* Function Name  : eFrame_Analy
* Description    : 将数据BUF解析为帧数据
* Input          : buf 接收原始数据 length 数据长度 RFR 接收帧结构体
* Output         : None 
* Return         : FrameErr_e 错误类型
*/
FrameErr_e eFrame_Analy(u8 *buf,u8 length,Rx_FrameRecord *RFR)
{
    u16 i;
    FrameErr_e FErr = FrameFromatErr;
    
    for(i=0;i<length;i++)
    {   
        /*检查帧头*/
        if(buf[i] != 0XAA)
        {
            continue;
        }
        /*检查帧功能字节是否合法*/
        if(buf[i+FRAME_FUNCTION_POS] < ffRespond || buf[i+FRAME_FUNCTION_POS] >= ffMax)
        {
            FErr=FrameFromatErr;continue;
        }
        RFR->tFrame.FrameFunction=(FrameFunction_e)buf[i+FRAME_FUNCTION_POS];//写入帧功能字节 防止识别到上一帧的帧功能字节
        
        /*检查帧数据长度字节是否合法*/
        if(buf[i+FRAME_DATA_LENGTH_POS] > FRAME_DATA_MAX_LENGTH)
        {
             FErr=FrameLengthErr;continue;
        }
        /*检查帧校验字节是否合法*/
        if( bJudgeChecksum(&buf[i] , 5+buf[i+FRAME_DATA_LENGTH_POS]) == false)
        {
            FErr=FrameCheckErr;continue;
        }
        
        if(false == RFR->IsStartReceive)//是否开始记录序列号
        {
            RFR->IsStartReceive = true;
            RFR->NextFrameSeq = buf[i+FRAME_SEQUENCE_POS];
        }
        /*接收到的帧序列不是应该的帧序列 表示不合法，响应帧没有帧序列*/
        if(RFR->NextFrameSeq != buf[i+FRAME_SEQUENCE_POS] && buf[i+FRAME_FUNCTION_POS] != ffRespond)//帧序列不合法
        {
            FErr=FrameSeqErr;continue;
        }
        else
        {
            /*成功接收到一帧数据*/ 
            RFR->ContinueErrNum=0;//清零错误计算
            RFR->FrameErrType=FrameSuccess;//标记错误类型 为解码成功
            RFR->IsFrameNotProcess=true;//标记有数据需要处理       
            if(buf[i+FRAME_FUNCTION_POS] != ffRespond)//响应帧 没有帧序列 不用自加序列号
                RFR->NextFrameSeq++;//帧序列加1
            RFR->tFrame.FrameData=RFR->DataBuf;//帧数据地址对应数据区
            
            RFR->tFrame.FrameHead=buf[i+FRAME_HEAD_POS];//写入帧头字节
            RFR->tFrame.FrameFunction=(FrameFunction_e)buf[i+FRAME_FUNCTION_POS];//写入帧功能字节
            RFR->tFrame.FrameSequence=buf[i+FRAME_SEQUENCE_POS];//写入帧序列字节
            RFR->tFrame.DataLength=buf[i+FRAME_DATA_LENGTH_POS];//写入帧数据长度字节
            RFR->tFrame.FrameChecksum=buf[i+4+buf[i+FRAME_DATA_LENGTH_POS]];//写入校验字节
 
            memcpy(RFR->DataBuf,&buf[i+FRAME_DATA_POS],buf[i+FRAME_DATA_LENGTH_POS]);//写入数据
            return FrameSuccess;
        }
    }
    
    
    RFR->ContinueErrNum++;
    RFR->FrameErrType=FErr;
    return FErr;

}

/*
* Function Name  : bJudgeChecksum
* Description    : 求解一个数组是否满足校验和
* Input          : buf 接收原始数据 length 数据长度 
* Output         : None 
* Return         : false 校验不通过   true 校验通过
*/
bool bJudgeChecksum(u8 *buf,u8 length)
{
    u8 check = 0;
	while(length--)
	{
		check += *buf++;//所有数据累计
	}
    check = check%256;
	if(check == 0xFF)//和为FF 表示校验正确
        return true;
    else
        return false;
}

/*
* Function Name  : ucCode_RespondFrame
* Description    : 配置一个回应帧数据
* Input          : Tx 发送记录结构体 Rx 接收记录结构体 buf 配置好的帧数据
* Output         : None 
* Return         : 0成功  其他失败
*/
u8 ucCode_RespondFrame(Tx_FrameRecord *Tx,Rx_FrameRecord *Rx,u8 *buf)
{
    Tx->tFrame.FrameHead=0XAA;
    Tx->tFrame.FrameFunction=ffRespond;//表明该帧数据为 响应帧
    Tx->tFrame.FrameSequence=0;//响应帧 的序列恒为0
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
    memcpy(buf,&(Tx->tFrame.FrameHead),4);//写入帧头信息
    memcpy(&buf[4],Tx->DataBuf,Tx->tFrame.DataLength);//写入数据
    Tx->tFrame.FrameChecksum=ucCodeChecksum(buf,4+Tx->tFrame.DataLength);//获取整帧的校验码
    
    buf[4+Tx->tFrame.DataLength]=Tx->tFrame.FrameChecksum;//写入校验码信息
    
    return 0;
}

/*
* Function Name  : ucCode_CommandFrame
* Description    : 配置一个命令帧数据
* Input          : Tx 发送记录结构体 Commandbuf 命令数据  length 长度 buf 配置好的帧数据
* Output         : None 
* Return         : 0成功  其他失败
*/
u8 ucCode_CommandFrame(Tx_FrameRecord *Tx,u8 *Commandbuf,u8 length,u8 *buf)
{
    u8 Err=0;
    if(length > FRAME_DATA_MAX_LENGTH)//数据超过一帧最大极限
    {
        length=FRAME_DATA_MAX_LENGTH;
        Err=1;
    }
        
    /*配置发送结构体*/
    Tx->tFrame.FrameHead=0XAA;
    Tx->tFrame.FrameFunction=ffCommand;//表明该帧数据为 命令帧
    Tx->tFrame.FrameSequence=Tx->NextFrameSeq++;//写序列号
    Tx->tFrame.DataLength=length;
    Tx->tFrame.FrameData=Tx->DataBuf;
    memcpy(Tx->DataBuf,Commandbuf,length);//写入数据信息
    
    /*将发送结构体数据 写入 发送buf*/
    memcpy(buf,&(Tx->tFrame.FrameHead),4);//写入帧头信息
    memcpy(&buf[4],Tx->DataBuf,Tx->tFrame.DataLength);//写入数据
    Tx->tFrame.FrameChecksum=ucCodeChecksum(buf,4+Tx->tFrame.DataLength);//获取整帧的校验码
    
    buf[4+Tx->tFrame.DataLength]=Tx->tFrame.FrameChecksum;//写入校验码信息
    
    return Err;
}
/*
* Function Name  : ucCode_DataFrame
* Description    : 配置一个数据帧数据
* Input          : Tx 发送记录结构体 Databuf 数据 length长度 buf 配置好的帧数据
* Output         : None 
* Return         : 0成功  其他失败
*/
u8 ucCode_DataFrame(Tx_FrameRecord *Tx,u8 *Databuf,u8 length,u8 *buf)
{
    u8 Err=0;
    if(length > FRAME_DATA_MAX_LENGTH)//数据超过一帧最大极限
    {
        length=FRAME_DATA_MAX_LENGTH;
        Err=1;
    }
        
    /*配置发送结构体*/
    Tx->tFrame.FrameHead=0XAA;
    Tx->tFrame.FrameFunction=ffData;//表明该帧数据为 数据帧
    Tx->tFrame.FrameSequence=Tx->NextFrameSeq++;//写序列号
    Tx->tFrame.DataLength=length;
    Tx->tFrame.FrameData=Tx->DataBuf;
    memcpy(Tx->DataBuf,Databuf,length);//写入数据信息
    
    /*将发送结构体数据 写入 发送buf*/
    memcpy(buf,&(Tx->tFrame.FrameHead),4);//写入帧头信息
    memcpy(&buf[4],Tx->DataBuf,Tx->tFrame.DataLength);//写入数据
    Tx->tFrame.FrameChecksum=ucCodeChecksum(buf,4+Tx->tFrame.DataLength);//获取整帧的校验码
    
    buf[4+Tx->tFrame.DataLength]=Tx->tFrame.FrameChecksum;//写入校验码信息
    
    return Err;
}

/*
* Function Name  : ucCode_IdleFrame
* Description    : 配置一个空闲帧数据
* Input          : Tx 发送记录结构体  buf 配置好的帧数据
* Output         : None 
* Return         : 0成功  其他失败
*/
u8 ucCode_IdleFrame(Tx_FrameRecord *Tx,u8 *buf)
{ 
    /*配置发送结构体*/
    Tx->tFrame.FrameHead=0XAA;
    Tx->tFrame.FrameFunction=ffIdle;//表明该帧数据为空闲帧
    Tx->tFrame.FrameSequence=Tx->NextFrameSeq++;//写序列号
    Tx->tFrame.DataLength=0;//无数据长度
    Tx->tFrame.FrameData=Tx->DataBuf;
    
    /*将发送结构体数据 写入 发送buf*/
    memcpy(buf,&(Tx->tFrame.FrameHead),4);//写入帧头信息
    Tx->tFrame.FrameChecksum=ucCodeChecksum(buf,4+Tx->tFrame.DataLength);//获取整帧的校验码
    
    buf[4+Tx->tFrame.DataLength]=Tx->tFrame.FrameChecksum;//写入校验码信息
    
    return 0;
}

/*
* Function Name  : ucCodeChecksum
* Description    : 得到一个数组的校验和值
* Input          : buf 数据 length 数据长度 
* Output         : None 
* Return         : u8 校验值
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

