/*********************************************************************
Start Date : 4/03/2016
Version 1.3
code by :chinmay.shah@colorado.edu
Code : To implement to perform different Functions of Timer

Following functions have been Implemented and described
WDTInitilization ();
avoidWDTReset ();
Timer0Initialization();
Timer();
TimerDisplay(unsigned char row ,unsigned char col );
resumeTimer();
stopTimer();
restartTimer();
disableAlarm(unsigned char alarmpos);
enableAlarm(unsigned char alarmpos);
editAlarm(unsigned char alarmpos,unsigned char minbyte,unsigned char secbyte,unsigned char msecbyte);
menuTimer();
timerReset();
checkAlarm();
stopAlarm();
statusAlarm();

*********************************************************************/

#include "timers.h"
#include <at89c51ed2.h>
#include "stdio.h"
#include "LCD.h"
#include "constraints.h"
#include "string.h"

/***************************************
Global Variables used by Timer Functions
****************************************/
volatile unsigned char Timervar=0x00;
volatile unsigned char mstimer=0;
volatile unsigned int stimer=0;
volatile unsigned int mtimer=0;
volatile unsigned char stoptimer=0;
volatile unsigned char resumetimer=1;

unsigned char setAlarmTime[MAX_ALARM][STATUS_POS+1];
unsigned char snooze;


//Initialize Watch Dog Timer
void WDTInitilization ()
{
    WDTPRG|=WDTTimeout;//
    WDTRST=0x1E;
    WDTRST=0xE1;
}

//Function to avoid  Watch Dog Timer,service Watch Dog Timer
void avoidWDTReset ()
{
    WDTRST=0x1E;
    WDTRST=0xE1;

}

/*********************************************************************
Initialize values for Timer 0
1)Configure Timer for 0.05 sec
    Theoretical  value TH0 = 0xB4   ;//Initialize
    value changed due to long ISR
*********************************************************************/



void Timer0Initialization ()
{
    TMOD |= TMOD0INI ; //0000 0001
                    //Timer 0 - Mode 1 (16 bit  Mode)
    //TH0 = 0x4B   ;//Initialize
    //TL0 = 0x00   ;//Initialize
                    //value for 0.05 secs overflow
    TH0 = 0xB4   ;//Initialize
    TL0 = 0x80   ;// value for 0.05 secs overflow
    TR0 = 1      ;//Start Timer
    TF0 =0       ;     //Clear overflow flag
    IEN0 |=0x82  ; //Enable intr for Timer
}



/*********************************************************************
Function - Calculating timer values after each Interrupt call
        -Display on LCD counter values
        -Check ALARM
*********************************************************************/

void Timer()
{
    //printf_tiny("\n\rInside Timer intr");

    if (resumetimer==1)
    {
        Timervar++;//counting Interrupts

            if (Timervar==1)//Check for 2x0.05 = .1 sec
            {
                mstimer++;//Increment of ms count
                Timervar=0;//Reinitialize count
            }
            if (mstimer == 9)//Check for overflow milliseconds
            {
                mstimer = 0;//milliseconds over flow
                if ((stimer&0x0F) == 0x09)//check if number (0-9)
                {
                   stimer+=0x06;//Increment by 6
                }
                stimer++;
            }
            if (stimer >= 0x59)//Check for overflow seconds,hex code for 59(0x3b)
            {
                stimer = 0;
                if ((mtimer&0x0F) == 0x09)//check if number (0-9)
                {
                   mtimer+=0x06;//Increment by 6 , to adjust hexa to number
                }
                mtimer++;

            }
            if (mtimer >= 0x59)//Check for overflow seconds
            {
                //Reinitialize all values
                Timervar=0;
                mstimer =0;
                stimer =0;
                mtimer =0;
            }

    }
    TimerDisplay(3,9);//Display on LCD
    checkAlarm();//Function to Check ALARMS


}

/*********************************************************************
Display timer on LCD using following format
------------------------------------------
|                                        |
|                                        |
|                                        |
|                                 MM:SS.S|
------------------------------------------
MM - minutes
SS - Seconds
S  - 1/10th of second
Pre  input -1) Row , column to position timer
            2) Integer values of Minutes ,seconds and Milli seconds
*********************************************************************/

void TimerDisplay(unsigned char row ,unsigned char col )
{
    LCDGotoxy(row,col);
    LCDPutchar((int)((mtimer&0xF0)>>4)+48);//Display Minutes(MSB)
    LCDPutchar((int)(mtimer&0x0F)+48);//Display Minutes(LSB)
    LCDPutchar(':');
    LCDPutchar((int)((stimer&0xF0)>>4)+48);//Display Seconds(MSB)
    LCDPutchar((int)(stimer&0x0F)+48);//Display Seconds(LSB)
    LCDPutchar('.');
    LCDPutchar((int)(mstimer&0x0F)+48);//Display Milli Seconds
}

/*********************************************************************
Reset all values of timer

*********************************************************************/

void timerReset()
{
    Timervar=0;
    mstimer=0;
    stimer=0;
    mtimer=0;
    TimerDisplay(3,9);
}

/*********************************************************************
Stop and  start Timer from same time
*********************************************************************/


void resumeTimer()
{
    resumetimer = ~resumetimer;//using a flag ,
}



/*********************************************************************
Stop  Timer from completely
*********************************************************************/


void stopTimer()
{
    IEN0 &= 0xFD;//Mask Timer 0 interrupt,to stop
}


