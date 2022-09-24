#include "applicationmotor.h"

/*
 * Filename   ：applicationmotor.c
 * Author     : pusheng       
 * Version    : 2.0
 * Date       : 2019.11.21 2020.1.14
 * Discription : 
 
 v1.0 电机处理应用函数
 
 v2.0 增加沿边处理应用函数
      将沿边方案由沿边差值来改变速度 变成 由沿边差值来改变机器人期望角度 在直线沿边效果更好
*/	

/*风机转速 单位（转/秒）*/
u32 g_FanRotationRate;

/***********************************   左右驱动轮  *****************************************/


//左轮前进 标记前进 Cycle：0-500  越大实际速度越慢
   
#define LEFT_WHEEL_FORWARD(Cycle)   LEFT_WHEEL_T3CH4_PWM_CYCLE(Cycle);\
                                    CTRL_WHEEL_L=0;\
                                    g_tLeftWheel.Direction = WheelForward;  
//左轮后退 标记后退 
#define LEFT_WHEEL_RETREAT(Cycle)   LEFT_WHEEL_T3CH4_PWM_CYCLE(Cycle);\
                                    CTRL_WHEEL_L=1;\
                                    g_tLeftWheel.Direction = WheelRetreat;
//右轮前进 标记前进
#define RIGHT_WHEEL_FORWARD(Cycle)  RIGHT_WHEEL_T3CH3_PWM_CYCLE( Cycle );\
                                    CTRL_WHEEL_R=0;\
                                    g_tRightWheel.Direction = WheelForward;
//右轮后退 标记后退 
#define RIGHT_WHEEL_RETREAT(Cycle)  RIGHT_WHEEL_T3CH3_PWM_CYCLE( Cycle );\
                                    CTRL_WHEEL_R=1;\
                                    g_tRightWheel.Direction = WheelRetreat;
 
#define SLOW_ACCELE_MAX_NUM   (5) //缓加速步进单位 mm/s


s16 g_sExpect_Angle=0;  //期望的角度
s16 g_sExpect_speed=200;//速度 mm/s
s16 g_sExpect_range=10; //角度误差范围 +- 1度 单位0.1度

/*驱动轮速度环PID参数 位置式 SpeedOut= P*E[n] + I*(E[n]+E[n-1]+E[n-2].....+E[1]))*/
float P_V           = 1;
float I_V           = 0.1;
float D_V           = 0;

__IO float    SpeedLeftUiV;         //左轮速度环积分结果
__IO float    SpeedRightUiV;        //右轮速度环积分结果
 
/*沿边传感器位置环PID参数  增量式 Out = P*( E[n]-E[n-1] ) + I*E[n] + D*(E[n]- 2*E[n-1] + E[n-2])  */
//float P_AE           = 3;
//float I_AE           = 0.1;
//float D_AE           = 0.1;
float P_AE           = 0;
float I_AE           = 8;
float D_AE           = 0;
IncPID_t g_tIncPID={0};
/*******************************************************************************
* Function Name  : usSpeed2Cycle(s16 speed)
* Description    : 速度转换为对应的PWM占空比
* Input          : speed :mm/s  范围 -400~400
* Output         : None
* Return         : 电机占空比 450~50
*******************************************************************************/
__inline u16 usSpeed2Cycle(s16 loc_Speed)
{
    if(loc_Speed<10 && loc_Speed>-10)//速度低于10mm/s 轮子响应线性太差 直接给0  电池电量AD大于3000 PWM线性响应450-0.
        return 500;//返回500占空比 电机停转
    
    if(loc_Speed<-400 && loc_Speed>400)//速度超过400mm/s 返回速度最大占空比
        return 450-400;
    
    if(loc_Speed>0)
        return 450-loc_Speed;
    else
        return 450+loc_Speed;
}

