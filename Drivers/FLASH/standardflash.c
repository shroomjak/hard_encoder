/*
 * The Clear BSD License
 * Copyright (c) 2018 Adesto Technologies Corporation, Inc
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 *  that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*!
 * @ingroup ADESTO_LAYER STANDARDFLASH
 */
/**
 * @file    standardflash.c
 * @brief   Definition of Standardflash functions.
 */
	
#include <standardflash.h>
#include "main.h"





uint8_t txStandardflashInternalBuffer[MAXIMUM_TX_BYTES];






void SPI_Delay(uint32_t delayTime);
uint8_t SPI1_RW(uint8_t txByte);



void SPI_Exchange(uint8_t *txBuffer,
				  uint32_t txNumBytes,
				  uint8_t *rxBuffer,
				  uint32_t rxNumBytes,
				  uint32_t dummyNumBytes);



void load4BytesToTxBuffer(uint8_t *txBuffer, uint8_t opcode, uint32_t address)
{
	txBuffer[0] = opcode;
	txBuffer[1] = (uint8_t) (address >> 16);
	txBuffer[2] = (uint8_t) (address >> 8);
	txBuffer[3] = (uint8_t) address;
} 




void fillArrayPattern(uint8_t * byteArray, uint32_t numBytes, int seedNumber)
{
	for(int i = 0; i < numBytes; i++)
	{
		byteArray[i] = (uint8_t)(seedNumber+i);
	}
}

void fillArrayConst(uint8_t * byteArray, uint32_t numBytes, int constantNum)
{
	for(int i = 0; i < numBytes; i++)
	{
		byteArray[i] = (uint8_t)(constantNum);
	}
} 





int compareByteArrays(uint8_t *arr1, uint8_t *arr2, uint32_t arrLength)
{
	uint32_t numErrors = 0;
	for(int i = 0; i < arrLength; i++)
	{
		if(arr1[i] != arr2[i])
		{
			numErrors++;
		}
	}
	return (numErrors == 0) ? 1 : 0;
} 





#if (PARTNO == AT25SF641) 	|| \
	(PARTNO == AT25SF321)	|| \
	(PARTNO == AT25SF161) 	|| \
	(PARTNO == AT25SF081) 	|| \
	(PARTNO == AT25SF041) 	|| \
	(PARTNO == AT25SL128A)  || \
	(PARTNO == AT25SL641) 	|| \
	(PARTNO == AT25SL321) 	|| \
	(PARTNO == AT25DL081) 	|| \
	(PARTNO == AT25DL161) 	|| \
	(PARTNO == AT25DF081A)  || \
	(PARTNO == AT25DF321A)  || \
	(PARTNO == AT25DF641A)  || \
	(PARTNO == AT25QL128A)  || \
 	(PARTNO == AT25QL641) 	|| \
	(PARTNO == AT25QL321) 	|| \
	(PARTNO == AT25QF641)	|| \
	(ALL == 1)





void standardflashWaitOnReady()
{
	uint8_t SRArray[2] = {0, 0};
	do
	{
#if (PARTNO == AT25DL081) 	|| \
	(PARTNO == AT25DL161) 	|| \
	(PARTNO == AT25DF081A)  || \
	(PARTNO == AT25DF321A)  || \
	(PARTNO == AT25DF641A)
		standardflashReadSR(SRArray);
		SPI_Delay(10);
	}
	while(SRArray[1] & (1<<0));
#else
		SRArray[0] = standardflashReadSRB1();
		SPI_Delay(10);
	}
	while(SRArray[0] & (1<<0));
#endif
}

void standardflashSetQEBit()
{
	uint8_t SRArray[2] = {0, 0};
	// Read both status register bytes.
#if (PARTNO == AT25DL081) 	|| \
	(PARTNO == AT25DL161) 	|| \
	(PARTNO == AT25DF081A)  || \
	(PARTNO == AT25DF321A)  || \
	(PARTNO == AT25DF641A)
	standardflashReadSR(SRArray);
#else
	SRArray[0] = standardflashReadSRB1();
	SRArray[1] = standardflashReadSRB2();
#endif
	// Store a 1 in the QE bit.
	SRArray[1] |= (1 << 1);
	// Altering the device data, so send over a write enable first.
	standardflashWriteEnable();
	// Store the command and new SRB values. NOTE this needs to be done after
	// the write enable since the internal buffer used is a shared resource.
#if (PARTNO == AT25DL081) 	|| \
	(PARTNO == AT25DL161) 	|| \
	(PARTNO == AT25DF081A)  || \
	(PARTNO == AT25DF321A)  || \
	(PARTNO == AT25DF641A)
	standardflashWriteSRB1(SRArray[0]);
	standardflashWriteSRB2(SRArray[1]);
#else
	standardflashWriteSR(SRArray, 2);
#endif
}

