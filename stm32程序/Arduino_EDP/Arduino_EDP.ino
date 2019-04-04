 /*
   stm32 程序采用arduino编写，使用有3个串口的stm32 
*/
#include "edp.c"
#include<dht11.h>
dht11 DHT11;
#define KEY  "***********="    //APIkey 
#define ID   "*****"                          //设备ID
#define PUSH_ID NULL
// 串口
#define _baudrate   115200
#define CE  9
#define CSN 10
#define IRQ 4
#define WIFI_UART   Serial1
#define DBG_UART    Serial2   //调试打印串口
#define DHT11PIN A0 //温湿度传感器的pin
#define LED  A1
#define LED2        A2 //定义耗电设备1
#define LED3        A3 //定义耗电设备2
#define LED4        A4 //定义常开设备
edp_pkt *pkt;
/*
 nrf24l01 define 
 */
 /*
 * nrf24l01 content
 */

#define TX_ADDR_WITDH 5//发送地址宽度设置为5个字节
#define RX_ADDR_WITDH 5
#define TX_DATA_WITDH 8
#define RX_DATA_WITDH 8
/******************************************************************
// nRF24L01指令格式：
*******************************************************************/
#define R_REGISTER    0x00  // 读寄存器
#define W_REGISTER    0x20  // 写寄存器
#define R_RX_PLOAD    0x61  // 读RX FIFO有效数据，1-32字节，当读数据完成后，数据被清除，应用于接收模式
#define W_TX_PLOAD    0xA0  // 写TX FIFO有效数据，1-32字节，写操作从字节0开始，应用于发射模式
#define FLUSH_TX    0xE1  // 清除TX FIFO寄存器，应用于发射模式
#define FLUSH_RX    0xE2  // 清除RX FIFO寄存器，应用于接收模式
#define REUSE_TX_PL 0xE3  // 重新使用上一包有效数据，当CE为高过程中，数据包被不断的重新发射
#define NOP         0xFF  // 空操作，可以用来读状态寄存器
/******************************************************************
// nRF24L01寄存器地址
*******************************************************************/
#define CONFIG      0x00  // 配置寄存器
#define EN_AA       0x01  // “自动应答”功能寄存器
#define EN_RX_ADDR  0x02  // 接收通道使能寄存器
#define SETUP_AW    0x03  // 地址宽度设置寄存器
#define SETUP_RETR  0x04  // 自动重发设置寄存器
#define RF_CH       0x05  // 射频通道频率设置寄存器
#define RF_SETUP    0x06  // 射频设置寄存器
#define STATUS      0x07  // 状态寄存器
#define OBSERVE_TX  0x08  // 发送检测寄存器
#define CD          0x09  // 载波检测寄存器
#define RX_ADDR_P0  0x0A  // 数据通道0接收地址寄存器
#define RX_ADDR_P1  0x0B  // 数据通道1接收地址寄存器
#define RX_ADDR_P2  0x0C  // 数据通道2接收地址寄存器
#define RX_ADDR_P3  0x0D  // 数据通道3接收地址寄存器
#define RX_ADDR_P4  0x0E  // 数据通道4接收地址寄存器
#define RX_ADDR_P5  0x0F  // 数据通道5接收地址寄存器
#define TX_ADDR     0x10  // 发送地址寄存器
#define RX_PW_P0    0x11  // 数据通道0有效数据宽度设置寄存器
#define RX_PW_P1    0x12  // 数据通道1有效数据宽度设置寄存器
#define RX_PW_P2    0x13  // 数据通道2有效数据宽度设置寄存器
#define RX_PW_P3    0x14  // 数据通道3有效数据宽度设置寄存器
#define RX_PW_P4    0x15  // 数据通道4有效数据宽度设置寄存器
#define RX_PW_P5    0x16  // 数据通道5有效数据宽度设置寄存器
#define FIFO_STATUS 0x17  // FIFO状态寄存器
unsigned char  sta;    // 状态变量
#define RX_DR  (sta & 0x40)  // 接收成功中断标志
#define TX_DS  (sta & 0x20)  // 发射成功中断标志
#define MAX_RT (sta & 0x10)  // 重发溢出中断标志


