/****************************************************************************************
    Function to communicate with MFRFC522 RFID Reader
*



*Reference :
*   [1] MFRC522 DataSheet
*   [2] Arduino Library on Github
*       https://github.com/miguelbalboa/rfid
    [3] http://stm32f4-discovery.com/2014/07/library-23-read-rfid-tag-mfrc522-stm32f4xx-devices/
* leveraged Registers and referenced writing different some functions  from [1] and others from [2]
*  Code changes Author for 8051 -chinmay.shah@colorado.edu



* Mode for MFRC522 - Mode 0 for SPI
*

*****************************************************************************************/
#include "SPI.h"
#include "MFRC522RFID.h"
#include <at89c51ed2.h>
#include <stdio.h>
#include "delay_custom.h"

/************************************************
Write Single value at an address
**************************************************/
void PCDRFIDWriteRegister(unsigned char  regaddr, unsigned char bytevalue)
{
    SS_RFID=0;

    SPI_Write(regaddr & MFRC522_WRITE);

    SPI_Write(bytevalue);

    SS_RFID=1;
}



/************************************************
Write n value at n Locations with start address
**************************************************/

void PCDBlockWriteRegister(unsigned char  regaddr,unsigned char length ,unsigned char bytevalue[])
{
    unsigned char i=0;

    SS_RFID=0;

    SPI_Write(regaddr & MFRC522_WRITE);
    while (i<length)
    {
            SPI_Write(bytevalue[i++]);
    }

    SS_RFID=1;
}

/**************************************************************
Reads a single byte from a particular address

**********************************************************/

unsigned char PCDReadRegister(unsigned char  regaddr)
{
    unsigned char  bytedata;
    SS_RFID=0;
    SPI_Write(MFRC522_READ|(regaddr & MFRC522_WRITE));
                    //MSB = 1- Read mode , LSB=0
    bytedata=SPI_Read(0);
    SS_RFID=1;
    return bytedata;
}

/**************************************************************
Reads a multiple byte from a particular address

**********************************************************/
//
//unsigned char *PCDBlockReadRegister(unsigned char  regaddr,unsigned char length)
//{
//    unsigned char *bytedata=NULL,i=0;
//    if (!length)
//    {
//        return 0;
//    }
//    SS_RFID=0;//Select Slave RFID
//    SPI_Write(MFRC522_READ|(regaddr & MFRC522_WRITE));//MSB = 1- Read mode , LSB=0
//
//    while (i<length)
//    {
//        bytedata[i++]=SPI_Read(0);//store each data
//        //printf("%02x",bytedata[i]);
//    }
//    bytedata[i]=SPI_Read(0);//capture last data
//
//    SS_RFID=1;
//
//    return (char *)bytedata;
//}





unsigned char *PCDBlockReadRegister(
                                unsigned char reg,		///< The register to read from. One of the PCD_Register enums.
								unsigned char count,		///< The number of bytes to read
									///< Byte array to store the values in.
								unsigned char rxAlign	///< Only bit positions rxAlign..7 in values[0] are updated.
								) {
    unsigned char address  ,index ,mask,i,value;
    unsigned char *values ;
	if (count == 0) {
		return;
	}
	//Serial.print(F("Reading ")); 	Serial.print(count); Serial.println(F(" bytes from register."));
	address = 0x80 | (reg & 0x7E);		// MSB == 1 is for reading. LSB is not used in address. Datasheet section 8.1.2.3.
	index = 0;							// Index in values array.
	SS_RFID=0;//Select Slave RFID
	count--;								// One read is performed outside of the loop
	SPI_Write(MFRC522_READ|(address & MFRC522_WRITE));//MSB = 1- Read mode , LSB=0
	//SPI.transfer(address);					// Tell MFRC522 which address we want to read
	while (index < count) {
		if (index == 0 && rxAlign) {		// Only update bit positions rxAlign..7 in values[0]
			// Create bit mask for bit positions rxAlign..7
			mask = 0;
			for ( i = rxAlign; i <= 7; i++) {
				mask |= (1 << i);
			}
			// Read value and tell that we want to read the same address again.
			 value = SPINoAddressRead();//MSB = 1- Read mode , LSB=0
			// Apply mask to both current value of values[0] and the new data in value.
			values[0] = (values[index] & ~mask) | (value & mask);
		}
		else { // Normal case
			values[index] = SPINoAddressRead();//MSB = 1- Read mode , LSB=0	// Read value and tell that we want to read the same address again.
		}
		index++;
	}
	values[index] = SPI_Read(0);// Read the final byte. Send 0 to stop reading.
    SS_RFID=1;

    return ((char *)values);
} // End PCD_ReadRegister()














