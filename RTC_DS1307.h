/************************************************************************************
* Prototype Functions to communicate with RTC DS1307
* Macro Definitions of Register Address of RTC
    [1]DS1307 Datasheet -
    http://pdf1.alldatasheet.com/datasheet-pdf/view/123888/DALLAS/DS1307.html


Complete Coded written by chinmay.shah@colorado.edu
* Interfaced using I2C


* Connection with AT89C51 mentioned in Header file
    RTC (DS1307)   AT89C51
    SDA             P1_4 (pulled by 5.1Kohm)
    SCL             P1_3 (pulled by 5.1Kohm)
    GND             GND
    5V              5V


**********************************************************************************************/

#ifndef RTC_DS1307_h
#define RTC_DS1307_h

//Address for communication using I2C
#define RTC_DS1307_SLAVEADDRESS_WR 0xD0
#define RTC_DS1307_SLAVEADDRESS_RD 0xD1

//Refer  DataSheet pg 4 [1]
#define RTC_DS1307_SECREG          0x00
#define RTC_DS1307_MINREG          0x01
#define RTC_DS1307_HRREG           0x02
#define RTC_DS1307_DAYREG          0x03
#define RTC_DS1307_DATEREG         0x04
#define RTC_DS1307_MONREG          0x05
#define RTC_DS1307_YRREG           0x06
#define RTC_DS1307_CTRLREG         0x07

//Structure definition for a clock
typedef struct time_stamp
{
     unsigned char  sec;
	 unsigned char 	minute;
	 unsigned char	hrs;
     unsigned char 	dte;
	 unsigned char	mon;
     unsigned char  day;
	 unsigned char	yr;
}timeclock;



//Prototype Definitions of functions



void RTCDS1307Init();
unsigned char RTCDS1307Read(unsigned char addr);
void RTCDS1307ClockInit();
struct time_stamp xdata * RTCClockRead();
void RTCDS1307Write(unsigned char addr,unsigned char databyte);
#endif // RTC_DS1307_h