/*******************************************************************************
* Function Name  : vMotionWheel(void)
* Description    : 将轮子的实际转速靠近期望转速 使用到全局结构体 LeftWheel RightWheel
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vMotionWheel(void)
{
    int16_t PidTemp;
    PidTemp = sSpeed_L_PID( g_tLeftWheel.SoftSpeed, g_tLeftWheel.RealSpeed );//PID计算下一时刻速度
    if( PidTemp < 0 )//左轮倒退
    {                         
        LEFT_WHEEL_RETREAT(usSpeed2Cycle(PidTemp));
    } 
    else//左轮前进
    {
        LEFT_WHEEL_FORWARD(usSpeed2Cycle(PidTemp));
    }
    
    PidTemp = sSpeed_R_PID( g_tRightWheel.SoftSpeed, g_tRightWheel.RealSpeed );

    if( PidTemp < 0 )//右轮后退
    {               
        RIGHT_WHEEL_RETREAT(usSpeed2Cycle(PidTemp));
    } 
    else//右轮前进
    {
        RIGHT_WHEEL_FORWARD(usSpeed2Cycle(PidTemp));
    }            
}
/*******************************************************************************
* Function Name  : sSpeed_R_PID(int16_t RefV,int16_t FdbV)
* Description    : 右轮速度环
* Input          : RefV-期望速度指令，FdbV-当前速度反馈，单位mm/s
* Output         : None
* Return         : 右轮下一个时刻应该的速度
*******************************************************************************/
int16_t sSpeed_R_PID(int16_t RefV,int16_t FdbV)
{
    int16_t  UpV=0;
    int16_t  ErrV,OutTempV;
    int16_t  OutputV;

    ErrV = RefV - FdbV;//误差速度

    UpV = P_V*ErrV;      // 计算出比例部分
    
    SpeedRightUiV = SpeedRightUiV + ((float)(UpV*I_V));//计算出积分累加部分

    
    /*积分范围限制，不能无限累加*/
    if(SpeedRightUiV>UIMAXV)
    {
        SpeedRightUiV=UIMAXV;
    }else
    if(SpeedRightUiV<UIMINV)
    {
        SpeedRightUiV=UIMINV;
    }
    
    OutTempV = UpV+(int16_t)SpeedRightUiV;  //PI

    /*输出速度范围限制*/
    if (OutTempV > OUTMAXV)
    {
        OutputV =  OUTMAXV;
    }
    else if (OutTempV < OUTMINV)
    {
        OutputV = OUTMINV;
    }
    else
    {
        OutputV = OutTempV;
    }

    if(RefV>=0)
    {
        if(OutputV<0) OutputV=1;
    }
    else if(RefV<0)
    {
        if(OutputV>0) OutputV=-1;
    }
    return OutputV;
}
/*******************************************************************************
* Function Name  : sSpeed_L_PID(int16_t RefV,int16_t FdbV)
* Description    : 左轮速度环
* Input          : RefV-期望速度指令，FdbV-当前速度反馈，单位mm/s
* Output         : None
* Return         : 左轮下一个时刻应该的速度
*******************************************************************************/
int16_t sSpeed_L_PID(int16_t RefV,int16_t FdbV)
{
    int16_t  UpV=0;
    int16_t  ErrV,OutTempV;
    int16_t  OutputV;

    ErrV = RefV - FdbV;//误差速度

    UpV = P_V*ErrV;      // 计算出比例部分
    
    SpeedLeftUiV = SpeedLeftUiV + ((float)(UpV*I_V));//计算出积分累加部分

    /*积分范围限制，不能无限累加*/
    if(SpeedLeftUiV>UIMAXV)
    {
        SpeedLeftUiV=UIMAXV;
    }else
    if(SpeedLeftUiV<UIMINV)
    {
        SpeedLeftUiV=UIMINV;
    }
    OutTempV = UpV+(int16_t)SpeedLeftUiV;  //PI

    /*输出速度范围限制*/
    if (OutTempV > OUTMAXV)
    {
        OutputV =  OUTMAXV;
    }
    else if (OutTempV < OUTMINV)
    {
        OutputV = OUTMINV;
    }
    else
    {
        OutputV = OutTempV;
    }
    
    if(RefV>=0)
    {
        if(OutputV<0) OutputV=1;
    }
    else if(RefV<0)
    {
        if(OutputV>0) OutputV=-1;
    }
    
    return OutputV;
}

/*******************************************************************************
* Function Name  : vSoftSpeedAdjust
* Description    : 缓加速
* Input          :
* Output         : None
* Return         : None
*******************************************************************************/
void vSoftSpeedAdjust(void)//调用周期 3ms
{
    if(g_tLeftWheel.ExpectSpeed != g_tLeftWheel.SoftSpeed)
    {
        if(g_tLeftWheel.ExpectSpeed > g_tLeftWheel.SoftSpeed)
            g_tLeftWheel.SoftSpeed+=SLOW_ACCELE_MAX_NUM;
        else 
            g_tLeftWheel.SoftSpeed-=SLOW_ACCELE_MAX_NUM;
    }
    if(g_tRightWheel.ExpectSpeed != g_tRightWheel.SoftSpeed)
    {
        if(g_tRightWheel.ExpectSpeed > g_tRightWheel.SoftSpeed)
            g_tRightWheel.SoftSpeed+=SLOW_ACCELE_MAX_NUM;
        else 
            g_tRightWheel.SoftSpeed-=SLOW_ACCELE_MAX_NUM;
    }    
}

