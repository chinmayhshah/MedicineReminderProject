#ifndef stlcd_h
#define stlcd_h

/*****************************************************************
Header file defining prototypes and macros of registers
Reference of Registers :ST7565 LCD library!
[1]2010 Limor Fried, Adafruit Industries

Connections



******************************************************************/
#include "RTC_DS1307.h"

#define CMD_DISPLAY_OFF   0xAE
#define CMD_DISPLAY_ON    0xAF

#define CMD_SET_DISP_START_LINE  0x40
#define CMD_SET_PAGE  0xB0

#define CMD_SET_COLUMN_UPPER  0x10
#define CMD_SET_COLUMN_LOWER  0x00

#define CMD_SET_ADC_NORMAL  0xA0
#define CMD_SET_ADC_REVERSE 0xA1

#define CMD_SET_DISP_NORMAL 0xA6
#define CMD_SET_DISP_REVERSE 0xA7

#define CMD_SET_ALLPTS_NORMAL 0xA4
#define CMD_SET_ALLPTS_ON  0xA5
#define CMD_SET_BIAS_9 0xA2
#define CMD_SET_BIAS_7 0xA3

#define CMD_RMW  0xE0
#define CMD_RMW_CLEAR 0xEE
#define CMD_INTERNAL_RESET  0xE2
#define CMD_SET_COM_NORMAL  0xC0
#define CMD_SET_COM_REVERSE  0xC8
#define CMD_SET_POWER_CONTROL  0x28
#define CMD_SET_RESISTOR_RATIO  0x20
#define CMD_SET_VOLUME_FIRST  0x81
#define  CMD_SET_VOLUME_SECOND  0
#define CMD_SET_STATIC_OFF  0xAC
#define  CMD_SET_STATIC_ON  0xAD
#define CMD_SET_STATIC_REG  0x0
#define CMD_SET_BOOSTER_FIRST  0xF8
#define CMD_SET_BOOSTER_234  0
#define  CMD_SET_BOOSTER_5  1
#define  CMD_SET_BOOSTER_6  3
#define CMD_NOP  0xE3
#define CMD_TEST  0xF0

//Changes to simple AT89C51 Functions by chinmay.shah@colorado.edu
//PIN Definition and Functions

#define BLA P3_4

#define A0 P1_2 //Select register Pin
#define RST P3_3 //Reset Pin
#define CS P1_1 //GLCD select


#define GLCD_GREEN P2_1
#define GLCD_RED   P2_0
void spiwrite(unsigned char c);
void setup(void);
void loop(void);
void st7565_init(void);
void st7565_command(unsigned char c);
void st7565_data(unsigned char c);
void st7565_set_brightness(unsigned char val);
void clear_screen(void);
void clear_buffer(unsigned char *buffer);
void write_buffer(unsigned char *buffer);
void displayChar(unsigned char datachar);
void displayString(unsigned char dataString[]);
void setGLCDLocation(unsigned char pagevalue);
void clear_buffer(unsigned char *buff);
void clear_screen(void);
void GLCDPage1(void);
void GLCDPage2(void);
void GLCDNoCard(void);
void displayUserString(unsigned char pagelocation,unsigned char customString[]);
void displayGLCDClock(timeclock *GLCDTime);
void GLCDHomePage();
void GLCDCUPage();
#endif // stlcd_h