// Sets the mask in Register Reg
void PCDSetRegisterBitMask(unsigned char regAddr, unsigned char bitmask)
{
    unsigned char temp;
    temp=PCDReadRegister(regAddr);
    PCDRFIDWriteRegister(regAddr,temp|bitmask);

}


// Clears the bit in Register Reg
void PCDClearRegisterBitMask(unsigned char regAddr, unsigned char bitmask)
{
    unsigned char temp;
    temp=PCDReadRegister(regAddr);//Read reg from address
    PCDRFIDWriteRegister(regAddr,temp & (~bitmask));//clear bit mask

}

//Turns on antenna by enabling pins Tx1 and Tx2
//After reset these pins are disabled
void PCDAntennaOn()
{
    unsigned char regvalue = PCDReadRegister(TxControlReg);
    if ((regvalue & 0x03 ) != 0x03)
    {
        PCDRFIDWriteRegister(TxControlReg,regvalue|0x03);
    }
}//End of PCD Antenna





//Reset the MFRC522

void PCDReset() {
	PCDRFIDWriteRegister(CommandReg, PCD_SoftReset);	// Issue the SoftReset command.
	// The datasheet does not mention how long the SoftRest command takes to complete.
	// But the MFRC522 might have been in soft power-down mode (triggered by bit 4 of CommandReg)
	// Section 8.8.2 in the datasheet says the oscillator start-up time is the start up time of the crystal + 37,74�s. Let us be generous: 50ms.
	delay_ms(50);
	// Wait for the PowerDown bit in CommandReg to be cleared

    while (PCDReadRegister(CommandReg) & (1<<4)) {
		// PCD still restarting - unlikely after waiting 50ms, but better safe than sorry.
	}
} // End PCD_Reset()




unsigned char PCDPerformSelfTest() {


	unsigned char  ZEROES[25] = {0x00};
    unsigned char  i=0,n=0,version=0;
	unsigned char  *result;
	const unsigned char *reference;
	// This follows directly the steps outlined in 16.1.1
	// 1. Perform a soft reset.
	PCDReset();
	// 2. Clear the internal buffer by writing 25 bytes of 00h
	PCDSetRegisterBitMask(FIFOLevelReg, 0x80);	// flush the FIFO buffer
	PCDBlockWriteRegister(FIFODataReg, 25, ZEROES);	// write 25 bytes of 00h to FIFO
	PCDRFIDWriteRegister(CommandReg, PCD_Mem);		// transfer to internal buffer
	// 3. Enable self-test
	PCDRFIDWriteRegister(AutoTestReg, 0x09);
	// 4. Write 00h to FIFO buffer
	PCDRFIDWriteRegister(FIFODataReg, 0x00);
	// 5. Start self-test by issuing the CalcCRC command
	PCDRFIDWriteRegister(CommandReg, PCD_CalcCRC);
	// 6. Wait for self-test to complete
	for (i = 0; i < 0xFF; i++) {
		n = PCDReadRegister(DivIrqReg);	// DivIrqReg[7..0] bits are: Set2 reserved reserved MfinActIRq reserved CRCIRq reserved reserved
		//printf("\n\r n %x\n\r",n);
		if (n & 0x04) {						// CRCIRq bit set - calculation done
            //printf("\n\r Iniside break n %x\n\r",n);
			break;
		}
	}
	////printf("\n\r Stop calculating \n\r");
	PCDRFIDWriteRegister(CommandReg, PCD_Idle);		// Stop calculating CRC for new content in the FIFO.
	// 7. Read out resulting 64 bytes from the FIFO buffer.
	result=PCDBlockReadRegister(FIFODataReg, 64,0);
	// Auto self-test done
	// Reset AutoTestReg register to be 0 again. Required for normal operation.
	PCDRFIDWriteRegister(AutoTestReg, 0x00);
	// Determine firmware version (see section 9.3.4.8 in spec)
	version = PCDReadRegister(VersionReg);
	// Pick the appropriate reference values
    printf("\n\r version %x\n\r",version);
	switch (version) {


		case 0x91:	// Version 1.0
			reference = MFRC522_firmware_referenceV1_0;
			break;
		case 0x92:	// Version 2.0
			reference = MFRC522_firmware_referenceV2_0;
			break;
		default:	// Unknown version
			return 0;
	}
    //printf("reading  Self test\n\r");
	// Verify that the results match up to our expectations
	i=0;
	while (i < 64) {
        //printf("%02x",result[i]);

		if (result[i] != reference[i]) {
		//if (result[i] != MFRC522_firmware_referenceV2_0[i]) {
            return 0;
		}
		i++;
    }


	// Test passed; all is good.
	return 1;
} // End PCD_PerformSelfTest()

