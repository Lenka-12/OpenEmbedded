/*
 * ssts751.c
 *
 *  Created on: June 7, 2024
 *      Author: Malefetsane
 */
#include "stts751.h"
/***
 * @brief Configures STTS751 basic parameters
 * @param[in] pxSTTS751 Pointer to Sensor handle structure
 * @return [HAL_OK, HAL_ERROR]
 */
HAL_StatusTypeDef xSTTS751Init(STTS751Handle_t* pxSTTS751){
	/* Disable SMBus timeout*/
	uint8_t timeout_diasble = 0x00;
	if (xSTTS751SendByte(STTS751_SMBUS_TIMEOUT_EN, &timeout_diasble) != HAL_OK){
		return HAL_ERROR;
	}
	/* Clear bits that are not implemented */
	pxSTTS751->config    &= STTS751_CONFIG_MASK;
	pxSTTS751->conv_rate  &= STTS751_CONV_MASK;

	/* Configuration */
	if (xSTTS751SendByte(STTS751_CONFIG, &pxSTTS751->config) != HAL_OK){
		return HAL_ERROR;
	}
	/* Set up conversion rate*/
	return xSTTS751SendByte(STTS751_CONV_RATE, &pxSTTS751->conv_rate);
}
/***
 * @brief Write a byte to STTS751
 * Define STTS751_I2C to i2c handle to be used, hi2c1 by default
 * @param[in] Register The register to write byte to
 * @param[in] pointer to byte to be sent
 * @return [HAL_OK, HAL_ERROR, HAL_BUSY]
 */
HAL_StatusTypeDef xSTTS751SendByte(uint8_t Register, uint8_t* byte){
	return HAL_I2C_Mem_Write(&STTS751_I2C, STTS751_ADDR<<1, Register, 1, byte, 1, HAL_MAX_DELAY);
}
/***
 * @brief Reads a byte from STTS751
 * Define STTS751_I2C to i2c handle to be used, hi2c1 by default
 * @param[in] Register The register to write byte to
 * @param[out] pointer to hold address of byet to be read
 * @return [HAL_OK, HAL_ERROR,HAL_BUSY]
 */
HAL_StatusTypeDef xSTTS751ReadByte(uint8_t Register, uint8_t* byte){
	return HAL_I2C_Mem_Read(&STTS751_I2C, STTS751_ADDR<<1, Register, 1, byte, 1, HAL_MAX_DELAY);
}
/**
 * @brief sets up temperature upper limit threshold [127.9375] max and -64 min
 * @param[in] fupperLimit, temperature upper limit threshold
 * @param[in] pxSTTS751 Pointer to Sensor handle structure
 * @return [HAL_OK, HAL_ERROR,HAL_BUSY]
 */
HAL_StatusTypeDef xSTTS751SetHighTempLimit(float fupperLimit,STTS751Handle_t*  pxSTTS751){
	return xSTTS751SetTempLimit(fupperLimit, pxSTTS751, STTS751_HTL_H);
}
/**
 * @brief sets up temperature lower limit threshold [127.9375] max and -64 min
 * @param[in] flowerLimit, temperature lower limit threshold
 * @param[in] pxSTTS751 Pointer to Sensor handle structure
 * @return [HAL_OK, HAL_ERROR,HAL_BUSY]
 */
HAL_StatusTypeDef xSTTS751SetLowTempLimit(float flowerLimit,STTS751Handle_t*  pxSTTS751){
	return xSTTS751SetTempLimit(flowerLimit, pxSTTS751, STTS751_LTL_H);
}
/**
 * @brief sets up temperature upper limit threshold [127.9375] max and -64 min
 * @param[in] fupperLimit, temperature upper limit threshold
 * @param[in] pxSTTS751 Pointer to Sensor handle structure
 * @param[in] HighByteReg high byte register for the temperature limit
 * @return [HAL_OK, HAL_ERROR,HAL_BUSY]
 */
HAL_StatusTypeDef xSTTS751SetTempLimit(float fupperLimit, STTS751Handle_t*  pxSTTS751, uint8_t HighByteReg){
	// clamp to min/max
	if (fupperLimit > STTS751_TEMP_MAX){
		fupperLimit = STTS751_TEMP_MAX;
	}
	else {
		if (fupperLimit < STTS751_TEMP_MIN){
			fupperLimit = STTS751_TEMP_MIN;
		}
	}

	// multiply by 16 to get whole number
	int temp = fupperLimit*16;
	// extract 12 bits, max resilution
	unsigned short usRegVal = temp&0xFFF;
	uint8_t UpperByte = usRegVal>>4;
	// lower byte, least significant bits are always zero
	uint8_t LowerByte = (usRegVal<<4)&0xFF;

	// set appropriate bits based on resolution
	uint8_t res = (pxSTTS751->config >> 2)&0x3;
	switch (res){
	case (STTS751_RES_12_BITS>>2):
		LowerByte&=STTS751_RES_12_BITS_MASK;
		break;
	case (STTS751_RES_11_BITS>>2):
		LowerByte&=STTS751_RES_11_BITS_MASK;
		break;
	case (STTS751_RES_10_BITS>>2):
		LowerByte&=STTS751_RES_10_BITS_MASK;
		break;
	case (STTS751_RES_9_BITS>>2):
		LowerByte&=STTS751_RES_9_BITS_MASK;
		break;
	default:
		// 10 bit res by default
		LowerByte&=STTS751_RES_10_BITS_MASK;
		break;
	}
	if (xSTTS751SendByte(HighByteReg, &UpperByte) != HAL_OK){
		return HAL_ERROR;
	}
	return xSTTS751SendByte(HighByteReg+1, &LowerByte);
}
/**
 * @brief sets up thermal threshold and hysteresis
 * @param[in] scThermLimit, thermal limit threshold
 * @param[in] scThermHyst,  thermal limit hysteresis
 * @return [HAL_OK, HAL_ERROR,HAL_BUSY]
 */
HAL_StatusTypeDef xSTTS751SetThermalLimit(signed char scThermLimit, signed char scThermHyst){
	if (xSTTS751SendByte(STTS751_THERM_LIMIT, (uint8_t*)&scThermLimit) != HAL_OK){
		return HAL_ERROR;
	}
	return xSTTS751SendByte(STTS751_THERM_HYST, (uint8_t*)&scThermHyst);
}
HAL_StatusTypeDef xSTTS751GetT(float* pfValue){
	uint8_t buffer[2];
	if (xSTTS751ReadByte(STTS751_T_H, &buffer[0]) != HAL_OK){
		return HAL_ERROR;
	}
	if (xSTTS751ReadByte(STTS751_T_L, &buffer[1]) != HAL_OK){
		return HAL_ERROR;
	}
	*pfValue = buffer[0] + (float)(buffer[1]>>4)/(16.0);
	__NOP();
}

HAL_StatusTypeDef xSTTS751ARA(uint8_t* buffer){
	return HAL_I2C_Master_Receive(&STTS751_I2C,ARA_ADDR<<1, buffer, 1, HAL_MAX_DELAY);
}
