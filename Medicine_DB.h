/*****************************************************************************************
Application code functions and Macros header file for Medicine Reminder functionality

*******************************************************************************************/
#ifndef MedicineDB_h
#define MedicineDB_h
#include "RTC_DS1307.h"
#define RFID_TAG_MAX 5 //Max number of bytes for Tag value
#define WINDOW_HOURS 2 //Window of hours to be checked

#define MAX_MEDICINES   0x0F //maximum number of Medicine details can be occupied

#define MEDCINE_DETAILS 0x07 //Block of EEPROM for storing Data

#define MEDCINE_NAME    0x02 //Block of EEPROM for storing Data
                            //Reserved for further application
#define TOTALMED        0x00 //location to store max Med added
#define TOTALMEDTODAY   0x01 //location to store /update total Med for today
#define WAITTAGDETECT   0x25 //Wait count for Tag to be detected

//location of page for a particular record in  EEPROM
#define MEDSTATUSLOCN   0x0A//Med Status of Medicine_Status_code
#define MEDDTELOCN      0x05//Record Date
#define MEDMONLOCN      0x06//Record Month
#define MEDYRLOCN       0x07//Record Day
#define MEDHRSLOCN      0x08//Record Hours
#define MEDMINLOCN      0x09//Record Minutes


//Enumerated Structure for Medicine Status
typedef enum
{
    MED_ERROR=0,//Medicine not present
                //Hour window or day didnot match with record
    MED_SUCCESS=1,//Medicine record added  success
    MED_FAIL=2,//Medicine record failed
    MED_NOT_REQ=3,//Medicine not required
    MED_REQ=4,//Medicine required
    MED_DONE=5//Medicine consumed
}Medicine_Status_code;


typedef struct Medicine
{
    unsigned char medicineTag[RFID_TAG_MAX+1];//Array to store Tag values
    timeclock clock;
    unsigned char status_bh ;//Day
    unsigned char pageAddress ;
}Med;

typedef struct Medicine_status
{
    Medicine_Status_code status_bh ;//Day
    unsigned char pageAddress ;
}MedStatus;


////Prototype of functions

MedStatus * checkMedicine(unsigned char tagValue[],timeclock  *Timecurr,unsigned char pageaddr);
void insertMedicine(unsigned char tag[],timeclock  *Timecurr);
unsigned char checkMedicineToday(unsigned char tagValue[],timeclock  *Timecurr);
unsigned char updateMedicine(unsigned char tagValue[],timeclock  *Timecurr);




#endif // MedicineDB_h


