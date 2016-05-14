/******************************************************************
code by :chinmay.shah@colorado.edu
library of functions designed for use for  LCD Functionality

Functions are as follows - have been described below
1) LCDCommandWrite(unsigned char cmd)
2) LCDCommandRead()
3) LCDBusyCheck()
4) LCDInitialization()
5) LCDDataWrite(unsigned char data)
6) LCDDataRead()
7) LCDGotoaddress(unsigned char addr)
8) LCDGotoxy(unsigned char row,unsigned char column)
9) LCDPutchar(unsigned char tempchar)
10)LCDPutstr(char *str)
11)LCDClear()
12)LCDCreateChar(unsigned char code,unsigned char rowvalue[])
13)LCDMenu()


Reference :
        1) Basics
            https://www.youtube.com/watch?v=YDJISiPUdA8
        2) Custom character
            http://www.8051projects.net/lcd-interfacing/lcd-custom-character.php
******************************************************************/

#include <at89c51ed2.h>
#include "delay_custom.h"
#include "LCD.h"
#include <string.h>
#include "constraints.h"
#include "timers.h"




/******************* Function Definition ****************
*Functions for LCD
********************************************************/


/******************* LCD Command Write Function*********
*Pre - Cmd - Command to write on LCD
********************************************************/
void LCDCommandWrite(unsigned char cmd)
{
    //printf("Inside Clear screen 1");

    RS = 0;       // Select Instruction Register
    RW = 0;       // Select Read Mode
    LCD_CMD(cmd); // Address of LCD reg accessed
                  // then Command Assigned on Port /data bus
    //E=0;          // Set Enable
    RW =1;        //Set RW
    RS =1;        //Set RS
    delay_ms(50); // Delay of 20 ms

}



/******************* LCD Command Read Function*********
*Read from on LCD
********************************************************/
void LCDCommandRead()
{

    RS = 0;       // Select Instruction Register
    RW = 1;       // Select Read Mode
    read = LCD_READ;//to read LCD
    //read &= 0x80; // Masking bits other than MSB

    //printf("\n\rRead Value %x",read);
    //not working , commented to remove comment
    RW =0;        //Set RW
    RS =1;        //Set RS
    delay_ms(50); // Delay of 20 ms

}





/******************* LCD Busy Check Function*********
*Check if LCD Controller is busy or not
********************************************************/
void LCDBusyCheck()
{
    LCDCommandRead();
    while(read &=0x80)//Separate D7 bit from read
    {
        LCDCommandRead();//Read all port
    }
}




/******************* LCD Initialization Function*********
*
********************************************************/
void LCDInitialization()
{
    delay_ms(1000);
    LCDCommandWrite(0x30); //Unlock Command
    delay_ms(150);
    LCDCommandWrite(0x30); //Unlock Command
    delay_ms(150);
    LCDCommandWrite(0x30); //Unlock Command
    delay_ms(150);


    LCDBusyCheck();        //Check if LCD is busy
    //delay_ms(150);//Dont use Busy wait
    LCDCommandWrite(0x38); //System Set /Function Set
                            //DB3 =1 (2 line display),DB2 - 0 =5x8
    LCDBusyCheck();
    ////delay_ms(150);//Dont use Busy wait
    LCDCommandWrite(0x08); //Display
                            //DB4 -1,DB3(DL)-0 = Display OFF

    LCDBusyCheck();
    ////delay_ms(150);//Dont use Busy wait
    LCDCommandWrite(0x0E); //Display ON / OFF
    //LCDCommandWrite(0x0C); //DB4 -1,DB3(DL)-1 = Display ON,DB2 - 1
    //LCDCommandWrite(0x0F); //DB4 -1,DB3(DL)-1 = Display ON,DB2 - Cursor On,DB1 - Blink

    LCDBusyCheck();
    ////delay_ms(150);//Dont use Busy wait
    LCDCommandWrite(0x06); //Entry Mode Set
                            //DB2 -1,DB1 - 1 = Increment ON ,DB0 -0 = Display Shift OFF
    LCDBusyCheck();
    ////delay_ms(150);//Dont use Busy wait
    LCDCommandWrite(0x01); //Clear Screen and Move to Home
                            //DB0 - 1 = Clear Screen
    delay_ms(100);

}





/******************* LCD Data Write Function*********
*Write on DDRAM
********************************************************/
void LCDDataWrite(unsigned char tempdata)
{

    RS = 1;       // Select Data Register
    RW = 0;       // Select Write Mode
    LCD_CMD(tempdata); // Address of LCD reg accessed
                  // then Command Assigned on Port /data bus
    RW =1;        //Set RW
    RS =0;        //Clear RS
    delay_ms(20); //Delay of 20 ms

}



