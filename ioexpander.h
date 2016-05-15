#ifndef IO_EXPANDER_H
#define IO_EXPANDER_H


#define IOEXP_ADDRWR 0x40 //Address for write
#define IOEXP_ADDRRD 0x41 //Address for Read

#define IOEXP_MAXDATA 0x08  //Maximum bits of I/O Expander



void IOWrite(unsigned char byte);
unsigned char IORead();
void IOSetInput(unsigned char bitposition );
void IOSetOutput(unsigned char bitposition );
unsigned char IOReadBit(unsigned char bitposition);
void IOMenu();



#endif // IO_EXPANDER_H