/*********************************************************************
Restart Timer from completely,Will display from last values
*********************************************************************/
void restartTimer()
{
    IEN0 |= 0x82;//Mask Timer 0 interrupt, to start
}


/*********************************************************************
Menu options for Timer Functionality
*********************************************************************/

void menuTimer()
{

    printf_tiny("\n\rTIMER MENU\r\n");
    printf_tiny("\n\rz) -Reset Clock \r\n");
    printf_tiny("\n\rp) -Resume/Pause Clock \r\n");
    printf_tiny("\n\rs) -Start Clock    \r\n");
    printf_tiny("\n\rq) -Stop  Clock    \r\n");
    printf_tiny("\n\ra) -Alarms Edit    \r\n");
    printf_tiny("\n\re) -Enable Alarms  \r\n");
    printf_tiny("\n\rd) -Disable Alarms \r\n");
    printf_tiny("\n\rc) -Check status Alarms \r\n");
    printf_tiny("\n\rt) -Terminate Alarm \r\n");
    printf_tiny("\n\ri) -Infinite condition \r\n");
    printf_tiny("\n\r?) -Menu Options \r\n");
    printf_tiny("\n\rx) -Main Menu \r\n");

}






/*********************************************************************
Function to Enable ALARM
*********************************************************************/
void enableAlarm(unsigned char alarmpos)
{
    setAlarmTime[alarmpos][ENABLE_POS]=1;//to Store zero for enabling alarm
}




/*********************************************************************
Function to Disable ALARM
*********************************************************************/
void disableAlarm(unsigned char alarmpos)
{
    setAlarmTime[alarmpos][ENABLE_POS]=0;//to Store zero for disabling alarm
}



/*********************************************************************
Function
1)to Stop Only ALRAM which was running
2)Does not disable all enabled alarms

*********************************************************************/
void stopAlarm()
{
    unsigned char i =0;
    for (i=0;i<MAX_ALARM;i++)
    {
        if (setAlarmTime[i][STATUS_POS])
        {
            setAlarmTime[i][STATUS_POS]=0;//to Store zero for disabling alarm
            setAlarmTime[i][ENABLE_POS]=0;//to disable alarm
                                           //this will disable other alarm
        }

    }
    snooze=1;

}




/*********************************************************************
Function to Edit ALARM
    1) Sets Minutes , Seconds and Milli Seconds of each ALARM
    2) By default - Enables ALARM which is set
*********************************************************************/

void editAlarm(unsigned char alarmpos,unsigned char minbyte,unsigned char secbyte,unsigned char msecbyte)
{
    unsigned char i=0;
    setAlarmTime[alarmpos][MINUTE_POS]=minbyte;
    setAlarmTime[alarmpos][SEC_POS]=secbyte;
    setAlarmTime[alarmpos][MSEC_POS]=msecbyte;
    setAlarmTime[alarmpos][STATUS_POS]=0;//Status 0 Initially ALARM when set
    setAlarmTime[alarmpos][ENABLE_POS]=1;//Enabling ALARM when set

}


/*********************************************************************
Function to Display Status of ALL ALARM in following format
ALARM NO |ALARM TIME|  STATUS  |SNOOZE
*********************************************************************/


void statusAlarm()
{
    unsigned char i=0;
    //printf_tiny("\n\r Status of ALARMS \n\r");
    printf_tiny("NO \t\t\t ALARM TIME \t\t\t STATUS \t\t\t SNOOZE");
    for (i=0;i<MAX_ALARM;i++)
    {

        printf_tiny("\n\r %d \t\t\t",i+1);
        printf("TIME %02x:%02x.%x \t\t\t",setAlarmTime[i][MINUTE_POS],setAlarmTime[i][SEC_POS],setAlarmTime[i][MSEC_POS]);
        printf("\t%x \t\t %x \n\r",setAlarmTime[i][ENABLE_POS],setAlarmTime[i][STATUS_POS]);
    }

}



/*********************************************************************
Function to Check if an ALARM time has reached and ring ALARM
*********************************************************************/

void checkAlarm()
{
    unsigned char i=0;
    //To check for all ALARM Timing
    for (i=0;i<MAX_ALARM;i++)
    {
        //Check if any of alarm times time has been reached
        if ((setAlarmTime[i][MINUTE_POS]<= mtimer && setAlarmTime[i][SEC_POS]<= stimer && setAlarmTime[i][MSEC_POS]<= mstimer && setAlarmTime[i][ENABLE_POS]==1 && setAlarmTime[i][STATUS_POS]==0 ))
        {
            setAlarmTime[i][STATUS_POS]=1;
        }
    }
    for (i=0;i<MAX_ALARM;i++)
    {
        //Display on LCD if ALARM Time has been reached
        if (setAlarmTime[i][STATUS_POS]==1)
        {
            LCDGotoxy(i,0);
            LCDPutchar(0x00);//Position for ALARM symbol
            LCDPutchar(0x00);//Position for ALARM symbol
            LCDPutstr("ALARM");//Display message if alarm reached
            LCDPutchar((i|0x30)+1);//Display message if alarm reached
            LCDPutchar(0x00);//Position for ALARM symbol
            LCDPutchar(0x00);//Position for ALARM symbol
        }

        else if (snooze)// Clear Screen only if a particular alarm was buzzing and terminated by user
        {
            LCDClear();//clear if No ALARM
            snooze=0;
        }
    }

}
