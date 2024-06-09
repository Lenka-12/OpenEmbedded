/*
 *
 *  Created on: May 14, 2024
 *      Author: Malefetsane Lenka
 */
#include "AT24CXX.h"
#define BYTE 1
unsigned short usFreeSpace(EEPROMHandle_t* pxEEPROM){
	return  MEM_SIZE - pxEEPROM->usWrAddr;
}
unsigned short usDataAvailable(EEPROMHandle_t* pxEEPROM){
	return pxEEPROM->usWrAddr - pxEEPROM->usRdAddr;
}
HAL_StatusTypeDef xRead(I2C_HandleTypeDef* hi2c, unsigned char* pucData,unsigned short usSize, EEPROMHandle_t *pxEEPROM){
	HAL_StatusTypeDef status;
	if (ucValidRd(pxEEPROM, usSize)){
		// valid read
		for (unsigned short i = 0; i < usSize; i++){
			status = HAL_I2C_Mem_Read(hi2c, ADDRESS<<1, pxEEPROM->usRdAddr, 2, pucData, BYTE, 1000);
			HAL_Delay(500);
			if (status == HAL_ERROR){
				return status;
			}
			pxEEPROM->usRdAddr += 1;
			pucData++;
		}
	}
	else {
		status = HAL_ERROR;
	}
	return status;

}
HAL_StatusTypeDef xWrite(I2C_HandleTypeDef* hi2c, unsigned char* pucData,unsigned short usSize, EEPROMHandle_t *pxEEPROM){
	HAL_StatusTypeDef status;
	if (ucValidWr(pxEEPROM, usSize)){
		// valid
		for (unsigned short i = 0; i < usSize; i++){
			status = HAL_I2C_Mem_Write(hi2c, ADDRESS<<1, pxEEPROM->usWrAddr, 2, pucData, BYTE, 1000);
			HAL_Delay(100);
			if (status == HAL_ERROR){
				return status;
			}
			pxEEPROM->usWrAddr+=1;
			pucData++;
		}
	}
	else {
		// note enough space left
		status = HAL_ERROR;
	}
	return status;
}

unsigned char ucFull(EEPROMHandle_t* pxEEPROM){
	return (pxEEPROM->usWrAddr == MEM_SIZE);
}

unsigned char ucEmpty(EEPROMHandle_t* pxEEPROM){
	return (pxEEPROM->usRdAddr == pxEEPROM->usWrAddr);
}

unsigned char ucValidWr(EEPROMHandle_t* pxEEPROM, unsigned short usBytes){
	if (ucFull(pxEEPROM)){
		// EEPROM Full
		return 0;
	}
	if (ucEmpty(pxEEPROM)){
		// empty reset addresses
		pxEEPROM->usRdAddr = 0x0000;
		pxEEPROM->usWrAddr = 0x0000;
	}
	unsigned short usBytesLeft = usFreeSpace(pxEEPROM);
	return (usBytesLeft >= usBytes);
}

unsigned char ucValidRd(EEPROMHandle_t* pxEEPROM, unsigned short usBytes){
	if (ucFull(pxEEPROM) || ucEmpty(pxEEPROM)){
		// invalid read if Full or Empty
		return 0;
	}
	unsigned short usDataLeft = usDataAvailable(pxEEPROM);
	return (usDataLeft >= usBytes);
}
