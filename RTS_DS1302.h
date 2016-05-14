#ifndef RTC_DS1302_h
#define RTC_DS1302_h



#define CE    P1_0//(RST)
/******************** Important Note ***********************************
P1_0 - RST for DS1302 used only for resetting , while accessing RTC
Therefore RFID should not be accessed as usage of same pin as
Slave Select of RFID (MFRC522)
************************************************************************/
#define SCLK  P3_4//Synchronize clock//T0
#define IO    P3_5//IO is same as SDA//T1

#define RTC_MAX_DATA 8

#define RTC_INI 1 //1 - True To Initialize RTC
                  //0 - False to not Initialize RTC


typedef struct time_stamp
{
     unsigned char  sec;
	 unsigned char 	minute;
	 unsigned char	hrs;
     unsigned char 	dte;
	 unsigned char	mon;
     unsigned char  day;
	 unsigned char	yr;
}RTCtime;





/***************************************************************************/
/* Prototypes                                                              */
/***************************************************************************/
void RTCReset();
void RTCWriteByte(unsigned char bytedata);
unsigned char RTCReadByte();
unsigned char RTCRegRead(unsigned char reg);
void RTCRegWrite(unsigned char reg,unsigned char data);
//void RTCClockRead();
struct time_stamp xdata * RTCClockRead();
void RTCBrustClockRead();
void RTCInitialize();
void RTCBurstInitialize();


/***************************************************************************/


#endif // RTC_DS1302_h
