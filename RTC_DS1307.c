/************************************************************************************
* Functions to communicate with RTC DS1307
* Reading:
*   [1] DS1307 DataSheet
    [2]https://learn.adafruit.com/ds1307-real-time-clock-breakout-board-kit
    [3]https://www.maximintegrated.com/en/app-notes/index.mvp/id/95
Complete Coded written by chinmay.shah@colorado.edu
* Interfaced using I2C
* Connection with AT89C51 mentioned in Header file

***********************************************************************************/

#include "RTC_DS1307.h"
#include <string.h>
#include "Serial.h"
#include <stdio.h>
#include <at89c51ed2.h>
#include "delay_custom.h"
#include "I2C.h"


/*************************************************************************
Function to Initialize RTC DS1307
*************************************************************************/
void RTCDS1307Init()
{
    I2CStart();
    I2CSend(RTC_DS1307_SLAVEADDRESS_WR);//Write mode for DS1307
    I2CSend(RTC_DS1307_CTRLREG);//address control register
                                //Disable SQWE
    I2CSend(0x00);
    I2CStop();//End of Control register
    //Only once for Initializing exact time
    RTCDS1307ClockInit();// Initializing values to set RTC
    /*
    DEBUG
    printf("\n\rSec %x ",RTCDS1307Read(RTC_DS1307_SECREG));
    printf("\n\rMin %x ",RTCDS1307Read(RTC_DS1307_MINREG));
    printf("\n\rHr %x ",RTCDS1307Read(RTC_DS1307_HRREG));
    printf("\n\rDate %x ",RTCDS1307Read(RTC_DS1307_DATEREG));
    printf("\n\rDAY %x ",RTCDS1307Read(RTC_DS1307_DAYREG));
    printf("\n\rMON %x ",RTCDS1307Read(RTC_DS1307_MONREG));
    printf("\n\rYr %x ",RTCDS1307Read(RTC_DS1307_YRREG));
    */
}



/*************************************************************************
Function to Write to registers of RTC DS1307
Input addr - Address of Register
      databyte - data to be written to Register
*************************************************************************/
void RTCDS1307Write(unsigned char addr,unsigned char databyte)
{

    I2CStart();//Start of I2C
    I2CSend(RTC_DS1307_SLAVEADDRESS_WR);//For Write command
    I2CSend(addr);//For address of data
    I2CSend(databyte);//Write a register value MSB
    I2CStop();//Stop bit of I2C

}


/*************************************************************************
Function to Read form registers of RTC DS1307
Input addr - Address of Register
*************************************************************************/

unsigned char RTCDS1307Read(unsigned char addr)
{
    unsigned char databyte;
    I2CStart();//Start of I2C
    I2CSend(RTC_DS1307_SLAVEADDRESS_WR);//Write mode for DS1307
    I2CSend(addr);//address of Sec address
    I2CStart();//Repeated Start
    I2CSend(RTC_DS1307_SLAVEADDRESS_RD);//Write mode for DS1307
    databyte=I2CRecieve();//Read Data from Slave
    I2CStop();//End of Control register
    return databyte;

}
/*************************************************************************
Function to Initialize value registers of  RTC DS1307
*************************************************************************/

void RTCDS1307ClockInit()
{
    RTCDS1307Write(RTC_DS1307_SECREG,0x00);//Second
    RTCDS1307Write(RTC_DS1307_MINREG,0x26);//Minute
    RTCDS1307Write(RTC_DS1307_HRREG,0x13);//Hour
    RTCDS1307Write(RTC_DS1307_DAYREG,0x07);//Day
    RTCDS1307Write(RTC_DS1307_DATEREG,0x30);//Date
    RTCDS1307Write(RTC_DS1307_MONREG,0x04);//Month
    RTCDS1307Write(RTC_DS1307_YRREG,0x16);// year
}



/*************************************************************************
Function to  read relevant(current) clock information from RTC DS1307
Output
 is a time_stamp structure storing all RTC  values
*************************************************************************/
struct time_stamp xdata * RTCClockRead()
{
    unsigned char prev_sec = 99;//, sec, min, hrs, dte, mon, day, yr;
    timeclock t2;
    timeclock * currentTime=&t2;//Pointer

    // Read and Display Clock
		currentTime->sec = RTCDS1307Read(RTC_DS1307_SECREG);//Read
		delay_ms(20);
		currentTime->minute = RTCDS1307Read(RTC_DS1307_MINREG);
		delay_ms(20);
		currentTime->hrs = RTCDS1307Read(RTC_DS1307_HRREG);
		delay_ms(20);
		currentTime->dte = RTCDS1307Read(RTC_DS1307_DATEREG);
		delay_ms(20);
		currentTime->mon = RTCDS1307Read(RTC_DS1307_MONREG);
		delay_ms(20);
		currentTime->day = RTCDS1307Read(RTC_DS1307_DAYREG);
		delay_ms(20);
		currentTime->yr  = RTCDS1307Read(RTC_DS1307_YRREG);

		if(currentTime->sec != prev_sec)	//Printing after a second
		{
			prev_sec = currentTime->sec;
		}

    return (struct time_stamp xdata *)currentTime;

}

