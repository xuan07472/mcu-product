//////////////////////////////////////////////////////////////////////////////////
//  功能描述   : 0.69寸OLED 接口演示例程(STM32F103C8T6 IIC)
//              说明:
//              ----------------------------------------------------------------
//              GND   电源地
//              VCC   接3.3v电源
//              SCL   接PB8（SCL）
//              SDA   接PB9（SDA）
//////////////////////////////////////////////////////////////////////////////////�

#include "oled.h"
#include "stdlib.h"
#include "oledfont.h"
#include "delay.h"
//OLED的显存
//存放格式如下.
//[0]0 1 2 3 ... 127
//[1]0 1 2 3 ... 127
//[2]0 1 2 3 ... 127
//[3]0 1 2 3 ... 127
//[4]0 1 2 3 ... 127
//[5]0 1 2 3 ... 127
//[6]0 1 2 3 ... 127
//[7]0 1 2 3 ... 127

u8 OLED_GRAM[128][8];//定义数组，先预存储在ram中，再一起写进OLED
/**********************************************
//IIC Start
**********************************************/
void IIC_Start(void)
{

    OLED_SCLK_Set() ;
    OLED_SDIN_Set();
    OLED_SDIN_Clr();
    OLED_SCLK_Clr();
}

/**********************************************
//IIC Stop
**********************************************/
void IIC_Stop(void)
{
    OLED_SCLK_Set() ;
//	OLED_SCLK_Clr();
    OLED_SDIN_Clr();
    OLED_SDIN_Set();

}

void IIC_Wait_Ack(void)
{
    OLED_SCLK_Set() ;
    OLED_SCLK_Clr();
}
/**********************************************
// IIC Write byte
**********************************************/

void Write_IIC_Byte(unsigned char IIC_Byte)
{
    unsigned char i;
    unsigned char m, da;
    da = IIC_Byte;
    OLED_SCLK_Clr();

    for(i = 0; i < 8; i++)
    {
        m = da;
        //	OLED_SCLK_Clr();
        m = m & 0x80;

        if(m == 0x80)
        {
            OLED_SDIN_Set();
        }
        else OLED_SDIN_Clr();

        da = da << 1;
        OLED_SCLK_Set();
        OLED_SCLK_Clr();
    }
}
/**********************************************
// IIC Write Command
**********************************************/
void Write_IIC_Command(unsigned char IIC_Command)
{
    IIC_Start();
    Write_IIC_Byte(0x78);            //Slave address,SA0=0
    IIC_Wait_Ack();
    Write_IIC_Byte(0x00);			//write command
    IIC_Wait_Ack();
    Write_IIC_Byte(IIC_Command);
    IIC_Wait_Ack();
    IIC_Stop();
}
/**********************************************
// IIC Write Data
**********************************************/
void Write_IIC_Data(unsigned char IIC_Data)
{
    IIC_Start();
    Write_IIC_Byte(0x78);			//D/C#=0; R/W#=0
    IIC_Wait_Ack();
    Write_IIC_Byte(0x40);			//write data
    IIC_Wait_Ack();
    Write_IIC_Byte(IIC_Data);
    IIC_Wait_Ack();
    IIC_Stop();
}
void OLED_WR_Byte(unsigned dat, unsigned cmd)
{
    if(cmd)
    {
        Write_IIC_Data(dat);
    }
    else
    {
        Write_IIC_Command(dat);
    }
}

/********************************************
// fill_Picture
********************************************/
void fill_picture(unsigned char fill_Data)
{
    unsigned char m, n;

    for(m = 0; m < 8; m++)
    {
        OLED_WR_Byte(0xb0 + m, 0);		//page0-page1
        OLED_WR_Byte(0x00, 0);		//low column start address
        OLED_WR_Byte(0x10, 0);		//high column start address

        for(n = 0; n < 128; n++)
        {
            OLED_WR_Byte(fill_Data, 1);
        }
    }
}

