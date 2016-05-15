/****************************************************************************************
* Function to communicate with MFRFC522 RFID Reader
* Reference :
*   [1] MFRC522 DataSheet
*   [2] Arduino Library on Github
*       https://github.com/miguelbalboa/rfid
    [3] http://stm32f4-discovery.com/2014/07/library-23-read-rfid-tag-mfrc522-stm32f4xx-devices/
        [@Author -Tilen Majerle @email- tilen@majerle.eu ]
* **leveraged Registers from and referenced writing some functions[2]
    each function mentioned reference
*  Code changes Author for AT89C51RC2(8051 flavor) -
        Author -chinmay Shah
        Email  -chinmay.shah@colorado.edu
        Version - V1.1
* Mode for MFRC522 - Mode 0 for SPI
* Connections with AT89C51RC2-->MFRC522RFID_STMBASED.h

 /***********************************************************************************************************************

I) Functions written by chinmay.shah@colorado.edu
1)void MFRC522Init(void);
2) void MFRC522WriteRegister(unsigned char addr, unsigned char val);
3) unsigned char MFRC522ReadRegister(unsigned char addr);
4) void MFRC522SetBitMask(unsigned char reg, unsigned char mask);
5) void MFRC522ClearBitMask(unsigned char reg, unsigned char mask);
6) void MFRC522AntennaOn(void);
7) void MFRC522AntennaOff(void);
8) void MFRC522Reset(void);
9) extern unsigned char PCDCheck(unsigned char* id);
10)unsigned char PCDCompare(unsigned char* CardID, unsigned char* CompareID);



II) Functions referred from [2]
    and modified for 8051 by changing data types

1) MFRC522Status_t MFRC522Request(unsigned char reqMode, unsigned char* TagType);
2) MFRC522Status_t MFRC522ToCard(unsigned char command, unsigned char* sendData, unsigned char sendLen, unsigned char* backData, int* backLen);
3) MFRC522Status_t MFRC522Anticoll(unsigned char* serNum);

III) *Additional functions(for mutiple cards detected at same time and reading complete data on card ) ,
     *not required to be used for implementation of this project
     and modified for 8051 by changing data types

4) void MFRC522CalculateCRC(unsigned char* pIndata, unsigned char len, unsigned char* pOutData);
5) unsigned char MFRC522SelectTag(unsigned char* serNum);
6) MFRC522Status_t MFRC522Auth(unsigned char authMode, unsigned char BlockAddr, unsigned char* Sectorkey, unsigned char* serNum);
7) MFRC522Status_t MFRC522Read(unsigned char blockAddr, unsigned char* recvData);
8) MFRC522Status_t MFRC522Write(unsigned char blockAddr, unsigned char* writeData);
9) void MFRC522Halt(void);




*****************************************************************************************/

#include "MFRC522RFID_STMBASED.h"
/***************************************
TO Initialize RFID MFRC522
**************************************/
void MFRC522Init(void) {
	MFRC522Reset();//Soft Reset
	MFRC522WriteRegister(MFRC522_REG_T_MODE, 0x8D);// TAuto=1; MSB bits of Prescalerp[11:8] value 0x0D
                                                   // timer starts automatically at the end of the transmission in all communication modes at all speeds
	MFRC522WriteRegister(MFRC522_REG_T_PRESCALER, 0x3E);//Lower bits of PreScaler [7:0],TPrescaler =0xD3E
                                                    //ftimer = 13.56 MHz / (2*TPreScaler+1).
                                                    //fimmer = 2Khz (timer period 50 ms)
	MFRC522WriteRegister(MFRC522_REG_T_RELOAD_L, 30);//// Reload timer with 0x300 = 768
	MFRC522WriteRegister(MFRC522_REG_T_RELOAD_H, 0);
	/* 48dB gain */
	MFRC522WriteRegister(MFRC522_REG_RF_CFG, 0x70);//Gain = 48 db
    MFRC522WriteRegister(MFRC522_REG_TX_AUTO, 0x40);// Default 0x00. Force a 100 % ASK modulation independent of the ModGsPReg register setting
	MFRC522WriteRegister(MFRC522_REG_MODE, 0x3D);// Default 0x3F. Set the preset value for the CRC coprocessor for the CalcCRC command to 0x6363 (ISO 14443-3 part 6.2.4)
                                                 //Refer [2]
    MFRC522AntennaOn();		//Open the antenna
}

/***************************************
To Check for A card and read its values

o/p - id value - store value in id
**************************************/