void standardflashClearQEBit()
{
	uint8_t SRArray[2] = {0, 0};
	// Read both status register bytes.
#if (PARTNO == AT25DL081) 	|| \
	(PARTNO == AT25DL161) 	|| \
	(PARTNO == AT25DF081A)  || \
	(PARTNO == AT25DF321A)  || \
	(PARTNO == AT25DF641A)
	standardflashReadSR(SRArray);
#else
	SRArray[0] = standardflashReadSRB1();
	SRArray[1] = standardflashReadSRB2();
#endif
	// Store a 0 in the QE bit.
	SRArray[1] &= ~(1 << 1);
	// Altering the device data, so send over a write enable first.
	standardflashWriteEnable();
	// Store the command and new SRB values. NOTE this needs to be done after
	// the write enable since the internal buffer used is a shared resource.
#if (PARTNO == AT25DL081) 	|| \
	(PARTNO == AT25DL161) 	|| \
	(PARTNO == AT25DF081A)  || \
	(PARTNO == AT25DF321A)  || \
	(PARTNO == AT25DF641A)
	standardflashWriteSRB1(SRArray[0]);
	standardflashWriteSRB2(SRArray[1]);
#else
	standardflashWriteSR(SRArray, 2);
#endif
}

void standardflashWriteEnable()
{
	txStandardflashInternalBuffer[0] = CMD_STANDARDFLASH_WRITE_ENABLE;

	SPI_Exchange(txStandardflashInternalBuffer, 1, NULL, 0, 0);

}

void standardflashWriteDisable()
{
	txStandardflashInternalBuffer[0] = CMD_STANDARDFLASH_WRITE_DISABLE;

        SPI_Exchange(txStandardflashInternalBuffer, 1, NULL, 0, 0);

}

void standardflashReadArrayLowFreq(uint32_t address, uint8_t *rxBuffer, uint32_t rxNumBytes)
{
	load4BytesToTxBuffer(txStandardflashInternalBuffer, CMD_STANDARDFLASH_READ_ARRAY_LF, address);
	SPI_Exchange(txStandardflashInternalBuffer, 4, rxBuffer, rxNumBytes, 0);
}

void standardflashReadArrayHighFreq(uint32_t address, uint8_t *rxBuffer, uint32_t rxNumBytes)
{
	load4BytesToTxBuffer(txStandardflashInternalBuffer, CMD_STANDARDFLASH_READ_ARRAY_HF, address);

	SPI_Exchange(txStandardflashInternalBuffer, 4, rxBuffer, rxNumBytes, 1);

}

void standardflashBytePageProgram(uint32_t address, uint8_t *txBuffer, uint32_t txNumBytes)
{
	load4BytesToTxBuffer(txStandardflashInternalBuffer, CMD_STANDARDFLASH_BYTE_PAGE_PROGRAM, address);
	// Offset the data bytes by 4; opcode+address takes up the first 4 bytes of a transmission.
	uint32_t totalBytes = txNumBytes + 4;

	for(uint32_t j = 0; j < txNumBytes; j++)
	{
		txStandardflashInternalBuffer[j+4] = txBuffer[j];
	}

	SPI_Exchange(txStandardflashInternalBuffer, totalBytes, NULL, 0, 0);

}

void standardflashBlockErase4K(uint32_t address)
{
	load4BytesToTxBuffer(txStandardflashInternalBuffer, CMD_STANDARDFLASH_BLOCK_ERASE_4K, address);
	SPI_Exchange(txStandardflashInternalBuffer, 4, NULL, 0, 0);
}

void standardflashBlockErase32K(uint32_t address)
{
	load4BytesToTxBuffer(txStandardflashInternalBuffer, CMD_STANDARDFLASH_BLOCK_ERASE_32K, address);
	SPI_Exchange(txStandardflashInternalBuffer, 4, NULL, 0, 0);
}

void standardflashBlockErase64K(uint32_t address)
{
	load4BytesToTxBuffer(txStandardflashInternalBuffer, CMD_STANDARDFLASH_BLOCK_ERASE_64K, address);
	SPI_Exchange(txStandardflashInternalBuffer, 4, NULL, 0, 0);
}

