#include "nrf24l01.h" 

//uchar sta;    // 状态变量
uchar code TX_Addr[]={0x34,0x43,0x10,0x10,0x01};
//uchar TX_Addr[]="Rec01";
uchar code TX_Buffer[]={0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x00};
uchar RX_Buffer[RX_DATA_WITDH];


void _delay_us(uint x)
{
    uint i,j;
    for (j=0;j<x;j++)
        for (i=0;i<12;i++);
}
void _delay_ms(uint x)
{
    uint i,j;
    for (j=0;j<x;j++)
        for (i=0;i<120;i++);
}
/*nRF24L01初始化*/
void nRF24L01_Init(void)
{
    CE=0;//待机模式Ⅰ
    CSN=1;
    SCK=0;
    IRQ=1;
}
/*SPI时序函数*/
uchar SPI_RW(uchar byte)
{
    uchar i;
    for(i=0;i<8;i++)//一字节8位循环8次写入
    {
        if(byte&0x80)//如果数据最高位是1
            MOSI=1;//向NRF24L01写1
        else //否则写0
            MOSI=0;
        byte<<=1;//低一位移到最高位
        SCK=1;//SCK拉高，写入一位数据，同时读取一位数据
        if(MISO)
            byte|=0x01;
        SCK=0;//SCK拉低
    }
    return byte;//返回读取一字节 
}
/*SPI写寄存器一字节函数*/
/*reg:寄存器地址*/
/*value:一字节（值）*/
uchar SPI_W_Reg(uchar reg,uchar value)
{
    uchar status;//返回状态
    CSN=0;//SPI片选
    status=SPI_RW(reg);//写入寄存器地址，同时读取状态
    SPI_RW(value);//写入一字节
    CSN=1;//
    return status;//返回状态
}
/*SPI读一字节*/
uchar SPI_R_byte(uchar reg)
{
    uchar reg_value;
    CSN=0;//SPI片选
    SPI_RW(reg);//写入地址
    reg_value=SPI_RW(0);//读取寄存器的值
    CSN=1;
    return reg_value;//返回读取的值
}
/*SPI读取RXFIFO寄存器数据*/
/*reg:寄存器地址*/
/*Dat_Buffer:用来存读取的数据*/
/*DLen:数据长度*/
uchar SPI_R_DBuffer(uchar reg,uchar *Dat_Buffer,uchar Dlen)
{
    uchar status,i;
    CSN=0;//SPI片选
    status=SPI_RW(reg);//写入寄存器地址，同时状态
    for(i=0;i<Dlen;i++)
    {
        Dat_Buffer[i]=SPI_RW(0);//存储数据
    }
    CSN=1;
    return status;
}
/*SPI向TXFIFO寄存器写入数据*/
/*reg:写入寄存器地址*/
/*TX_Dat_Buffer:存放需要发送的数据*/
/*Dlen:数据长度*/  
uchar SPI_W_DBuffer(uchar reg,uchar *TX_Dat_Buffer,uchar Dlen)
{
    uchar status,i;
    CSN=0;//SPI片选，启动时序
    status=SPI_RW(reg);
    for(i=0;i<Dlen;i++)
    {
        SPI_RW(TX_Dat_Buffer[i]);//发送数据
    }
    CSN=1;
    return status;  
}
/*设置发送模式*/
void nRF24L01_Set_TX_Mode(uchar *TX_Data)
{
    CE=0;//待机（写寄存器之前一定要进入待机模式或掉电模式）
    SPI_W_DBuffer(W_REGISTER+TX_ADDR,TX_Addr,TX_ADDR_WITDH);/*写寄存器指令+接收节点地址+地址宽度*/
    SPI_W_DBuffer(W_REGISTER+RX_ADDR_P0,TX_Addr,TX_ADDR_WITDH);/*为了接收设备应答信号，接收通道0地址与发送地址相同*/
    SPI_W_DBuffer(W_TX_PLOAD,TX_Data,TX_DATA_WITDH);/*写有效数据地址+有效数据+有效数据宽度*/
    SPI_W_Reg(W_REGISTER+EN_AA,0x01);/*接收通道0自动应答*/
    SPI_W_Reg(W_REGISTER+EN_RX_ADDR,0x01);/*使能接收通道0*/
    SPI_W_Reg(W_REGISTER+SETUP_RETR,0x0a);/*自动重发延时250US+86US，重发10次*/
    SPI_W_Reg(W_REGISTER+RF_CH,0);/*(2400)MHZ射频通道*/
    SPI_W_Reg(W_REGISTER+RF_SETUP,0x07);/*1Mbps速率,发射功率:0DBM,低噪声放大器增益*/
    SPI_W_Reg(W_REGISTER+CONFIG,0x0e);/*发送模式,上电,16位CRC校验,CRC使能*/
    CE=1;//启动发射
    _delay_ms(5);/*CE高电平持续时间最少10US以上*/
}
 
uchar Check_Rec(void)
{
    uchar status;
    sta=SPI_R_byte(R_REGISTER+STATUS);
    if(RX_DR)
    {
        CE=0;
        SPI_R_DBuffer(R_RX_PLOAD,RX_Buffer,RX_DATA_WITDH);
        status=1;
    }
    SPI_W_Reg(W_REGISTER+STATUS,0xff);
    return status;  
}
/*检测应答信号*/
uchar Check_Ack(void)
{
    sta=SPI_R_byte(R_REGISTER+STATUS);/*读取寄存状态*/
    if(TX_DS||MAX_RT)/*如果TX_DS或MAX_RT为1,则清除中断和清除TX_FIFO寄存器的值*/
    {
        SPI_W_Reg(W_REGISTER+STATUS,0xff);
        CSN=0;
        SPI_RW(FLUSH_TX);/*如果没有这一句只能发一次数据，大家要注意*/
        CSN=1;
        return 0;
    }
    else
        return 1;
}
void nRF24L01_Set_RX_Mode()
{
    
	CE=0;
    SPI_W_DBuffer(W_REGISTER+TX_ADDR,TX_Addr,TX_ADDR_WITDH);
    SPI_W_DBuffer(W_REGISTER+RX_ADDR_P0,TX_Addr,TX_ADDR_WITDH);
    SPI_W_Reg(W_REGISTER+EN_AA,0x01);//auot ack
    SPI_W_Reg(W_REGISTER+EN_RX_ADDR,0x01);
    SPI_W_Reg(W_REGISTER+SETUP_RETR,0x0a);
    SPI_W_Reg(W_REGISTER+RX_PW_P0,RX_DATA_WITDH);
    SPI_W_Reg(W_REGISTER+RF_CH,0);
    SPI_W_Reg(W_REGISTER+RF_SETUP,0x07);//0db,lna
    SPI_W_Reg(W_REGISTER+CONFIG,0x0f);
     
    
	CE=1;
    _delay_ms(5);/*CE高电平持续时间最少10US以上*/
}
 
unsigned char nRF24L01_RX_Data()
{
//  unsigned char i,status;
    sta=SPI_R_byte(R_REGISTER+STATUS);
    if(RX_DR)
    {
        
		CE =0;
        SPI_R_DBuffer(R_RX_PLOAD,RX_Buffer,RX_DATA_WITDH);
        SPI_W_Reg(W_REGISTER+STATUS,0xff);
       
		CSN =0;
        SPI_RW(FLUSH_RX);
        CSN =1;
        return 1;
    }
    else
        return 0;
     
}
/* 检查接收数据*/ 
unsigned char CheckNrf2401()
{
	if(nRF24L01_RX_Data())
	{
		return 1;
	}
	else return 0;
}

