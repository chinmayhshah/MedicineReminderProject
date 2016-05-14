#ifndef MFMFRC522RFID_STMBASED_h
#define MFMFRC522RFID_STMBASED_h

/******************************************************************************************************
 * Mifare MFRC522 RFID Card reader
 * It works on 13.56 MHz.
 *
 *  This library uses SPI for driving MFRC255 chip written by -chinmay.shah@colorado.edu.
 *  Leverage registers from
 *   [2] http://stm32f4-discovery.com/2014/07/library-23-read-rfid-tag-mfrc522-stm32f4xx-devices/
        [@Author -Tilen Majerle @email- tilen@majerle.eu ]
 *	@author 	Tilen Majerle
 *	@email		tilen@majerle.eu
 *

 * Comments for AT89C51RC2 - MFRC522 connections (chinmay.shah@colorado.edu)
 * MF RC522 Default pinout
 *
 * 		MFRC522		8051	DESCRIPTION
 *	    NSS     	P1_0    	Chip select for SPI
 *		SCK			SCK(P1.6)	Serial Clock for SPI
 *		MISO		MISO(P1.5)	Master In Slave Out for SPI
 *		MOSI		MOSI(P1.7)	Master Out Slave In for SPI
 *		GND			GND			Ground
 *		VCC			NA  		3.3V power(LD1117 (3.3 Regulator - approx 3.29V provided))
 *		RST			NC		    Reset pin   - Not Connected
 *      NA          VCC         5 V
 */
/**
 * Library dependencies
 * - SPI.h
 * - at89c51ed2.h
*/

#include <at89c51ed2.h>
#include "SPI.h"
#include "stdio.h"


/**
 * Status enumeration
 *
 * Used with most functions
 */
typedef enum {
	MI_OK = 0,
	MI_NOTAGERR,
	MI_ERR
} MFRC522Status_t;


//Slave Select for RFID
#define SS_RFID P1_0



/* MFRC522 Commands */
#define PCD_IDLE						0x00   //NO action; Cancel the current command
#define PCD_AUTHENT						0x0E   //Authentication Key
#define PCD_RECEIVE						0x08   //Receive Data
#define PCD_TRANSMIT					0x04   //Transmit data
#define PCD_TRANSCEIVE					0x0C   //Transmit and receive data,
#define PCD_RESETPHASE					0x0F   //Reset
#define PCD_CALCCRC						0x03   //CRC Calculate

/* Mifare_One card command word */
#define PICC_REQIDL						0x26   // find the antenna area does not enter hibernation
#define PICC_REQALL						0x52   // find all the cards antenna area
#define PICC_ANTICOLL					0x93   // anti-collision
#define PICC_SElECTTAG					0x93   // election card
#define PICC_AUTHENT1A					0x60   // authentication key A
#define PICC_AUTHENT1B					0x61   // authentication key B
#define PICC_READ						0x30   // Read Block
#define PICC_WRITE						0xA0   // write block
#define PICC_DECREMENT					0xC0   // debit
#define PICC_INCREMENT					0xC1   // recharge
#define PICC_RESTORE					0xC2   // transfer block data to the buffer
#define PICC_TRANSFER					0xB0   // save the data in the buffer
#define PICC_HALT						0x50   // Sleep

