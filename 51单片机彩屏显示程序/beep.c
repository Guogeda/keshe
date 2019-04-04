#include "bbp.h"

 void delay(unsigned int i)
{
    char j;
    for(i; i > 0; i--)
        for(j = 200; j > 0; j--);
}

void beep_alarm()
{
	beep= 1;
	delay(5);
	beep= 0;
	delay(5);
}

void beep_safe()
{
	beep= 0;
	delay(5);
	beep= 1;
	delay(5);
}