/*******************************************************************************
* Function Name  : s16 iIncPIDCalc(s16 NewData)
* Description    : 沿边传感器 增量式PID 计算
* Input          : NewData 新得到沿边传感器de 误差数据
* Output         : None
* Return         : 速度改变值
*******************************************************************************/
s16 iIncPIDCalc(s16 NewData)
{
    s16 loc_ChangeSpeed;


    loc_ChangeSpeed = P_AE*( NewData-g_tIncPID.LastData ) + \
                      I_AE*NewData + D_AE*(NewData- (2*g_tIncPID.LastData) + g_tIncPID.PreData);

    g_tIncPID.PreData=g_tIncPID.LastData;
    g_tIncPID.LastData=NewData;

    return loc_ChangeSpeed;
}
/*******************************************************************************
* Function Name  : ucRouteAdjustByAlongEdgeDistance(s16 loc_FbAngle,s16 loc_ExpAngle,s16 loc_Speed)
* Description    : 沿边路线微调 
* Input          : loc_FbValue ：沿边传感器的距离值（mm） loc_ExpValue 期望的距离值（mm）  loc_Speed 速度（mm/s）
* Output         : None 
* Return         : 0 函数执行成功 1 原地转向还未完成 其他失败
*******************************************************************************/
u8 ucRouteAdjustByAlongEdgeDistance(u16 loc_FbValue,u16 loc_ExpValue,s16 loc_Speed)
{
    s16 loc_DiffValue = loc_ExpValue - loc_FbValue;//差值
    s16 loc_Divisor;//角度 转 速度值 
    
    loc_Divisor=iIncPIDCalc(loc_DiffValue);//速度值的调整基准 

    
    if( abs(loc_Divisor) > 100)//角度太大了 速度调整值不能超过最大速度
    {
        if(loc_Divisor>0) 
            loc_Divisor=100;
        else
            loc_Divisor=-100;
    }
    /*
        如果loc_Diff_Angle 小于0 表示表示向右转去接近目标角度 右轮速度减小 左轮速度增加
        如果loc_Diff_Angle小于0  则loc_Divisor也小于0 为负数
        如果是后退 则loc_Speed为负数，负数越小 则速度越大 
    */
//		g_sExpect_Angle = CURRENT_ANGLE+loc_Divisor;
//		ucRoute_Adjust(CURRENT_ANGLE,g_sExpect_Angle,g_sExpect_speed);
g_tLeftWheel.ExpectSpeed=    loc_Speed - loc_Divisor;
g_tRightWheel.ExpectSpeed=   loc_Speed + loc_Divisor;
    return 0;
   
}
/*******************************************************************************
* Function Name  : ucRouteAdjustByAlongEdge(s16 loc_FbAngle,s16 loc_ExpAngle,s16 loc_Speed)
* Description    : 沿边路线微调 
* Input          : loc_FbValue ：沿边传感器的AD值  loc_ExpValue 期望的AD值 loc_Speed 速度
* Output         : None 
* Return         : 0 函数执行成功 1 原地转向还未完成 其他失败
*******************************************************************************/
u8 ucRouteAdjustByAlongEdge(u16 loc_FbValue,u16 loc_ExpValue,s16 loc_Speed)
{
    s16 loc_DiffValue = loc_ExpValue - loc_FbValue;//差值
    s16 loc_ChangeSpeed;//需要改变的速度值    

    if(loc_DiffValue> 500)
        loc_DiffValue=500;
    if(loc_DiffValue< -500)
        loc_DiffValue=-500;
        
    loc_ChangeSpeed=iIncPIDCalc(loc_DiffValue);
    
    if( abs(loc_ChangeSpeed) > 80)//角度太大了 速度调整值不能超过最大速度
    {
        if(loc_ChangeSpeed>0) 
            loc_ChangeSpeed=80;
        else
            loc_ChangeSpeed=-80;
    }
    /*
        如果loc_Diff_Angle 小于0 表示表示向右转去接近目标角度 右轮速度减小 左轮速度增加
        如果loc_Diff_Angle小于0  则loc_Divisor也小于0 为负数
        如果是后退 则loc_Speed为负数，负数越小 则速度越大 
    */
    g_tLeftWheel.ExpectSpeed=    loc_Speed + loc_ChangeSpeed;
    g_tRightWheel.ExpectSpeed=   loc_Speed - loc_ChangeSpeed;
       
    return 0;
   
}
/*******************************************************************************
* Function Name  : ucRoute_Adjust(s16 loc_FbAngle,s16 loc_ExpAngle,s16 loc_Speed)
* Description    : 路线微调 支持前进后退
* Input          : loc_FbAngle 当前角度 loc_ExpAngle 期望角度  范围1800~-1790 单位1/10度  
                   当前角度和目标角度 相差超过30度则调用原地旋转函数
                   loc_Speed 单位mm/s 范围-400-400 速度正值表示前进 负值表示后退
* Output         : None
* Return         : 0 函数执行成功 1 原地转向还未完成 其他失败
*******************************************************************************/
u8 ucRoute_Adjust(s16 loc_FbAngle,s16 loc_ExpAngle,s16 loc_Speed)
{
    static u8 loc_ucIsTurn=0;//是否需要原地转向 0 不需要
    s16 loc_Divisor;//角度转速度值
    u8 ERR=0;
    
    s16 loc_Diff_Angle = loc_ExpAngle - loc_FbAngle;//期望角度和当前实际角度的差值 正差值表示向左靠近 （最近原则） 

    if( loc_ExpAngle>3600 || loc_ExpAngle< -3600)//错误的角度
    {
        printf("ERROR!! %s, %d",__FILE__,__LINE__);
        return 2;//直接返回
    }
    
    if(loc_Diff_Angle<-1800)//超过半圈 用反向角度更近
    {
        loc_Diff_Angle=loc_Diff_Angle+3600;
    }
    else if(loc_Diff_Angle>1800)
    {
        loc_Diff_Angle=loc_Diff_Angle-3600;
    }
        
    if( abs(loc_Diff_Angle) > 300)//角度差距大于30度  
    {
        loc_ucIsTurn=1;//标记需要原地转向        
    }
    
    if( loc_ucIsTurn )//原地转向
    {
        if(ucTurn_Angle_Round_NoCheak(loc_Diff_Angle,abs(loc_Speed),g_sExpect_range) == 0)//角度误差范围1度
        {
            loc_ucIsTurn=0;//原地转向结束
            
            /*转向结束后设定轮子速度方向  ucTurn_Angle_Round()到达目标角度后会停止机器人 
            但是轮子前进角度还是一直保持上次的状态（一个前进一个后退）,所以这里直接设置为确定状态 方便下面计算*/
            if(loc_Speed>=0)
            {
                g_tLeftWheel.Direction=WheelForward;//速度正值 表示前进
                g_tRightWheel.Direction=WheelForward;
            }else
            {
                g_tLeftWheel.Direction=WheelRetreat;//表示后退
                g_tRightWheel.Direction=WheelRetreat;
            }
        }else
            return 1;//原地转向尚未结束
    }else
    {
        loc_Divisor=loc_Diff_Angle/2;//将角度值除以2 作为速度值的调整基准 
        
        if( abs(loc_Divisor) > abs(loc_Speed))//角度太大了 速度调整值不能超过最大速度
        {
            if(loc_Divisor)
                loc_Divisor=abs(loc_Speed);//正数赋值正最大值
            else
                loc_Divisor= - abs(loc_Speed);//负数赋值负最大值
            
            //printf("loc_Divisor %d,loc_Speed %d\r\n",loc_Divisor,loc_Speed);
            //printf("err! File Path:%s Line：%d\r\n ",__FILE__,__LINE__);
            ERR = 3;
        }
        /*
            如果loc_Diff_Angle 小于0 表示表示向右转去接近目标角度 右轮速度减小 左轮速度增加
            如果loc_Diff_Angle小于0  则loc_Divisor也小于0 为负数
            如果是后退 则loc_Speed为负数，负数越小 则速度越大 
        */
        g_tLeftWheel.ExpectSpeed=    loc_Speed - loc_Divisor;
        g_tRightWheel.ExpectSpeed=   loc_Speed + loc_Divisor;
 
 
    }        
    return ERR;
   
}
/*******************************************************************************
* Function Name  : ucTurn_Angle_Round(s16 loc_Current_Angle,s16 loc_Expect_Angle,u16 loc_Speed,u16 loc_AngRange)
* Description    : 机器人从当前角loc_Current_Angle 以给定速度 loc_Speed 转到期望角度（原地转向） 
* Input          : current_Angle当前角度 Expect_Angle期望角度 单位1/10度 1800~-1790  速度单位mm/s 范围100-400
                   loc_AngRange 角度允许误差范围 单位1/10度
* Output         : None
* Return         : 0 到达期望角度 1 还未到达  2 角度错误 3速度错误
*******************************************************************************/
u8 ucTurn_Angle_Round(s16 loc_Current_Angle,s16 loc_Expect_Angle,u16 loc_Speed,u16 loc_AngRange)
{
 
    s16 Diff_Angle = loc_Expect_Angle - loc_Current_Angle;//期望角度和当前实际角度的差值 正差值表示向左靠近 （最近原则） 
    
    if( loc_Expect_Angle>3600 || loc_Expect_Angle< -3600)//错误的角度  会导致半圈算法不准
    {
        printf("ERROR!! %s, %d",__FILE__,__LINE__);
        return 2;//直接返回错误
    }
        
    if( loc_Speed>400)//错误的速度
    {
        printf("ERROR!! %s, %d %d",__FILE__,__LINE__,loc_Speed);
        return 3;//直接返回错误
    }
    
    if(Diff_Angle<-1800)//超过半圈 用反向角度更近
    {
        Diff_Angle=Diff_Angle+3600;
    }
    else if(Diff_Angle>1800)
    {
        Diff_Angle=Diff_Angle-3600;
    }
        
    if(Diff_Angle < 0-loc_AngRange)//如果差值角度小于1度 表示向右转去接近目标角度    +-loc_AngRange范围的允许误差
    {
        if(Diff_Angle > -100)//误差角度小于10度了 减速
        {
            g_tLeftWheel.ExpectSpeed=    -Diff_Angle;//100-0
            g_tRightWheel.ExpectSpeed=   Diff_Angle;
        }else 
        {
            g_tLeftWheel.ExpectSpeed=    loc_Speed;//全速靠近
            g_tRightWheel.ExpectSpeed=  -loc_Speed;
        }
    }
    else if(Diff_Angle > 0+loc_AngRange)
    {
        if(Diff_Angle < 100)//误差角度小于10度了 减速
        {
            g_tLeftWheel.ExpectSpeed=   -Diff_Angle;//100-0
            g_tRightWheel.ExpectSpeed=  Diff_Angle;
        }else 
        {
            g_tLeftWheel.ExpectSpeed=  -loc_Speed;
            g_tRightWheel.ExpectSpeed=  loc_Speed;
        }
    }
    else//目标角度和实际角度在误差范围内 停止电机
    {   
        g_tLeftWheel.ExpectSpeed=  0;
        g_tRightWheel.ExpectSpeed= 0;
        return 0;
    }  
        return 1;
}
/*******************************************************************************
* Function Name  : u8 ucTurn_Angle_Round_NoCheak(s16 loc_Diff_Angle,u16 loc_Speed,u16 loc_AngRange)
* Description    : 机器人判断误差角度然后靠近目标角度 减少误差角度（原地转向）,不检查角度的有效性 传参注意
                   如果输入目标角度和期望角度可以参考这个函数：ucTurn_Angle_Round();

                   转弯速度需要大于 减速角度，所以speed必须大于200mm/s !!! 也可以减小减速角度要适配更小的转弯速度

* Input          : loc_Diff_Angle误差角度 单位1/10度 1800~-1790  正值 表示为左转靠近。 负值 右转
                   loc_AngRange 角度允许误差范围 单位1/10度  速度单位mm/s 范围100-400
* Output         : None
* Return         : 0 到底期望角度 1 还未到达 
*******************************************************************************/
u8 ucTurn_Angle_Round_NoCheak(s16 loc_Diff_Angle,u16 loc_Speed,u16 loc_AngRange)
{  
    if( (loc_Diff_Angle > -200) && (loc_Diff_Angle < 200) )//误差角度小于20度了 减速
    {
        /*loc_Diff_Angle 正值 为左转。 负值 右转*/
        g_tLeftWheel.ExpectSpeed=   -loc_Diff_Angle;//100-0
        g_tRightWheel.ExpectSpeed=   loc_Diff_Angle;
        
        //如果差值角度在范围值  表示已经达到目标角度   +-loc_AngRange范围的允许误差
        if( (loc_Diff_Angle > -loc_AngRange) && (loc_Diff_Angle < +loc_AngRange))  
        {
            g_tLeftWheel.ExpectSpeed=  0;
            g_tRightWheel.ExpectSpeed= 0;
            return 0;
        }
    }else//还差的多呢，加大力度！最大速度转向
    {
        if(loc_Diff_Angle < 0)//右转
        {
            g_tLeftWheel.ExpectSpeed=    loc_Speed;
            g_tRightWheel.ExpectSpeed=  -loc_Speed;
        }
        else if(loc_Diff_Angle > 0)
        {
            g_tLeftWheel.ExpectSpeed=  -loc_Speed;
            g_tRightWheel.ExpectSpeed=  loc_Speed;
        } 
    }
        return 1;
}

