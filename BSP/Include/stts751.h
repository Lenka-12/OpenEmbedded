/*
 * stts751.h
 *
 *  Created on: Jun 7, 2024
 *      Author: malef
 */

#ifndef INCLUDE_STTS751_H_
#define INCLUDE_STTS751_H_
#include "main.h"
#include "Config.h"
/* Registers */
#define STTS751_T_H                        0x00
#define STTS751_STATUS                     0x01
#define STTS751_T_L                        0x02
#define STTS751_CONFIG                     0x03
#define STTS751_CONV_RATE                  0x04
#define STTS751_HTL_H                      0x05
#define STTS751_HTL_L                      0x06
#define STTS751_LTL_H                      0x07
#define STTS751_LTL_L                      0x08
#define STTS751_ONE_SHOT                   0x0F
#define STTS751_THERM_LIMIT                0x20
#define STTS751_THERM_HYST                 0x21
#define STTS751_SMBUS_TIMEOUT_EN           0x22
#define STTS751_PROD_ID                    0xFD
#define STTS751_MAN_ID                     0xFE
#define STTS751_REV_NUM                    0xFF

/* RUN/STOP */
#define STTS751_STANDBY_MODE               BIT_SHIFT(0x1,6)
#define STTS751_CONT_CONV                  BIT_SHIFT(0x0,6)
/* EVENT CONFIG */
#define STTS751_EVENT_EN                   BIT_SHIFT(0x0,7)
#define STTS751_EVENT_DIS                  BIT_SHIFT(0x1,7)
/* RESOLUTION */
#define STTS751_RES_10_BITS                BIT_SHIFT(0x0,2)
#define STTS751_RES_11_BITS                BIT_SHIFT(0x1,2)
#define STTS751_RES_12_BITS                BIT_SHIFT(0x3,2)
#define STTS751_RES_9_BITS                 BIT_SHIFT(0x2,2)

/* Conversions per second */
#define STTS751_CONV_0_0625	              BIT_SHIFT(0x0,0)     /*0.0625*/
#define STTS751_CONV_0_1250	              BIT_SHIFT(0x1,0)     /*0.125*/
#define STTS751_CONV_0_2500	              BIT_SHIFT(0x2,0)
#define STTS751_CONV_0_5000	              BIT_SHIFT(0x3,0)
#define STTS751_CONV_1_0000	              BIT_SHIFT(0x4,0)
#define STTS751_CONV_2_0000	              BIT_SHIFT(0x5,0)
#define STTS751_CONV_4_0000	              BIT_SHIFT(0x6,0)
#define STTS751_CONV_8_0000	              BIT_SHIFT(0x7,0)
#define STTS751_CONV_16_000	              BIT_SHIFT(0x8,0)
#define STTS751_CONV_32_000	              BIT_SHIFT(0x9,0)  /* 32.000 */

#if !defined(STTS751_I2C)
	#define STTS751_I2C hi2c1
#endif

#define STTS751_ADDR                       (unsigned char)(0x4A)
#define ARA_ADDR                           (unsigned char)(0xC)

#define STTS751_CONFIG_MASK                 (unsigned char)(0xCC)
#define STTS751_CONV_MASK                   (unsigned char)(0x0F)

#define STTS751_TEMP_MAX                     (float)(127.9375)
#define STTS751_TEMP_MIN                     (float)(-64)

#define STTS751_RES_12_BITS_MASK              (unsigned char)(0xF0)
#define STTS751_RES_11_BITS_MASK              (unsigned char)(0xE0)
#define STTS751_RES_10_BITS_MASK              (unsigned char)(0xC0)
#define STTS751_RES_9_BITS_MASK               (unsigned char)(0x80)
typedef struct{
	uint8_t config;
	uint8_t conv_rate;

}STTS751Handle_t;
extern I2C_HandleTypeDef hi2c1;
HAL_StatusTypeDef xSTTS751SendByte(uint8_t Register, uint8_t* byte);
HAL_StatusTypeDef xSTTS751ReadByte(uint8_t Register, uint8_t* byte);
HAL_StatusTypeDef xSTTS751SetHighTempLimit(float fupperLimit, STTS751Handle_t*  pxSTTS751);
HAL_StatusTypeDef xSTTS751SetTempLimit(float fupperLimit, STTS751Handle_t*  pxSTTS751, uint8_t HighByteReg);
HAL_StatusTypeDef xSTTS751SetLowTempLimit(float flowerLimit, STTS751Handle_t*  pxSTTS751);
HAL_StatusTypeDef xSTTS751SetThermalLimit(signed char scThermLimit, signed char scThermHyst);
HAL_StatusTypeDef xSTTS751Init(STTS751Handle_t* pxSTTS751);
HAL_StatusTypeDef xSTTS751GetT(float* pfValue);
HAL_StatusTypeDef xSTTS751Init(STTS751Handle_t* pxSTTS751);
HAL_StatusTypeDef xSTTS751ARA(uint8_t* buffer);
#endif /* INCLUDE_STTS751_H_ */
