/*************************************************************
Start Date : 3/20/2016
Version 1.4
code by :chinmay.shah@colorado.edu
Code : This Files provides option for different Menus for
        1) Main Menu
        2) Timer Menu
        3) I/O Expander Menu



*********************************************************************/

#include "I2C.h"
#include "eeprom.h"
#include "menu.h"
#include "Serial.h"
#include "constraints.h"
#include "LCD.h"
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include "ioexpander.h"
#include <at89c51ed2.h>
#include "timers.h"




/********************************************
*   SFR Definition
*********************************************/
__sfr __at (0xA8) IEN0 ;


/************************************************************************
Initialization code
*************************************************************************/
void INT0Initilization ()
{
    //IEN0 |= INTR_INT0;//Enable INTR0
    IEN0 |=0x81;//Enable INTR0
    TCON|=0x01;//Enable High to Low pulse trigger
    //printf("\n\rINt0 intialized\n\r");
}






/********************************************************************************
Description :Function to take input from user
    Pre : Input from user for a three digit Hex.
    Post : Store Byte into address of EEPROM specified by user.

    Details:Valid address range from 0x000-0x7FF(2KByte - 2048 - 16Kbits)
            Split 0xBAA
                 B - Used for selecting B of byte(by default zero block selected)
                    - Min value - 0
                    - Maximum value - 7
                 AA - Address location of byte in EEPROM
                    - Min value - 0x00
                    - Max value - 0xFF


***********************************************************************************/
unsigned char WriteByte()
{
    unsigned char *Number=NULL;
    unsigned int  hexAddr;
    unsigned int  hexData;
    unsigned char block;
    printf_tiny("\n\r Enter address to be written :");
    Number = StringNumber(0);//Store input as array of ASCII Characters
    if (Number)
    {
        block = Number[0];//Store block temporarily
        hexAddr = ASCIItoNumber(Number,0);//Convert AsCII to Hex for address
        if (hexAddr > 0xFF)//Check if Address is out of bound
        {
            return 0 ;
        }
        printf_tiny("\n\r Enter data to be written :");
        Number = StringNumber(1);//Store input as array of ASCII Characters
        if (Number)
        {
            hexData = ASCIItoNumber(Number,1);// convert ASCII into Hex value for data
            if (hexData > 0xFF)//Check if Address is out of bound
            {
                return 0 ;
            }
            EEPROMWrite((block&0x0F),hexAddr,hexData);//Call EEprom Write
        }

    }
                                                 //passing HEX value of block ,hex Address of data

   return 1;
}

unsigned char ReadByte()
{
    unsigned char *Number=NULL;
    printf_tiny("\n\r Enter address to be read from  :");
    Number = StringNumber(0);//Store input address
    hexDisplay(Number,1,Number);
    return 1;
}



void symbol_Character () __critical
{

	unsigned char customarray1[8]={0x01,0x03,0x07,0x0F,0x1F,0x1F,0x1F};
    unsigned char customarray2[8]={0x10,0x18,0x1C,0x1E,0x1F,0x1F,0x1F};
    unsigned char customarray3[8]={0x1F,0x1F,0x1F,0x0F,0x07,0x03,0x01};
    unsigned char customarray4[8]={0x1F,0x1F,0x1F,0x1E,0x1C,0x18,0x10};


    LCDCreateChar(1,customarray1);//Create custom character
    LCDCreateChar(2,customarray2);//Create custom character
    LCDCreateChar(3,customarray3);//Create custom character
    LCDCreateChar(4,customarray4);//Create custom character

    LCDGotoxy(0,0);//Place Character on LCD at x position
    LCDPutchar(1);//Display Character on LCD
    LCDGotoxy(0,1);//Place Character on LCD at x position
    LCDPutchar(2);//Display Character on LCD
    LCDGotoxy(1,0);//Place Character on LCD at x position
    LCDPutchar(3);//Display Character on LCD
    LCDGotoxy(1,1);//Place Character on LCD at x position
    LCDPutchar(4);//Display Character on LCD
    LCDGotoxy(2,0);//Place Character on LCD at x position
    LCDPutchar(1);//Display Character on LCD
    LCDGotoxy(2,1);//Place Character on LCD at x position
    LCDPutchar(2);//Display Character on LCD



}