MFRC522Status_t MFRC522Check(unsigned char* id) {
	MFRC522Status_t status;
	//Find cards, return card type
	status = MFRC522Request(PICC_REQIDL, id);//Check if card present
	if (status == MI_OK) { //Card detected

		status = MFRC522Anticoll(id); //Anti-collision, return card serial number 4 bytes
	}
	MFRC522Halt();	//Command card into hibernation

	return status;
}

/*************************************************************
To Compare a card value to a value present to another card
i/p - CardID and CompareID
o/p - MI_OK - If card match is successful
      MI_ERR - If card match is unsuccessful
*************************************************************/


MFRC522Status_t MFRC522Compare(unsigned char* CardID, unsigned char* CompareID) {
	unsigned char i;
	for (i = 0; i < 5; i++) {
		if (CardID[i] != CompareID[i]) {
			return MI_ERR;
		}
	}
	return MI_OK;
}


/*************************************************************
Function to write to a register
i/p - addr - Address of Register
      val - value  to be written on register
o/p - value to be written
      no return
*************************************************************/

void MFRC522WriteRegister(unsigned char addr, unsigned char val) {
	//CS low

	SS_RFID=0;
	//Send address
	SPI_Write((addr << 1) & 0x7E);// MSB - 1(always) and LSB - RD
	//Send data
	SPI_Write(val);
	//CS high
	SS_RFID=1;
}



/*************************************************************
Function to read from  a register
i/p - addr - Address of Register
      val - value  to be written on register
o/p - value of register return
*************************************************************/

unsigned char MFRC522ReadRegister(unsigned char addr) {
	unsigned char val;
	//CS low
	SS_RFID=0;;
    //Send Address
	SPI_Write(((addr << 1) & 0x7E) | 0x80);// MSB - 1(always) and LSB - RD
    //Dummy address read on MOSI
	val = SPI_Read(0);//Read from MISO
	//CS high
	SS_RFID=1;

	return val;
}


/*************************************************************
Function to mask(set) a bit of  a register
i/p - reg - Address of Register
      mask - mask value to be written
o/p - no return
*************************************************************/

void MFRC522SetBitMask(unsigned char reg, unsigned char mask) {
	MFRC522WriteRegister(reg, MFRC522ReadRegister(reg) | mask);
}


/*************************************************************
Function to unmask(clear) a bit of  a register
i/p - reg - Address of Register
      mask - mask value to be written
o/p - no return
*************************************************************/

void MFRC522ClearBitMask(unsigned char reg, unsigned char mask){
	MFRC522WriteRegister(reg, MFRC522ReadRegister(reg) & (~mask));
}



/*************************************************************
Function to On Antenna
i/p - No input
o/p - no return
*************************************************************/


void MFRC522AntennaOn(void) {
	unsigned char temp;

	temp = MFRC522ReadRegister(MFRC522_REG_TX_CONTROL);//Read
	if (!(temp & 0x03)) {//Dont Set if already set ,as it off the antenna
		MFRC522SetBitMask(MFRC522_REG_TX_CONTROL, 0x03);
	}
}



/*************************************************************
Function to Off Antenna
i/p - No input
o/p - no return
*************************************************************/
void MFRC522AntennaOff(void) {
	MFRC522ClearBitMask(MFRC522_REG_TX_CONTROL, 0x03);
}
/*************************************************************
Function for a Soft Reset of RC522
i/p - No input
o/p - no return
*************************************************************/
void MFRC522Reset(void) {
	MFRC522WriteRegister(MFRC522_REG_COMMAND, PCD_RESETPHASE);
}

/*************************************************************
Function for Checking if RFID card is detected
i/p - No input
o/p - return Status code
*************************************************************/

unsigned char PCDCheck(unsigned char* id) {
	unsigned char status,i=0;
	//Find cards, return card type
	status = MFRC522Request(PICC_REQIDL, id);
	if (status == MI_OK) {
		//Card detected
		//Anti-collision, return card serial number 4 bytes
		status = MFRC522Anticoll(id);
	}
	MFRC522Halt();			//Command card into hibernation

	return status;
}

/*************************************************************
Function to compare detected card and card stored
*************************************************************/
unsigned char PCDCompare(unsigned char* CardID, unsigned char* CompareID) {
	unsigned char i;

	for (i = 0; i < 5; i++) {
        printf("%02x",CardID[i]);
	}
	for (i = 0; i < 5; i++) {
		if (CardID[i] != CompareID[i]) {
			return MI_ERR;
		}
	}
	return MI_OK;
}

