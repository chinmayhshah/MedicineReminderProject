/************************************************************************************
* Prototype Functions,Register definitions to use SPI Protocol
* Reading:
*   [1] AT89C51 DataSheet
Complete Coded written by chinmay.shah@colorado.edu

***********************************************************************************/
#ifndef SPI_h
#define SPI_h

//Macro  Definitions

#define MASTER_MODE 0x10
#define SPICLK_128  0x82
#define CPOL_IDLE   0x08
#define CPHA_NOTIDLE  0x04
#define SPEN_BIT  0x40






/********************************************
*   SFR Definition
*********************************************/
__sfr __at (0xC3) SPCON ;
__sfr __at (0xC4) SPSTA ;
__sfr __at (0xC5) SPDAT ;

//Prototype Definitions
void SPI_Intialization();
void SPI_Write (unsigned char SPIWriteData);
unsigned char SPI_Read (unsigned char SPIAddr);
void SPI_Init();
unsigned char SPINoAddressRead ();

#endif // SPI_h