unsigned char TX_Addr[]={0x34,0x43,0x10,0x10,0x01};
unsigned char TX_Buffer[]={0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};
unsigned char RX_Buffer[RX_DATA_WITDH];
unsigned  int waring;

void nRF24L01_Init(void)
{
    delay(2);
    digitalWrite(CE,LOW);
    digitalWrite(CSN,HIGH);
    digitalWrite(SCK,LOW);
//    digitalWrite(MOSI,LOW);
//    digitalWrite(MISO,LOW);
    digitalWrite(IRQ,HIGH);
}
 
unsigned char SPI_RW(unsigned char byte)
{
    unsigned char i;
    for(i=0;i<8;i++)
    {
        if(byte&0x80)
            digitalWrite(MOSI,HIGH);
        else
            digitalWrite(MOSI,LOW);
        byte<<=1;
        digitalWrite(SCK,HIGH);
        if(digitalRead(MISO))
     //     Serial.println(10);
            byte|=0x01;
        digitalWrite(SCK,LOW);
    }
    return byte;
}
 
unsigned char SPI_W_Reg(unsigned char reg,unsigned char value)
{
    unsigned char status;
    digitalWrite(CSN,LOW);
    status=SPI_RW(reg);
    SPI_RW(value);
    digitalWrite(CSN,HIGH);
    return status;
}
 
unsigned char SPI_R_byte(unsigned char reg)
{
    unsigned char status;
    digitalWrite(CSN,LOW);
    SPI_RW(reg);
    status=SPI_RW(0);
    digitalWrite(CSN,HIGH);
    return status;
}
 
unsigned char SPI_R_DBuffer(unsigned char reg,unsigned char *Dat_Buffer,unsigned char Dlen)
{
    unsigned char reg_value,i;
    digitalWrite(CSN,LOW);
    reg_value=SPI_RW(reg);
    for(i=0;i<Dlen;i++)
    {
        Dat_Buffer[i]=SPI_RW(0);
    }
    digitalWrite(CSN,HIGH);
    return reg_value;
}
 
unsigned char SPI_W_DBuffer(unsigned char reg,unsigned char *TX_Dat_Buffer,unsigned char Dlen)
{
    unsigned char reg_value,i;
    digitalWrite(CSN,LOW);
    reg_value=SPI_RW(reg);
    for(i=0;i<Dlen;i++)
    {
        SPI_RW(TX_Dat_Buffer[i]);
    }
    digitalWrite(CSN,HIGH);
    return reg_value;   
}
 
void nRF24L01_Set_RX_Mode()
{
    digitalWrite(CE,LOW);//待机
    SPI_W_DBuffer(W_REGISTER+TX_ADDR,TX_Addr,TX_ADDR_WITDH);
    SPI_W_DBuffer(W_REGISTER+RX_ADDR_P0,TX_Addr,TX_ADDR_WITDH);
    SPI_W_Reg(W_REGISTER+EN_AA,0x01);//auot ack
    SPI_W_Reg(W_REGISTER+EN_RX_ADDR,0x01);
    SPI_W_Reg(W_REGISTER+SETUP_RETR,0x0a);
    SPI_W_Reg(W_REGISTER+RX_PW_P0,RX_DATA_WITDH);
    SPI_W_Reg(W_REGISTER+RF_CH,0);
    SPI_W_Reg(W_REGISTER+RF_SETUP,0x07);//0db,lna
    SPI_W_Reg(W_REGISTER+CONFIG,0x0f);
     
    digitalWrite(CE,HIGH);
    delay(5);
}
 
