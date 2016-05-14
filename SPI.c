/************************************************************************************
* Functions to use SPI Protocol
* Reading:
*   [1] AT89C51 DataSheet
Coded written by chinmay.shah@colorado.edu

***********************************************************************************/
#include <at89c51ed2.h>
#include "SPI.h"
#include "delay_custom.h"


/*********************************************
Function to write using SPI to slave
Input - SPIWriteData - Data to be written(MOSI)
*********************************************/
void SPI_Write (unsigned char SPIWriteData)
{
    SPDAT=SPIWriteData;//Sample data to be send
    while (SPSTA != 0x80);//Check if SPIF is set
   SPSTA &= ~0x80;//Reset SPIF flag
}


/*********************************************
Function to Read using SPI from slave
Input -Address , It acts as dummy address on MOSI
output -
        return Data read from Slave(MISO)
*********************************************/
unsigned char SPI_Read (unsigned char SPIAddr)

{
    unsigned char SPIReadData=0x00;
    SPDAT=SPIAddr;//Data send to generate SCK Signal,Normally zero
    while (SPSTA != 0x80);//Check if SPIF is set
    SPSTA &= ~0x80;//Reset SPIF flag
    SPIReadData = SPDAT;// Read data from Slave
    return SPIReadData;
}




/*********************************************
Function to Read using SPI from slave
    without providing any address
output -
        return Data read from Slave(MISO)
*********************************************/
unsigned char SPINoAddressRead (void)

{
    unsigned char SPIReadData=0x00;
    SPIReadData = SPDAT;//read data
    SPSTA &= ~0x80;//Reset SPIF flag
    return SPIReadData;
}



/*********************************************
Function to Initialize SPI Protocol

1) Initialize SPI for polling mode
2) Select Clock Peripheral
3) AT89C51 always acts as Master
4) Use Mode 0

*********************************************/

void SPI_Init()
{

    SPCON &= 0xBF;                /* RUN SPI */
    delay_us();
    SPCON |= 0x10;                /* Master mode */
    SPCON |= 0x82;                /* Fclk Periph/128 */
    SPCON |= 0x20;                /* To not /SS */
    SPCON &= 0xF7;               /* CPOL=0;  */
    SPCON &= ~0x04;                /* CPHA=0;  */
    IEN1 |= 0x04;                 /* enable spi interrupt */
    SPCON |= 0x40;                /* run spi */
}




/*********************************************
Extra Function to Initialize SPI Protocol

1) Initialize SPI for polling mode
2) Select Clock Peripheral
3) AT89C51 always acts as Master
4) Use Mode 0
** not used
*********************************************/
void SPI_Intialization()
{
    SPCON |= MASTER_MODE; //Mode to be Master
    SPCON |=SPICLK_128; //FClk to be = CLK Peri/128
    //choosing Mode zero operation for RFID and GLCD
    SPCON &= ~CPOL_IDLE; //CPOL IDLE STATE
    //SPCON |= CPHA_NOTIDLE;//CPHA SCK not in mode
    SPCON &= ~CPHA_NOTIDLE;//CPHA SCK not in mode
    SPCON |= SPEN_BIT;//Start SPI

}


