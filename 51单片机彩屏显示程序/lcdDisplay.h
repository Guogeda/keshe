#ifndef  _lcdDisplay_H
#define  _lcdDisplay_H

#include <reg51.h>

#ifndef uchar
#define uchar unsigned char
#endif

#ifndef uint 
#define uint unsigned int
#endif

#include "REG51.H"
#include "lcd_driver.h"
#include "gui.h"
#include "rsd.h"
#include "nrf24l01.h"
#include "temp.h"
#include "bbp.h"

void LcdDisplay(int temp);