unsigned char nRF24L01_RX_Data()
{
//  unsigned char i,status;
    sta=SPI_R_byte(R_REGISTER+STATUS);
    if(RX_DR)
    {
        digitalWrite(CE,LOW);
        SPI_R_DBuffer(R_RX_PLOAD,RX_Buffer,RX_DATA_WITDH);
        SPI_W_Reg(W_REGISTER+STATUS,0xff);
        digitalWrite(CSN,LOW);
        SPI_RW(FLUSH_RX);
        digitalWrite(CSN,HIGH);
        return 1;
    }
    else
        return 0;
     
}
void nRF24L01_Set_TX_Mode(unsigned char *TX_Data)
{
   digitalWrite(CE,LOW);//待机
    SPI_W_DBuffer(W_REGISTER+TX_ADDR,TX_Addr,TX_ADDR_WITDH);
    SPI_W_DBuffer(W_REGISTER+RX_ADDR_P0,TX_Addr,TX_ADDR_WITDH);
    SPI_W_DBuffer(W_TX_PLOAD,TX_Data,TX_DATA_WITDH);
    SPI_W_Reg(W_REGISTER+EN_AA,0x01);
    SPI_W_Reg(W_REGISTER+EN_RX_ADDR,0x01);
    SPI_W_Reg(W_REGISTER+SETUP_RETR,0x0a);
    SPI_W_Reg(W_REGISTER+RF_CH,0);
    SPI_W_Reg(W_REGISTER+RF_SETUP,0x07);
    SPI_W_Reg(W_REGISTER+CONFIG,0x0e);
    digitalWrite(CE,HIGH);
    delay(5);
}
unsigned char Check_Ack(void)
{
    sta=SPI_R_byte(R_REGISTER+STATUS);
    if(TX_DS||MAX_RT)
    {
        SPI_W_Reg(W_REGISTER+STATUS,0xff);
        digitalWrite(CSN,LOW);
        SPI_RW(FLUSH_TX);
        digitalWrite(CSN,HIGH);
        return 0;
    }
    else
        return 1;
} 
unsigned char Check_Rec(void)
{
    unsigned char status;
    sta=SPI_R_byte(R_REGISTER+STATUS);
    if(RX_DR)
    {
        digitalWrite(CE,LOW);;
        SPI_R_DBuffer(R_RX_PLOAD,RX_Buffer,RX_DATA_WITDH);
        status=1;
    }
    SPI_W_Reg(W_REGISTER+STATUS,0xff);
    return status;  
}
/*
* doCmdOk
* 发送命令至模块，从回复中获取期待的关键字
* keyword: 所期待的关键字
* 成功找到关键字返回true，否则返回false
*/
bool doCmdOk(String data, char *keyword)
{
  bool result = false;
  if (data != "")   //对于tcp连接命令，直接等待第二次回复
  {
    WIFI_UART.println(data);  //发送AT指令
    DBG_UART.print("SEND: ");
    DBG_UART.println(data);
  }
  if (data == "AT")   //检查模块存在
    delay(2000);
  else
    while (!WIFI_UART.available());  // 等待模块回复

  delay(200);
  if (WIFI_UART.find(keyword))   //返回值判断
  {
    DBG_UART.println("do cmd OK");
    result = true;
  }
  else
  {
    DBG_UART.println("do cmd ERROR");
    result = false;
  }
  while (WIFI_UART.available()) WIFI_UART.read();   //清空串口接收缓存
  delay(500); //指令时间间隔
  return result;
}
/*
 * 检测nrf24l01传过来的数据，如果有值，返回true 如果没有值返回 flase 
 */
bool checknrf24l01( )
{
    bool result = false;
    if(nRF24L01_RX_Data())
  {
        result = true;
  }
  else
     result = false;
   return result;
 }
void setup()
{
  char buf[100] = {0};
  int tmp;
 pinMode(A1, OUTPUT);    //LED1
  pinMode(A2, OUTPUT);    //LED2
  pinMode(A3, OUTPUT);    //LED3
  pinMode(A4, OUTPUT);    //LED4
  digitalWrite(A1, LOW);
  digitalWrite(A2, LOW);
  digitalWrite(A3, LOW);
  digitalWrite(A4, LOW);
  pinMode(CE,OUTPUT);
  pinMode(CSN,OUTPUT);
  pinMode(SCK,OUTPUT);
  pinMode(MOSI,OUTPUT);
  pinMode(MISO,INPUT);
 
  WIFI_UART.begin( _baudrate );
  DBG_UART.begin( _baudrate );
  WIFI_UART.setTimeout(3000);    //设置find超时时间
  delay(3000);
  DBG_UART.println("hello world!");

  delay(2000);
  
  while (!doCmdOk("AT", "OK"));
  

  while (!doCmdOk("AT+CWMODE=3", "OK"));            //工作模式
  while (!doCmdOk("AT+CWJAP=\"123\",\"1234567890\"", "OK"));
  while (!doCmdOk("AT+CIPSTART=\"TCP\",\"183.230.40.39\",876", "CONNECT"));
  while (!doCmdOk("AT+CIPMODE=1", "OK"));           //透传模式
  while (!doCmdOk("AT+CIPSEND", ">"));              //开始发送
  
}

