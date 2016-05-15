/*******************************************************
Library of Functions to generate delays


Reference: 1)generating delays in micro sec and usage of Assembly
            http://sdccokr.dl9sec.de/resources.htm

********************************************************/

#include "delay_custom.h"

/******************* Function Definition ****************
*To generate delay in uS
********************************************************/
void delay_oneus(void)
{
__asm
    nop
__endasm;

}


/******************* Function Definition ****************
*To generate delay in uS
********************************************************/
void delay_us(void)
{
__asm
    nop
    nop
    nop
    nop
    nop
    nop
__endasm;

}


/******************* Delay_ms Function******************
*Function to generate delay of one milliseconds
* It generates a delay of 1.0200 ms
* with count of oneMs= 64
********************************************************/

void delay_onems()
{
    unsigned char i ;
    for (i=0;i<oneMs;i++);
}


/******************* Delay_ms Function******************
*   Function to generate delay of x milliseconds
*
********************************************************/

void delay_ms(unsigned int x)
{   unsigned int i;
    for (i=0;i<x;i++)
    {
        delay_onems();//call a function of one milli second delay
    }
}




/******************* Delay_ms Function******************
*   Function to generate delay of x seconds
*
********************************************************/



void delay_seconds(unsigned int x)
{   unsigned int i;
    for (i=0;i<x;i++)
    {
        delay_ms(1000);//call a function of one milli second delay
    }
}