/**************************************************************
Function to print data in following format
            AAA: DD

Pre-Condition  - input fro user
                       a)EEPROM Address of Data
                       b)Hex Data
                       c)Row Address of LCD
Post-Condition - o/p is a display on LCD Screen
------------------------------------------
|(y,0)AAA: DD                            |
|                                        |
|                                        |
|                                        |
------------------------------------------
where
    AAA- EEPROM Address of Data
    DD - Data Fetched from EEPROM Address of Data
    y   - Row number input by user


***************************************************************/
void LCDDisplay() __critical
{
    unsigned char *Number=NULL;
    //unsigned char Displaydata[3]={'\0','\0','\0'};
    unsigned char row,valid;//bytedata;
    //unsigned int hexAddr;
//    avoidWDTReset();
    printf_tiny("\n\r Enter address to be read from on LCD :");
    Number = StringNumber(0);//Store input address
//    avoidWDTReset();
    printf_tiny("\n\r Enter row of LCD(0,1,2,3):");
    row=getchar();//Store Input
    putchar(row);//Display data input
    printf_tiny("\n\r");
    valid=checkInputCharacter(row);//Check Validity of Input
//    avoidWDTReset();
    if (!valid)
        {
            row = ((row &0x0F) % 0x04);//convert to hexadecimal and 0-4
            displayLCD(Number,row);//Display on LCD
        }
    else
        {
            printf_tiny("\n\r Incorrect row selected .Please try again!!!");
        }

}


/**************************************************************
Function to clear LCD Screen
------------------------------------------
|                                        |
|                                        |
|                                        |
|                                        |
------------------------------------------
***************************************************************/
void ClearDisplay()
{
    LCDClear();
}

/**************************************************************
Function to print data in following format
            AAA: DD

Pre-Condition  - input fro user
                       a)EEPROM Address of Data
                       b)Hex Data
                       c)Row Address of LCD
Post-Condition - o/p is a display on LCD Screen


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
//            avoidWDTReset();
        }


    }

}


/**************************************************************
Function to print data in following format for DDRAM
Post-Condition - o/p is a display on LCD Screen
***************************************************************/

void DDRAMDump() __critical
{
    unsigned char bytedata,i=0,j=0,tempaddr=LCD_LINE0|LCD_DB7;
    printf_tiny("\n\r-------------7--- DDRAM Dump Display----------------- \n\r");
    //LCDCommandWrite(tempaddr); //Passing address of DDRAM
    //while ( i< (LCD_MAXCOL+1) * (LCD_MAXROW+1) )
    while ( i< 64 )
    {
        //if (i%20==0)
        if (i%16==0)
       {

            switch(j)
            {
                case 0:
                        tempaddr=LCD_LINE0|LCD_DB7;//Line 0
                        break;
                case 1:
                        tempaddr=LCD_LINE1|LCD_DB7;//Line 1
                        break;
                case 2:
                        tempaddr=LCD_LINE2|LCD_DB7;//Line 2
                        break;
                case 3:
                        tempaddr=LCD_LINE3|LCD_DB7;//Line 3
                        break;
                default:
                        tempaddr=LCD_LINE0|LCD_DB7;
                        break;

            }
             printf("\n\r %02x :",(tempaddr&0x7F));//Print Address for each line
             LCDCommandWrite(tempaddr); //Passing line address of DDRAM
                                        //Address is incremented internally

             j++; //Keep account of LCD line number
       }

        bytedata=LCDDataRead();//Read data from DDRAM
        printf_tiny("%x  ",bytedata);
        i++;//Increment i as each character  printed
    }


}


/**************************************************************
Function to print data in following format
            AAA: DD

Pre-Condition  - input fro user
                       a)EEPROM Address of Data
                       b)Hex Data
                       c)Row Address of LCD
Post-Condition - o/p is a display on LCD Screen


***************************************************************/