//坐标设置
void OLED_Set_Pos(unsigned char x, unsigned char y)
{
    OLED_WR_Byte(0xb0 + y, OLED_CMD);
    OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);
    OLED_WR_Byte((x & 0x0f), OLED_CMD);
}
//开启OLED显示
void OLED_Display_On(void)
{
    OLED_WR_Byte(0X8D, OLED_CMD); //SET DCDC命令
    OLED_WR_Byte(0X14, OLED_CMD); //DCDC ON
    OLED_WR_Byte(0XAF, OLED_CMD); //DISPLAY ON
}
//关闭OLED显示
void OLED_Display_Off(void)
{
    OLED_WR_Byte(0X8D, OLED_CMD); //SET DCDC命令
    OLED_WR_Byte(0X10, OLED_CMD); //DCDC OFF
    OLED_WR_Byte(0XAE, OLED_CMD); //DISPLAY OFF
}
//清屏函数,清完屏,整个屏幕是黑色的,和没点亮一样
void OLED_Clear(void)
{
    u8 i, n;

    for(i = 0; i < 8; i++)
    {
        OLED_WR_Byte (0xb0 + i, OLED_CMD); //设置页地址（0~7）
        OLED_WR_Byte (0x00, OLED_CMD);     //设置显示位置—列低地址
        OLED_WR_Byte (0x10, OLED_CMD);     //设置显示位置—列高地址

        for(n = 0; n < 128; n++)OLED_WR_Byte(0, OLED_DATA);
    } //更新显示
}
//重置数组OLED_GRAM，使全部为0
void OLED_Clear_GRAM(void)
{
    u8 x, y;

    for(x = 0; x < 128; x++)
    {
        for(y = 0; y < 8; y++)
        {
            OLED_GRAM[x][y] = 0;
        }
    }
}

//更新显存到OLED
void OLED_Refresh_Gram(void)
{
    u8 i, n;

    for(i = 0; i < 8; i++)
    {
        OLED_WR_Byte (0xb0 + i, OLED_CMD); //设置页地址（0~7）
        OLED_WR_Byte (0x00, OLED_CMD);     //设置显示位置—列低地址
        OLED_WR_Byte (0x10, OLED_CMD);     //设置显示位置—列高地址

        for(n = 0; n < 128; n++)OLED_WR_Byte(OLED_GRAM[n][i], OLED_DATA);
    }
}

//画点
//x:0~127
//y:0~63
//t:1 填充 0,清空
void OLED_DrawPoint(u8 x, u8 y, u8 t)
{
    u8 pos, bx, temp = 0;

    if(x > 127 || y > 63)return; //超出范围了.

    pos = 7 - y / 8;
    bx = y % 8;
    temp = 1 << (7 - bx);

    if(t)OLED_GRAM[x][pos] |= temp;
    else OLED_GRAM[x][pos] &= ~temp;
}
//画加号
//x:1~126，中心坐标的x
//y:1~62，中心坐标的y
void OLED_DrawPlusSign(u8 x, u8 y)
{
    OLED_DrawPoint(x, y - 1, 1);
    OLED_DrawPoint(x, y, 1);
    OLED_DrawPoint(x, y + 1, 1);
    OLED_DrawPoint(x - 1, y, 1);
    OLED_DrawPoint(x + 1, y, 1);
}
//x1,y1,x2,y2 填充区域的对角坐标
//确保x1<=x2;y1<=y2 0<=x1<=127 0<=y1<=63
//dot:0,清空;1,填充
void OLED_Fill(u8 x1, u8 y1, u8 x2, u8 y2, u8 dot)
{
    u8 x, y;

    for(x = x1; x <= x2; x++)
    {
        for(y = y1; y <= y2; y++)OLED_DrawPoint(x, y, dot);
    }
}