/*******************************************************************************
* Function Name  : ucRotateAroundAxle(s16 loc_Current_Angle,s16 loc_Expect_Angle,u16 loc_Speed,u16 loc_AngRange)
* Description    : 机器人从当前角loc_Current_Angle 以给定速度 loc_Speed 转到期望角度
                   （以轮子为圆心转向 一个轮子动 一个不动） 
* Input          : current_Angle当前角度 Expect_Angle期望角度 单位1/10度 1800~-1790  速度单位mm/s 范围100-400
                   loc_AngRange 角度允许误差范围 单位1/10度
                   Wnum 0 LWheel 左轮转动 1 右轮转动
* Output         : None
* Return         : 0 到达期望角度 1 还未到达  2 到达5度以内  3 角度错误 4 速度错误
*******************************************************************************/
u8 ucRotateAroundAxle(s16 loc_CurrAngle,s16 loc_ExpAngle,u16 loc_Speed,u16 loc_AngRange,WheelNum_e Wnum)
{
#define NEED_RETREAT 0 //是否需要后退 0 不需要

#if (NEED_RETREAT == 1)
    s16 Diff_Angle = loc_ExpAngle - loc_CurrAngle;//期望角度和当前实际角度的差值 正差值表示向左靠近 （最近原则） 
    
    if( loc_ExpAngle>3600 || loc_ExpAngle< -3600)//错误的角度  会导致半圈算法不准
        return 3;//直接返回错误 
    
    if( loc_Speed>400)//错误的速度
    {
        printf("ERROR!! %s, %d",__FILE__,__LINE__);
        return 4;//直接返回错误
    }
    
    if(Diff_Angle<-1800)//超过半圈 用反向角度更近
    {
        Diff_Angle=Diff_Angle+3600;
    }
    else if(Diff_Angle>1800)
    {
        Diff_Angle=Diff_Angle-3600;
    }
   
    if( (Diff_Angle > -loc_Speed) && (Diff_Angle < loc_Speed) )//误差角度小于10度了 减速。速度越大，减速角度越大 
    {
        /*Diff_Angle 正值 为左转。 负值 右转*/
        if(LWheel == Wnum)
        {
            g_tLeftWheel.ExpectSpeed=   -Diff_Angle;//100-0
            g_tRightWheel.ExpectSpeed=   0;
        }else
        {
            g_tLeftWheel.ExpectSpeed=    0;//100-0
            g_tRightWheel.ExpectSpeed=   Diff_Angle;
        }
        //如果差值角度在范围值  表示已经达到目标角度   +-loc_AngRange范围的允许误差
        if( (Diff_Angle > -loc_AngRange) && (Diff_Angle < +loc_AngRange))  
        {
            g_tLeftWheel.ExpectSpeed=  0;
            g_tRightWheel.ExpectSpeed= 0;
            return 0;
        }
        if((Diff_Angle > -50) && (Diff_Angle < 50))//误差在5度以内
        {
            return 2;
        }
    }else//还差的多呢，加大力度！最大速度转向
    {
        if(LWheel == Wnum)
        {
            if(Diff_Angle>=0)//左转
            {
                g_tLeftWheel.ExpectSpeed=    -loc_Speed;
                g_tRightWheel.ExpectSpeed=   0;
            }else
            {
                g_tLeftWheel.ExpectSpeed=    loc_Speed;
                g_tRightWheel.ExpectSpeed=   0;
            }
        }else
        {
            if(Diff_Angle>=0)//左转
            {
                g_tLeftWheel.ExpectSpeed=    0;
                g_tRightWheel.ExpectSpeed=   loc_Speed;
            }else
            {
                g_tLeftWheel.ExpectSpeed=    0;
                g_tRightWheel.ExpectSpeed=   -loc_Speed;
            }
        }
    }
    return 1;    
#endif
    
#if (NEED_RETREAT == 0)
    s16 Diff_Angle = loc_ExpAngle - loc_CurrAngle;//期望角度和当前实际角度的差值 正差值表示向左靠近 （最近原则） 
    
    if( loc_ExpAngle>3600 || loc_ExpAngle< -3600)//错误的角度  
        return 3;//直接返回错误 
    
    if( loc_Speed>400)//错误的速度
    {
        printf("ERROR!! %s, %d",__FILE__,__LINE__);
        return 4;//直接返回错误
    }
    if(Wnum == LWheel)//左轮向前转动 将角度全部转换成负值，表示右转
    {
        if(Diff_Angle>0)
        {
            Diff_Angle=Diff_Angle-3600;
        }
    }else
    {
        if(Diff_Angle<0)
        {
            Diff_Angle=Diff_Angle+3600;
        }
    }

    if( (Diff_Angle > -loc_Speed) && (Diff_Angle < loc_Speed) )//误差角度小于10度了 减速。速度越大，减速角度越大 
    {
        /*Diff_Angle 正值 为左转。 负值 右转*/
        if(LWheel == Wnum)
        {
            g_tLeftWheel.ExpectSpeed=    abs(Diff_Angle);//100-0
            g_tRightWheel.ExpectSpeed=   0;
        }else
        {
            g_tLeftWheel.ExpectSpeed=    0;
            g_tRightWheel.ExpectSpeed=   abs(Diff_Angle);
        }
        //如果差值角度在范围值  表示已经达到目标角度   +-loc_AngRange范围的允许误差
        if( (Diff_Angle > -loc_AngRange) && (Diff_Angle < +loc_AngRange))  
        {
            g_tLeftWheel.ExpectSpeed=  0;
            g_tRightWheel.ExpectSpeed= 0;
            return 0;
        }
        if((Diff_Angle > -50) && (Diff_Angle < 50))//误差在5度以内
        {
            return 2;
        }
    }else//还差的多呢，加大力度！最大速度转向
    {
        if(LWheel == Wnum)
        {
            g_tLeftWheel.ExpectSpeed=    loc_Speed;
            g_tRightWheel.ExpectSpeed=   0;

        }else
        {
            g_tLeftWheel.ExpectSpeed=    0;
            g_tRightWheel.ExpectSpeed=   loc_Speed;

        }
    }
    return 1;    
#endif
}