void CGRAMDump()    __critical
{
    unsigned char bytedata,i=0,j=0,tempaddr=0x00|0x40;//TO access CGRAM 0100 0000
    //LCDCommandWrite(tempaddr); //Passing address of DDRAM
                               // Increments by itself
    printf_tiny("\n\r---------------- CGRAM Dump Display----------------- \n\r");
    while ( i< 64 )
    {

        if (i%8==0)
       {
             //tempaddr = ((tempaddr & ((j++)<<3))|0x40);//Increment the address for each row                                      //1011 111   1
             printf("\n\r %02x :",((tempaddr)&0xBF));//Print Address for each character
//             avoidWDTReset();
             //DEBUG
             //printf("address %x",tempaddr);
             //
       }
        LCDCommandWrite(tempaddr); //Passing address of DDRAM
        bytedata=LCDDataRead();//Read data from DDRAM
        printf("%02x  ",bytedata);
        i++;//Increment i as each character  printed
        tempaddr++;//increment CGRAM address
    }
}
/************************************************************************
Function to allow user to create a custom character
i/p -
o/p - Custom character stored at row , with input pixels provided by user


------------------------------------------------------------------------
for ex:CU boulder logo
    _________
    |
    |   |      |
    |   |      |
    |________  |
        |      |
        |______|

        all hex i/p from user
        0x9E 0x92 0x90 0x95 0x95 0x9F 0x85 0x87(using cursor location)


        for Rupee Symbol(Indian Currency /with cursor )
            9F 82 9F 84 98 84 82 00

        for Bell (Alarm Symbol)
            0x04 0x0E 0x0E 0x0E 0x1F 0x00 0x04 0x00


*************************************************************************/


void customCharacter()
{
    unsigned char x,y,tempret ,i=0;
    unsigned char *customstr;

    unsigned char customarray[8];


    printf_tiny("\n\r Enter character code to be created/updated \n\r");
//    avoidWDTReset();
    x=getchar();//store input value
    putchar(x);//echo character
    avoidWDTReset();
    tempret=checkInputCharacter(x);
    if (!tempret & (x<'8'))
    {
        printf_tiny("\n\r Enter Location on LCD \n\r");
        avoidWDTReset();
        y=getchar();//store input value
        putchar(y);//echo character
        tempret=checkInputCharacter(y);
        avoidWDTReset();
        if (!tempret & (y<='3'))
        {
            printf_tiny("\n\r Enter eight hex code for customer characters  \n\r");
            avoidWDTReset();
            while (i<8)
            {
                customstr = StringNumber(1);//store in an array input values
                if (customstr !=NULL)
                {
                    avoidWDTReset();
                    customarray[i++]=ASCIItoNumber(customstr,1);//store Pixel for customer character
                    avoidWDTReset();
                    x=x&0x0F;
                    EA=0;
                    LCDCreateChar(x,customarray);//Create custom character
                    //LCDGotoxy(y&0x0F,0);//Place Character on LCD at x position
                    LCDGotoxy(x,0);//Place Character on LCD at x position
                    LCDPutchar(x);//Display Character on LCD
                    EA=1;
                }
                else
                {
                    printf_tiny("\n\r Please enter correct hex value . \n\r");
                }

            }



        }
        else
        {
            printf_tiny("\n\r Please enter a number from 0-3 for LCD location  \n\r");
        }
    }
    else
    {
        printf_tiny("\n\r Please enter a number from 0-7 for custom code  \n\r");
    }
}

/**************************************************************
Function to provide menu for  Intrrupt from IO expander
1)to display BCD on LCD and covert

***************************************************************/


void BCDCounter()
{
    unsigned char *str;
    LCDGotoxy(0,0);

    //printf("%x",tempcount);

    if (tempcount >= 9)
        {
            tempcount =0;
        }

    str= hextoASCII (tempcount);
    //LCDPutstr(str);
    LCDPutchar(tempcount|0x30);
    IOWrite((tempcount << 4)|0x08);
    tempcount ++;
}



void resetCounter()
{
    //unsigned char *str;
    tempcount=0x00;
    LCDGotoxy(0,0);
    LCDPutchar(0x30);
}
/**************************************************************
Function to provide menu for  I/O expander and use I2C for
communication

***************************************************************/

