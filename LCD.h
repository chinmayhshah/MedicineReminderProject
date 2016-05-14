#ifndef LCD_h
#define LCD_h
#define LCD_ADDR 0xF000 // Memory address of LCD
#define LCD_CMD(x) ((*(xdata unsigned char *)LCD_ADDR) = (xdata unsigned char)x)

#define RS P1_1 // Register Select Port 1 ,1 bit
#define RW P1_2 // Read /Write, Port 1,2 bit

#define BY P0_7 // Busy Flag= D7 = Port 0 ,7 bit

#define LCD_LINE0   0x00 // Line 0 of LCD
#define LCD_LINE1   0x40 // Line 1 of LCD
#define LCD_LINE2   0x10 // Line 2 of LCD
#define LCD_LINE3   0x50 // Line 3 of LCD
#define LCD_DB7     0x80 // DB7=1 bit of Command for each line
#define LCD_MAXROW  0x03 // maximum number of rows for LCD`
#define LCD_MAXCOL  0x0F //maximum number of columns for each row of LCD

#define LCD_MAXCHAR 8   //Maximum number of custom characters



//For Reading data from LCD
#define LCD_READ *(xdata unsigned char *)0xF000;
xdata unsigned char read ;
xdata unsigned char lcd_row;
xdata unsigned char lcd_column;


/**Prototype definition **/
void LCDCommandWrite(unsigned char cmd);
void LCDCommandRead();
void LCDBusyCheck();
void LCDInitialization();
void LCDDataWrite(unsigned char data);
unsigned char LCDDataRead();
void LCDGotoaddress(unsigned char addr);
void LCDGotoxy(unsigned char row,unsigned char column);
void LCDPutchar(unsigned char tempchar);
void LCDPutstr(char *str);
void LCDClear();
void LCDCreateChar(unsigned char code,unsigned char rowvalue[]);
void LCDMenu();
//void LCDCreateChar();


#endif // LCD_h