MFRC522Status_t MFRC522Request(unsigned char reqMode, unsigned char* TagType) {
	MFRC522Status_t status;
	int backBits;			//The received data bits

	MFRC522WriteRegister(MFRC522_REG_BIT_FRAMING, 0x07);		//Bit Framing

	TagType[0] = reqMode;
	status = MFRC522ToCard(PCD_TRANSCEIVE, TagType, 1, TagType, &backBits);

	if ((status != MI_OK) || (backBits != 0x10)) {
		status = MI_ERR;
	}

	return status;
}

MFRC522Status_t MFRC522ToCard(unsigned char command, unsigned char * sendData, unsigned char  sendLen, unsigned char* backData, int* backLen) {

	MFRC522Status_t status = MI_ERR;
	unsigned char irqEn = 0x00;
	unsigned char waitIRq = 0x00;
	unsigned char lastBits;
	unsigned char n;
	int i;

	switch (command) {
		case PCD_AUTHENT: {
			irqEn = 0x12;
			waitIRq = 0x10;
			break;
		}
		case PCD_TRANSCEIVE: {
			irqEn = 0x77;
			waitIRq = 0x30;
			break;
		}
		default:
			break;
	}

	MFRC522WriteRegister(MFRC522_REG_COMM_IE_N, irqEn | 0x80);
	MFRC522ClearBitMask(MFRC522_REG_COMM_IRQ, 0x80);
	MFRC522SetBitMask(MFRC522_REG_FIFO_LEVEL, 0x80);
    MFRC522WriteRegister(MFRC522_REG_COMMAND, PCD_IDLE);

	//Writing data to the FIFO
	for (i = 0; i < sendLen; i++) {
		MFRC522WriteRegister(MFRC522_REG_FIFO_DATA, sendData[i]);
	}

	//Execute the command
	MFRC522WriteRegister(MFRC522_REG_COMMAND, command);
	if (command == PCD_TRANSCEIVE) {
		MFRC522SetBitMask(MFRC522_REG_BIT_FRAMING, 0x80);		//StartSend=1,transmission of data starts
	}

	//Waiting to receive data to complete
	i = 2000;	//i according to the clock frequency adjustment, the operator M1 card maximum waiting time 25ms???
	do {
		//CommIrqReg[7..0]
		//Set1 TxIRq RxIRq IdleIRq HiAlerIRq LoAlertIRq ErrIRq TimerIRq
		n = MFRC522ReadRegister(MFRC522_REG_COMM_IRQ);
		i--;
	} while ((i!=0) && !(n&0x01) && !(n&waitIRq));

	MFRC522ClearBitMask(MFRC522_REG_BIT_FRAMING, 0x80);			//StartSend=0

	if (i != 0)  {
		if (!(MFRC522ReadRegister(MFRC522_REG_ERROR) & 0x1B)) {
			status = MI_OK;
			if (n & irqEn & 0x01) {
				status = MI_NOTAGERR;
			}

			if (command == PCD_TRANSCEIVE) {
				n = MFRC522ReadRegister(MFRC522_REG_FIFO_LEVEL);
				lastBits = MFRC522ReadRegister(MFRC522_REG_CONTROL) & 0x07;
				if (lastBits) {
					*backLen = (n - 1) * 8 + lastBits;
				} else {
					*backLen = n * 8;
				}

				if (n == 0) {
					n = 1;
				}
				if (n > MFRC522_MAX_LEN) {
					n = MFRC522_MAX_LEN;
				}

				//Reading the received data in FIFO
				for (i = 0; i < n; i++) {
					backData[i] = MFRC522ReadRegister(MFRC522_REG_FIFO_DATA);
				}
			}
		} else {
			status = MI_ERR;
		}
	}

	return status;
}

MFRC522Status_t MFRC522Anticoll(unsigned char * serNum) {
	MFRC522Status_t status;
	unsigned char  i;
	unsigned char  serNumCheck = 0;
	int unLen;

	MFRC522WriteRegister(MFRC522_REG_BIT_FRAMING, 0x00);		//TxLastBists = BitFramingReg[2..0]

	serNum[0] = PICC_ANTICOLL;
	serNum[1] = 0x20;
	status = MFRC522ToCard(PCD_TRANSCEIVE, serNum, 2, serNum, &unLen);

	if (status == MI_OK) {
		//Check card serial number
		for (i = 0; i < 4; i++) {
			serNumCheck ^= serNum[i];
		}
		if (serNumCheck != serNum[i]) {
			status = MI_ERR;
		}
	}
	return status;
}