void ioExpander()
{
    unsigned char temp=0x00 ,x=0,checkx,tempbit,iomenu=0;

    //Initialize Interrupts
    INT0Initilization();


    //Calling function to write and read from IO Expander
    //IOWrite(0x00);//Dummy value as of now
    //temp = IORead();
    //printf("\n\r data read from IO expander %02x",temp);
    IOMenu();// Display Menu
    while (!iomenu)
    {
        //printf_tiny("\n\r************      Please Enter your Choice !!!!!       **********\r\n");
        x=getchar();//Store input number
        putchar(x);
        printf_tiny("\n\r");
        checkx=checkInputCharacter(x);//Check validity of Input
        if (checkx)//check for character
        {

            switch (x)
            {
                case 'i':

                        x=checkx=0x00;
                        printf_tiny("\n\r Enter Pin to be set as Input(0-7) ");
                        x=getchar();
                        putchar(x);

                        checkx=checkInputCharacter(x);
                        if (!checkx )//check for number and limit of 7
                        {
                            x&=0x0F;
                            if( x < IOEXP_MAXDATA)
                            {
                                IOSetInput(x);
                            }
                            else
                            {
                                printf_tiny("\n\r Incorrect input or specified bit exceeds 7  !!!!");
                            }

                        }
                        else
                        {
                            printf_tiny("\n\rIncorrect input or specified bit exceeds 7  !!!!");
                            //printf("x value %02x",x);
                        }
                        break;


                case 'o':

                        x=checkx=0x00;
                        printf_tiny("\n\r Enter Pin to be set as Output(0-7) ");
                        x=getchar();
                        putchar(x);

                        checkx=checkInputCharacter(x);
                        if (!checkx )//check for number and limit of 7
                        {
                            x&=0x0F;
                            if( x < IOEXP_MAXDATA)
                            {
                                IOSetOutput(x);
                            }
                            else
                            {
                                printf_tiny("\n\rIncorrect input or specified bit exceeds 7  !!!!");
                            }

                        }
                        else
                        {
                            printf_tiny("\n\rIncorrect input or specified bit exceeds 7  !!!!");
                            //printf("x value %02x",x);
                        }
                        break;
                case 's':

                        x=checkx=0x00;
                        printf_tiny("\n\r Enter Pin to be check status(0-7) ");
                        x=getchar();
                        putchar(x);

                        checkx=checkInputCharacter(x);
                        if (!checkx )//check for number and limit of 7
                        {
                            x&=0x0F;
                            if( x < IOEXP_MAXDATA)
                            {
                                tempbit=IOReadBit(x);
                                //BCDCounter();//Remove once INTR works fine s
                                printf_tiny("\n\rStatus of pin/bit  %d",tempbit);
                            }
                            else
                            {
                                printf_tiny("\n\rIncorrect input or specified bit exceeds 7  !!!!");
                            }

                        }
                        else
                        {
                            printf_tiny("\n\rIncorrect input or specified bit exceeds 7  !!!!");
                            //printf("x value %02x",x);
                        }
                        break;

                case 'r':
                        resetCounter();//Reset Counter to zero
                        break;
                case 'x':
                        iomenu++;
                        break;
                case '?':IOMenu();// Display Menu
                        break;
                default:
                        printf_tiny("\n\r Incorrect choice .Please try again  !!!!");

            }

        }
        else
        {
            printf_tiny("Incorrect choice .Please try again later !!!!");
        }
    }


}












void block()
{
    unsigned char *startAddr=NULL;
    unsigned char *endAddr=NULL;
    unsigned char *temp=NULL,*bytedata;
    unsigned char i=0,hexdata=0;

    printf_tiny("\n\r Enter start address to be written from   :");
    startAddr = StringNumber(0);//Store input start address
    if (startAddr != NULL)
    {
        strcpy(temp,startAddr);
        printf_tiny("\n\r Enter end address to be written  to  :");
        endAddr = StringNumber(0);//Store input  end address
        if (endAddr != NULL)
        {
            printf_tiny("\n\r Enter data to be written \n\r");
            bytedata=StringNumber(1);
            if (bytedata !=NULL)
            {
                hexdata=ASCIItoNumber(bytedata,1);
                if (hexdata <= 0xFF )
                {
                    temp[MAXADDR_LENGTH]='\0';
                    blockWrite(temp,endAddr,hexdata);
                    avoidWDTReset();
                }
                else
                {
                    printf_tiny("\n\r Data Exceeds FF \n\r");
                }

            }


        }


    }

}




/********************************************************************
*Block Fill of EEPROM
*
*********************************************************************/
unsigned char blockWrite (unsigned char startAddr[],unsigned char endAddr[],unsigned char hexdata)
{
    unsigned char byte=0,i=0,j=0,k=0,l=0,blockdiff;
    unsigned int starthexAddr=0;
    unsigned int endhexAddr=0;
    unsigned int diffaddr=0,tempdiff=0,sameblock=0;
    //Printing address and data in AAA:DD format

    starthexAddr = ASCIItoNumber(startAddr,0);//calculating Start Address

    if (starthexAddr >0xFF )//Check if Hex address was out of bound of memory available
    {
        return 0;
    }
    else
    {
        endhexAddr = ASCIItoNumber(endAddr,0);//calculating End Address
        if (endhexAddr > 0xFF)
        {
            return 0;
        }
    }
    if (endAddr[0] >= startAddr[0])//Check for different blocks
    {
        blockdiff = (int)(endAddr[0] -startAddr[0]);//calculate block difference
        if (!blockdiff)
            {
                if (endhexAddr >= starthexAddr)
                {
                        diffaddr = endhexAddr - starthexAddr;//Total number of data to be displayed within a block

                }
                else
                {
                        printf_tiny("\n\r End Address less than Start Address .Please try again !!!");
                        return 0;
                }

            }
            else
            {
                diffaddr = endhexAddr;
            }
            //printf("\n\r Adrr diff %d\n\r",diffaddr);
    }
    else
    {
        printf_tiny("\n\r End Address less than Start Address .Please try again !!!");
        return 0;
    }


    EEPROMBlockFill((startAddr[0]&0x0F),starthexAddr,starthexAddr-endhexAddr,hexdata);
    return 1;
}

