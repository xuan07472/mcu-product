#ifndef PROTOCOL__H_
#define PROTOCOL__H_

#include "sys.h"
#include "stdbool.h"
#include "string.h"

#define FRAME_DATA_MAX_LENGTH   (50)//帧数据字节最大长度

#define FRAME_HEAD_POS         (0)//帧头字节在一帧中的位置
#define FRAME_FUNCTION_POS     (1)//帧功能字节在一帧中的位置
#define FRAME_SEQUENCE_POS     (2)//帧序列字节在一帧中的位置
#define FRAME_DATA_LENGTH_POS  (3)//帧数据长度字节在一帧中的位置
#define FRAME_DATA_POS         (4)//帧数据字节在一帧中的位置

/*单字节 帧命令枚举*/
typedef enum
{
    UpdateFirmwareStart  =0XC0,//开始固件更新命令
    UpdateFirmwareDone   =0XC1,//固件更新完毕命令
    BehaviorControl      =0xC2,//行为控制命令
    RequestState         =0xC3,//请求状态命令
    RouteCoord           =0xC4,//线路坐标命令
}fCommand_e;
/*功能字节枚举*/
typedef enum
{
    ffRespond=0xb0,//回应
    ffCommand=0xb1,//命令
    ffData   =0xb2,//数据
    ffIdle   =0xb3,//空闲
    ffMax,//功能字节结尾
}FrameFunction_e;

/*帧错误类型枚举*/
typedef enum
{
    FrameFromatErr=0x1,//格式错误
    FrameCheckErr =0x2,//校验错误
    FrameSeqErr   =0x3,//序列错误
    FrameLengthErr=0x4,//长度错误
    FrameSuccess  =0x5,//无错误
}FrameErr_e;

/*帧结构体*/
__packed typedef struct
{
    u8 FrameHead;//帧头 固定为0XAA
    FrameFunction_e FrameFunction;//帧功能标识字节
    u8 FrameSequence;//帧序列 0-255 循环加1
    u8 DataLength;//帧数据长度 0-50
    u8 *FrameData;//帧数据地址
    u8 FrameChecksum;//帧校验
}Frame_t;

/*帧接收记录*/
__packed typedef struct
{
    Frame_t tFrame;//帧信息
    u8 DataBuf[FRAME_DATA_MAX_LENGTH];//帧数据
    u8 NextFrameSeq;//下次应该的序列
    
    u8 ContinueErrNum;//连续错误次数
    bool IsStartReceive;//是否开始接收
    bool IsFrameNotProcess;//帧数据还未处理
    FrameErr_e FrameErrType;//帧错误类型
}Rx_FrameRecord;


/*帧发送记录*/
__packed typedef struct
{
    Frame_t tFrame;//帧信息
    u8 DataBuf[FRAME_DATA_MAX_LENGTH];//帧数据
    u8 NextFrameSeq;//下次应该的序列
    bool IsSendOut;//是否发送了一帧数据？
    bool IsReceiveRespond;//是否接受到回应
}Tx_FrameRecord;

extern Rx_FrameRecord FR_Rx;
extern Tx_FrameRecord FR_Tx;

bool bJudgeChecksum(u8 *buf,u8 length);
u8 ucCodeChecksum(u8 *buf,u8 length);

FrameErr_e eFrame_Analy(u8 *buf,u8 length,Rx_FrameRecord *RFR);

u8 ucCode_RespondFrame(Tx_FrameRecord *Tx,Rx_FrameRecord *Rx,u8 *buf);
u8 ucCode_CommandFrame(Tx_FrameRecord *Tx,u8 *Commandbuf,u8 length,u8 *buf);
u8 ucCode_DataFrame(Tx_FrameRecord *Tx,u8 *Databuf,u8 length,u8 *buf);
u8 ucCode_IdleFrame(Tx_FrameRecord *Tx,u8 *buf);

#endif

