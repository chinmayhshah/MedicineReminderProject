/******************************************************************
code by :chinmay.shah@colorado.edu
library of functions designed for use for  EEPROM Functionality

Functions are as follows - have been described below
1) EEPROMWrite(unsigned char block, char addr,unsigned char databyte);
2) EEPROMRead(unsigned char block,unsigned char addr);
3) menuEPPROMDisplay();
4) Mode(unsigned char x);
5) EEPROMReset();
6) EEPROMBlockFill(unsigned char block, char startaddr,unsigned char datadiff,unsigned char filldata);
7) EEPROMPageWrite(unsigned char block, char startaddr,unsigned char datadiff,unsigned char filldata);


******************************************************************/


#include <at89c51ed2.h>
#include "I2C.h"
#include "eeprom.h"
#include "delay_custom.h"
#include "constraints.h"


/********************************************************************
* Function  To write to a particular Address of EEPROM
*Pre - Inputs include
        block - block value ,
        addr - address within block,
        databyte - data to be written
* Post - databyte is written at at block and address input
* I2C statements format
*START -- SLAVE ADDR -- WORD ADDR -- WRITE BYTE-- STOP-- ACK
*********************************************************************/
void EEPROMWrite(unsigned char block, char addr,unsigned char databyte)
{

    printf("\n\r address %02x",addr);
    printf("\n\r block %02x",block);
    //P1_7=0;
    //P1_7=1;
    I2CStart();
    I2CSend((WRCMD|(block<<1)));//For Write command
    I2CSend(addr);//For address of data
    I2CSend(databyte);//Write a particular data
    I2CStop();//Stop bit of I2C
    //P1_7=0;
}



/*************************************************************************************************
* Function  To read from a  particular Address of EEPROM
*Pre - Inputs include
        block - block value ,
        addr - address within block,

* Post - databyte is read  from a  block and address input and returned
* I2C statements format using random read
*START -- SLAVE ADDR(WR) -- WORD ADDR --REPEATED START -- SLAVE ADDR(RD)-- READ BYTE-- STOP-- ACK
***************************************************************************************************/

unsigned char EEPROMRead(unsigned char block,unsigned char addr)
{
    unsigned char temp;
    I2CStart(); //I2C start bit
    I2CSend((WRCMD|(block<<1)));//For Write command, pointing to a page specified by address
    I2CSend(addr);//For address of data
    I2CStart();
    I2CSend((RDCMD|(block<<1)));//For Read command
    temp=I2CRecieve();//Read from current address
    I2CStop();//Stop of I2C
    return temp;
}


/************************************************************************************

function for software Reset of EEPROM to standby mode
START - High 9bits -- START -- STOP
**************************************************************************************/
void EEPROMReset()
{


    unsigned char i ;


    I2CStart();//First Start

    for (i=0;i<9;i++)
    {
    SCL=0;// To restrict change in SDA
    SDA=1; //SDA<-1b
    delay_us();
    //SDA=0;  //SDA<-0b
    SCL=1;// To SDA to change
    }
    I2CStart();
    I2CStop();


}




/********************************************************************
* Function  To write to a particular Address of EEPROM
*Pre - Inputs include
        block - block value ,
        startaddr - start address within block,
        datadiff - total no(bytes)  , which is 16
        filldata to be written within page
* Post
        Fill a page of eeprom with filldata

* I2C statements format
*START -- SLAVE ADDR -- WORD ADDR -- WRITE BYTE(0)-- ACK --WRITE BYTE(1) --ACK
        ------ WRITE BYTE(datadiff-1)
* made critical to find exact timings
*********************************************************************/

unsigned char EEPROMPageWrite(unsigned char block, char startaddr,unsigned char datadiff,unsigned char filldata) __critical
{
    unsigned char i =0,j=0,tempack;
    P1_5 =0;
    P1_5 =1;
    //
    I2CStart();
    I2CSend((WRCMD|(block<<1)));//For Write command
    I2CSend(startaddr);//For address of data
    while (i <= datadiff)
    {
        I2CSend(filldata);//Write a particular data
        i++;
    }
    I2CStop();//Stop bit of I2C
    //Polling of Acknowledge
    I2CStart();
    tempack=I2CSend(WRCMD);//For Write command
    //printf_tiny("here %x",tempack);
    while(tempack)
      {
            I2CStart();
            tempack=I2CSend(WRCMD);//For Write command
     }
    //For Checking Timing
    P1_5 =0;
    //
    return 0;//return value if NACK (SDA = 0)
}


/********************************************************************
* Function  To write to a particular Address of EEPROM
*Pre - Inputs include
        block - block value ,
        startaddr - start address within block,
        datadiff - total no(bytes)  , which is 16
        filldata to be written within page
* Post
        Fill a page of eeprom with filldata

* calls EEPROMPageWrite

*********************************************************************/

void EEPROMBlockFill(unsigned char block, char startaddr,unsigned char datadiff,unsigned char filldata)
{
    unsigned char i =0,j=0,k=0,tempret=0;
    //For Checking Timing
    P1_6 =1;
    //
    while (i<datadiff)
    {
        tempret=EEPROMPageWrite(block,(startaddr+i),0x0F,filldata);
        if (!tempret)
        {
            i=i+0x0F;
        }

    }

    //For Checking Timing
    P1_6 =0;
    //
}


















/********************************************************************
* Function  To write to a particular Address of EEPROM
*Pre - Inputs include
        block - block value ,
        startaddr - start address within block,
        datadiff - total no(bytes)  , which is 16
        filldata to be written within page
* Post
        Fill a page of eeprom with array of Data

* I2C statements format
*START -- SLAVE ADDR -- WORD ADDR -- WRITE BYTE(0)-- ACK --WRITE BYTE(1) --ACK
        ------ WRITE BYTE(datadiff-1)
* made critical to find exact timings
*********************************************************************/

unsigned char EEPROMPageWriteArray(unsigned char block, char startaddr,unsigned char datadiff,unsigned char filldata[])
{
    unsigned char i =0,j=0,tempack;
    P1_5 =0;
    P1_5 =1;
    //
    I2CStart();
    I2CSend((WRCMD|(block<<1)));//For Write command
    I2CSend(startaddr);//For address of data
    while (i < datadiff)
    {
        I2CSend(filldata[i]);//Write a particular data
        i++;
    }
    I2CStop();//Stop bit of I2C
    //Polling of Acknowledge
    I2CStart();
    tempack=I2CSend(WRCMD);//For Write command
    //printf_tiny("here %x",tempack);
    while(tempack)
      {
            I2CStart();
            tempack=I2CSend(WRCMD);//For Write command
     }
    //For Checking Timing
    P1_5 =0;
    //
    return tempack;//return value if NACK (SDA = 0)
}
