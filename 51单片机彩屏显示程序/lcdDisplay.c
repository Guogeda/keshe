#include "lcdDisplay.h"

/******
����汾�Ĳ�����ʾ����
�¶ȣ�+25.44
����  0
sending.....
�¶ȣ�25
ʪ�ȣ�84
******/
void LcdDisplay(int temp) 	 //lcd��ʾ
{
	uchar i,j=100;
	unsigned char datas[]={0,0,0,0,0,0}; //��������
	unsigned char xdata codes[][10] = {"0","1","2","3","4","5","6","7","8","9"};
	float tp; 
	GUI_Write32CnChar(125, 20, "�¶�", RED, WHITE);
	GUI_WriteASCII(125, 80, ":", RED, WHITE);

	 
	if(temp< 0)				//���¶�ֵΪ����
  	{
	  	
		GUI_WriteASCII(125, 90, "-", RED, WHITE);
		//��Ϊ��ȡ���¶���ʵ���¶ȵĲ��룬���Լ�1����ȡ�����ԭ��
		temp=temp-1;
		temp=~temp;
		tp=temp;
		temp=tp*0.0625*100+0.5;	
		//������С�����*100��+0.5���������룬��ΪC���Ը�����ת��Ϊ���͵�ʱ���С����
		//��������Զ�ȥ���������Ƿ����0.5����+0.5֮�����0.5�ľ��ǽ�1�ˣ�С��0.5�ľ�
		//�����0.5��������С������档
 
  	}
 	else
  	{			
	  	GUI_WriteASCII(125, 90, "+", RED, WHITE);
		tp=temp;//��Ϊ���ݴ�����С�������Խ��¶ȸ���һ�������ͱ���
		//����¶���������ô����ô������ԭ����ǲ���������
		temp=tp*0.0625*100+0.5;	
		//������С�����*100��+0.5���������룬��ΪC���Ը�����ת��Ϊ���͵�ʱ���С����
		//��������Զ�ȥ���������Ƿ����0.5����+0.5֮�����0.5�ľ��ǽ�1�ˣ�С��0.5�ľ�
		//�����0.5��������С������档
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
	nRF24L01_Set_TX_Mode(&datas);//��������	 ��
	while(Check_Ack());//�ȴ��������

	GUI_WriteASCII(95, 95,"sendover", RED, WHITE);
	SPI_W_Reg(W_REGISTER+STATUS,0xff);//����״̬�Ĵ�����
	if(check_rsd()==1)
		GUI_Write32CnChar(95, 20, "����", RED, WHITE);
	else
	    GUI_Write32CnChar(95, 20, "����", RED, WHITE);
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
				GUI_Write32CnChar(35, 20, "�¶�",WHITE, RED);
				GUI_WriteASCII(35, 95+i*15,&codes[RX_Buffer[i]], WHITE,RED);
			}
			for (i=2;i<4;i++)
			{
				GUI_Write32CnChar(5, 20, "ʪ��", WHITE,RED );
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