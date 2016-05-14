/*****************************************************************************************************************
Functions to write on GLCD ST7565

Read :ST7565 LCD library
[1]2010 Limor Fried, Adafruit Industries
[2]some of this code was written by <cstone@pobox.com> originally;
The above library does not work with AT89C51 due to a large back buffer concept due to issues
with SDCC and Space crunch
[3] tutorial - http://www.ladyada.net/learn/lcd/st7565.html

Redesigned library with Simple functions for AT89C51 library  Functions  for ST7565  by chinmay.shah@colorado.edu


Referred following from [1]
1)st7565_init(void);
2)st7565_command(unsigned char c);
3)st7565_data(unsigned char c);
4)st7565_set_brightness(unsigned char val);
5)clear_screen(void);
----------------------------IMP NOTE----------------------------------------------
As above functions were not working correctly with AT89C51 and SDCC due to use of large buffer.
Redesigned by removing buffer concept and changing above functions plus
added new functions by

chinmay.shah@colorado.edu
-----------------------------------------------------------------------------------
New Functions

6)displayChar(unsigned char datachar);
7)displayString(unsigned char dataString[]);
8)setGLCDLocation(unsigned char pagevalue);
9)clear_buffer(unsigned char *buff);
11)GLCDPage1(void);
12)GLCDPage2(void);
13)GLCDNoCard(void);
14)displayUserString(unsigned char pagelocation,unsigned char customString[]);
15)displayGLCDClock(timeclock *GLCDTime);
16)GLCDHomePage();
17)GLCDCUPage();

Connections in Header File
*****************************************************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "stlcd.h"
#include "glcd.h"
#include <at89c51ed2.h>
#include "SPI.h"
#include "delay_custom.h"
#include "myFont.h"
#include "string.h"
#include "RTC_DS1307.h"



                //     0 1 2 3 4 5 6 7 //array locations
const int pagemap[] = {4,5,6,7,0,1,2,3};//Page Map of Graphical LCD

void setup(void) {

    unsigned char i=0;
    CS=0;   //GLCD Selected
    st7565_init();
    CS=1;//GLCD deselected
}


/**********************************************
Initializing sequence of LCD
Refer Section 4.7 of LM6059BCW manual Rev 3.0
changes incoopporated by chinmay.shah@colorado.edu
*********************************************/
void st7565_init(void) {
  //CS = 0;//Selecting Graphical LCD
  RST = 0;//Maintaining Reset
  delay_ms(500);//Delay for 500 ms
  RST = 1;//Release
  // LCD bias select
  st7565_command(CMD_SET_BIAS_7);//0xA3 Instead of CMD_SET_BIAS_9(0xA2) not working
  // ADC select
  st7565_command(CMD_SET_ADC_NORMAL);
  // SHL select
  st7565_command(CMD_SET_COM_REVERSE);//Instead ,CMD_SET_COM_NORMAL, as it is reversed
  // Initial display line
  st7565_command(CMD_SET_DISP_START_LINE);//Start Line match
  // turn on voltage converter (VC=1, VR=0, VF=0)
  st7565_command(CMD_SET_POWER_CONTROL | 0x4);//0x2C
  // wait for 50% rising
  delay_ms(50);
  // turn on voltage regulator (VC=1, VR=1, VF=0)
  st7565_command(CMD_SET_POWER_CONTROL | 0x6);//0x2E
  // wait >=50ms
  delay_ms(50);
  // turn on voltage follower (VC=1, VR=1, VF=1)
  st7565_command(CMD_SET_POWER_CONTROL | 0x7);//0x2F
  // wait
  delay_ms(10);
  // set lcd operating voltage (regulator resistor, ref voltage resistor)
  st7565_command(CMD_SET_RESISTOR_RATIO | 0x6);//0x26
  //On Display
  st7565_command(CMD_DISPLAY_ON);//0xAF
  //Not included in library
  // set page address
  st7565_command(CMD_SET_PAGE);//0xB0, Write Buffer function sets page again
  // set column address
  st7565_command(CMD_SET_ALLPTS_NORMAL);
  st7565_set_brightness(0x18);
  clear_screen();
  // initial display line
  // write display data
  //CS=1;
}



/**********************************************
Write from Master to Salve

**********************************************/
 void spiwrite(unsigned char c) {
    SPI_Write(c);
}


/**********************************************
Write Command from Master to Salve

**********************************************/
void st7565_command(unsigned char c) {

  A0 = 0;
  spiwrite(c);
}

/**********************************************
Write Data from Master to Salve

**********************************************/
void st7565_data(unsigned char c) {

  A0 = 1;
  spiwrite(c);
}

/**********************************************
Set brightness of LCD

**********************************************/
void st7565_set_brightness(unsigned char val) {
    st7565_command(CMD_SET_VOLUME_FIRST);
    st7565_command(CMD_SET_VOLUME_SECOND | (val & 0x3f));
}



/**********************************************
Function used to write into buffer and copy
that to the each pixel values of GLCD
---- Does not work correctly as Buffer has
not been used in current implementation
**********************************************/

