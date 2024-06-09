/*
 * HTS221.c
 *
 *  Created on: Jun 6, 2024
 *      Author: malef
 */


#include "HTS221.h"

HAL_StatusTypeDef xHTS221Init(HTS221Handle_t* pHTS221){

	HAL_StatusTypeDef xStatus;

	/* Configure control registers */

	// ctrl_reg1
	pHTS221->ctrl_reg1 = pHTS221->ctrl_reg1&HTS221_CTRL_REG1_MASK;
	xStatus =  xHTS221WriteReg(HTS221_CTRL_REG1, &pHTS221->ctrl_reg1, 1);
	if (xStatus != HAL_OK) return xStatus;
	HAL_Delay(100);

	// ctrl_reg2
	pHTS221->ctrl_reg2 = pHTS221->ctrl_reg2&HTS221_CTRL_REG2_MASK;
	xStatus =  xHTS221WriteReg(HTS221_CTRL_REG2, &pHTS221->ctrl_reg2, 1);
	if (xStatus != HAL_OK) return xStatus;
	HAL_Delay(100);

	// ctrl_reg3
	pHTS221->ctrl_reg3 = pHTS221->ctrl_reg3&HTS221_CTRL_REG3_MASK;
	xStatus =  xHTS221WriteReg(HTS221_CTRL_REG3, &pHTS221->ctrl_reg3, 1);
	if (xStatus != HAL_OK) return xStatus;
	HAL_Delay(100);

	// average
	pHTS221->average_config &= HTS221_A_CONF_MASK;
	return xHTS221WriteReg(HTS221_AV_CONF, &pHTS221->average_config, 1);


}

HAL_StatusTypeDef xHTS221WriteReg(uint8_t Register,uint8_t* buffer, uint16_t usSize){
	return HAL_I2C_Mem_Write(&I2C,HTS221_ADDR<<1, Register, 1, buffer, usSize, HAL_MAX_DELAY);
}
HAL_StatusTypeDef xHTS221ReadReg(uint8_t Register,uint8_t* buffer, uint16_t usSize){
	if (usSize >= 2){
		// auto increment
		return HAL_I2C_Mem_Read(&I2C,HTS221_ADDR<<1, Register|HTS221_AUTO_INC, 1, buffer, usSize, HAL_MAX_DELAY);
	}
	// 1 byte
	return HAL_I2C_Mem_Read(&I2C,HTS221_ADDR<<1, Register, 1, buffer, usSize, HAL_MAX_DELAY);
}

HAL_StatusTypeDef xHTS221GetT(float* pfValue){
	uint8_t buffer[4];
	uint16_t T0_degC, T1_degC;
	uint8_t temp;
	/* 1. Read the value of coefficients T0_degC_x8 and T1_degC_x8 from registers 0x32 & 0x33.
	 */
	/*3. Read the MSB bits of T1_degC (T1.9 and T1.8 bit) and T0_degC (T0.9 and T0.8 bit) from register 0x35 to
	 	 compute T0_degC and T1_degC.
	*/
	/*  Divide by 8 the content of registers 0x32 (T0_degC_x8) and 0x33 (T1_degC_x8)
	 * in order to obtain the value
	 * of coefficients T0_degC and T1_degC */
	if (xHTS221ReadReg(HTS221_T0_DEGC_X8, buffer, 2)  != HAL_OK)
		return HAL_ERROR;

	if (xHTS221ReadReg(HTS221_T1_OR_T0_MSB, &temp, 1)  != HAL_OK)
			return HAL_ERROR;
	temp = temp & 0x0F;
	T0_degC = ((temp&0x03)<<8) | buffer[0];
	T1_degC = ((temp&0x0C)<<6) | buffer[1];

	T0_degC = T0_degC >> 3;
	T1_degC = T1_degC >> 3;
	/* 4. Read from registers 0x3C & 0x3D the value of T0_OUT */
	/* 5. Read from 0x3E & 0x3F registers the value of  T1_OUT*/
	unsigned short T0_OUT,T1_OUT,T_OUT;
	if (xHTS221ReadReg(HTS221_T0_OUT_L, buffer, 4)  != HAL_OK)
			return HAL_ERROR;
	T0_OUT = buffer[1]<< 8 |  buffer[0];
	T1_OUT = buffer[3]<< 8 |  buffer[2];
	/* 6. Read the value T_OUT (ADC_OUT) from 0x2A & 0x2B registers.*/
	if (xHTS221ReadReg(HTS221_T_OUT_L, buffer, 2)  != HAL_OK)
				return HAL_ERROR;
	T_OUT = buffer[1]<< 8 |  buffer[0];

	/* Compute the T [degC] value, by linear interpolation, applying the following formula */
	unsigned short usDenom = T1_OUT - T0_OUT;
	unsigned long uiNumerator = (T1_degC - T0_degC)*(T_OUT-T0_OUT);

	*pfValue = (float)uiNumerator  / (float)usDenom;
	*pfValue += T0_degC;
	__NOP();
	return HAL_OK;
}

HAL_StatusTypeDef xHTS221GetH(float* pfValue){
	uint8_t buffer[2];
	unsigned short H0_rH, H1_rH;
	short H_OUT, H0_T0_OUT, H1_T0_OUT;

	/* 1. Read the value of coefficients H0_rH_x2 and H1_rH_x2 from registers 0x30 & 0x31 */
	if (xHTS221ReadReg(HTS221_H0_RH_X2, buffer, 2) != HAL_OK)
		return HAL_ERROR;

	/* 2. Divide by two the content of registers 0x30 (H0_rH_x2) and 0x31 (H1_rH_x2)
	 * in order to obtain the value of coefficients H0_rH and H1_rH. */
	H0_rH = buffer[0] >> 2;
	H1_rH = buffer[1] >> 2;

	/*3. Read the value of H0_T0_OUT from registers 0x36 & 0x37 */
	if (xHTS221ReadReg(HTS221_H0_T0_OUT_L, buffer, 2) != HAL_OK)
		return HAL_ERROR;
	H0_T0_OUT = buffer[1]<<8 | buffer[0];

	/*4. Read the value of H1_T0_OUT from registers 0x3A & 0x3B*/
	if (xHTS221ReadReg(HTS221_H1_T0_OUT_L, buffer, 2) != HAL_OK)
		return HAL_ERROR;
	H1_T0_OUT = buffer[1]<<8 | buffer[0];

	/*5 Read the humidity value in raw counts H_T_OUT from registers 0x28 & 0x29 */
	if (xHTS221ReadReg(HTS221_H_OUT_L, buffer, 2) != HAL_OK)
		return HAL_ERROR;
	H_OUT = buffer[1]<<8 | buffer[0];

	/* 6 Compute the RH [%] value, by linear interpolation */
	short denominator = H1_T0_OUT - H0_T0_OUT;
	long numerator = (H1_rH-H0_rH)*(H_OUT-H0_T0_OUT);
	*pfValue = (float)numerator / (float)denominator;
	*pfValue += H0_rH;

	return HAL_OK;


}

HAL_StatusTypeDef xHTS221OneShotEnable(){
	uint8_t buffer = HTS221_ONE_SHOT_EN;
	return xHTS221WriteReg(HTS221_CTRL_REG2,&buffer ,1);
}
