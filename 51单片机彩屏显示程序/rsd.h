#ifndef  _RSD_H
#define  _RSD_H

#include <reg51.h>

#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint 
#define uint unsigned int
#endif


sbit rsd=P3^6;

uchar check_rsd();


#endif