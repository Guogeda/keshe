#ifndef __BEEP_H__
#define __BEEP_H__

#include<reg51.h>

#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint 
#define uint unsigned int
#endif

sbit beep =  P2^3 ;
void beep_alarm();
void beep_safe();
void delay(unsigned int i);

#endif