/********************************************************************
*Menu Display for EPPROM Functionality
*
*********************************************************************/


/*******************************************************
Menu Display for
********************************************************/

void menuEPPROMDisplay()
{

    printf_tiny("\r MENU LIST\r\n");
    printf_tiny("\n\r1) -Write Byte \r\n");
    printf_tiny("\n\r2) -Read Byte  \r\n");
    printf_tiny("\n\r3) -LCD Display\r\n");
    printf_tiny("\n\r4) -Clear LCD  \r\n");
    printf_tiny("\n\r5) -Hex Dump   \r\n");
    printf_tiny("\n\r6) -DDRAM Dump \r\n");
    printf_tiny("\n\r7) -CGRAM Dump \r\n");
    printf_tiny("\n\r8) -LCD Option \r\n");
    printf_tiny("\n\r9) -Custom Character \r\n");
    printf_tiny("\n\ri) -I/O Expander \r\n");
    printf_tiny("\n\re) -EEPROM Reset \r\n");
    printf_tiny("\n\rb) -Block Fill(incomp) \r\n");
    printf_tiny("\n\rc) -Clock Mode \r\n");
    printf_tiny("\n\rp) -Page Write \r\n");
    printf_tiny("\n\rs) -Byte Write \r\n");
    printf_tiny("\n\r?) -Menu Options \r\n");
    printf_tiny("\n\rPlease Enter your Choice !!!!!\r\n");

}





/*********************************************************************
        TO Select Between different modes of required elements
***********************************************************************/
void Mode(unsigned char x)
{
            unsigned char tempack=0;
            //unsigned char ccode =0x2;
            //unsigned char rowdata[]={0x1B,0x1B,0x00,0x04,0x04,0x11,0x0E,0x00};

            //x &= 0x0F;//Convert ASCII into Number
            #ifdef DEBUGALL
            printf("To check mode %d\n\r",x);
            #endif // DEBUGALL
            switch(x)
            {
                case '1':
                        WriteByte();
                        break;
                case '2':
                        ReadByte();
                        break;
                case '3':
                        LCDDisplay();
                        break;
                case '4':
                        ClearDisplay();
                        break;
                case '5':
                        HexDump();
                        break;
                case '6':
                        DDRAMDump();
                        break;

                case '7':
                        CGRAMDump();
                        break;
                case '8':
                        LCDMenu();
                        break;
                case '9':
                        customCharacter();
                        break;
                case 'i':
                        ioExpander();
                        break;
                case 'e':
                        EEPROMReset();
                        break;
                case 'c':
                        timerMode();
                        break;
                case 'b':
                        block();
                        break;
                case 'l':
                        EEPROMBlockFill(0x05,0x00,0xFF,0x33);
                        break;
                case 'p':
                        EEPROMPageWrite(0x02,0x00,0x0F,0x32);
                        break;
                case 's':
                        P1_7=0;
                        P1_7=1;
                        EEPROMWrite(0x02,0x00,0xF0);
                        I2CStart();
                        tempack=I2CSend(WRCMD);//For Write command
                        //printf_tiny("here %x",tempack);
                        while(tempack)
                          {
                                I2CStart();
                                tempack=I2CSend(WRCMD);//For Write command
                         }
                         P1_7=0;

                        break;
                case '?':menuEPPROMDisplay();// Display Menu
                        break;
                default:
                        printf_tiny("Invalid Choice !!!Please try again\n\r");
                        break;

            }

}

/**********************************************************************************
Menu Options for Timer
***********************************************************************************/