/******************* LCD Data Read Function*********
*Read from DDRAM
********************************************************/
unsigned char LCDDataRead()
{
    unsigned char tempReadData;
    RS = 0;       // Select Data Register
    RW = 0;       // Select Read Mode
    delay_ms(10);
    RS = 1;       // Select Data Register
    RW = 1;       // Select Read Mode
    tempReadData = LCD_READ;//to read LCD
    delay_ms(10);
    RW =0;        //Set RW
    RS =0;        //Set RS
    delay_ms(50); // Delay of 50 ms

    return tempReadData;

}



/******************* LCD goto address*****************
*Pre : Position address
*Post : Cursor points specified DDRAM address
********************************************************/
void LCDGotoaddress(unsigned char addr)
{
    LCDCommandWrite(addr);//Jump to location 0x00
    LCDBusyCheck();
    lcd_column = addr & 0x0F;
    lcd_row =   ((addr & 0xF0) >> 4);
}



/******************* LCD goto xy address*****************
*Pre : Position row and column provided
*Function calculates the addressing mapping from input
*row and column
*Post : Cursor points at row,column position
********************************************************/

void LCDGotoxy(unsigned char row,unsigned char column)
{
    unsigned char addr;

    //Need to refine logic for inputs greater than 16
    //column &= 0x0F;// converting ASCII value to HEX
    //Check if column value is greater than LCD_MAXCOL


    if (column > LCD_MAXCOL)
    {
        row = row + 1;
        column =0;
    }
    //check row value does not exceed max of LCD
    //ReInitialize it to first row
    if (row > LCD_MAXROW )
    {
        row =0 ;
    }

    //printf("\n\rValue of x and y co %d:%d",row,column);

    //To select row mapped address of LCD with respect to input row
   switch(row)
   {

    case 0:
            row = LCD_LINE0;
            //printf("\n\rInside switch  %04x",row);
            break;
    case 1:
            row = LCD_LINE1;
            //printf("\n\rInside switch  %04x",row);
            break;
    case 2:
            row = LCD_LINE2;
            //printf("\n\rInside switch  %04x",row);
            break;
    case 3:
            row = LCD_LINE3;
            //printf("\n\rInside switch  %04x",row);
            break;
    default : row = LCD_LINE0;

   }

   addr= row | column;
   addr |= LCD_DB7;


   lcd_row =row;
   lcd_column =column;

   //printf("\n\rValue of address %x",addr);
   LCDGotoaddress(addr);
   delay_ms(10);
}




/******************* LCD goto address*****************
*Pre : input is a charactcer(ASCII)
*Post : prints data at current Cursor points
********************************************************/
void LCDPutchar(unsigned char tempchar)
{
    LCDDataWrite(tempchar);
    delay_ms(20);
}



/******************* LCD goto address*****************
*Pre : An input string terminated with "\n"
*Post :
        1)prints string on LCD starting current LCD cursor
        position
        2)Shifts to next line if length greater than
          max size of row length of LCD (MAXCOL)
********************************************************/
void LCDPutstr(char *str)
{

    unsigned char i=0;
    unsigned char col,row;
    unsigned char templen = strlen(str);

    // To obtain current value of row and column
    row=lcd_row;
    col=lcd_column;
    //row=*(LCDDataRead());//works incorrectly

    while (str[i]!='\0' & i<templen)
    {

        //DEBUG
        //printf("%c",str[i]);
        //
        //If length of string is greater than MAXCOL
        //change the location to next line first position
        if (col>LCD_MAXCOL)
        {
            col=0;//Reintialize value of row if exceeded
            row++;//Increment to move to next row
            //Reintialize value of row if exceeded
            if (row > LCD_MAXROW)
            {
                row = 0;
            }
            LCDGotoxy(row,col);//Reset location as new row and column
            delay_ms(100);
        }
        LCDPutchar(str[i]);
        i++;//Increment i as each character  printed
        col++;//Increment col as data printed


    }
    //
    lcd_row=row;
    lcd_column=col;
    //


}
/*****************************************************************************
* Function to   Clear Display of LCD  and move cursor to  to home address
*
******************************************************************************/
void LCDClear()
{
    LCDCommandWrite(0x01); //Clear Screen and Move to Home
                            //DB0 - 1 = Clear Screen
    lcd_column = 0x00;
    lcd_row = 0x00;
    delay_ms(100);
}


//void LCDCreateChar(unsigned char code,unsigned char rowvalue[])
/*
void LCDCreateChar()
{
    LCDCommandWrite(0x40);
    LCDDataWrite(0x04);
    LCDDataWrite(0x0E);
    LCDDataWrite(0x0E);
    LCDDataWrite(0x0E);
    LCDDataWrite(0x1F);
    LCDDataWrite(0x00);
    LCDDataWrite(0x04);
    LCDDataWrite(0x00);

}
*/