void write_buffer(unsigned char *buffer) {
  unsigned char c, p;
  for(p = 0; p < 8; p++) {
        st7565_command(CMD_SET_PAGE | pagemap[p]);//Set Page address ,0xB0
        st7565_command(CMD_SET_COLUMN_LOWER | (0x0 & 0xf));//Set Lower
        st7565_command(CMD_SET_COLUMN_UPPER | ((0x0 >> 4) & 0xf));//Set Upper
        st7565_command(CMD_RMW);//Enter into REad Modify Write Mode
        st7565_data(0xff);
        //Display data of Buffer
        for(c = 0; c < 128; c++) {

        st7565_data(buffer[(128*p)+c]);

    }
  }
}





void setGLCDLocation(unsigned char pagevalue)
{
    st7565_command(CMD_SET_PAGE | pagemap[pagevalue]);//Set Page address ,0xB0
    st7565_command(CMD_SET_COLUMN_LOWER | (0x0 & 0xf));//Set Lower
    st7565_command(CMD_SET_COLUMN_UPPER | ((0x0 >> 4) & 0xf));//Set Upper
    st7565_command(CMD_RMW);//Enter into REad Modify Write Mode
    st7565_data(0xff);
}

// clear everything
//Not been used by this project
void clear_buffer(unsigned char *buff) {
  memset(buff, 0, 1024);
}



void clear_screen(void) {
  unsigned char p, c;

  for(p = 0; p < 8; p++) {
    st7565_command(CMD_SET_PAGE | p);
    for(c = 0; c < 129; c++) {
      st7565_command(CMD_SET_COLUMN_LOWER | (c & 0xf));
      st7565_command(CMD_SET_COLUMN_UPPER | ((c >> 4) & 0xf));
      st7565_data(0x0);
    }
  }
}


//New Functions added by chinmay.shah@colorado.edu



void displayChar(unsigned char datachar) {
    unsigned char i=0;
    for (i=0;i<8;i++)
    {
        st7565_data(myfont[datachar-OFFSET_CHARACTER][i]);

    }
  }

void displayString(unsigned char dataString[]) {
    unsigned char i=0;
    while (dataString[i]!='\0')
    {
            displayChar(dataString[i++]);
    }
}

void GLCDPage1()
{

    displayUserString(0,"Medicine Remind");
    GLCDCUPage();
    displayUserString(7,"      System        ");
    delay_seconds(3);
    CS=0;
    clear_screen();
    CS=1;
}


void displayUserString(unsigned char pagelocation,unsigned char customString[])
{

    CS=0;
    setGLCDLocation(pagelocation);
    displayString(customString);
    CS=1;
}

void GLCDPage2()
{
    CS=0;
    clear_screen();
    CS=1;
    displayUserString(2,"PLACE MED NEAR");
    displayUserString(3,"     READER  ");
    delay_seconds(3);
    CS=0;
    clear_screen();
    CS=1;
    displayGLCDClock(RTCClockRead());
}


void GLCDNoCard()
{
    GLCD_RED=0;
    displayUserString(2," NO MED CARD ");
    displayUserString(3,"      READ ");
    delay_seconds(3);
    CS=0;
    clear_screen();
    CS=1;
    GLCD_RED=1;
    displayGLCDClock(RTCClockRead());
}

void GLCDHomePage()
{
    displayUserString(0,"-----******-HOME");
    displayUserString(2,"         RECORD ");
    displayUserString(2," CHECK ");
    //displayUserString(4,"         *LIST ");
    displayUserString(4," INSERT ");
    displayUserString(6,"-----******-----");
}
/*******************************************************
Display Clock on GLCD

********************************************************/
void displayGLCDClock(timeclock *GLCDTime)
{
    const unsigned char displayDay[7][4]=
    {   // 0      1        2       3       4       5       6
        {"SUN\0"},{"MON\0"},{"TUE\0"},{"WED\0"},{"THR\0"},{"FRI\0"},{"SAT\0"}
    };
    CS=0;//Select GLCD
    setGLCDLocation(7);//Bottom most line of GLCD
    //Date
    displayChar(((GLCDTime->dte) >>4) & 0x0F|0x30); //MSB of date
    displayChar(GLCDTime->dte & 0x0F|0x30);//LSB of date
    displayChar('/');//Semi colon
    //Month
    displayChar(((GLCDTime->mon) >>4) & 0x0F|0x30); //MSB of mon
    displayChar(GLCDTime->mon & 0x0F|0x30);//LSB of date
    displayChar(' ');//Space
    //Displaying Day of Week
    displayString(displayDay[GLCDTime->day -1]);
    displayChar(' ');//Space
    displayChar(((GLCDTime->hrs) >>4) & 0x0F|0x30); //MSB of hr
    displayChar(GLCDTime->hrs & 0x0F|0x30);//LSB of hr
    displayChar(':');//Semi colon
    displayChar(((GLCDTime->minute) >>4) & 0x0F|0x30);//MSB of minute
    displayChar(GLCDTime->minute & 0x0F|0x30);//LSB of minute
    CS=1;//DeSelect GLCDs
}


void GLCDCUPage()
{

    displayUserString(1,"   ******");
    displayUserString(2,"   *   *     *");
    displayUserString(3,"   *   *     *");
    displayUserString(4,"   *******   *");
    displayUserString(5,"       *******");

}

void GLCDCUNewPage()
{

    displayUserString(1,"   ******");
    displayUserString(2,"   *   *     *");
    displayUserString(3,"   *   *     *");
    displayUserString(4,"   *******   *");
    displayUserString(5,"       *******");

}

