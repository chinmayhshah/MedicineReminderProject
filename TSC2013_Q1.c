/*****************************************************************************************************************
Functions to write on GLCD ST7565
Author @chinmay.shah@colorado.edu
Reference DataSheet :http://www.ti.com/lit/ds/symlink/tsc2013-q1.pdf


Complete code author by :chinmay.shah@colorado.edu
*****************************************************************************************************************/

#include "TSC2013_Q1.h"
#include <at89c51ed2.h>
#include "I2C.h"
#include "Serial.h"
#include "stdio.h"
#include "delay_custom.h"


/********************************************************************************
Initializing sequence for TSC2013_Q1 and Start Conversion for TSC2013
********************************************************************************/
//Write Cycle
void TSC2013_Init()
{


    //Initializing Config Reg 0
    TSCWrite(TSC2013_CONTROL_BYTE0|(TSC2013_CONFIG_REG0) ,0x81,0x24);//Write on TSC Control register 0                                                                 //ADC - Normal Operation
                                                                    //RM - 10 bit resolution
                                                                    //CL1=0 ,CL0 = 0 4-Mhz ADC
    TSCWrite(TSC2013_CONTROL_BYTE0|(TSC2013_CONFIG_REG1),0x00,0x00);
    //TSCWrite(TSC2013_CONTROL_BYTE0|(TSC2013_CONFIG_REG2),0x00,0x00);
    TSCWrite(TSC2013_CONTROL_BYTE0|(TSC2013_CONFIG_REG2),0x80,0x00);
    TSCStartConversion();
    //TSCRegRead ();
    //Initializing Config Reg 1

}


/********************************************************************************
Read all registers of Touch Controller
********************************************************************************/


void TSCRegRead ()

{
    unsigned char retdata;
    retdata=TSCRead(TSC2013_X1_REG);
    retdata=TSCRead(TSC2013_X2_REG);
    retdata=TSCRead(TSC2013_Y1_REG);
    retdata=TSCRead(TSC2013_Y2_REG);
    retdata=TSCRead(TSC2013_Z1_REG);
    retdata=TSCRead(TSC2013_Z2_REG);
    retdata=TSCRead(TSC2013_IX_REG);
    retdata=TSCRead(TSC2013_IY_REG);
    retdata=TSCRead(TSC2013_CONFIG_REG0);
    retdata=TSCRead(TSC2013_CONFIG_REG1);
    retdata=TSCRead(TSC2013_CONFIG_REG2);
}




/********************************************************************************
Read all registers position from All registers
********************************************************************************/


touchPosition TSCLocationRead ()

{

//    unsigned  char positionOnTouch;


    TSCLocation TSCL1;
    TSCLocation *TLocation=&TSCL1;
    TLocation->Status=TSCRead(TSC2013_STATUS_REG);
    //printf("Status1:%02x\n\r",TLocation->Status);
    //Reading values from Registers of Touch Screen
    TLocation->TX1=TSCRead(TSC2013_X1_REG);
    TLocation->TX2=TSCRead(TSC2013_X2_REG);
    TLocation->TY1=TSCRead(TSC2013_Y1_REG);
    TLocation->TY2=TSCRead(TSC2013_Y2_REG);
    TLocation->TZ1=TSCRead(TSC2013_Z1_REG);
    TLocation->TZ2=TSCRead(TSC2013_Z2_REG);
    TLocation->TIX=TSCRead(TSC2013_IX_REG);
    TLocation->TIY=TSCRead(TSC2013_IY_REG);

    TLocation->AUX=TSCRead(TSC2013_AUX_REG);
    TLocation->Status=TSCRead(TSC2013_STATUS_REG);


    //Select the poistion according to touch on screen
    //each as a position defined
    if ( TLocation->TX1 ==0x02 || TLocation->TX2 ==0x03 || TLocation->TX2 ==0x02 || TLocation->TX2 ==0x03)
    {
        if (TLocation->TY1 ==0x01 ||  TLocation->TY2 ==0x01 ||TLocation->TY1 ==0x00 ||  TLocation->TY2 ==0x00)//Check if within (3,0) to (2,0) or (3,1) to (2,1)
            TLocation->position=POS1;
        else if (TLocation->TY1 == 0x02 || TLocation->TY2 ==0x02 )//Check if within (3,0) to (2,0) or (3,1) to (2,1)
            TLocation->position=POS3;
    }
    else if ( TLocation->TX1 ==0x00 || TLocation->TX1 ==0x01 || TLocation->TX2 ==0x00 || TLocation->TX2 ==0x01)
    {
        if (TLocation->TY1 ==0x01 || TLocation->TY2 ==0x01 )//Check if within (3,0) to (2,0) or (3,1) to (2,1)
            TLocation->position=POS2;
        else if (TLocation->TY1 ==0x02 || TLocation->TY2 ==0x02 )//Check if within (3,0) to (2,0) or (3,1) to (2,1)
            TLocation->position=POS4;
    }
    if ((TLocation->TX1==0 && TLocation->TY1==0) || ( TLocation->TX2==0 && TLocation->TY2==0))
    {
        TLocation->position=POS5;
    }

    return (TLocation->position);//If all above fail ,default choice


}















/********************************************************************
* Function  To write to a particular Address of TSC
*Pre - Inputs include addr and dataLSB and DataMSB
**********************************************************************/
void TSCWrite(unsigned char addr,unsigned char databyteMSB,unsigned char databyteLSB)
{

    I2CStart();
    I2CSend(TSC2013_SLAVE_ADDRESS  & 0xFE);//For Write command
    //printf("send addr %02",addr);
    I2CSend(addr);//For address of data
    I2CSend(databyteMSB);//Write a register value MSB
    I2CSend(databyteLSB);//Write a register value LSB
    I2CStop();//Stop bit of I2C

}



/********************************************************************
* START Conversion by writing into Control byte 1
*********************************************************************/
void TSCStartConversion()
{

    I2CStart();
    I2CSend(TSC2013_SLAVE_ADDRESS  & 0xFE);//For Write command
    I2CSend(TSC2013_CONTROL_BYTE1);//For address of data
    //I2CSend(databyteMSB);//Write a register value MSB
    //I2CSend(databyteLSB);//Write a register value LSB
    I2CStop();//Stop bit of I2C

}


/********************************************************************
* Function  To read from  a particular Address of TSC
*Pre - Inputs include addr and return only MSB
    as within code only MSB being used
**********************************************************************/
unsigned char TSCRead(unsigned char addr)
{
    unsigned char dataRxMSB,dataRxLSB;
    I2CStart();
    I2CSend(TSC2013_SLAVE_ADDRESS  & 0xFE);//For Write command
    I2CSend(TSC2013_CONTROL_BYTE0|addr|0x01);//For address of data
    //I2CSend(databyteMSB);//Write a register value MSB
    //I2CSend(databyteLSB);//Write a register value LSB
    I2CStop();//Stop bit of I2C

    I2CStart();
    I2CSend(TSC2013_SLAVE_ADDRESS  | 0x01);//For Write command
    dataRxMSB=I2CRecieve();
    dataRxLSB=I2CRecieve();
    I2CStop();//Stop bit of I2C
    //printf("%02x\n\r",dataRxMSB);
    //printf("%02x\b\r",dataRxLSB);
    return dataRxMSB;
}


/********************************************************************
* Function  To hardware reset the system

**********************************************************************/

void TSCReset()
{
    P3_4=0;
    delay_ms(20);
    P3_4=1;
}