/**************************************************************************


*******************************************************************/
void PCDRFIDInit()
{

    PCDReset();
    PCDRFIDWriteRegister(TModeReg, 0x80);			// TAuto=1; timer starts automatically at the end of the transmission in all communication modes at all speeds
    printf("\n\r TModeReg value %x",PCDReadRegister(TModeReg));
	PCDRFIDWriteRegister(TPrescalerReg, 0xAE);		// TPreScaler = TModeReg[3..0]:TPrescalerReg, ie 0x0A9 = 169 => f_timer=40kHz, ie a timer period of 25�s.
	printf("\n\r TPrescalerReg value %x",PCDReadRegister(TPrescalerReg));
	PCDRFIDWriteRegister(TReloadRegH, 0x03);		// Reload timer with 0x3E8 = 1000, ie 25ms before timeout.
	printf("\n\r TReloadRegH value %x",PCDReadRegister(TReloadRegH));
	PCDRFIDWriteRegister(TReloadRegL, 0xE8);
	printf("\n\r TReloadRegL value %x",PCDReadRegister(TReloadRegL));
	PCDRFIDWriteRegister(RFCfgReg, 0x70);       //Gain = 48 db
	printf("\n\r RFCfgReg value %x",PCDReadRegister(RFCfgReg));
	PCDRFIDWriteRegister(TxASKReg, 0x40);		// Default 0x00. Force a 100 % ASK modulation independent of the ModGsPReg register setting
	printf("\n\r TxASKReg value %x",PCDReadRegister(TxASKReg));
	PCDRFIDWriteRegister(ModeReg, 0x3D);		// Default 0x3F. Set the preset value for the CRC coprocessor for the CalcCRC command to 0x6363 (ISO 14443-3 part 6.2.4)
	printf("\n\r ModeReg value %x",PCDReadRegister(ModeReg));
	PCDAntennaOn();						// Enable the antenna driver pins TX1 and TX2 (they were disabled by the reset)
}

