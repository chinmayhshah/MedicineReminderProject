#ifndef constraints_h
#define constraints_h

#define MAXADDR_LENGTH 3
#define MAXDATA_LENGTH 2

//Function Prototypes
unsigned char checkInputCharacter(char input);
unsigned char *StringNumber(unsigned char type);
unsigned int ASCIItoNumber(unsigned char number[],unsigned char type);
unsigned char hexDisplay (unsigned char startAddr[],unsigned char maxLine,unsigned char endAddr[]);
unsigned char *hextoASCII(unsigned char hexvalue);
#endif // constraints_h




