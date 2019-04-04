#include "lcd_driver.h"

/****************************************************************************
*函数名：TFT_WriteCmd
*输  入：cmd
*输  出：
*功  能：写命令。
****************************************************************************/	  

void TFT_WriteCmd(uint cmd)
{
	uchar cmdH, cmdL;
	cmdH = cmd >> 8;
	cmdL = cmd & 0x00FF;

	TFT_WR = 1;	 		  //初始化WR
	TFT_CS = 0;			  //打开片选
	
	TFT_RD = 1;           //选择写
	TFT_RS = 0;			  //选择命令
		
	TFT_DATAPORTH = cmdH; //放置命令数据
	TFT_DATAPORTL = cmdL; //放置命令数据

	TFT_WR = 0;			  //写入时序
	TFT_WR = 1;
	
	TFT_CS = 1;			  //关闭片选
}

/****************************************************************************
*函数名：TFT_WriteCmdData
*输  入：cmd, dat
*输  出：
*功  能：写命令， 写数据。
****************************************************************************/	  

void TFT_WriteData(uint dat)
{
    uchar datH, datL;
	datH = dat >> 8;
	datL = dat & 0x00FF;

	TFT_WR = 1;	 		  //初始化WR
	TFT_CS = 0;			  //打开片选
	
	TFT_RD = 1;           //选择写
	TFT_RS = 1;			  //选择发送数据
		
	TFT_DATAPORTH = datH;  //放置命令数据
	TFT_DATAPORTL = datL;  //放置命令数据

	TFT_WR = 0;			  //写入时序
	TFT_WR = 1;
	
	TFT_CS = 1;			  //关闭片选
}

/****************************************************************************
*函数名：TFT_Init
*输  入：
*输  出：
*功  能：初始化TFT。
****************************************************************************/	  