/*******************************************************************************
* Function Name  : vDataMemoryCopy
* Description    : 数据拷贝
* Input          : *pusTarget：目标缓冲地址，*pusSource：源地址，ucLength：数据长度
* Output         : None
* Return         : None
*******************************************************************************/
void vDataMemoryCopy( uint16_t *pusTarget, uint16_t *pusSource, uint8_t ucLength )
{
    uint8_t i;
    
    for(i=0;i<ucLength;i++)
    {
        pusTarget[i] = pusSource[i];
    }
    
}
/*******************************************************************************
* Function Name  : usGetMiddleValue
* Description    : 获取一维数组大小中间值的数 并清空一半最大的数据
* Input          : *pusBuffer：数组首地址，ucLength：数据长度
* Output         : None
* Return         : usMiddle：中间值
*******************************************************************************/
__inline uint16_t usGetMiddleValue( uint16_t *pusBuffer, uint8_t ucLength )
{
    uint8_t i;
    uint16_t usMiddle,usMax,usCount = 0,usPoint;
    
    while( usCount <= (ucLength/2) )
    {
        usPoint = 0;
        usMax = pusBuffer[0];
        for( i=1;i<ucLength;i++ ) 
        {      
            if( pusBuffer[i] > usMax ) 
            {
                usMax = pusBuffer[i];
                usPoint = i;
            }                
            
        }
        usMiddle = usMax;
        pusBuffer[usPoint] = 0;   
        usCount++;       
    }
    
    return usMiddle;
}
/*******************************************************************************
* Function Name  : usGetWheelSpeed
* Description    : 获取轮子速度，编码采样缓冲大小中间值作为捕获时间
* Input          :
* Output         : None
* Return         : None
*******************************************************************************/
int16_t usGetWheelSpeed( uint16_t usBuffer[], WheelDirection_e Direction ) 
{ 

    int16_t usSpeed;     
    uint16_t usArray[EncodeNumber];
        
    vDataMemoryCopy( usArray, usBuffer, EncodeNumber );               
    usSpeed = SINGLE_ENCODE_DISTANCE/usGetMiddleValue( usArray, EncodeNumber );

    if( Direction == WheelRetreat )
    {
        usSpeed = -usSpeed;
    }
    else if( Direction == WheelStop )
    {
        usSpeed = 0;
    }
    
    return usSpeed;
} 

