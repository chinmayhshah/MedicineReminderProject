/**********************************************************************************************************************
Functions of software to implement the functionality of Medicine Reminder system

    Code by chinmay.shah@colorado.edu

Assumtions/Restriction:
1) Maximum number of Medicines - Stored in single block of EEPROM - MEDCINES_NAME -- 0 to MAX_MEDICINES-1
2) Everyday unique  Medication Stored in another page of EEPROM - MEDCATION_DETAILS -- 0 to MAX_MEDICINES-1
    MAX_MEDICINES - Maximum allowed 16 , as restriction of block
-- can be upgraded to Database emulation on EEPROM for better accessibility and indexing

Format of Storage MEDCINES_DETAILS in EEPROM
Hex Tag Details -xx

                EEPROM each page format
0  1   2  3  4   5   6   7   8     9    A           B   C   D   E   F
xx xx xx xx xx   DTE MON YR  HRS  MIN  MED_STATUS  REV REV REV REV REV

Dependency Library
1)RTC_DS1307.h or RTC_DS1302.h
2)eeprom.h
3)stlcd.h
4)at89c51ed2.h
****************************************************************************************************************************/
#include "Medicine_DB.h"
#include "RTC_DS1307.h"
#include "eeprom.h"
#include "stdio.h"
#include "delay_custom.h"
#include "stlcd.h"
#include <at89c51ed2.h>




/***************************************************************
Global Variables
****************************************************************/

Med M1;
Med xdata *addMed=&M1;
unsigned char DetailsPageAddress=0; //max value 16
unsigned char NamePageAddress=0; //max value 16
unsigned char totalMed; //location in EEPROM stored
unsigned char totalMedtoday; //location to store in EEPROM
/***************************************************************
Function to add tag of Medicine in following format
Format of Storage MEDCINES_DETAILS
Hex Tag Details -xx
Format on a page of EEPROM
0  1   2  3  4   5   6   7   8     9    A           B   C   D   E   F
xx xx xx xx xx   DTE MON YR  HRS  MIN  MED_STATUS  REV REV REV REV REV

MED_STATUS -
 MED_DONE - Medication taken
 MED_REQ  -  Medication Required
 MED_NOTREQ  -  Medication not Required

 *** Medicine always added as MED_REQ by default

***************************************************************/
void insertMedicine(unsigned char tag[],timeclock *Timecurr)
{
    unsigned char i=0,addr=0;//dataToDB[16],ewrite;
    unsigned char status=0,readstatus=MED_REQ;

    while(i<RFID_TAG_MAX)//copy the medicine tag value
    {
            addMed->medicineTag[i]=tag[i];
            i++;
    }
  //Store values in Memory
  addMed->clock.dte=Timecurr->dte;
  addMed->clock.mon=Timecurr->mon;
  addMed->clock.yr=Timecurr->yr;
  addMed->clock.hrs=Timecurr->hrs;
  addMed->clock.minute=Timecurr->minute;
  addMed->status_bh=MED_REQ;//Initially for the day required Medicine
  //DEBUG
   //printf_tiny("\n\r Medicine added for day ");
   i=0;
    //
   while (i<5)
   {
        printf("%02x",addMed->medicineTag[i++]);
   }
   printf("\n\r%02x\t%02x\t%02x\t%02x\n\r  DA %02x ",addMed->clock.dte,addMed->clock.hrs,addMed->clock.minute,addMed->status_bh,DetailsPageAddress);
   //END OF DEBUG
   i=0;
   addr= addr|(DetailsPageAddress++<<4);
   if (DetailsPageAddress <= MAX_MEDICINES)//check if exceeds total number of Medicines
   {
        totalMedtoday=addr;//Store start of Page address accessed
                           //for storing details
   }
   else
   {
        totalMedtoday=DetailsPageAddress=0;//Reset value to zero
   }
    //Store Data in to EEPROM in above mentioned format
       while (i<5)//Add Tag value
       {
         EEPROMWrite(MEDCINE_DETAILS,addr+i,addMed->medicineTag[i]);
         delay_ms(30);
         i++;
       }
       //Add Time value
       addr =addr +i;
       EEPROMWrite(MEDCINE_DETAILS,addr++,addMed->clock.dte);
       delay_ms(30);
       EEPROMWrite(MEDCINE_DETAILS,addr++,addMed->clock.mon);
       delay_ms(30);
       EEPROMWrite(MEDCINE_DETAILS,addr++,addMed->clock.yr);
       delay_ms(30);
       EEPROMWrite(MEDCINE_DETAILS,addr++,addMed->clock.hrs);
       delay_ms(30);
       EEPROMWrite(MEDCINE_DETAILS,addr++,addMed->clock.minute);
       delay_ms(30);
       EEPROMWrite(MEDCINE_DETAILS,addr++,addMed->status_bh);
       delay_ms(30);
        //store total med today
       EEPROMWrite(0,TOTALMEDTODAY,totalMedtoday);//store value totalMedtoday

        //Display Message of successful addition
        //LCD part
        GLCD_GREEN=0;//light Green light
        displayUserString(2,"  Medicine Tag  ");
        displayUserString(3,"      Added   ");
        delay_seconds(3);
        CS=0;
        clear_screen();
        CS=1;
        GLCD_GREEN=1;


}