void TFT_Init(void)
{
	uint i;

	TFT_RST = 1;
	for(i=500; i>0; i--);
	TFT_RST = 0;
	for(i=500; i>0; i--);
	TFT_RST = 1;
	for(i=5000; i>0; i--);
	
	 TFT_CS = 0;
  	 TFT_WriteCmd(0x0014);TFT_WriteData(0x0030);
	 for(i=500; i>0; i--); //							 
	 TFT_WriteCmd(0x000F);TFT_WriteData(0x0008); 
	 for(i=500; i>0; i--);// 							
	 TFT_WriteCmd(0x0011);TFT_WriteData(0x0231);
	 for(i=500; i>0; i--); // 
	 TFT_WriteCmd(0x0012);TFT_WriteData(0x0009); 
	 for(i=500; i>0; i--);// 
	 TFT_WriteCmd(0x0013);TFT_WriteData(0x1448); 
	 for(i=500; i>0; i--);// 
	 TFT_WriteCmd(0x0010);TFT_WriteData(0x5268);  
	 for(i=500; i>0; i--);//	
	 TFT_WriteCmd(0x0012);TFT_WriteData(0x0019);  
	 for(i=500; i>0; i--);	
	 TFT_WriteCmd(0x0010);TFT_WriteData(0x5260);  
	 for(i=500; i>0; i--);//
	 TFT_WriteCmd(0x0013);TFT_WriteData(0x2d48);  
	 for(i=500; i>0; i--);	
	 TFT_WriteCmd(0x0030);TFT_WriteData(0x0000);
	 for(i=500; i>0; i--);// 
	 TFT_WriteCmd(0x0031);TFT_WriteData(0x0501); 
	 for(i=500; i>0; i--);// 
	 TFT_WriteCmd(0x0032);TFT_WriteData(0x0207); 
	 for(i=500; i>0; i--);// 
	 TFT_WriteCmd(0x0033);TFT_WriteData(0x0502);
	 for(i=500; i>0; i--);//  
	 TFT_WriteCmd(0x0034);TFT_WriteData(0x0007); 
	 for(i=500; i>0; i--);// 
	 TFT_WriteCmd(0x0035);TFT_WriteData(0x0601); 
	 for(i=500; i>0; i--);// 
	 TFT_WriteCmd(0x0036);TFT_WriteData(0x0707); 
	 for(i=500; i>0; i--);// 
	 TFT_WriteCmd(0x0037);TFT_WriteData(0x0305);
	 for(i=500; i>0; i--);//  
	 TFT_WriteCmd(0x0038);TFT_WriteData(0x040e); 
	 for(i=500; i>0; i--);// 
	 TFT_WriteCmd(0x0039);TFT_WriteData(0x040e); 
	 for(i=500; i>0; i--);//

	 TFT_WriteCmd(0x003a);TFT_WriteData(0x0101);
	 for(i=500; i>0; i--);//  
	 TFT_WriteCmd(0x003b);TFT_WriteData(0x0101);  
	 for(i=500; i>0; i--);//
	 TFT_WriteCmd(0x003c);TFT_WriteData(0x0101); 
	 for(i=500; i>0; i--);// 
	 TFT_WriteCmd(0x003d);TFT_WriteData(0x0101); 
	 for(i=500; i>0; i--);// 
	 TFT_WriteCmd(0x003e);TFT_WriteData(0x0001);  
	 TFT_WriteCmd(0x003f);TFT_WriteData(0x0001);
	 for(i=500; i>0; i--);//
 
	 TFT_WriteCmd(0x0001);TFT_WriteData(0x011B); 
	 for(i=500; i>0; i--);// 
	 TFT_WriteCmd(0x0002);TFT_WriteData(0x0700); 
	 for(i=500; i>0; i--);// 
	 TFT_WriteCmd(0x0003);TFT_WriteData(0x1028);
	 for(i=500; i>0; i--);//  
	 TFT_WriteCmd(0x0008);TFT_WriteData(0x1010); 
	 for(i=500; i>0; i--);// 
	 TFT_WriteCmd(0x000A);TFT_WriteData(0x4420); 
	 for(i=500; i>0; i--);// 
	 TFT_WriteCmd(0x000B);TFT_WriteData(0x5030); 
	 for(i=500; i>0; i--);// 
	 TFT_WriteCmd(0x0007);TFT_WriteData(0x0005); 
	 for(i=500; i>0; i--);// 
	 TFT_WriteCmd(0x0007);TFT_WriteData(0x0025);  
	 for(i=500; i>0; i--);//
	 TFT_WriteCmd(0x0007);TFT_WriteData(0x0027);  
	 for(i=500; i>0; i--);//
	 TFT_WriteCmd(0x0007);TFT_WriteData(0x0037); 
	 for(i=500; i>0; i--);//

	 TFT_WriteCmd(0x44);TFT_WriteData(0xaf00);
	
	 TFT_WriteCmd(0x45);TFT_WriteData(0xdb00);

	
	 TFT_WriteCmd(0x20);TFT_WriteData(0x0000);
	 
	 TFT_WriteCmd(0x21);TFT_WriteData(0x0000); 
	
	 TFT_WriteCmd(0x22);
}

/****************************************************************************
*函数名：TFT_SetWindow
*输  入：xStart, yStart, xEnd, yEnd
*输  出：
*功  能：设置要操作的窗口。
****************************************************************************/	  

void TFT_SetWindow(uint xStart, uint yStart, uint xEnd, uint yEnd)
{
	 uint x,y,xy;

	 x=(xEnd <<8)|xStart;
	 y=(yEnd <<8)|yStart;
	 xy = (yStart<<8)|xStart;

	 TFT_WriteCmd(0x44);TFT_WriteData(x);
	 TFT_WriteCmd(0x45);TFT_WriteData(y);
	 TFT_WriteCmd(0x21);TFT_WriteData(xy);

	 TFT_WriteCmd(0x22); 
}


/****************************************************************************
*函数名：GUI_Clearcreen
*输  入：backColor
*输  出：
*功  能：清屏并涂上颜色。
****************************************************************************/
	  
void TFT_ClearScreen(uint color)
{
  	uint i, j;

	TFT_SetWindow(0, 0, TFT_XMAX,TFT_YMAX);	 //作用区域,TFT_XMAX,TFT_YMAX
  	for(i=0; i<=TFT_XMAX; i++)
	{
		for (j=0; j<=TFT_YMAX; j++)
		{
			TFT_WriteData(color);
		}
	}
}