//在指定位置显示一个字符,包括部分字符
//x:0~127
//y:0~63
//mode:0,反白显示;1,正常显示
//size:选择字体 12/16/24
void OLED_ShowChar(u8 x, u8 y, u8 chr, u8 size, u8 mode)
{
    u8 temp, t, t1;
    u8 y0 = y;
    u8 csize = (size / 8 + ((size % 8) ? 1 : 0)) * (size / 2);		//得到字体一个字符对应点阵集所占的字节数
    chr = chr - ' '; //得到偏移后的值

    for(t = 0; t < csize; t++)
    {
        if(size == 12)temp = asc2_1206[chr][t]; 	 	//调用1206字体
        else if(size == 16)temp = asc2_1608[chr][t];	//调用1608字体
        else if(size == 24)temp = asc2_2412[chr][t];	//调用2412字体
        else return;								//没有的字库

        for(t1 = 0; t1 < 8; t1++)
        {
            if(temp & 0x80)OLED_DrawPoint(x, y, mode);
            else OLED_DrawPoint(x, y, !mode);

            temp <<= 1;
            y++;

            if((y - y0) == size)
            {
                y = y0;
                x++;
                break;
            }
        }
    }
}
//m^n函数
u32 mypow(u8 m, u8 n)
{
    u32 result = 1;

    while(n--)result *= m;

    return result;
}
//显示2个数字
//x,y :起点坐标
//len :数字的位数
//size:字体大小12/16/24
//mode:模式	0,反白模式;1,正常模式
//num:数值(0~4294967295);
void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len, u8 size, u8 mode)
{
    u8 t, temp;
    u8 enshow = 0;

    for(t = 0; t < len; t++)
    {
        temp = (num / mypow(10, len - t - 1)) % 10;

        if(enshow == 0 && t < (len - 1))
        {
            if(temp == 0)
            {
                OLED_ShowChar(x + (size / 2)*t, y, ' ', size, mode);
                continue;
            }
            else enshow = 1;

        }

        OLED_ShowChar(x + (size / 2)*t, y, temp + '0', size, mode);
    }
}
//显示字符串
//x,y:起点坐标
//size:字体大小12/16/24
//*p:字符串起始地址
//mode:0,反白显示;1,正常显示
void OLED_ShowString(u8 x, u8 y, u8 *p, u8 size, u8 mode)
{
    while((*p <= '~') && (*p >= ' ')) //判断是不是非法字符!
    {
        if(x > (128 - (size / 2)))
        {
            x = 0;
            y += size;
        }

        if(y > (64 - size))
        {
            y = x = 0;
            OLED_Clear();
        }

        OLED_ShowChar(x, y, *p, size, mode);
        x += size / 2;
        p++;
    }
}


//在指定位置，显示一个size*size大小的汉字
//x:0~127
//y:0~63
//index:汉字编号（在字库数组里面的编号）
//size:选择字体 12/16/24
//mode:0,反白显示;1,正常显示

void OLED_ShowChinese(u16 x, u16 y, u8 index, u8 size, u8 mode)
{
    u8 temp, t, t1;
    u16 y0 = y;
    u8 *dzk;
    u8 csize = (size / 8 + ((size % 8) ? 1 : 0)) * size; //一个24*24的汉字72字节

    if(size == 12) dzk = (u8*)Chinese_1212[index];      //得到汉字编号对应的点阵库
    else if(size == 16)	dzk = (u8*)Chinese_1616[index]; //得到汉字编号对应的点阵库
    else dzk = (u8*)Chinese_2424[index]; //得到汉字编号对应的点阵库

    for(t = 0; t < csize; t++)
    {
        temp = dzk[t];                              //得到点阵数据

        for(t1 = 0; t1 < 8; t1++)                  //按照从高位到低位的顺序画点
        {
            if(temp & 0x80)OLED_DrawPoint(x, y, mode);
            else OLED_DrawPoint(x, y, !mode);

            temp <<= 1;
            y++;

            if((y - y0) == size)                    //对y坐标的处理，当y坐标距起始坐标差24个像素点，x坐标加1
            {
                y = y0;
                x++;
                break;
            }
        }
    }
}

//在指定位置，显示一个16*16大小的汉字
//x:0~127
//y:0~63
//hzIndex[]:汉字编号（在字库数组里面的编号）
//len;汉字个数
//mode:0,反白显示;1,正常显示
void OLED_ShowChineseWords(u16 x, u16 y, u8 hzIndex[], u8 len, u8 mode)
{
    u8 n;

    for(n = 0; n < len; n++)
    {
        OLED_ShowChinese(x + n * 16, y, hzIndex[n], 16, mode);
    }
}

