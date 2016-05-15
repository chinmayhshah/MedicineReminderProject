#ifndef timers_h
#define timers_h


#define WDTTimeout 0x07//Set for around 2 seconds(as FClk <12 Mhz)
#define TMOD0INI 0x01   //Timer 0 Initializing
#define MAX_ALARM 3
#define MINUTE_POS  0  //Store Minutes
#define SEC_POS     1  //Store Seconds
#define MSEC_POS    2  //Store Seconds
#define ENABLE_POS  3 //Enable or Disable
#define STATUS_POS  4 //



//SFR definition
__sfr __at (0xA6) WDTRST ;//SFR location of Watch Dog Reset
__sfr __at (0xA7) WDTPRG ;//SFR location of Watch Dog Program Reg

//Global Variable



//Prototype Declaration
void WDTInitilization ();
void avoidWDTReset ();
void Timer0Initialization();
void Timer();
void TimerDisplay(unsigned char row ,unsigned char col );
void resumeTimer();
void stopTimer();
void restartTimer();
//void editAlarm();
void disableAlarm(unsigned char alarmpos);
void enableAlarm(unsigned char alarmpos);
void editAlarm(unsigned char alarmpos,unsigned char minbyte,unsigned char secbyte,unsigned char msecbyte);
void menuTimer();
void timerReset();
void checkAlarm();
void stopAlarm();
void statusAlarm();
#endif // timers_h
