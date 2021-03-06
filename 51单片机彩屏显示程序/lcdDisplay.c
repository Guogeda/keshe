#include "lcdDisplay.h"

/******
最初版本的彩屏显示函数
温度：+25.44
无人  0
sending.....
温度：25
湿度：84
******/
void LcdDisplay(int temp) 	 //lcd显示
{
	uchar i,j=100;
	unsigned char datas[]={0,0,0,0,0,0}; //定义数组
	unsigned char xdata codes[][10] = {"0","1","2","3","4","5","6","7","8","9"};
	float tp; 
	GUI_Write32CnChar(125, 20, "温度", RED, WHITE);
	GUI_WriteASCII(125, 80, ":", RED, WHITE);

	 
	if(temp< 0)				//当温度值为负数
  	{
	  	
		GUI_WriteASCII(125, 90, "-", RED, WHITE);
		//因为读取的温度是实际温度的补码，所以减1，再取反求出原码
		temp=temp-1;
		temp=~temp;
		tp=temp;
		temp=tp*0.0625*100+0.5;	
		//留两个小数点就*100，+0.5是四舍五入，因为C语言浮点数转换为整型的时候把小数点
		//后面的数自动去掉，不管是否大于0.5，而+0.5之后大于0.5的就是进1了，小于0.5的就
		//算由�0.5，还是在小数点后面。
 
  	}
 	else
  	{			
	  	GUI_WriteASCII(125, 90, "+", RED, WHITE);
		tp=temp;//因为数据处理有小数点所以将温度赋给一个浮点型变量
		//如果温度是正的那么，那么正数的原码就是补码它本身
		temp=tp*0.0625*100+0.5;	
		//留两个小数点就*100，+0.5是四舍五入，因为C语言浮点数转换为整型的时候把小数点
		//后面的数自动去掉，不管是否大于0.5，而+0.5之后大于0.5的就是进1了，小于0.5的就
		//算加上0.5，还是在小数点后面。
	}
	datas[0] = temp / 10000;
	datas[1] = temp % 10000 / 1000;
	datas[2] = temp % 1000 / 100;
	datas[3] = temp % 100 / 10;
	datas[4] = temp % 10;
 	for (i=0;i<3;i++)
	{
		 GUI_WriteASCII(125, 105+i*15,&codes[datas[i]], RED, WHITE);											  +
		 GUI_WriteASCII(95, 95,"sending...", RED, WHITE);
	}
		 GUI_WriteASCII(125, 150,".", RED, WHITE);
	for (i=3;i<5;i++)
	{
		GUI_WriteASCII(125, 120+i*15,&codes[datas[i]], RED, WHITE);
	}
	datas[5] = check_rsd();
	nRF24L01_Set_TX_Mode(&datas);//发送数据	 成
	while(Check_Ack());//等待发送完成

	GUI_WriteASCII(95, 95,"sendover", RED, WHITE);
	SPI_W_Reg(W_REGISTER+STATUS,0xff);//清零状态寄存器？
	if(check_rsd()==1)
		GUI_Write32CnChar(95, 20, "有人", RED, WHITE);
	else
	    GUI_Write32CnChar(95, 20, "无人", RED, WHITE);
	nRF24L01_Init();
	nRF24L01_Set_RX_Mode();
  
   	Delay1ms(1000);
	while(j--)
	{
	
		if (nRF24L01_RX_Data())
		{
			j=1000;
			for (i=0;i<2;i++)
			{
				GUI_Write32CnChar(35, 20, "温度",WHITE, RED);
				GUI_WriteASCII(35, 95+i*15,&codes[RX_Buffer[i]], WHITE,RED);
			}
			for (i=2;i<4;i++)
			{
				GUI_Write32CnChar(5, 20, "湿度", WHITE,RED );
				GUI_WriteASCII(5, 65+i*15,&codes[RX_Buffer[i]], WHITE,RED );
			}
			if(RX_Buffer[4]==1)
			{									  
				GUI_WriteASCII(70, 20,"warning",  WHITE,RED);
				beep_alarm();
			}
			else
			{
				GUI_WriteASCII(70, 20,"safeing", WHITE,RED);
				beep_safe();
			}
		}
		Delay1ms(10);
		break;	
	}
}							