void standardflashChipErase1()
{
	txStandardflashInternalBuffer[0] = CMD_STANDARDFLASH_CHIP_ERASE1;
	SPI_Exchange(txStandardflashInternalBuffer, 1, NULL, 0, 0);
}

void standardflashChipErase2()
{
	txStandardflashInternalBuffer[0] = CMD_STANDARDFLASH_CHIP_ERASE2;
	SPI_Exchange(txStandardflashInternalBuffer, 1, NULL, 0, 0);
}

void standardflashDPD()
{
	txStandardflashInternalBuffer[0] = CMD_STANDARDFLASH_DEEP_POWER_DOWN;
	SPI_Exchange(txStandardflashInternalBuffer, 1, NULL, 0, 0);
}

void standardflashResumeFromDPD()
{
	txStandardflashInternalBuffer[0] = CMD_STANDARDFLASH_RESUME_FROM_DPD;
	SPI_Exchange(txStandardflashInternalBuffer, 1, NULL, 0, 0);
}

void standardflashReadID(uint8_t *rxBuffer)
{
	txStandardflashInternalBuffer[0] = CMD_STANDARDFLASH_READ_ID;
	SPI_Exchange(txStandardflashInternalBuffer, 1, rxBuffer, 2, 3);
}

void standardflashReadMID(uint8_t *rxBuffer)
{
	txStandardflashInternalBuffer[0] = CMD_STANDARDFLASH_READ_MID;
	SPI_Exchange(txStandardflashInternalBuffer, 1, rxBuffer, 3, 0);
}
#endif
#if (PARTNO == AT25SF641) 	|| \
	(PARTNO == AT25SF321)	|| \
	(PARTNO == AT25SF161) 	|| \
	(PARTNO == AT25SF081) 	|| \
	(PARTNO == AT25SF041) 	|| \
	(PARTNO == AT25SL128A) 	|| \
	(PARTNO == AT25SL641) 	|| \
	(PARTNO == AT25SL321) 	|| \
	(PARTNO == AT25QL128A) 	|| \
	(PARTNO == AT25QL641) 	|| \
	(PARTNO == AT25QL321) 	|| \
	(PARTNO == AT25QF641)   || \
	(ALL == 1)

void standardflashWriteSR(uint8_t *txBuffer, uint8_t txNumBytes)
{
	txStandardflashInternalBuffer[0] = CMD_STANDARDFLASH_WRITE_SR;
	txStandardflashInternalBuffer[1] = txBuffer[0];
	if(txNumBytes > 1)
	{
		txStandardflashInternalBuffer[2] = txBuffer[1];
	}
	SPI_Exchange(txStandardflashInternalBuffer, txNumBytes+1, NULL, 0, 0);
}
#endif

#if (PARTNO == AT25SF641) 	|| \
	(PARTNO == AT25SL128A) 	|| \
	(PARTNO == AT25SL641) 	|| \
	(PARTNO == AT25SL321) 	|| \
	(PARTNO == AT25QL128A) 	|| \
 	(PARTNO == AT25QL641) 	|| \
	(PARTNO == AT25QL321) 	|| \
	(PARTNO == AT25QF641)   || \
	(PARTNO == AT25DL081) 	|| \
	(PARTNO == AT25DL161) 	|| \
	(PARTNO == AT25DF081A)  || \
	(PARTNO == AT25DF321A)  || \
	(PARTNO == AT25DF641A)	|| \
	(ALL == 1)

void standardflashWriteSRB1(uint8_t regVal)
{
	txStandardflashInternalBuffer[0] = CMD_STANDARDFLASH_WRITE_SRB1;
	txStandardflashInternalBuffer[1] = regVal;
	SPI_Exchange(txStandardflashInternalBuffer, 2, NULL, 0, 0);
}

void standardflashWriteSRB2(uint8_t regVal)
{
	txStandardflashInternalBuffer[0] = CMD_STANDARDFLASH_WRITE_SRB2;
	txStandardflashInternalBuffer[1] = regVal;
	SPI_Exchange(txStandardflashInternalBuffer, 2, NULL, 0, 0);
}
#endif

#if (PARTNO == AT25SF641) 	|| \
	(PARTNO == AT25SF321)	|| \
	(PARTNO == AT25SF161) 	|| \
	(PARTNO == AT25SF081) 	|| \
	(PARTNO == AT25SF041) 	|| \
	(PARTNO == AT25SL128A)  || \
	(PARTNO == AT25SL641) 	|| \
	(PARTNO == AT25SL321) 	|| \
	(PARTNO == AT25QL128A)  || \
 	(PARTNO == AT25QL641) 	|| \
	(PARTNO == AT25QL321) 	|| \
	(PARTNO == AT25QF641)	|| \
	(ALL == 1)
