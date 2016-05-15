#ifndef EEPROM_h
#define EPPROM_h

/* Macro Definition */
#define WRCMD 0xA0 // Command for Write
#define RDCMD 0xA1  //Command for Read
//extern volatile unsigned char WRCMD;
//extern volatile unsigned char RDCMD;

/* Prototype definition  */

void EEPROMWrite(unsigned char block, char addr,unsigned char databyte);
unsigned char EEPROMRead(unsigned char block,unsigned char addr);
void menuEPPROMDisplay();
void Mode(unsigned char x);
void EEPROMReset();
void EEPROMBlockFill(unsigned char block, char startaddr,unsigned char datadiff,unsigned char filldata);
unsigned char EEPROMPageWrite(unsigned char block, char startaddr,unsigned char datadiff,unsigned char filldata);
unsigned char EEPROMPageWriteArray(unsigned char block, char startaddr,unsigned char datadiff,unsigned char filldata[]);

#endif // EEPROM_h