/**********************************************************************************
Function to transverse to all records in a EEPROM till
    (incrementing pages) till totalMedtoday or Tag and day match is  found
   uses checkMedicineToday()
   Input - Tag ,Current Time
   Output - Display on GLCD
        MEDS_STATUS
                MED_ERROR- Medicine is not present in DB
                MED_REQ- Medicine required for today
                MED_NOT_REQ -Medicine not required for today
                MED_DONE - Medicine consumed for today


********************************************************************************/

unsigned char checkMedicineToday(unsigned char tagValue[],timeclock  *Timecurr)
{
    unsigned char i=0,addr=0,pagecounter=0;
    MedStatus MedS;
    MedStatus *checkStatus=&MedS;
    Med MedI;
    Med *addMed=&MedI;
    checkStatus->status_bh=MED_ERROR;
    totalMedtoday = EEPROMRead(0,TOTALMEDTODAY);//Read store TOTALMEDTODAY value
    //transverse to all records in a EEPROM  (incrementing pages)
    //till totalMedtoday or Tag and day match is  found
    while ((pagecounter<<4) <= totalMedtoday && checkStatus->status_bh!=MED_REQ && checkStatus->status_bh!=MED_NOT_REQ && checkStatus->status_bh!=MED_DONE)//check if address exceeds medicine
    {
        addr=0;
        addr= addr|(pagecounter<<4);
        pagecounter++;
        checkStatus=checkMedicine(tagValue,Timecurr,addr);
    }
    //Display message on GLCD according to status
    switch (checkStatus->status_bh){
    case MED_ERROR: GLCD_RED=0;
                    displayUserString(2,"  Medicine not   ");
                    displayUserString(3,"      Present    ");
                    delay_seconds(4);
                    CS=0;
                    clear_screen();
                    CS=1;
                    GLCD_RED=1;
                    break;
    case MED_REQ:   GLCD_RED=0;
                    displayUserString(2,"  Medicine not  ");
                    displayUserString(3,"      Taken     ");
                    delay_seconds(4);
                    CS=0;
                    clear_screen();
                    CS=1;
                    GLCD_RED=1;
                    break;
    case MED_NOT_REQ:
                    GLCD_RED=0;
                    displayUserString(2,"  Medicine not  ");
                    displayUserString(3,"      Required  ");
                    delay_seconds(4);
                    CS=0;
                    clear_screen();
                    CS=1;
                    GLCD_RED=1;

                    break;
    case MED_DONE:  GLCD_GREEN=0;
                    displayUserString(2,"    Medicine    ");
                    displayUserString(3,"      Taken     ");
                    delay_seconds(4);
                    CS=0;
                    clear_screen();
                    CS=1;
                    GLCD_GREEN=1;
                    break;

    }
    return (checkStatus->status_bh);

}

/***************************************************************
Function to compare a input tag with record in EEPROM
    Input - Tag value
            Current Time
            pageaddr - page Address
   Output - Display on GLCD
        MEDS_STATUS
                MED_ERROR- Medicine is not present in DB
                         - Day of Medicine not matched
                         - Window of Hour does not matched
                MED_REQ- Medicine required for today
                MED_NOT_REQ -Medicine not required for today
                MED_DONE - Medicine consumed for today

***************************************************************/

struct Medicine_status * checkMedicine(unsigned char tagValue[],timeclock  *Timecurr,unsigned char pageaddr)
{
    unsigned char i=0,addr=pageaddr;
    MedStatus MedStatus2;
    MedStatus *checkStatus=&MedStatus2;
    checkStatus->pageAddress=pageaddr;//Assigning Tag page value read
    Timecurr=RTCClockRead();
    i=0;

