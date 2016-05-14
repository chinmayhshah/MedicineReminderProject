/***************************************************************************
Reference for this Library is
Most of the code is derived from the solution provided from this site
        [1]https://www.maximintegrated.com/en/app-notes/index.mvp/id/3449
        [2]https://datasheets.maximintegrated.com/en/ds/DS1302.pdf

 based on I2C and SPI
 DS1302 does not implement both protocols,but in between 3 wire protocol
 Referring [1] and [2] following is library of functions using bit banging

***************************************************************************/
#include "RTC_DS1307.h"
#include <string.h>
#include "Serial.h"
#include <stdio.h>
#include <at89c51ed2.h>
#include "delay_custom.h"

timeclock *currentTime=0xE900;//Pointer



// Reset and Enable the interface for 3 Wire
void RTCReset()
{
   SCLK=0;
   CE=0;
   CE=1;
}



// Write Byte to DS1302
void RTCWriteByte(unsigned char bytedata)
{
    unsigned char i;
    //bytedata = bytedata|0x80;//Making MSB as 1
    for(i = 0; i < 8; ++i)
	{
		IO = 0;
		if(bytedata & 0x01)
		{
			IO = 1;	/* set port pin high to read data */
		}
		SCLK = 0;
		SCLK = 1;
		bytedata >>= 1;
    }
}

// Read Byte from DS1302
unsigned char RTCReadByte()
{
unsigned char i;
unsigned char ReadByte;
unsigned char TmpByte;

	ReadByte = 0x00;
	IO = 1;
	for(i = 0; i < 8; i++)
	{
		SCLK = 1;
		SCLK = 0;
		TmpByte = (unsigned char)IO;
		TmpByte <<= 7;
		ReadByte >>= 1;
		ReadByte |= TmpByte;
	}
	return ReadByte;
}


//Write byte at a particular Addr and Data
void RTCRegWrite(unsigned char reg,unsigned char bytedata)
{
	RTCReset();
	RTCWriteByte(reg);
	RTCWriteByte(bytedata);
	RTCReset();
}


//Write byte at a particular Addr and Data
unsigned char RTCRegRead(unsigned char reg)
{
    unsigned char bytedata;
	RTCReset();
	RTCWriteByte(reg);
	bytedata=RTCReadByte();
	RTCReset();
	return bytedata;
}



// Read a burst of data to read the Clock from RTC
void RTCBrustClockRead()
{
unsigned char prev_sec = 99;//, sec, min, hrs, dte, mon, day, yr;

    // Read and Display Clock

		RTCReset();
		RTCWriteByte(0xBF);	//Clock Burst Address location
		currentTime->sec = RTCReadByte();
		delay_ms(20);
		currentTime->minute= RTCReadByte();
		delay_ms(20);
		currentTime->hrs = RTCReadByte();
		delay_ms(20);
		currentTime->dte = RTCReadByte();
		delay_ms(20);
		currentTime->mon = RTCReadByte();
		delay_ms(20);
		currentTime->day = RTCReadByte();
		delay_ms(20);
		currentTime->yr  = RTCReadByte();
		delay_ms(20);
		RTCReset();
		if(currentTime->sec != prev_sec)	//Printing after a second
		{
			printf("\n\r %02x/%02x/%02x %02x", currentTime->yr, currentTime->dte, currentTime->mon, currentTime->day);
			printf(" %02x:%02x:%02x", currentTime->hrs,currentTime->minute, currentTime->sec);
			prev_sec = currentTime->sec;
		}



}


// Initialize RTC using Brust Write
void RTCBurstInitialize()
{
    RTCReset();
	RTCWriteByte(0x8e);	// control register */
	RTCWriteByte(0);		// disable write protect */
	RTCReset();
	RTCWriteByte(0x90);	// trickle charger register */
	RTCWriteByte(0xab);	// enable, 2 diodes, 8K resistor */
	RTCReset();
	RTCWriteByte(0xbe);	// clock burst write (eight registers) */
	RTCWriteByte(0x00);//second
	RTCWriteByte(0x37);//min
	RTCWriteByte(0x22);//hour
	RTCWriteByte(0x18);//date
	RTCWriteByte(0x04);//month
	RTCWriteByte(0x02);//day
	RTCWriteByte(0x16);//year
	RTCWriteByte(0);    // must write control register in burst mode */
	RTCReset();//End Burst Write
}





void RTCInitialize()
{

    RTCRegWrite(0x8e,0);	// control register ,disable protect*/
    delay_ms(20);
    RTCRegWrite(0x90,0xab);	// trickle charger register ,disable protect*/
    delay_ms(20);
    RTCRegWrite(0x80,0x00);	// seconds register
    delay_ms(20);
    RTCRegWrite(0x82,0x39);	// min register
    delay_ms(20);
    RTCRegWrite(0x84,0x12);	// hour register
    delay_ms(20);
    RTCRegWrite(0x86,0x14);	// date register
    delay_ms(20);
    RTCRegWrite(0x88,0x04);	// Month register
    delay_ms(20);
    RTCRegWrite(0x8A,0x04);	// Day of Week
    delay_ms(20);
    RTCRegWrite(0x8C,0x16);	// Year

}



// Read a burst of data to read the Clock from RTC
struct time_stamp xdata * RTCClockRead()
{
    unsigned char prev_sec = 99;//, sec, min, hrs, dte, mon, day, yr;

    // Read and Display Clock

		//RTCReset();
		//RTCWriteByte(0x81);	//Clock Burst Address location
		currentTime->sec = RTCRegRead(0x81);
		delay_ms(20);
		currentTime->minute = RTCRegRead(0x83);
		delay_ms(20);
		currentTime->hrs = RTCRegRead(0x85);
		delay_ms(20);
		currentTime->dte = RTCRegRead(0x87);
		delay_ms(20);
		currentTime->mon = RTCRegRead(0x89);
		delay_ms(20);
		currentTime->day = RTCRegRead(0x8B);
		delay_ms(20);
		currentTime->yr  = RTCRegRead(0x8D);

		if(currentTime->sec != prev_sec)	//Printing after a second
		{
			printf("\n\r %02x/%02x/%02x %02x", currentTime->yr, currentTime->dte, currentTime->mon, currentTime->day);
			printf(" %02x:%02x:%02x", currentTime->hrs, currentTime->minute, currentTime->sec);
			prev_sec = currentTime->sec;
		}

    return (struct time_stamp xdata *)currentTime;

}



