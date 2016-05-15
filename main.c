/*********************************************************************
Start Date : 4/25/2016
Version

code by :chinmay.shah@colorado.edu
Code : To implement a  menu driven program for different options for
                        Medicine Reminder System
       Menu is as follows on Graphical LCD (with touch screen )
       -------------------------------HOME-

            CHECK           RECORD

            INSERT

       ------------------------------------
main.c
Integrated following and Libraries coded for the same with details
mentioned in each code and libraries


New Firmware
1) "SPI.h"                  - SPI Library
2) "MFRC522RFID_STMBASED.h" - RFID MFRC522 Library using SPI
3) "RTC_DS1307.h"           - RTC  DS1307 Library using I2C
4) "RTC_DS1307.h"           - RTC  DS1307 Library using 3 wire protocol
5) "stlcd.h"                - Graphical LCD ST7565 Library using SPI
6) "TSC2013_Q1.h"           - Touch Screen (TS2013) Library using I2C
7) "Medicine_DB.h"          - Medicine Library using all library to create a
                             Reminder System


Leveraged firmware from Lab3 and Lab4
1) "Serial.h" - RS232 Interface for Serial Communication
2) "delay_custom.h"- Delays ranging from us to Seconds
3) "I2C.h" - I2C bit banging code
4) "eeprom.h" -  EEPROM Interface functions- Acts as a Database
5) "constraints.h" - Used for some debugging


*********************************************************************/
#include <at89c51ed2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mcs51/8051.h>
#include "Serial.h"
#include "delay_custom.h"
#include "I2C.h"
#include "eeprom.h"
#include "SPI.h"
#include "MFRC522RFID_STMBASED.h"
#include "RTC_DS1307.h"
#include "Medicine_DB.h"
#include "stlcd.h"
#include "TSC2013_Q1.h"
#include "constraints.h"

/********************************************
*   Macro Definition
*********************************************/

#define BAUD_RATE 0xFD      // Timer Count for default BAUDRATE 9600
#define TMODINI 0x20        //Timer 1 Initializing
#define SCONINI 0x50        //SCON Initializing value by default
                            //Serial Mode 1 , 8 bit data with Start and Stop bi//#define AUXR
                            //This is been used for Serial debugging
#define DEBUG

/********************************************
*   SFR Definition
*********************************************/

__sfr __at (0xA8) IEN0 ;
__sfr __at (0xC3) SPCON ;
__sfr __at (0xC4) SPSTA ;
__sfr __at (0xC5) SPDAT ;

/********************************************
*   Global Variable Definitions
*********************************************/


timeclock t1;
timeclock *presentTime=&t1;
/**************************************************
*   Defining Prototypes
*********************************************/
void _sdcc_external_startup();
void UART_Initialization(void);
void INT0Initilization ();
void Init();

/********************************************
* HW Initialization Function
* All processor like XRAM enabled before the call to main()
*********************************************/

void _sdcc_external_startup()
{
                //AUXR = 00001100b;
    AUXR |= 0x0C;// Configure AUXR fr full access for XRAM
                 // XS1=1,XS0=0
}

/*********************** Initialization Functions ***************************************/

/********************************************
* UART Initialization
* 1) Timer 1 - Mode 2 (8 bit Auto Reload Mode)
* 2) Set Baud Rate according to BAUDRATE(by default)
*********************************************/
void UART_Initialization()
{
    //Disabled to Allow PAULMON to auto detect and configure BAUD Rate
    TMOD = TMODINI  ;// Initializing Timer 1
                    //Timer 1 - Mode 2 (8 bit Auto Reload Mode)
    TH1 = BAUD_RATE   ;//Initialize baud as per BAUDRATE
    SCON = SCONINI  ;//Initialize SCON for SMOD
    TR1 = 1      ;//Start Timer
    TI =  1      ;//Start Timer
    //printf_tiny("UARt Init");
}



/************************************************************************
ISR for External Interrupt Interrupt code
*************************************************************************/
void isr_ExtInterrupt0(void) __interrupt (0)
{
    //printf_tiny("\n\rInside a ISR for Interrupt\n\r");
    //BCDCounter();
    TCON&=0xFE;//Enable High to Low pulse trigger
    GLCD_RED=0;
    GLCD_RED=1;


}


/************************************************************************
*   Interrupt Initialization code(for PENIRQ connected from TSC2013)
*
*************************************************************************/
void INT0Initilization ()
{
    //IEN0 |= INTR_INT0;//Enable INTR0
    IEN0 |=0x81;//Enable INTR0
    TCON|=0x01;//Enable High to Low pulse trigger
}

/*******************************************************
Initialize Function calling different Initializing functions

********************************************************/
void Init()
{
    UART_Initialization();
    SPI_Init();//SPI Initialize for RFID and GLCD
    MFRC522Init();//Initialize of RFID MFC522(SPI mode)
    /***********Only done once for setting the time***********/
    //RTCDS1307Init();
    /**********************************************************/
    setup();//Initialize Setup of GLCD
    TSC2013_Init();//Initialize touch screen
    INT0Initilization();//Initialize INTR 0
}