void standardflashWriteEnableVolatileSR()
{
	txStandardflashInternalBuffer[0] = CMD_STANDARDFLASH_WE_FOR_VOLATILE_SR;
	SPI_Exchange(txStandardflashInternalBuffer, 1, NULL, 0, 0);
}

uint8_t standardflashReadSRB1()
{
	uint8_t regVal = 0;
	txStandardflashInternalBuffer[0] = CMD_STANDARDFLASH_READ_SRB1;
	SPI_Exchange(txStandardflashInternalBuffer, 1, &regVal, 1, 0);
	return regVal;
}

uint8_t standardflashReadSRB2()
{
	uint8_t regVal = 0;
	txStandardflashInternalBuffer[0] = CMD_STANDARDFLASH_READ_SRB2;
	SPI_Exchange(txStandardflashInternalBuffer, 1, &regVal, 1, 0);
	return regVal;
}








void standardflashEraseSecurityRegister(uint32_t address)
{
	load4BytesToTxBuffer(txStandardflashInternalBuffer, CMD_STANDARDFLASH_ERASE_SECURTIY_REG_PAGE, address);
	SPI_Exchange(txStandardflashInternalBuffer, 4, NULL, 0, 0);
}

void standardflashProgramSecurityRegisters(uint32_t address, uint8_t *txBuffer, uint32_t txNumBytes)
{
	load4BytesToTxBuffer(txStandardflashInternalBuffer, CMD_STANDARDFLASH_PROGRAM_SECURITY_REG_PAGE, address);
	// Offset the data bytes by 4; opcode+address takes up the first 4 bytes of a transmission.
	uint32_t totalBytes = txNumBytes + 4;

	for(uint32_t j = 0; j < txNumBytes; j++)
	{
		txStandardflashInternalBuffer[j+4] = txBuffer[j];
	}

	SPI_Exchange(txStandardflashInternalBuffer, totalBytes, NULL, 0, 0);
}

void standardflashReadSecurityRegisters(uint32_t address, uint8_t *rxBuffer, uint32_t rxNumBytes)
{
	load4BytesToTxBuffer(txStandardflashInternalBuffer, CMD_STANDARDFLASH_READ_SECURITY_REG_PAGE, address);
	SPI_Exchange(txStandardflashInternalBuffer, 4, rxBuffer, rxNumBytes, 1);
}

void standardflashResumeFromDPDReadID(uint8_t *rxBuffer)
{
	txStandardflashInternalBuffer[0] = CMD_STANDARDFLASH_RESUME_FROM_DPD;
	SPI_Exchange(txStandardflashInternalBuffer, 1, rxBuffer, 1, 3);
}


#endif


#if (PARTNO == AT25SF641) 	|| \
	(PARTNO == AT25SF321)	|| \
	(PARTNO == AT25SF161) 	|| \
	(PARTNO == AT25SL128A)  || \
	(PARTNO == AT25SL641) 	|| \
	(PARTNO == AT25SL321) 	|| \
	(PARTNO == AT25QL128A)  || \
 	(PARTNO == AT25QL641) 	|| \
	(PARTNO == AT25QL321) 	|| \
	(PARTNO == AT25QF641)	|| \
	(ALL == 1)

void standardflashEraseProgramSuspend()
{
	txStandardflashInternalBuffer[0] = (uint8_t) CMD_STANDARDFLASH_ERASE_PROGRAM_SUSPEND;
	SPI_Exchange(txStandardflashInternalBuffer, 1, NULL, 0, 0);
}

void standardflashEraseProgramResume()
{
	txStandardflashInternalBuffer[0] = (uint8_t) CMD_STANDARDFLASH_ERASE_PROGRAM_RESUME;
	SPI_Exchange(txStandardflashInternalBuffer, 1, NULL, 0, 0);
}

#endif


#if (PARTNO == AT25SF641) 	|| \
	(PARTNO == AT25SL128A)  || \
	(PARTNO == AT25SL641) 	|| \
	(PARTNO == AT25SL321) 	|| \
	(PARTNO == AT25QL128A)  || \
 	(PARTNO == AT25QL641) 	|| \
	(PARTNO == AT25QL321) 	|| \
	(PARTNO == AT25QF641)	|| \
	(ALL == 1)


void standardflashEnableReset()
{
	txStandardflashInternalBuffer[0] = (uint8_t) CMD_STANDARDFLASH_ENABLE_RESET;
	SPI_Exchange(txStandardflashInternalBuffer, 1, NULL, 0, 0);
}