//在指定位置，以画点的方式显示图片
//x,y:起始点坐标x的范围0~127，y为页的范围0~64
//BMP：图片数组
//length：图片的像素长0-128
//height：图片的像素高0-64
//mode:0,反白显示;1,正常显示
void OLED_DrawPointBMP(u8 x, u8 y, unsigned char BMP[], u8 length, u8 height, u8 mode)
{
    u16 temp, t, t1;
    u8 y0 = y;

    for(t = 0; t < length * height / 8; t++)
    {
        temp = BMP[t];

        for(t1 = 0; t1 < 8; t1++)
        {
            if(temp & 0x80)OLED_DrawPoint(x, y, mode);
            else OLED_DrawPoint(x, y, !mode);

            temp <<= 1;
            y++;

            if((y - y0) == height)
            {
                y = y0;
                x++;
                break;
            }
        }
    }
}

/***********功能描述：显示显示BMP图片128×64起始点坐标(x,y),x的范围0～127，y为页的范围0～7*****************/
void OLED_DrawBMP(unsigned char x0, unsigned char y0, unsigned char x1, unsigned char y1, unsigned char BMP[])
{
    unsigned int j = 0;
    unsigned char x, y;

    if(y1 % 8 == 0) y = y1 / 8;
    else y = y1 / 8 + 1;

    for(y = y0; y < y1; y++)
    {
        OLED_Set_Pos(x0, y);

        for(x = x0; x < x1; x++)
        {
            OLED_WR_Byte(BMP[j++], OLED_DATA);
        }
    }
}

//初始化SSD1306
void OLED_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);	 //使能B端口和AFIO复用功能模块时钟

    GPIO_PinRemapConfig(GPIO_Remap_I2C1, ENABLE);//IIC1重映射 -> PB8,9

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//速度50MHz
    GPIO_Init(GPIOB, &GPIO_InitStructure);	  //初始化GPIOB8,9
    GPIO_SetBits(GPIOB, GPIO_Pin_8 | GPIO_Pin_9);


    delay_ms(10);
    OLED_WR_Byte(0xAE, OLED_CMD); //--显示关闭
    OLED_WR_Byte(0x00, OLED_CMD); //---设置最小列地址
    OLED_WR_Byte(0x10, OLED_CMD); //---设置最大列地址
    OLED_WR_Byte(0x40, OLED_CMD); //--set start line address
    OLED_WR_Byte(0xB0, OLED_CMD); //--set page address
    OLED_WR_Byte(0x81, OLED_CMD); // contract control
    OLED_WR_Byte(0xFF, OLED_CMD); //--128
    OLED_WR_Byte(0xA1, OLED_CMD); //set segment remap
    OLED_WR_Byte(0xA6, OLED_CMD); //--normal / reverse
    OLED_WR_Byte(0xA8, OLED_CMD); //--set multiplex ratio(1 to 64)
    OLED_WR_Byte(0x3F, OLED_CMD); //--1/32 duty
    OLED_WR_Byte(0xC0, OLED_CMD); //Com扫描方向，若显示的是镜对称，改为C8
    OLED_WR_Byte(0xD3, OLED_CMD); //-set display offset
    OLED_WR_Byte(0x00, OLED_CMD); //

    OLED_WR_Byte(0xD5, OLED_CMD); //set osc division
    OLED_WR_Byte(0x80, OLED_CMD); //

    OLED_WR_Byte(0xD8, OLED_CMD); //set area color mode off
    OLED_WR_Byte(0x05, OLED_CMD); //

    OLED_WR_Byte(0xD9, OLED_CMD); //Set Pre-Charge Period
    OLED_WR_Byte(0xF1, OLED_CMD); //

    OLED_WR_Byte(0xDA, OLED_CMD); //set com pin configuartion
    OLED_WR_Byte(0x12, OLED_CMD); //

    OLED_WR_Byte(0xDB, OLED_CMD); //set Vcomh
    OLED_WR_Byte(0x30, OLED_CMD); //

    OLED_WR_Byte(0x8D, OLED_CMD); //set charge pump enable
    OLED_WR_Byte(0x14, OLED_CMD); //

    OLED_WR_Byte(0xAF, OLED_CMD); //--turn on oled panel
}