/*****************************************************************************
*Function to   create Custom character using CGRAM of LCD and display it on LCD
*I/p-
    ccode - character code to be stored in CGRAM
    rowvalue -Hex codes for each row
*O/p-To store a custom character at a location of LCD character CGRAM
******************************************************************************/


void LCDCreateChar(unsigned char ccode,unsigned char rowvalue[])
{

    unsigned char j=0;

    if (ccode < 8  )
    {
            LCDCommandWrite((ccode*8)+0x40);
            for (j=0;j<8;j++)
            {
                    LCDDataWrite(rowvalue[j]);
       //             avoidWDTReset();
            }

    }
    else
    {
        printf_tiny("\n\r Custom code exceeds limit \n\r");
    }


}







/*******************************************************
Menu Display for LCD Options for different functionality

********************************************************/
/*

void LCDMenu()
{

    unsigned char z=0,temp=0,lcdmenu=0,temprow=0,tempcol=0,j=0,daddr=0,ctemp=0;
    unsigned char *tempaddr;
    unsigned char *inputStr;

    //
    while (!lcdmenu)
    {
    printf_tiny("\n\r                                        *************************************************************************\r\n");
    printf_tiny("\r                                          *************************     LCD MENU LIST      ************************\r\n");
    printf_tiny("\n\r                                        *************************************************************************r\n");
    printf_tiny("\n\r                                        ***********            1) -Display cursor at an address         **********\r\n");
    printf_tiny("\n\r                                        ***********            2) -Displace cursor at row,col           **********\r\n");
    printf_tiny("\n\r                                        ***********            3) -Display a character                  **********\r\n");
    printf_tiny("\n\r                                        ***********            4) -Display a string                     **********\r\n");
    printf_tiny("\n\r                                        ***********            5) -Clear LCD                            **********\r\n");
    printf_tiny("\n\r                                        ***********            6) -Main Menu                            **********\r\n");
    printf_tiny("\n\r                                           **********************************************************************\r\n");
    printf_tiny("\n\r                                             ************      Please Enter your Choice !!!!!       **********\r\n");
    z=getchar();
    putchar(z);
    temp=checkInputCharacter(z);
    z = z & 0x0F;
    //
    //printf("\n\r temp value %02x\n\r",z);


            if (!temp)
            {
                    switch (z)
                    {
                        case 0x01:
                                daddr=0x00;
                                printf("\n\rEnter an address for DDRAM \n\r");
                                tempaddr=StringNumber(1);//store address
                                daddr=ASCIItoNumber(tempaddr,1);
                                daddr |= LCD_DB7;
                                LCDGotoaddress(daddr);
                                break;
                        case 0x02:
                                temprow = 0x00;
                                tempcol =0x00;
                                tempaddr=0x00;
                                printf_tiny("\n\r Enter row of LCD(0,1,2,3):");
                                temprow=getchar();//Store Input
                                putchar(temprow);//Display data input
                                printf_tiny("\n\r");
                                temprow = ((temprow % 0x04) & 0x0F);
                                //
                                printf("row value %x",temprow);
                                //
                                printf_tiny("\n\r Enter Column of LCD(0-F):");
                                tempcol=getchar();//Store Input
                                putchar(tempcol);//Display data input
                                printf_tiny("\n\r");
                                //printf("col value 1 %02x",col);
                                //col = (col & 0x0F);
                                tempaddr[0]='0';
                                tempaddr[1]=tempcol;
                                tempaddr[2]='\0';
                                tempcol=ASCIItoNumber(tempaddr,1);

                                printf("col value 2 %02x",tempcol);

                                LCDGotoxy(temprow,tempcol);
                                break;
                        case 0x03:
                                printf_tiny("\n\rEnter character to be stored and Displayed on LCD\n\r");
                                ctemp=getchar();
                                putchar(ctemp);
                                LCDPutchar(ctemp);
                                //LCDPutchar('\n');
                                break;
                        case 0x04:
                                j=0;
                                printf_tiny("\n\rEnter string to be displayed on LCD\n\r");
                                do
                                {
                                    inputStr[j]=getchar();
                                    putchar(inputStr[j]);
                                }while (inputStr[j++]!='\r');
                                inputStr[j]='\0';
                                printf_tiny("\n\rDisplay on LCD\n\r");
                                LCDPutstr(inputStr);
                                break;
                        case 0x05:
                                LCDClear();
                                break;
                        case 0x06:
                                lcdmenu ++;//To move to main menu
                                break;
                        default:
                                printf_tiny("\n\r Invalid choice .Please try again !!!\n\r");


                    }
            }
            else
            {
                printf_tiny("\n\rInvalid Input .Please try again with a number !!!!!\n\r");
            }


    }
}
*/

