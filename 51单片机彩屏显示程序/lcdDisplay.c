#include "lcdDisplay.h"

/******
×î³õ°æ±¾µÄ²ÊÆÁÏÔÊ¾º¯Êı
ÎÂ¶È£º+25.44
ÎŞÈË  0
sending.....
ÎÂ¶È£º25
Êª¶È£º84
******/
void LcdDisplay(int temp) 	 //lcdÏÔÊ¾
{
	uchar i,j=100;
	unsigned char datas[]={0,0,0,0,0,0}; //¶¨ÒåÊı×é
	unsigned char xdata codes[][10] = {"0","1","2","3","4","5","6","7","8","9"};
	float tp; 
	GUI_Write32CnChar(125, 20, "ÎÂ¶È", RED, WHITE);
	GUI_WriteASCII(125, 80, ":", RED, WHITE);

	 
	if(temp< 0)				//µ±ÎÂ¶ÈÖµÎª¸ºÊı
  	{
	  	
		GUI_WriteASCII(125, 90, "-", RED, WHITE);
		//ÒòÎª¶ÁÈ¡µÄÎÂ¶ÈÊÇÊµ¼ÊÎÂ¶ÈµÄ²¹Âë£¬ËùÒÔ¼õ1£¬ÔÙÈ¡·´Çó³öÔ­Âë
		temp=temp-1;
		temp=~temp;
		tp=temp;
		temp=tp*0.0625*100+0.5;	
		//ÁôÁ½¸öĞ¡Êıµã¾Í*100£¬+0.5ÊÇËÄÉáÎåÈë£¬ÒòÎªCÓïÑÔ¸¡µãÊı×ª»»ÎªÕûĞÍµÄÊ±ºò°ÑĞ¡Êıµã
		//ºóÃæµÄÊı×Ô¶¯È¥µô£¬²»¹ÜÊÇ·ñ´óÓÚ0.5£¬¶ø+0.5Ö®ºó´óÓÚ0.5µÄ¾ÍÊÇ½ø1ÁË£¬Ğ¡ÓÚ0.5µÄ¾Í
		//ËãÓÉÏ0.5£¬»¹ÊÇÔÚĞ¡ÊıµãºóÃæ¡£
 
  	}
 	else
  	{			
	  	GUI_WriteASCII(125, 90, "+", RED, WHITE);
		tp=temp;//ÒòÎªÊı¾İ´¦ÀíÓĞĞ¡ÊıµãËùÒÔ½«ÎÂ¶È¸³¸øÒ»¸ö¸¡µãĞÍ±äÁ¿
		//Èç¹ûÎÂ¶ÈÊÇÕıµÄÄÇÃ´£¬ÄÇÃ´ÕıÊıµÄÔ­Âë¾ÍÊÇ²¹ÂëËü±¾Éí
		temp=tp*0.0625*100+0.5;	
		//ÁôÁ½¸öĞ¡Êıµã¾Í*100£¬+0.5ÊÇËÄÉáÎåÈë£¬ÒòÎªCÓïÑÔ¸¡µãÊı×ª»»ÎªÕûĞÍµÄÊ±ºò°ÑĞ¡Êıµã
		//ºóÃæµÄÊı×Ô¶¯È¥µô£¬²»¹ÜÊÇ·ñ´óÓÚ0.5£¬¶ø+0.5Ö®ºó´óÓÚ0.5µÄ¾ÍÊÇ½ø1ÁË£¬Ğ¡ÓÚ0.5µÄ¾Í
		//Ëã¼ÓÉÏ0.5£¬»¹ÊÇÔÚĞ¡ÊıµãºóÃæ¡£
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
	nRF24L01_Set_TX_Mode(&datas);//·¢ËÍÊı¾İ	 ³É
	while(Check_Ack());//µÈ´ı·¢ËÍÍê³É

	GUI_WriteASCII(95, 95,"sendover", RED, WHITE);
	SPI_W_Reg(W_REGISTER+STATUS,0xff);//ÇåÁã×´Ì¬¼Ä´æÆ÷£¿
	if(check_rsd()==1)
		GUI_Write32CnChar(95, 20, "ÓĞÈË", RED, WHITE);
	else
	    GUI_Write32CnChar(95, 20, "ÎŞÈË", RED, WHITE);
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
				GUI_Write32CnChar(35, 20, "ÎÂ¶È",WHITE, RED);
				GUI_WriteASCII(35, 95+i*15,&codes[RX_Buffer[i]], WHITE,RED);
			}
			for (i=2;i<4;i++)
			{
				GUI_Write32CnChar(5, 20, "Êª¶È", WHITE,RED );
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