//void PCDRFIDInit()
//{
//
//    PCDReset();
//    PCDRFIDWriteRegister(TModeReg, 0x8D);			// TAuto=1; timer starts automatically at the end of the transmission in all communication modes at all speeds
//    printf("\n\r TModeReg value %x",PCDReadRegister(TModeReg));
//	PCDRFIDWriteRegister(TPrescalerReg, 0x3E);		// TPreScaler = TModeReg[3..0]:TPrescalerReg, ie 0x0A9 = 169 => f_timer=40kHz, ie a timer period of 25�s.
//	printf("\n\r TPrescalerReg value %x",PCDReadRegister(TPrescalerReg));
//	PCDRFIDWriteRegister(TReloadRegH, 0x00);		// Reload timer with 0x3E8 = 1000, ie 25ms before timeout.
//	printf("\n\r TReloadRegH value %x",PCDReadRegister(TReloadRegH));
//	PCDRFIDWriteRegister(TReloadRegL, 0x30);
//	printf("\n\r TReloadRegL value %x",PCDReadRegister(TReloadRegL));
//	PCDRFIDWriteRegister(RFCfgReg, 0x70);       //Gain = 48 db
//	printf("\n\r RFCfgReg value %x",PCDReadRegister(RFCfgReg));
//	PCDRFIDWriteRegister(TxASKReg, 0x40);		// Default 0x00. Force a 100 % ASK modulation independent of the ModGsPReg register setting
//	printf("\n\r TxASKReg value %x",PCDReadRegister(TxASKReg));
//	PCDRFIDWriteRegister(ModeReg, 0x3D);		// Default 0x3F. Set the preset value for the CRC coprocessor for the CalcCRC command to 0x6363 (ISO 14443-3 part 6.2.4)
//	printf("\n\r ModeReg value %x",PCDReadRegister(ModeReg));
//	PCDAntennaOn();						// Enable the antenna driver pins TX1 and TX2 (they were disabled by the reset)
//}

//Off Antenna
void PCDAntennaOff()
{
    PCDClearRegisterBitMask(TxControlReg,0x03);
}






//Get Rx Gain of MFRC522
unsigned char PCDGetAntennaGain()
{
    return (PCDReadRegister(RFCfgReg) & (0x07 << 4));// RFCReg have reserved bits
}


//Set Gain  of Rx
void PCDSetAntennaGain(unsigned char mask)
{
    if (PCDGetAntennaGain() != mask)//Change only if not already existing
    {
        PCDClearRegisterBitMask(RFCfgReg,(0x07<<4));//clearing
        PCDSetRegisterBitMask(RFCfgReg,mask &(0x07<<4));
    }
}

/*//////////////////////////////////////////////////////////////////

Functions working with PICC
/////////////////////////////////////////////////////////////////*/