void standardflashReset()
{
	txStandardflashInternalBuffer[0] = (uint8_t) CMD_STANDARDFLASH_RESET;
	SPI_Exchange(txStandardflashInternalBuffer, 1, NULL, 0, 0);
}

void standardflashEnterSecureOTP()
{
	txStandardflashInternalBuffer[0] = (uint8_t) CMD_STANDARDFLASH_ENTER_SECURED_OTP;
	SPI_Exchange(txStandardflashInternalBuffer, 1, NULL, 0, 0);
}

void standardflashExitSecuredOTP()
{
	txStandardflashInternalBuffer[0] = (uint8_t) CMD_STANDARDFLASH_EXIT_SECURED_OTP;
	SPI_Exchange(txStandardflashInternalBuffer, 1, NULL, 0, 0);
}

#endif


#if (PARTNO == AT25DL081) 	|| \
	(PARTNO == AT25DL161) 	|| \
	(PARTNO == AT25DF081A)  || \
	(PARTNO == AT25DF321A)  || \
	(PARTNO == AT25DF641A)	|| \
	(ALL == 1)

void standardflashReadSR(uint8_t *rxBuffer)
{
	txStandardflashInternalBuffer[0] = CMD_STANDARDFLASH_READ_SR;
	SPI_Exchange(txStandardflashInternalBuffer, 1, rxBuffer, 2, 0);
}



void standardflashProgramEraseSuspend()
{
	txStandardflashInternalBuffer[0] = (uint8_t) CMD_STANDARDFLASH_PROGRAM_ERASE_SUSPEND;
	SPI_Exchange(txStandardflashInternalBuffer, 1, NULL, 0, 0);
}

void standardflashProgramEraseResume()
{
	txStandardflashInternalBuffer[0] = (uint8_t) CMD_STANDARDFLASH_PROGRAM_ERASE_RESUME;
	SPI_Exchange(txStandardflashInternalBuffer, 1, NULL, 0, 0);
}

void standardflashProtectSector(uint32_t address)
{
	load4BytesToTxBuffer(txStandardflashInternalBuffer, CMD_STANDARDFLASH_PROTECT_SECTOR, address);
	SPI_Exchange(txStandardflashInternalBuffer, 4, NULL, 0, 0);
}
void standardflashUnprotectSector(uint32_t address)
{
	load4BytesToTxBuffer(txStandardflashInternalBuffer, CMD_STANDARDFLASH_UNPROTECT_SECTOR, address);
	SPI_Exchange(txStandardflashInternalBuffer, 4, NULL, 0, 0);
}

uint8_t standardflashReadSectorProtectionReg(uint32_t address)
{
	uint8_t registerVal = 0;
	load4BytesToTxBuffer(txStandardflashInternalBuffer, CMD_STANDARDFLASH_READ_SECT_PROT_REG, address);
	SPI_Exchange(txStandardflashInternalBuffer, 4, &registerVal, 1, 0);
	return registerVal;
}

void standardflashFreezeLockdownState()
{
	load4BytesToTxBuffer(txStandardflashInternalBuffer, CMD_STANDARDFLASH_FREEZE_LOCKDOWN_STATE, (uint32_t) 0x0055AA40);
	txStandardflashInternalBuffer[4] = 0xD0;
	SPI_Exchange(txStandardflashInternalBuffer, 5, NULL, 0, 0);
}

uint8_t standardflashReadLockdownReg(uint32_t address)
{
	uint8_t registerVal = 0;
	load4BytesToTxBuffer(txStandardflashInternalBuffer, CMD_STANDARDFLASH_READ_LOCKDOWN_REG, address);
	SPI_Exchange(txStandardflashInternalBuffer, 4, &registerVal, 1, 1);
	return registerVal;
}

void standardflashProgramOTPReg(uint32_t address, uint8_t *txBuffer, uint32_t txNumBytes)
{
	load4BytesToTxBuffer(txStandardflashInternalBuffer, CMD_STANDARDFLASH_PROGRAM_OTP_REG, address);
	// Offset the data bytes by 4; opcode+address takes up the first 4 bytes of a transmission.
	uint32_t totalBytes = txNumBytes + 4;

	for(uint32_t j = 0; j < txNumBytes; j++)
	{
		txStandardflashInternalBuffer[j+4] = txBuffer[j];
	}

	SPI_Exchange(txStandardflashInternalBuffer, totalBytes, NULL, 0, 0);
}
#endif
