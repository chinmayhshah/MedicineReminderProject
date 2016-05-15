/******************************************************************
code by :chinmay.shah@colorado.edu
library of functions designed for use for  constraints
and display according to requirement

1) checkInputCharacter(char input);
       -- Check validity of input character between Hex values (0-F)
2) StringNumber(unsigned char type);
       -- Stores input of (hex values) as separate values
            as array of ASCII values

3) ASCIItoNumber(unsigned char number[],unsigned char type);
    --convert ASCII values to Integer two digit numbers
    --customized for data and address (EEPROM)

4) hexDisplay (unsigned char startAddr[],unsigned char maxLine,unsigned char endAddr[]);
    -- Customized display Hex values in following format
    -- incorporates condition to take into account , different block
    -- checks validity of input address
        AAA : DD DD DD DD

5) displayLCD (unsigned char addr[],unsigned char hexrow);
    -- Display EEPROM data on LCD for particular address and row number on LCD
6) hextoASCII(unsigned char hexvalue);
    -- Convert Hex Input to ASCII to be displayed



Reference
1)for only for conversion from hex to decimal, referred for logic
    http://robustprogramming.com/convert-hexadecimal-to-decimal-c/

******************************************************************/
#include <stdio.h>
#include "constraints.h"
#include  <string.h>
#include  <stdlib.h>
#include <math.h>
#include "eeprom.h"
#include "LCD.h"
#include "timers.h"


#define MAX_BLOCK_DATA 255
#define DEBUGALL
#undef DEBUG


/****************************************************************************************
Function to Check for type of Characters
Pre-Condition : A single Character input
Post-Condition :
 return 0   -For Numbers -0x30 to 0x39
        1   -For Character  (Lower or Upper Character for a to f or A to F )
        2- Any other character


******************************************************************************************/
unsigned char checkInputCharacter(char input)
{
//    avoidWDTReset();
    if ( input>='0' && input<='9')  //Check for Numbers
        {
        return 0;
        }

    else if ((input>='a' && input<= 'f') || (input>='A' && input<= 'F') )//Check for Upper and Lower Case characters
        {
        return 1;
        }
    else
        {
        return 2;
        }

}



/*************************************************
Description :Function to store contents in an array
Pre: Input of type of data
    0 - Address(length 3)(numbers from 0 -F)
    1 - Data(length 2)(numbers from 0 -F)
    2- Data Length (only numbers from 0-9)
Post:
1) Store contents   contents as character by
character till NULL "\r" encountered
2) Display Invalid Message if not a number
**************************************************/
unsigned char *StringNumber(unsigned char type)
{
    unsigned char ch1[MAXADDR_LENGTH+1]={'\0','\0','\0','\0'};
    //unsigned char *ch1=NULL;
    unsigned char z=0;
    unsigned char i=0,j=0;
    ch1[z]=getchar();
    while ((ch1[z] != '\r' && z<MAXADDR_LENGTH && !type) || (ch1[z] != '\r' && z<MAXDATA_LENGTH && type))
    {
//        avoidWDTReset();
        putchar(ch1[z]);//echo character

        i=checkInputCharacter(ch1[z]);//Check if character is number or not
        if (i==2)// Return value for character other than 0-9 or a-f or A-F
        {
            printf_tiny("\r\nInvalid Hex number. Please try with number again!!!\r\n");
            return NULL;
        }
//        avoidWDTReset();
        if (i && type ==2 )// Return value for character other than 0-9 or a-f or A-F
        {
            printf_tiny("\r\nInvalid input number. Please try with number again(0-9)!!!\r\n");
            return NULL;
        }
        z++;//Increment counter
        ch1[z]=getchar();//get a character and Store in array for conversion

   }
    ch1[z]='\0';
    printf_tiny("\r\n");



    #ifdef DEBUG
    printf("\n\r Inside String Number \n\r");
    j=0;
    while(ch1[j]!='\0')
    {
        printf("\n\r %c",ch1[j++]);
    }
    #endif // DEBUGALL
    //return &ch1;
    return (unsigned char*)ch1;

}



/**************************************************************
Function to Convert ASCII values to Number
Pre-Condition  - input is a array of ASCII values
                - type of data to be converted (data or address)
                - 0 -address(2)
                - 1 -data(2)
Post-Condition - o/p is a number between 0 to 256
***************************************************************/


unsigned int ASCIItoNumber(unsigned char number[],unsigned char type)
{
     unsigned char i=0,j=0 ;
     unsigned char tempnumber=0;
     long int decimalValue=0;
     long int tempSum=0;
     unsigned int length=0;
     const int base =16 ;


     while (number[j]!='\0')
     {
         length ++;
         #ifdef DEBUG
         printf("\n\r %c",number[j]);
         #endif // DEBUGALL
         j++;

     }
     #ifdef DEBUG
     j=0;
     #endif // DEBUGALL


     //Check if input address is less than 3 digit and address < 0x7FF
     if ((length > MAXADDR_LENGTH || number[i] >'7') && (type == 0 ))
     {
         printf_tiny("\n\rInput address exceeds limit please try again");
         return 260;
     }
     else if ((length > MAXDATA_LENGTH) && (type == 1 ))
     {
         printf_tiny("\n\rInput data exceeds 8 bit value please try again");
         return 260;
     }

     else
     {
        if (!type)//Increment counter if address to be converted
        {
            i++;//Skip first position as it is hundred value
        }
        //Convert if there is some thing left to convert
         if (number[i]=='\0')
         {
            printf_tiny("\n\rInput inadequate please try again");
            return 260;
         }
         else
         {
            //Conversion of ASCII to hex value
             decimalValue =0;//Reinitialize value to zero
             while (number[i]!='\0') //Loop till null character
             {

                tempSum =powf(base,(length -i -1));
                if (number[i] >= '0' && number[i] <= '9')//for decimal number from 0 to 9
                    {
                        decimalValue += ((((int)(number[i]))-48 ) *tempSum);
                    }
                else if ((number[i] >= 'A' && number[i] <= 'F')) //for character from A to F
                    {
                        decimalValue += ((((int)(number[i]))-55) * tempSum);

                    }
                else if ((number[i] >= 'a' && number[i] <= 'f')) //for character from a to f
                    {
                        decimalValue += ((((int)(number[i]))-87) * tempSum);

                    }
                else
                {
                    printf_tiny("\n\rInvalid Hex Number");
                }



                 i++;
             }
         }
        }
        return decimalValue;
}