unsigned char PCDTransceiveData(	unsigned char *sendData,		///< Pointer to the data to transfer to the FIFO.
									unsigned char sendLen,		///< Number of bytes to transfer to the FIFO.
									unsigned char *backData,		///< NULL or pointer to buffer if data should be read back after executing the command.
									unsigned char *backLen,		///< In: Max number of bytes to write to *backData. Out: The number of bytes returned.
									unsigned char *validBits,	///< In/Out: The number of valid bits in the last byte. 0 for 8 valid bits. Default NULL.
									unsigned char rxAlign,		///< In: Defines the bit position in backData[0] for the first bit received. Default 0.
									unsigned char checkCRC		///< In: True => The last two bytes of the response is assumed to be a CRC_A that must be validated.
								 )
								 {
	unsigned char waitIRq = 0x30;		// RxIRq and IdleIRq
	return (PCDCommunicateWithPICC(PCD_Transceive, waitIRq, sendData, sendLen, backData, backLen, validBits, rxAlign, checkCRC));
}
/*//////////////////////////////////////////////////////////////////
//Executes command
waits for completion
transfers data from the FIFO

/////////////////////////////////////////////////////////////////*/
unsigned char PCDCommunicateWithPICC(	unsigned char command,		///< The command to execute. One of the PCD_Command enums.
										unsigned char waitIRq,		///< The bits in the ComIrqReg register that signals successful completion of the command.
										unsigned char *sendData,		///< Pointer to the data to transfer to the FIFO.
										unsigned char sendLen,		///< Number of bytes to transfer to the FIFO.
										unsigned char *backData,		///< NULL or pointer to buffer if data should be read back after executing the command.
										unsigned char *backLen,		///< In: Max number of bytes to write to *backData. Out: The number of bytes returned.
										unsigned char *validdataBits,	///< In/Out: The number of valid bits in the last byte. 0 for 8 valid bits.
										unsigned char rxAlign,		///< In: Defines the bit position in backData[0] for the first bit received. Default 0.
										unsigned char checkCRC		///< In: True => The last two bytes of the response is assumed to be a CRC_A that must be validated.
									 )
{
    unsigned char n,validBits;
	unsigned int i;
	unsigned char errorRegValue;
	unsigned char controlBuffer[2];

	// Prepare values for BitFramingReg
    unsigned char  txLastBits = validBits ? *validdataBits : 0;
	//unsigned char  txLastBits = (*validBits);
	unsigned char bitFraming = (rxAlign << 4) + txLastBits;		// RxAlign = BitFramingReg[6..4]. TxLastBits = BitFramingReg[2..0]

	PCDRFIDWriteRegister(CommandReg, PCD_Idle);			// Stop any active command.
	PCDRFIDWriteRegister(ComIrqReg, 0x7F);					// Clear all seven interrupt request bits
	PCDRFIDWriteRegister(FIFOLevelReg, 0x80);			// FlushBuffer = 1, FIFO initialization
	PCDBlockWriteRegister(FIFODataReg, sendLen, sendData);	// Write sendData to the FIFO
	//PCDRFIDWriteRegister(BitFramingReg, bitFraming);		// Bit adjustments
	PCDRFIDWriteRegister(CommandReg, command);				// Execute the command
	if (command == PCD_Transceive) {
		PCDSetRegisterBitMask(BitFramingReg, 0x80);	// StartSend=1, transmission of data starts
	}

	// Wait for the command to complete.
	// In PCD_Init() we set the TAuto flag in TModeReg. This means the timer automatically starts when the PCD stops transmitting.
	// Each iteration of the do-while-loop takes 17.86�s.
	i = 2000;
	while (1) {
		n = PCDReadRegister(ComIrqReg);	// ComIrqReg[7..0] bits are: Set1 TxIRq RxIRq IdleIRq HiAlertIRq LoAlertIRq ErrIRq TimerIRq
		if (n & waitIRq) {					// One of the interrupts that signal success has been set.
			break;
		}
		if (n & 0x01) {						// Timer interrupt - nothing received in 25ms
			return STATUS_TIMEOUT;
		}
		if (--i == 0) {						// The emergency break. If all other condions fail we will eventually terminate on this one after 35.7ms. Communication with the MFRC522 might be down.
			return STATUS_TIMEOUT;
		}
	}

	// Stop now if any errors except collisions were detected.
	errorRegValue = PCDReadRegister(ErrorReg); // ErrorReg[7..0] bits are: WrErr TempErr reserved BufferOvfl CollErr CRCErr ParityErr ProtocolErr
	if (errorRegValue & 0x13) {	 // BufferOvfl ParityErr ProtocolErr
		return STATUS_ERROR;
	}

	// If the caller wants data back, get it from the MFRC522.
	if (backData && backLen) {
		n = PCDReadRegister(FIFOLevelReg);			// Number of bytes in the FIFO
		if (n > *backLen) {
			return STATUS_NO_ROOM;
		}
		*backLen = n;											// Number of bytes returned
		backData=PCDBlockReadRegister(FIFODataReg, n, rxAlign);	// Get received data from FIFO
		validBits = PCDReadRegister(ControlReg) & 0x07;		// RxLastBits[2:0] indicates the number of valid bits in the last received byte. If this value is 000b, the whole byte is valid.
		if (validBits) {
			*validdataBits = validBits;
		}
	}

	// Tell about collisions
	if (errorRegValue & 0x08) {		// CollErr
		return STATUS_COLLISION;
	}

	// Perform CRC_A validation if requested.
	if (backData && backLen && checkCRC) {
		// In this case a MIFARE Classic NAK is not OK.
		if (*backLen == 1 && validBits == 4) {
			return STATUS_MIFARE_NACK;
		}
		// We need at least the CRC_A value and all 8 bits of the last byte must be received.
		if (*backLen < 2 || validBits != 0) {
			return STATUS_CRC_WRONG;
		}
		// Verify CRC_A - do our own calculation and store the control in controlBuffer.

		n = PCDCalculateCRC(&backData[0], *backLen - 2, &controlBuffer[0]);
		if (n != STATUS_OK) {
			return n;
		}
		if ((backData[*backLen - 2] != controlBuffer[0]) || (backData[*backLen - 1] != controlBuffer[1])) {
			return STATUS_CRC_WRONG;
		}
	}

	return (STATUS_OK);
}
//

