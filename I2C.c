/******************************************************************
code by :chinmay.shah@colorado.edu
library of functions designed for use for I2C Bit banging

Functions are as follows - have been described below
1) void I2CStart();
2) void I2CStop();
3) unsigned char I2CRecieve (void);
4) unsigned char I2CSend (unsigned char data);

Reference:
    1) http://blog.siliconlabs.com/t5/Official-Blog-of-Silicon-Labs/Chapter-10-2-Control-an-accelerometer-over-I2C-Part-2-Initialize/ba-p/164580

    Leveraged from LAB4 Author chinmay.shah@colorado.edu
******************************************************************/


#include <at89c51ed2.h>
#include "I2C.h"
#include "delay_custom.h"



/********************************************************************
* Library functions to use create bit banging code for I2C protocol
*
*********************************************************************/


/********************************************************************
* Functions such to create a Start for I2C protocol
*Conditions - SCL =1
*             SDA = Transition from 1 to 0
*********************************************************************/
void I2CStart()
{
    SCL=0;// To SDA to change
    SCL=1;// To restrict change in SDA
    SDA=1;
    SDA=0;

}

/********************************************************************
* Functions such to create a Stop for I2C protocol
*Conditions - SCL =1
*             SDA =Transition from 0 to 1
*********************************************************************/

void I2CStop()
{
    SCL=0;
    SDA=0;
    SCL=1;
    SDA=1;

}


/********************************************************************
* Functions such to Send a byte  to using I2C protocol
* Data  send from Master to Slave(LSB to MSB)
*********************************************************************/

unsigned char  I2CSend (unsigned char I2Cdata)
{
    unsigned char i ;
    unsigned char temptx=0;
    //temptx=I2Cdata;
    for (i=0;i<8;i++)
    {
        //  delay_us();//NOt  Transmission of data
        SCL=0;// To restrict change in SDA
        temptx = I2Cdata & 0x80; //Mask for checking MSB of Data
        if (temptx)//if temptx >0
        {
            SDA=1; //SDA<-1b
        }
        else
        {
            SDA=0;  //SDA<-0b
        }
        I2Cdata = I2Cdata << 1;//Shift data by one bit
        SCL=1;// To SDA to change

    }

    SCL=0;// To SDA to change
    //delay_us();
    SDA=1;
    //delay_us();
    temptx= SDA;//Store SDA value at this time for checking
    //delay_us();
    SCL=1;// To restrict change in SDA

    delay_us();
    return temptx;
}



/********************************************************************
* Functions such to Send a byte  to using I2C protocol
* Data  send from Slave to Master bit by bit (LSB to MSB )
*********************************************************************/

unsigned char I2CRecieve (void)
{
    unsigned char i ;
    unsigned char temprx = 0;
    SDA=1;// To ensure it can pulled down
    for (i=0;i<9;i++)
    {
        delay_us();
        SCL=1;//Pull SCL high
        delay_us();//NOt Allow Transmission of data
        //capture value of SDA in buffer
        temprx = temprx << 1;//Shit bit rx in last iteration
        if (SDA)//if SDA >0
        {
            temprx = temprx | 0x01;//Store one on LSB
        }
        else
        {
            temprx = temprx & 0xFE;//Store zero on LSB
        }
        SCL=0;// To SDA to change
    }
    return(temprx);

}


