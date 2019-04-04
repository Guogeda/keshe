#include"lcdDisplay.h"

/****************************************************************************
*函数名：main
*输  入：无
*输  出：无
*功  能：主函数
****************************************************************************/

void main (void)
{

	TFT_Init();                   //初始化TFT
	TFT_ClearScreen(WHITE);	  	  
	nRF24L01_Init();
	while(1)
	{	
		LcdDisplay(Ds18b20ReadTemp());
	}		
}