/*******************************************************
Debugging Function to Check if correct Initializing is
Completed for RFID MFC522
--Not Used
********************************************************/
void RFIDisplay()
{
    unsigned char version = MFRC522ReadRegister(MFRC522_REG_VERSION);
    printf_tiny("\n\r New version value %x!!!\r\n",version);


    printf_tiny("\n\r Configured values!!!\r\n");
    printf_tiny("\n\r MFRC522_REG_T_MODE %x!!!\r\n",MFRC522ReadRegister(MFRC522_REG_T_MODE));
    printf_tiny("\n\r MFRC522_REG_T_PRESCALER %x!!!\r\n",MFRC522ReadRegister(MFRC522_REG_T_PRESCALER));
    printf_tiny("\n\r MFRC522_REG_T_RELOAD_L %x!!!\r\n",MFRC522ReadRegister(MFRC522_REG_T_RELOAD_L));
    printf_tiny("\n\r MFRC522_REG_T_RELOAD_H %x!!!\r\n",MFRC522ReadRegister(MFRC522_REG_T_RELOAD_H));
    printf_tiny("\n\r MFRC522_REG_RF_CFG %x!!!\r\n",MFRC522ReadRegister(MFRC522_REG_RF_CFG));
    printf_tiny("\n\r MFRC522_REG_TX_AUTO %x!!!\r\n",MFRC522ReadRegister(MFRC522_REG_TX_AUTO));
    printf_tiny("\n\r MFRC522_REG_MODE %x!!!\r\n",MFRC522ReadRegister(MFRC522_REG_MODE));
}





/**************************************************************
Function to print data in following format
            AAA: DD

Pre-Condition  - input fro user
                       a)EEPROM Address of Data
                       b)Hex Data
                       c)Row Address of LCD
Post-Condition - o/p is a display on LCD Screen

--Used for Debugging purposes as EEPROM storing data as DataBase
***************************************************************/

void HexDump()
{
    unsigned char *startAddr=NULL;
    unsigned char *endAddr=NULL;
    unsigned char *temp=NULL;
    unsigned char i=0;
    printf_tiny("\n\r Enter start address to be read from  :");
    startAddr = StringNumber(0);//Store input start address
    if (startAddr != NULL)
    {
        strcpy(temp,startAddr);
        printf_tiny("\n\r Enter end address to be read from  :");
        endAddr = StringNumber(0);//Store input  end address
        if (endAddr != NULL)
        {
            temp[MAXADDR_LENGTH]='\0';
            hexDisplay(temp,16,endAddr);
        }


    }

}

/******************************
        Main of the program
*******************************/

void main(void)
{
    TSCLocation TSL2;
    TSCLocation *Touchposition =&TSL2;
    unsigned char x=0,i=WAITTAGDETECT,j=0,choice=0;
    unsigned char newLocation=Touchposition->position=POS4;//Default location of Touch
    unsigned char tempaddr=0x40;
    unsigned char CardID[5];
    unsigned char tagDetect=0;//variable to check

    Init();//Initialize all peripherals
    GLCDPage1();//Display Page 1 for GLCD
    delay_seconds(1);//delay
    displayGLCDClock(RTCClockRead());//Displaying Clock on LCD
        while(1)
        {
            GLCDHomePage();//home page
            displayGLCDClock(RTCClockRead());//Displaying Clock on LCD
            TSC2013_Init();//Initialize touch screen
            newLocation=TSCLocationRead();
            TSCReset();//Reset Touch Screen for 0,0
            switch (newLocation)//according to touch
            {
                case POS3://Insert option
                        i=WAITTAGDETECT,tagDetect=0;
                        GLCDPage2();//Message  Display
                        delay_seconds(4);
                        while (i-- && !tagDetect)//wait till tag is detected
                        {
                            if (PCDCheck(CardID) == MI_OK) {//Read Tag
                                presentTime=RTCClockRead();//Display Clock
                                tagDetect++;//increment as tag detected
                                insertMedicine(CardID,presentTime);//Add a new record for medicine
                            }
                        }
                        if (!tagDetect)//If no card Detected
                        {
                            GLCDNoCard();//Message  Display
                        }
                        break;
                case POS1://Check option
                        i=WAITTAGDETECT,tagDetect=0;
                        GLCDPage2();//Message  Display
                        delay_seconds(4);
                        while(i-- && !tagDetect)//Wait for Tag
                        {
                            if (PCDCheck(CardID) == MI_OK)//Read Fpr Tag
                            {
                                tagDetect++;//Increment as Tag Detected
                                presentTime=RTCClockRead();//
                                checkMedicineToday(CardID,presentTime);//Check if Medicine present
                            }
                        }
                        if (!tagDetect)//If no card detected
                        {
                            GLCDNoCard();//Display Message
                        }

                        break;
                case 9:
                        HexDump();//Debug purposes , used for checking
                        break;
                case POS2://Record Tags

                        i=WAITTAGDETECT,tagDetect=0;
                        GLCDPage2();
                        delay_seconds(4);
                        while(i-- && !tagDetect)//Continous check till a limit
                        {
                            if (PCDCheck(CardID) == MI_OK)//Read Tag
                            {
                                tagDetect++;//Record as tag detected
                                presentTime=RTCClockRead();//Display Clock
                                updateMedicine(CardID,presentTime);//update Medicine details in EEPROM

                            }
                        }
                        if (!tagDetect)
                        {
                            GLCDNoCard();
                        }
                        break;
                case POS4:
                        GLCDHomePage();
                        break;
                case POS5:
                        GLCDHomePage();
                        break;
                default:
                        GLCDHomePage();
                        break;
            }

        }
}