void timerMode ()
{
    unsigned char temp=0x00 ,x=0,checkx,timemenu=0;
    unsigned char *tempmin,*tempsec,*tempmsec;
    unsigned char minTemp=0,secTemp=0,msecTemp=0;

    menuTimer();// Display Menu
    while (!timemenu)
    {
        avoidWDTReset();
        //printf_tiny("\n\r************      Please Enter your Choice !!!!!       **********\r\n");
        x=getchar();//Store input number
        putchar(x);
        printf_tiny("\n\r");
        checkx=checkInputCharacter(x);//Check validity of Input
        if (checkx)//check for character
        {

            switch (x)
            {
                case 'z':
                        timerReset();
                        break;
                case 'p':
                        resumeTimer();
                        break;
                case 'q':
                        stopTimer();
                        break;
                case 's':
                        restartTimer();//Reset Counter to zero
                        break;
                case 'a':
                        x=checkx=0x00;
                        printf_tiny("\n\r Choose Alarm to set (1,2,3) \n\r");
                        x=getchar();
                        putchar(x);
                        checkx=checkInputCharacter(x);
                        if (!checkx && (x&0x0F)<=MAX_ALARM && (x&0x0F) !=0)//check if input between 1-3
                        {
                            printf_tiny("\n\r Enter Minutes \n\r");
                            tempmin=StringNumber(2);
                            if (tempmin !=NULL)
                            {
                                minTemp=ASCIItoNumber(tempmin,1);
                                if (minTemp >=0x59)//Hex value for check of 59
                                {
                                    printf_tiny("\n\r Minutes exceeds limit of 59 \n\r");
                                    break;
                                }
                                else
                                {
                                    printf_tiny("\n\r Enter seconds \n\r");
                                    tempsec=StringNumber(2);
                                    if (tempsec !=NULL)
                                    {
                                        secTemp=ASCIItoNumber(tempsec,1);
                                         if (secTemp >=0x59)//min value of 59
                                        {
                                            printf_tiny("\n\r Seconds exceeds limit of 59 \n\r");
                                            break;
                                        }
                                        else
                                        {   printf_tiny("\n\r Enter milli seconds \n\r");
                                            msecTemp=getchar();
                                            putchar(msecTemp);
                                            if ((msecTemp &0x0F) > 9)
                                            {
                                                printf_tiny("\n\r Milli Seconds exceeds limit 9 \n\r");
                                                break;
                                            }
                                            else
                                            {
                                                editAlarm(((x&0x0F)-1),minTemp,secTemp,msecTemp&0x0F);
                                            }

                                        }
                                    }

                                }


                            }
                            else
                            {
                                break;
                            }


                        }
                        else
                        {
                            printf_tiny("\n\r Incorrect choice of Alarm\n\r");
                            break;
                        }

                        break;
                case 'e':
                        x=checkx=0x00;
                        printf_tiny("\n\r Choose Alarm to enable (1,2,3) \n\r");
                        x=getchar();
                        putchar(x);
                        checkx=checkInputCharacter(x);
                        if (!checkx && (x&0x0F) <=MAX_ALARM && (x&0x0F) !=0)//check if input between 1-3
                        {
                            enableAlarm((x&0x0F)-1);
                        }
                        else
                        {
                            printf_tiny("\n\r Incorrect choice of Alarm\n\r");
                            break;
                        }
                        break;
                case 'd':
                        x=checkx=0x00;
                        printf_tiny("\n\r Choose Alarm to enable (1,2,3) \n\r");
                        x=getchar();
                        putchar(x);
                        checkx=checkInputCharacter(x);
                        if (!checkx && (x&0x0F) <=MAX_ALARM &&  (x&0x0F) !=0 )//check if input between 1-3
                        {
                            disableAlarm((x&0x0F)-1);
                        }
                        else
                        {
                            printf_tiny("\n\r Incorrect choice of Alarm\n\r");
                            break;
                        }
                        break;
                case '?':menuTimer();// Display Menu
                        break;
                case 'x':timemenu++;// Display Menu
                        break;
                case 'c':statusAlarm();// Display Menu
                        break;
                case 't':stopAlarm();
                        break;
                case 'i':
                            EA=0;
                            while (1)
                            {
                                printf_tiny("\n\r Infinite LOOP\n\r");
                            }
                            EA=1;
                        break;
                default:
                        printf_tiny("\n\r Incorrect choice .Please try again  !!!!");

            }

        }
        else
        {
            printf_tiny("Incorrect choice .Please try again later !!!!");
        }
    }


}

