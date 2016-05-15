#ifndef Menu_h
#define Menu_h

#define INTR_INT0 10000001
#define DEBUG


volatile unsigned char tempcount=0x00;

void Mode(unsigned char x);
void menuEPPROMDisplay();
unsigned char WriteByte();
unsigned char ReadByte();
void LCDDisplay();
void ClearDisplay();
void ClearLCD();
void HexDump();
void DDRAMDump();
void CGRAMDump();
void BCDCounter();
void INT0Initilization();
void timerMode ();
void menu();
unsigned char blockWrite (unsigned char startAddr[],unsigned char endAddr[],unsigned char hexdata);


#endif // Menu_h

