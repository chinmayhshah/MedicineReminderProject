 /***********************************************************************
 Prototype functions and registers

Author @chinmay.shah@colorado.edu
Reference :http://www.ti.com/lit/ds/symlink/tsc2013-q1.pdf
**************************************************************************/


#ifndef TSC2013_Q1_h
#define TSC2013_Q1_h

#define TSC2013_SLAVE_ADDRESS 0x90
#define TSC2013_CONTROL_BYTE1         0x80//Start Conversion control byte
#define TSC2013_CONTROL_BYTE0         0x00//

//Registers (as Register address is D6,D5,D4,D3 --- A3 to A0)
#define TSC2013_X1_REG                0x00 << 3//
#define TSC2013_X2_REG                0x01 << 3//
#define TSC2013_Y1_REG                0x02 << 3//
#define TSC2013_Y2_REG                0x03 << 3//
#define TSC2013_IX_REG                0x04 << 3//
#define TSC2013_IY_REG                0x05 << 3//
#define TSC2013_Z1_REG                0x06 << 3//
#define TSC2013_Z2_REG                0x07 << 3//
#define TSC2013_STATUS_REG            0x08 << 3//
#define TSC2013_AUX_REG               0x09 << 3//
#define TSC2013_CONFIG_REG0           0x0C << 3//
#define TSC2013_CONFIG_REG1           0x0D << 3//
#define TSC2013_CONFIG_REG2           0x0E << 3//
#define TSC2013_CONFUNC_REG           0x0F << 3//


//PIN Definition
#define TSC2013_RESET                 P3_4




typedef enum
{
    POS1=1,
    POS2=2,
    POS3=3,
    POS4=4,
    POS5=5
}touchPosition;

typedef struct touchLocation
{
    unsigned char TX1;
    unsigned char TX2;
    unsigned char TY1;
    unsigned char TY2;
    unsigned char TZ1;
    unsigned char TZ2;
    unsigned char TIX;
    unsigned char TIY;
    unsigned char Status;
    unsigned char AUX;

    //fourth quadrant of X,Y Axis
    touchPosition position;
}TSCLocation;


void TSC2013_Init();
void TSCWrite(unsigned char addr,unsigned char databyteMSB,unsigned char databyteLSB);
void TSCStartConversion();
unsigned char TSCRead(unsigned char addr);
void TSCRegRead ();
touchPosition TSCLocationRead ();
void TSCReset();

#endif // TSC2013_Q1_h