/* MFRC522 Registers */
//Page 0: Command and Status
#define MFRC522_REG_RESERVED00			0x00
#define MFRC522_REG_COMMAND				0x01
#define MFRC522_REG_COMM_IE_N			0x02
#define MFRC522_REG_DIV1_EN				0x03
#define MFRC522_REG_COMM_IRQ			0x04
#define MFRC522_REG_DIV_IRQ				0x05
#define MFRC522_REG_ERROR				0x06
#define MFRC522_REG_STATUS1				0x07
#define MFRC522_REG_STATUS2				0x08
#define MFRC522_REG_FIFO_DATA			0x09
#define MFRC522_REG_FIFO_LEVEL			0x0A
#define MFRC522_REG_WATER_LEVEL			0x0B
#define MFRC522_REG_CONTROL				0x0C
#define MFRC522_REG_BIT_FRAMING			0x0D
#define MFRC522_REG_COLL				0x0E
#define MFRC522_REG_RESERVED01			0x0F
//Page 1: Command
#define MFRC522_REG_RESERVED10			0x10
#define MFRC522_REG_MODE				0x11
#define MFRC522_REG_TX_MODE				0x12
#define MFRC522_REG_RX_MODE				0x13
#define MFRC522_REG_TX_CONTROL			0x14
#define MFRC522_REG_TX_AUTO				0x15
#define MFRC522_REG_TX_SELL				0x16
#define MFRC522_REG_RX_SELL				0x17
#define MFRC522_REG_RX_THRESHOLD		0x18
#define MFRC522_REG_DEMOD				0x19
#define MFRC522_REG_RESERVED11			0x1A
#define MFRC522_REG_RESERVED12			0x1B
#define MFRC522_REG_MIFARE				0x1C
#define MFRC522_REG_RESERVED13			0x1D
#define MFRC522_REG_RESERVED14			0x1E
#define MFRC522_REG_SERIALSPEED			0x1F
//Page 2: CFG
#define MFRC522_REG_RESERVED20			0x20
#define MFRC522_REG_CRC_RESULT_M		0x21
#define MFRC522_REG_CRC_RESULT_L		0x22
#define MFRC522_REG_RESERVED21			0x23
#define MFRC522_REG_MOD_WIDTH			0x24
#define MFRC522_REG_RESERVED22			0x25
#define MFRC522_REG_RF_CFG				0x26
#define MFRC522_REG_GS_N				0x27
#define MFRC522_REG_CWGS_PREG			0x28
#define MFRC522_REG__MODGS_PREG			0x29
#define MFRC522_REG_T_MODE				0x2A
#define MFRC522_REG_T_PRESCALER			0x2B
#define MFRC522_REG_T_RELOAD_H			0x2C
#define MFRC522_REG_T_RELOAD_L			0x2D
#define MFRC522_REG_T_COUNTER_VALUE_H	0x2E
#define MFRC522_REG_T_COUNTER_VALUE_L	0x2F
//Page 3:TestRegister
#define MFRC522_REG_RESERVED30			0x30
#define MFRC522_REG_TEST_SEL1			0x31
#define MFRC522_REG_TEST_SEL2			0x32
#define MFRC522_REG_TEST_PIN_EN			0x33
#define MFRC522_REG_TEST_PIN_VALUE		0x34
#define MFRC522_REG_TEST_BUS			0x35
#define MFRC522_REG_AUTO_TEST			0x36
#define MFRC522_REG_VERSION				0x37
#define MFRC522_REG_ANALOG_TEST			0x38
#define MFRC522_REG_TEST_ADC1			0x39
#define MFRC522_REG_TEST_ADC2			0x3A
#define MFRC522_REG_TEST_ADC0			0x3B
#define MFRC522_REG_RESERVED31			0x3C
#define MFRC522_REG_RESERVED32			0x3D
#define MFRC522_REG_RESERVED33			0x3E
#define MFRC522_REG_RESERVED34			0x3F
//Dummy byte
#define MFRC522_DUMMY					0x00

#define MFRC522_MAX_LEN					16

/**
 * Initialize MFRC522 RFID reader
 * Prepare MFRC522 to work with RFIDs
 */
extern void MFRC522Init(void);
extern MFRC522Status_t MFRC522Check(unsigned char* id);
extern MFRC522Status_t MFRC522Compare(unsigned char* CardID, unsigned char* CompareID);

/**
 * Private functions
 */
extern void MFRC522InitPins(void);
extern void MFRC522WriteRegister(unsigned char addr, unsigned char val);
extern unsigned char MFRC522ReadRegister(unsigned char addr);
extern void MFRC522SetBitMask(unsigned char reg, unsigned char mask);
extern void MFRC522ClearBitMask(unsigned char reg, unsigned char mask);
extern void MFRC522AntennaOn(void);
extern void MFRC522AntennaOff(void);
extern void MFRC522Reset(void);
extern MFRC522Status_t MFRC522Request(unsigned char reqMode, unsigned char* TagType);
extern MFRC522Status_t MFRC522ToCard(unsigned char command, unsigned char* sendData, unsigned char sendLen, unsigned char* backData, int* backLen);
extern MFRC522Status_t MFRC522Anticoll(unsigned char* serNum);
extern void MFRC522CalculateCRC(unsigned char* pIndata, unsigned char len, unsigned char* pOutData);
extern unsigned char MFRC522SelectTag(unsigned char* serNum);
extern MFRC522Status_t MFRC522Auth(unsigned char authMode, unsigned char BlockAddr, unsigned char* Sectorkey, unsigned char* serNum);
extern MFRC522Status_t MFRC522Read(unsigned char blockAddr, unsigned char* recvData);
extern MFRC522Status_t MFRC522Write(unsigned char blockAddr, unsigned char* writeData);
extern void MFRC522Halt(void);

extern unsigned char PCDCheck(unsigned char* id);
unsigned char PCDCompare(unsigned char* CardID, unsigned char* CompareID);

#endif // MFMFRC522RFID_STMBASED_h

