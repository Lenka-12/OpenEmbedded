/*
 * AT24CXX.h
 *
 *  Modified on: May 31, 2024
 *      Author: malef
 */

#ifndef INC_AT24CXX_H_
#define INC_AT24CXX_H_
#include "main.h"
/*Change to your own address*/
// EEEPROM Address 1010A2A1A0
// if A0,A1,A2 are grounded -> 1010000 ->0x50
#if !defined(ADDRESS)
	#define ADDRESS 0x50
#endif

/*Change to your own memory size here*/
// EEPROM memory size in bytes
// for 32,768 x 8  -> MEM_SIZE = 32768 = 0x8000
#if !defined(MEM_SIZE)
	#define MEM_SIZE 0x8000
#endif
struct EEPROM {
	 unsigned short usWrAddr;
	 unsigned short usRdAddr;
};
typedef struct EEPROM EEPROMHandle_t;

// writes 1 byte to the EEPROM
HAL_StatusTypeDef xWrite(I2C_HandleTypeDef* hi2c, unsigned char* pucData,unsigned short usSize, EEPROMHandle_t *pxEEPROM);
HAL_StatusTypeDef xRead(I2C_HandleTypeDef* hi2c, unsigned char* pucData,unsigned short usSize, EEPROMHandle_t *pxEEPROM);
unsigned char ucFull(EEPROMHandle_t* pxEEPROM);
unsigned char ucEmpty(EEPROMHandle_t* pxEEPROM);
unsigned char ucValidWr(EEPROMHandle_t* pxEEEPROM, unsigned short usBytes);
unsigned char ucValidRd(EEPROMHandle_t* pxEEEPROM, unsigned short usBytes);
unsigned short usFreeSpace(EEPROMHandle_t* pxEEPROM);
unsigned short usDataAvailable(EEPROMHandle_t* pxEEPROM);

#endif /* INC_AT24CXX_H_ */