void MFRC522CalculateCRC(unsigned char *  pIndata, unsigned char  len, unsigned char * pOutData) {
	unsigned char  i, n;

	MFRC522ClearBitMask(MFRC522_REG_DIV_IRQ, 0x04);			//CRCIrq = 0
	MFRC522SetBitMask(MFRC522_REG_FIFO_LEVEL, 0x80);			//Clear the FIFO pointer
	//Write_MFRC522(CommandReg, PCD_IDLE);

	//Writing data to the FIFO
	for (i = 0; i < len; i++) {
		MFRC522WriteRegister(MFRC522_REG_FIFO_DATA, *(pIndata+i));
	}
	MFRC522WriteRegister(MFRC522_REG_COMMAND, PCD_CALCCRC);

	//Wait CRC calculation is complete
	i = 0xFF;
	do {
		n = MFRC522ReadRegister(MFRC522_REG_DIV_IRQ);
		i--;
	} while ((i!=0) && !(n&0x04));			//CRCIrq = 1

	//Read CRC calculation result
	pOutData[0] = MFRC522ReadRegister(MFRC522_REG_CRC_RESULT_L);
	pOutData[1] = MFRC522ReadRegister(MFRC522_REG_CRC_RESULT_M);
}

unsigned char  MFRC522SelectTag(unsigned char * serNum) {
	unsigned char  i;
	MFRC522Status_t status;
	unsigned char  size;
	int recvBits;
	unsigned char  buffer[9];

	buffer[0] = PICC_SElECTTAG;
	buffer[1] = 0x70;
	for (i = 0; i < 5; i++) {
		buffer[i+2] = *(serNum+i);
	}
	MFRC522CalculateCRC(buffer, 7, &buffer[7]);		//??
	status = MFRC522ToCard(PCD_TRANSCEIVE, buffer, 9, buffer, &recvBits);

	if ((status == MI_OK) && (recvBits == 0x18)) {
		size = buffer[0];
	} else {
		size = 0;
	}

	return size;
}

MFRC522Status_t MFRC522Auth(unsigned char  authMode, unsigned char  BlockAddr, unsigned char * Sectorkey, unsigned char * serNum) {
	MFRC522Status_t status;
	int recvBits;
	unsigned char  i;
	unsigned char  buff[12];

	//Verify the command block address + sector + password + card serial number
	buff[0] = authMode;
	buff[1] = BlockAddr;
	for (i = 0; i < 6; i++) {
		buff[i+2] = *(Sectorkey+i);
	}
	for (i=0; i<4; i++) {
		buff[i+8] = *(serNum+i);
	}
	status = MFRC522ToCard(PCD_AUTHENT, buff, 12, buff, &recvBits);

	if ((status != MI_OK) || (!(MFRC522ReadRegister(MFRC522_REG_STATUS2) & 0x08))) {
		status = MI_ERR;
	}

	return status;
}

MFRC522Status_t MFRC522Read(unsigned char  blockAddr, unsigned char * recvData) {
	MFRC522Status_t status;
	int unLen;

	recvData[0] = PICC_READ;
	recvData[1] = blockAddr;
	MFRC522CalculateCRC(recvData,2, &recvData[2]);
	status = MFRC522ToCard(PCD_TRANSCEIVE, recvData, 4, recvData, &unLen);

	if ((status != MI_OK) || (unLen != 0x90)) {
		status = MI_ERR;
	}

	return status;
}

MFRC522Status_t MFRC522Write(unsigned char  blockAddr, unsigned char * writeData) {
	MFRC522Status_t status;
	int recvBits;
	unsigned char  i;
	unsigned char  buff[18];

	buff[0] = PICC_WRITE;
	buff[1] = blockAddr;
	MFRC522CalculateCRC(buff, 2, &buff[2]);
	status = MFRC522ToCard(PCD_TRANSCEIVE, buff, 4, buff, &recvBits);

	if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A)) {
		status = MI_ERR;
	}

	if (status == MI_OK) {
		//Data to the FIFO write 16Byte
		for (i = 0; i < 16; i++) {
			buff[i] = *(writeData+i);
		}
		MFRC522CalculateCRC(buff, 16, &buff[16]);
		status = MFRC522ToCard(PCD_TRANSCEIVE, buff, 18, buff, &recvBits);

		if ((status != MI_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A)) {
			status = MI_ERR;
		}
	}

	return status;
}

void MFRC522Halt(void) {
	int unLen;
	unsigned char  buff[4];

	buff[0] = PICC_HALT;
	buff[1] = 0;
	MFRC522CalculateCRC(buff, 2, &buff[2]);

	MFRC522ToCard(PCD_TRANSCEIVE, buff, 4, buff, &unLen);
}