        //Read Card Value and Time from EEPROM
            while(i<RFID_TAG_MAX)
            {
                    if (addMed->medicineTag[i]!=tagValue[i])//Check for a read Tag
                    {
                        checkStatus->status_bh=MED_ERROR;//Not for current day
                        return checkStatus;
                        //break;
                    }
                    i++;
            }
            addr=addr+i;
            addMed->clock.dte=EEPROMRead(MEDCINE_DETAILS,addr++);
            addMed->clock.mon=EEPROMRead(MEDCINE_DETAILS,addr++);
            addMed->clock.yr=EEPROMRead(MEDCINE_DETAILS,addr++);
            addMed->clock.hrs=EEPROMRead(MEDCINE_DETAILS,addr++);
            addMed->clock.minute=EEPROMRead(MEDCINE_DETAILS,addr++);
            addMed->status_bh=EEPROMRead(MEDCINE_DETAILS,addr++);
            //Check current value with record value
            if (addMed->clock.dte == Timecurr->dte && addMed->clock.mon == Timecurr->mon && addMed->clock.yr == Timecurr->yr)//Check for current day
            {

                //DEBUG
                //printf_tiny("\n\r Check day %x \t %x ",addMed->clock.dte,Timecurr->dte);
                //
                //Compare time of Med to be consumed
                if (addMed->clock.hrs >=(Timecurr->hrs - WINDOW_HOURS) && addMed->clock.hrs <=(Timecurr->hrs + WINDOW_HOURS))//Check for within the hour
                {
                    //DEBUG
                    //printf_tiny("\n\r Check hr %x %x ",Timecurr->hrs,addMed->clock.hrs);
                    //

                    if (addMed->status_bh != MED_DONE)//Check if Medicine has been already consumed
                    {
                        //DEBUG
                        //printf_tiny("\n\r Check status %x ",addMed->status_bh);
                        //
                        checkStatus->status_bh=MED_REQ;
                        return checkStatus;

                    }
                    else
                    {
                        //DEBUG
                        //printf_tiny("\n\r MED DONE");
                        //
                        checkStatus->status_bh=MED_DONE;//Medicine already consumed
                        return checkStatus;

                    }
                }
                else// Time window does not match
                {
                    checkStatus->status_bh=MED_ERROR;
                    return checkStatus;//not within hours

                }

            }
            else//Day for Medicine not matching
            {
                //DEBUG
                //printf_tiny("\n\r day not matching %x  %x ",addMed->clock.dte,Timecurr->dte);
                //
                 checkStatus->status_bh=MED_ERROR;//Not for current day
                 return checkStatus;

            }
           // return MED_SUCCESS;
           return checkStatus;

}




/*************************************************************************************

Function to transverse to all records in a EEPROM till
    (incrementing pages) till totalMedtoday or Tag and day match is  found
    Input - Tag

   Output - Display on GLCD
        MEDS_STATUS
                MED_ERROR- Medicine is not present in DB
                MED_REQ- Medicine required for today
                MED_NOT_REQ -Medicine not required for today
                MED_DONE - Medicine consumed for today
    uses checkMedicineToday()
************************************************************************************/
unsigned char updateMedicine(unsigned char tagValue[],timeclock  *Timecurr)
{
    unsigned char i=0,addr=0,pagecounter=0,checkvalue;
    MedStatus MedS2;
    MedStatus *checkStatus=&MedS2;
    checkStatus->status_bh=MED_ERROR;

    totalMedtoday = EEPROMRead(0,TOTALMEDTODAY);//Read store TOTALMEDTODAY value

    //transverse to all records in a EEPROM  (incrementing pages)
    //till totalMedtoday or Tag and day match is  found
    while ((pagecounter<<4) <= totalMedtoday && checkStatus->status_bh!=MED_REQ && checkStatus->status_bh!=MED_NOT_REQ && checkStatus->status_bh!=MED_DONE )//check if address exceeds medicine
    {
        addr=0;
        addr= addr|(pagecounter<<4);
        pagecounter++;
        checkStatus=checkMedicine(tagValue,Timecurr,addr);
    }
    //Depending on status display message on GLCD
    switch (checkStatus->status_bh){
    case MED_ERROR:
                    GLCD_RED=0;
                    displayUserString(2,"  Medicine not    ");
                    displayUserString(3,"     Present     ");
                    delay_seconds(4);
                    GLCD_RED=1;
                    CS=0;
                    clear_screen();
                    CS=1;
                    break;
    case MED_NOT_REQ:
                    GLCD_RED=0;
                    displayUserString(2,"  Medicine not  ");
                    displayUserString(3,"    Required   ");
                    delay_seconds(4);
                    CS=0;
                    clear_screen();
                    CS=1;
                    GLCD_RED=1;
                    break;

                    //Update the MED_STATUS From MED_REQ to MED_DONE
    case MED_REQ:  GLCD_GREEN=0;
                   displayUserString(2,"  Medicine Taken   ");
                   displayUserString(3,"     Updated   ");
                   EEPROMWrite(MEDCINE_DETAILS,((checkStatus->pageAddress)|MEDSTATUSLOCN),MED_DONE);
                   delay_seconds(4);
                   CS=0;
                   clear_screen();
                   CS=1;
                   GLCD_GREEN=1;
                   break;
    case MED_DONE: GLCD_GREEN=0;
                   displayUserString(2,"Medicine already   ");
                   displayUserString(3,"     Updated       ");
                   delay_seconds(4);
                   CS=0;
                   clear_screen();
                   CS=1;
                   GLCD_GREEN=1;
                    break;

    }
    return (checkStatus->status_bh);

}