unsigned char PCDSelectTag(unsigned char* serNum) {
	unsigned char i;
	unsigned char status;
	unsigned char size;
	int recvBits;
	unsigned char buffer[9];
    unsigned char waitIRq = 0x30;
	buffer[0] = PICC_CMD_SEL_CL1;
	buffer[1] = 0x70;
	for (i = 0; i < 5; i++) {
		buffer[i+2] = *(serNum+i);
	}
	PCDCalculateCRC(buffer, 7, &buffer[7]);		//??
	status = PCDCommunicateWithPICC(PCD_Transceive,waitIRq, buffer, 9, buffer, &recvBits);

	if ((status == STATUS_OK) && (recvBits == 0x18)) {
		size = buffer[0];
	} else {
		size = 0;
	}

	return size;
}

unsigned char PCDAuth(unsigned char authMode, unsigned char BlockAddr, unsigned char* Sectorkey, unsigned char* serNum) {
	unsigned char status;
	int recvBits;
	unsigned char i;
	unsigned char buff[12];

	//Verify the command block address + sector + password + card serial number
	buff[0] = authMode;
	buff[1] = BlockAddr;
	for (i = 0; i < 6; i++) {
		buff[i+2] = *(Sectorkey+i);
	}
	for (i=0; i<4; i++) {
		buff[i+8] = *(serNum+i);
	}
	status = PCDCommunicateWithPICC(PCD_MFAuthent, buff, 12, buff, &recvBits);

	if ((status != STATUS_OK) || (!(PCDReadRegister(Status2Reg) & 0x08))) {
		status = STATUS_ERROR;
	}

	return status;
}

unsigned char PCDRead(unsigned char blockAddr, unsigned char* recvData) {
	unsigned char status;
	int unLen;

	recvData[0] = PICC_CMD_MF_READ;
	recvData[1] = blockAddr;
	PCDCalculateCRC(recvData,2, &recvData[2]);
	status = PCDCommunicateWithPICC(PCD_Transceive, recvData, 4, recvData, &unLen);

	if ((status != STATUS_OK) || (unLen != 0x90)) {
		status = STATUS_ERROR;
	}

	return status;
}

unsigned char PCDWrite(unsigned char blockAddr, unsigned char* writeData) {
	unsigned char status;
	int recvBits;
	unsigned char i;
	unsigned char buff[18];

	buff[0] = PICC_CMD_MF_WRITE;
	buff[1] = blockAddr;
	PCDCalculateCRC(buff, 2, &buff[2]);
	status = PCDCommunicateWithPICC(PCD_Transceive, buff, 4, buff, &recvBits);

	if ((status != STATUS_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A)) {
		status = STATUS_ERROR;
	}

	if (status == STATUS_OK) {
		//Data to the FIFO write 16Byte
		for (i = 0; i < 16; i++) {
			buff[i] = *(writeData+i);
		}
		PCDCalculateCRC(buff, 16, &buff[16]);
		status = PCDCommunicateWithPICC(PCD_Transceive, buff, 18, buff, &recvBits);

		if ((status != STATUS_OK) || (recvBits != 4) || ((buff[0] & 0x0F) != 0x0A)) {
			status = STATUS_ERROR;
		}
	}

	return status;
}

void PCDHalt(void) {
	int unLen;
	unsigned char buff[4];

	buff[0] = PICC_CMD_HLTA;
	buff[1] = 0;
	PCDCalculateCRC(buff, 2, &buff[2]);

	PCDCommunicateWithPICC(PCD_Transceive, buff, 4, buff, &unLen);
}