/*******************************************************************************
* Function Name  : vMotorSpeedUpdateTask(void *p)
* Description    : 电机速度更新任务
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vMotorSpeedUpdateTask(void *p)
{
    while(1)
    {
        if(g_tLeftWheel.TimeOut++ > 10) //超过10次 都没有捕获到脉冲
            g_tLeftWheel.RealSpeed=0;  //速度标记为0
        else        
            g_tLeftWheel.RealSpeed = usGetWheelSpeed( g_tLeftWheel.usEncodeBuffer, g_tLeftWheel.Direction );

        if(g_tRightWheel.TimeOut++ > 10) 
            g_tRightWheel.RealSpeed=0;
        else  
            g_tRightWheel.RealSpeed = usGetWheelSpeed( g_tRightWheel.usEncodeBuffer, g_tRightWheel.Direction );
        
        g_FanRotationRate=g_FanEncodeCount*1000/SPEED_UPDATE_CYCLE/FAN_ENCODE_COUNT;//风机转速
        g_FanEncodeCount=0;
        
        vTaskDelay(SPEED_UPDATE_CYCLE);
    }    
}
/*******************************************************************************
* Function Name  : vAllMotorMotionTask(void *p)
* Description    : 扫地机器人上所有的电机的运动任务
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vAllMotorMotionTask(void *p)
{
    while(1)
    {
//        DataScope_Get_Channel_Data(g_tLeftWheel.RealSpeed,1);
//        DataScope_Get_Channel_Data(g_tRightWheel.RealSpeed,2);
//        DataScope_Get_Channel_Data(g_tLeftWheel.SoftSpeed,3);
//        DataScope_Get_Channel_Data(g_tRightWheel.SoftSpeed,4);
//        vWaveFromOut();
        vMotionWheel();//驱动轮速度调整
        vTaskDelay(10);
    }
}

/*******************************************************************************
* Function Name  : vSoftSpeedAdvanceTask(void *p)
* Description    : 软加速任务
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void vSoftSpeedAdvanceTask(void *p)
{
    while(1)
    {
        vSoftSpeedAdjust();
        vTaskDelay(2);
    }
}

/*************************   风机  *******************************/