/**************************************************************
Function to print data in following format
            AAA: DD DD DD DD DD DD

Pre-Condition  - input a)Start Address
                       b)Max value per line
                       c)End Address
Post-Condition - o/p is a display of data on Terminal Screen
---------------------------------------------------------------
|    AAA: DD DD DD DD DD DD DD DD DD DD DD DD DD DD DD DD     |
|    AAA: DD DD DD DD DD DD DD DD DD DD DD DD DD DD DD DD     |
|    AAA: DD DD DD DD DD DD DD DD DD DD DD DD DD DD DD DD     |
---------------------------------------------------------------
                (max - maxLine data per line )
***************************************************************/
unsigned char hexDisplay (unsigned char startAddr[],unsigned char maxLine,unsigned char endAddr[])
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


    //Check if EndAddr > StartAddr with Blocking into consideration
    //If there are multiple blocks between addresses
    //display value till MAX_BLOCK_DATA
    if (blockdiff)
    {
        sameblock++;//use to indicate if this other section are used
        while (blockdiff!=0)
        {
            //temp
           k=starthexAddr;
           //print complete block of data
           while(k++<(MAX_BLOCK_DATA))// change according to last address
            {

                if ((j)%(maxLine)==0)//check for maxLine for a line
                {
                   j=0;//re-initialize with zero

                   //Printing address of byte after every 16 bytes
                   printf_tiny("\n\r%x",(startAddr[0]&0x0F));//Display block value
                   printf("%02x :",(starthexAddr));//Display address of byte

                }

                //change block according to end of start address or Sequential read
                byte=EEPROMRead((startAddr[0]&0x0F),starthexAddr++);//Call EEprom Read
                printf("%02x ",byte);//Print Data in DD format
                j++;


            }

            blockdiff --;//Decrement block value
            byte=EEPROMRead((startAddr[0]&0x0F),starthexAddr++);//Call EEprom Read
            printf("%02x ",byte);//Print last data( of maxLine) in DD format,if tiny used 00 is not printed
            startAddr[0]+=0x01;//Increment block value
            starthexAddr=0x00;//Reinitialize to zero
            j++;


        }


    }
    //i=j=k=0;//Reinitialize value
    i=k=0;//Reinitialize value
    if (!blockdiff)//within same block
    {

            while(k++<diffaddr +1)// change according to last address
            {
                k=starthexAddr;
                if (j%(maxLine)==0)
                {
                   printf_tiny("\n\r");
                   j=0;
                   printf_tiny("%x",(startAddr[0]&0x0F));
                   printf("%02x :",starthexAddr);
                }
                //change block according to end of start address or Sequential read
                byte=EEPROMRead((startAddr[0]&0x0F),starthexAddr);//Call EEprom Read
                printf("%02x ",byte);//Print Data in DD format
                starthexAddr++;// for now ,need to use sequential read
                j++;


                if ((startAddr[0]=='7' && starthexAddr==0xFF ))
                {
                   byte=EEPROMRead((startAddr[0]&0x0F),starthexAddr);//Call EEprom Read
                   printf("%02x ",byte);//Print Data in DD format
                   break;
                }

            }
    }
    return 1;
}

/************************************************************************************
Function to convert value from Hex 0xDD to ASCII (displayable on LCD)
            where D value from 0 to9 or a to f or A-F
Pre-Condition  - input :Hex value of address
Post-Condition - return  is a character array of ASCII values
*************************************************************************************/



unsigned char *hextoASCII(unsigned char hexvalue)
{
      unsigned char ASCIIdata[]={'\0','\0','\0'};
      unsigned char temp=0,i=0;



    while ((i<MAXDATA_LENGTH)) //Divide by 16 as base is hex
    {

       temp = hexvalue % 0x10; // LSB value
       hexvalue /= 0x10;// MSB value

       if (temp >= 0x00 && temp <= 0x09)//for decimal number from 0 to 9
                    {
                    #ifdef DEBUG
                    printf("\n\r hex value %02x\n\r",temp);
                    #endif // DEBUGALL

                        ASCIIdata[MAXDATA_LENGTH-i-1] = (int)temp + 48 ;
                    }
                else if ((temp >= 0x0A && temp<= 0x0F )) //for character from A to F
                    {

                        ASCIIdata[MAXDATA_LENGTH-i-1] = (int)temp + 55 ;

                    }
                    i++;
    }

    ASCIIdata[i]='\0';
    #ifdef DEBUG
    i=0;
    printf("\n\r Inside hex to ASCII\n\r");
    while(ASCIIdata[i]!='\0')
    {
        printf("%x \n\r",ASCIIdata[i++]);
    }
    #endif // DEBUGALL

    return &ASCIIdata;
}