unsigned char PCDAnticoll(unsigned char* serNum) {
	unsigned char status;
	unsigned char i;
	unsigned char serNumCheck = 0;
	int unLen;

	PCDRFIDWriteRegister(BitFramingReg, 0x00);		//TxLastBists = BitFramingReg[2..0]

	serNum[0] = PICC_CMD_SEL_CL1;
	serNum[1] = 0x20;
	status = PCDCommunicateWithPICC(PCD_Transceive, serNum, 2, serNum, &unLen);

	if (status == STATUS_OK) {
		//Check card serial number
		for (i = 0; i < 4; i++) {
			serNumCheck ^= serNum[i];
		}
		if (serNumCheck != serNum[i]) {
			status = STATUS_ERROR;
		}
	}
	return status;
}




unsigned char PCDRequest(unsigned char reqMode, unsigned char* TagType) {
	unsigned char status;
	int backBits;			//The received data bits

	PCDRFIDWriteRegister(BitFramingReg, 0x07);		//TxLastBists = BitFramingReg[2..0]	???

	TagType[0] = reqMode;
	status = PCDCommunicateWithPICC(PCD_Transceive, TagType, 1, TagType, &backBits);

	if ((status != STATUS_OK) || (backBits != 0x10)) {
		status = STATUS_ERROR;
	}
    printf("sTATUS IN Pcd rEQUEST %x\n\r",status);
	return status;
}



unsigned char PCDCheck(unsigned char* id) {
	unsigned char status;
	//Find cards, return card type
	printf("Inside Check\n\r");
	status = PCDRequest(PICC_CMD_REQA, id);
	if (status == STATUS_OK) {
		//Card detected
		//Anti-collision, return card serial number 4 bytes
		printf("Inside Anti collision \n\r");
		status = PCDAnticoll(id);
	}
	PCDHalt();			//Command card into hibernation

	return status;
}

unsigned char PCDCompare(unsigned char* CardID, unsigned char* CompareID) {
	unsigned char i;
	for (i = 0; i < 5; i++) {
        printf("card value\n\r %02x",CardID[i]);
		if (CardID[i] != CompareID[i]) {
			return STATUS_ERROR;
		}
	}
	return STATUS_OK;
}



unsigned char PCDCalculateCRC(unsigned char *bytedata,		///< In: Pointer to the data to transfer to the FIFO for CRC calculation.
                              unsigned char length,	///< In: The number of bytes to transfer.
                              unsigned char *result	///< Out: Pointer to result buffer. Result is written to result[0..1], low byte first.
                            )
{
	// Wait for the CRC calculation to complete. Each iteration of the while-loop takes 17.73�s.
    int i  = 5000;
	unsigned char n;
	unsigned char *result;
	PCDRFIDWriteRegister(CommandReg, PCD_Idle);		// Stop any active command.
	PCDRFIDWriteRegister(DivIrqReg, 0x04);				// Clear the CRCIRq interrupt request bit
	PCDSetRegisterBitMask(FIFOLevelReg, 0x80);		// FlushBuffer = 1, FIFO initialization
	PCDBlockWriteRegister(FIFODataReg, length, bytedata);	// Write data to the FIFO
	PCDRFIDWriteRegister(CommandReg, PCD_CalcCRC);		// Start the calculation



	while (1) {
		n = PCDReadRegister(DivIrqReg);	// DivIrqReg[7..0] bits are: Set2 reserved reserved MfinActIRq reserved CRCIRq reserved reserved
		if (n & 0x04) {						// CRCIRq bit set - calculation done
			break;
		}
		if (--i == 0) {						// The emergency break. We will eventually terminate on this one after 89ms. Communication with the MFRC522 might be down.
			return STATUS_TIMEOUT;
		}
	}
	PCDRFIDWriteRegister(CommandReg, PCD_Idle);		// Stop calculating CRC for new content in the FIFO.

	// Transfer the result from the registers to the result buffer
	result[0] = PCDReadRegister(CRCResultRegL);
	result[1] = PCDReadRegister(CRCResultRegH);
	return STATUS_OK;
}