/*******************************************************************************
* Function Name  : uiGetFanRotationRate(void)
* Description    : 获取风机转速
* Input          : None
* Output         : 转速 单位 r/s
* Return         : None
*******************************************************************************/
u32 uiGetFanRotationRate(void)
{
    return g_FanRotationRate;
}
/*******************************************************************************
* Function Name  : vSetFanSpeed(void)
* Description    : 设置风机转速
* Input          : None
* Output         : PWM占空比 0-500  越大越快
* Return         : None
*******************************************************************************/
void vSetFanSpeed(u16 PWMPulse)
{
    if(PWMPulse)
        FAN_POWER_UP
    else     
        FAN_POWER_DOWN
    
    FAN_T5CH2_PWM_CYCLE(PWMPulse); 
}
    /*************************   边刷  *******************************/

/*******************************************************************************
* Function Name  : vSetSideBrushSpeed(void)
* Description    : 设置边刷转速
* Input          : None
* Output         : PWM占空比 0-500  越大越快 ,Dir方向 1为正
* Return         : None
*******************************************************************************/
void vSetSideBrushSpeed(u8 Dir,u16 PWMPulse)
{
    if(Dir)
        SIDE_BRUSH_CTRL=1;
    else     
        SIDE_BRUSH_CTRL=0;
    
    SIDE_BRUSH_T4CH3_PWM_CYCLE(PWMPulse); 
}

    /*************************   滚刷  *******************************/
/*******************************************************************************
* Function Name  : vSetRollBrushSpeed(void)
* Description    : 设置滚刷转速
* Input          : None
* Output         : PWM占空比 0-500  越大越快 ,Dir方向 1为正
* Return         : None
*******************************************************************************/
void vSetRollBrushSpeed(u8 Dir,u16 PWMPulse)
{
    if(Dir)
        ROLL_BRUSH_CTRL=0;
    else     
        ROLL_BRUSH_CTRL=1;
    
    ROLL_BRUSH_T4CH2_PWM_CYCLE(PWMPulse); 
}