void loop()
{
  unsigned int t_while,j=100;

  static int edp_connect = 0;
  bool trigger = false;
  edp_pkt rcv_pkt;
  unsigned char pkt_type;
  int i, tmp;
  char num[10];
  int wd;  //温度
  int sd;  //湿度
  int rsd;
  int dsl8b20;
  char wd1[10]; //字符串格式温度
  char sd1[10]; //字符串格式湿度
  char rsd1[1];
  char dsl8b201[10];
  int chk = DHT11.read(DHT11PIN);//读DHT11

  /* EDP 连接 */
  if (!edp_connect)
  {
    while (WIFI_UART.available()) WIFI_UART.read(); //清空串口接收缓存
    packetSend(packetConnect(ID, KEY));             //发送EPD连接包
    while (!WIFI_UART.available());                 //等待EDP连接应答
    if ((tmp = WIFI_UART.readBytes(rcv_pkt.data, sizeof(rcv_pkt.data))) > 0 )
    {
      rcvDebug(rcv_pkt.data, tmp);
      if (rcv_pkt.data[0] == 0x20 && rcv_pkt.data[2] == 0x00 && rcv_pkt.data[3] == 0x00)
      {
        edp_connect = 1;
        DBG_UART.println("EDP connected.");
      }
      else
        DBG_UART.println("EDP connect error.");
    }
    packetClear(&rcv_pkt);
  }
  wd = (float)DHT11.temperature;//获取温度
  sd = (float)DHT11.humidity;//获取湿度
  trigger = wd;  //传感器是否工作
  nRF24L01_Init();
  nRF24L01_Set_RX_Mode();
  delay(100);
  while(!checknrf24l01());
  
      DBG_UART.println((int)RX_Buffer[0]*100+(int)RX_Buffer[1]*10+(int)RX_Buffer[2]+0.1*(int)RX_Buffer[3]+0.01*(int)RX_Buffer[4]);
      dsl8b20 =(int)RX_Buffer[0]*100+(int)RX_Buffer[1]*10+(int)RX_Buffer[2]+0.1*(int)RX_Buffer[3]+0.01*(int)RX_Buffer[4];
      if ((int)RX_Buffer[5]==1)
      DBG_UART.println("some one in here");
      else
      DBG_UART.println("no people!");
      rsd =(int)RX_Buffer[5]==1;

  
  if (edp_connect && trigger)
  {
    DBG_UART.print("temperature: ");
    DBG_UART.println((float)DHT11.temperature, 2);
    DBG_UART.print("humidity: ");
    DBG_UART.println((float)DHT11.humidity, 2);
  
    sprintf(wd1,"%d",wd);  //int型转换char型
    sprintf(sd1,"%d",sd);  //int型转换char型
    sprintf(rsd1,"%d",rsd);  //int型转换char型
    sprintf(dsl8b201,"%d",dsl8b20);  //int型转换char型
     nRF24L01_Init();
     delay(100);
    TX_Buffer[0]=wd/10;
    TX_Buffer[1]=wd%10;
    TX_Buffer[2]=sd/10;
    TX_Buffer[3]=sd%10;
    TX_Buffer[4]=waring;
     while(j--)
     {            
               
         nRF24L01_Set_TX_Mode(TX_Buffer);
         while(Check_Ack());
        // DBG_UART.print(int(TX_Buffer[4]));
            
     }
      

    packetSend(packetDataSaveTrans(PUSH_ID, "WD", wd1));  //发送数据存储包   当PUSH_ID不为NULL时转发至PUSH_ID    
    delay(100);
    packetSend(packetDataSaveTrans(NULL, "SD", sd1)); //发送的数据必须为字符串 "" 
    delay(100);   
     packetSend(packetDataSaveTrans(NULL, "rsd", rsd1)); //发送的数据必须为字符串 "" 
    delay(100); 
     packetSend(packetDataSaveTrans(NULL, "dsl8b20", dsl8b201)); //发送的数据必须为字符串 "" 
    delay(100);           
  }
  while (WIFI_UART.available())
  {
    readEdpPkt(&rcv_pkt);
    if (isEdpPkt(&rcv_pkt))
    {
      pkt_type = rcv_pkt.data[0];
      switch (pkt_type)
      {
        case CMDREQ:
          char edp_command[50];
          char edp_cmd_id[40];
          long id_len, cmd_len, rm_len;
          char datastr[20];
          char val[10];
          memset(edp_command, 0, sizeof(edp_command));
          memset(edp_cmd_id, 0, sizeof(edp_cmd_id));
          edpCommandReqParse(&rcv_pkt, edp_cmd_id, edp_command, &rm_len, &id_len, &cmd_len);
          DBG_UART.print("rm_len: ");
          DBG_UART.println(rm_len, DEC);
          delay(10);
          DBG_UART.print("id_len: ");
          DBG_UART.println(id_len, DEC);
          delay(10);
          DBG_UART.print("cmd_len: ");
          DBG_UART.println(cmd_len, DEC);
          delay(10);
          DBG_UART.print("id: ");
          DBG_UART.println(edp_cmd_id);
          delay(10);
          DBG_UART.print("cmd: ");
          DBG_UART.println(edp_command);
 
          //数据处理与应用中EDP命令内容对应
          //本例中格式为  datastream:[1/0] 
        sscanf(edp_command, "%[^=]=%s", datastr, val);//datastr为数据流名
          if (atoi(val) == 1)
          { 
            switch((int)datastr[3]){
              case 49:
                digitalWrite(LED, HIGH);   // 使Led1亮
                DBG_UART.println(datastr[3]);
                DBG_UART.println("high");
                waring =1;
                delay(10);
                break;
              case 50:
                digitalWrite(LED2, HIGH);   // 使Led2亮
                break;
              case 51:
                digitalWrite(LED3, HIGH);   // 使Led3亮
                break;
              case 52:
                digitalWrite(LED4, HIGH);   // 使Led4亮
                break;
              default:
                DBG_UART.println(datastr[3]);
                DBG_UART.println((int)datastr[3]);
                break;          
            } 
          }
          else
          { 
            switch((int)datastr[3]){
              case 49:
                digitalWrite(LED, LOW);   // 使Led1灭
                waring =0;
                break;
              case 50:
                digitalWrite(LED2, LOW);   // 使Led2灭
                break;
              case 51:
                digitalWrite(LED3, LOW);   // 使Led3灭
                break;
              case 52:
                digitalWrite(LED4, LOW);   // 使Led4灭
                break;
              default:
                DBG_UART.print("datastr[3]: ");
                DBG_UART.println(datastr[3]);
                DBG_UART.print("datastr[3]toint: ");
                DBG_UART.println((int)datastr[3]);
                break;
            }        
          }
          packetSend(packetDataSaveTrans(NULL, datastr,val)); //将新数据值上传至数据流
          break;
        default:
          DBG_UART.print("unknown type: ");
          DBG_UART.println(pkt_type, HEX);
          break;
      }
    }
    //delay(4);
  }
  if (rcv_pkt.len > 0)
    packetClear(&rcv_pkt);
  delay(150); 
  //packetSend(packetDataSaveTrans(NULL, "text", 5)); //将新数据值上传至数据流
}

/*
* readEdpPkt
* 从串口缓存中读数据到接收缓存
*/
bool readEdpPkt(edp_pkt *p)
{
  int tmp;
  if ((tmp = WIFI_UART.readBytes(p->data + p->len, sizeof(p->data))) > 0 )
  {
    rcvDebug(p->data + p->len, tmp);
    p->len += tmp;
  }
  return true;
}

/*
* packetSend
* 将待发数据发送至串口，并释放到动态分配的内存
*/
void packetSend(edp_pkt* pkt)
{
  if (pkt != NULL)
  {
    WIFI_UART.write(pkt->data, pkt->len);    //串口发送
    WIFI_UART.flush();
    free(pkt);              //回收内存
  }
}

void rcvDebug(unsigned char *rcv, int len)
{
  int i;

  DBG_UART.print("rcv len: ");
  DBG_UART.println(len, DEC);
  for (i = 0; i < len; i++)
  {
    DBG_UART.print(rcv[i], HEX);
    DBG_UART.print(" ");
  }
  DBG_UART.println("");
}
