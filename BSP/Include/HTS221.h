/*
 * HTS221.h
 *
 *  Created on: Jun 6, 2024
 *      Author: malef
 */

#ifndef INCLUDE_HTS221_H_
#define INCLUDE_HTS221_H_
// Includes
#include "main.h"
#include "Config.h"

#define HTS221_ADDR                          0x5F
#define HTS221_AUTO_INC                      (unsigned char)(1<<7)
/*register addresses*/
// Who am I / or device Identification Register
#define HTS221_DEVICE_ID                     (unsigned char)(0x0F)
// Humidity and Temperature average samples configuration register
#define HTS221_AV_CONF                       (unsigned char)(0x10)
// Control Registers
#define HTS221_CTRL_REG1                     (unsigned char)(0x20)
#define HTS221_CTRL_REG2                     (unsigned char)(0x21)
#define HTS221_CTRL_REG3                     (unsigned char)(0x22)
// Status Register
#define HTS221_STATUS_REG                    (unsigned char)(0x27)

// Humidity output registers  L-Lower Byte U-Upper Byte
#define HTS221_H_OUT_L                       (unsigned char)(0x28)
#define HTS221_H_OUT_H                       (unsigned char)(0x29)

// Temperature output registers  L-Lower Byte U-Upper Byte
#define HTS221_T_OUT_L                	     (unsigned char)(0x2A)
#define HTS221_T_OUT_H                       (unsigned char)(0x2B)

// Calibration Registers
#define HTS221_H0_RH_X2                           (unsigned char)(0x30)
#define HTS221_H1_RH_X2                           (unsigned char)(0x31)
#define HTS221_T0_DEGC_X8                         (unsigned char)(0x32)
#define HTS221_T1_DEGC_X8                         (unsigned char)(0x33)
#define HTS221_T1_OR_T0_MSB                       (unsigned char)(0x35)
#define HTS221_H0_T0_OUT_L                        (unsigned char)(0x36)
#define HTS221_H0_T0_OUT_H                        (unsigned char)(0x37)
#define HTS221_H1_T0_OUT_L                        (unsigned char)(0x3A)
#define HTS221_H1_T0_OUT_H                        (unsigned char)(0x3B)
#define HTS221_T0_OUT_L                           (unsigned char)(0x3C)
#define HTS221_T0_OUT_H                           (unsigned char)(0x3D)
#define HTS221_T1_OUT_L                           (unsigned char)(0x3E)
#define HTS221_T1_OUT_H                           (unsigned char)(0x3F)

/* Humidity configuration options */
#define HTS221_H_AVG_SAMPLES_4                             BIT_SHIFT(0b000,0)
#define HTS221_H_AVG_SAMPLES_8                             BIT_SHIFT(0b001,0)
#define HTS221_H_AVG_SAMPLES_16                            BIT_SHIFT(0b010,0)
#define HTS221_H_AVG_SAMPLES_32                            BIT_SHIFT(0b011,0)
#define HTS221_H_AVG_SAMPLES_64                            BIT_SHIFT(0b100,0)
#define HTS221_H_AVG_SAMPLES_128                           BIT_SHIFT(0b101,0)
#define HTS221_H_AVG_SAMPLES_256                           BIT_SHIFT(0b110,0)
#define HTS221_H_AVG_SAMPLES_512                           BIT_SHIFT(0b111,0)


/* Temperature configuration options */
#define HTS221_T_AVG_SAMPLES_2                                 BIT_SHIFT(0b000,3)
#define HTS221_T_AVG_SAMPLES_4                                 BIT_SHIFT(0b001,3)
#define HTS221_T_AVG_SAMPLES_8                                 BIT_SHIFT(0b010,3)
#define HTS221_T_AVG_SAMPLES_16                                BIT_SHIFT(0b011,3)
#define HTS221_T_AVG_SAMPLES_32                                BIT_SHIFT(0b100,3)
#define HTS221_T_AVG_SAMPLES_64                                BIT_SHIFT(0b101,3)
#define HTS221_T_AVG_SAMPLES_128                               BIT_SHIFT(0b110,3)
#define HTS221_T_AVG_SAMPLES_256                               BIT_SHIFT(0b111,3)

/*************************************
 * Other user configurable options
 *************************************
 */

/* Output Data Rate  (ODR)*/
#define HTS221_ODR_1Hz                                              BIT_SHIFT(0b01,0)  /* 1Hz */
#define HTS221_ODR_7Hz                                              BIT_SHIFT(0b10,0)  /* 7Hz */
#define HTS221_ODR_12_5Hz                                           BIT_SHIFT(0b11,0)  /* 12.5Hz */

// One shot
#define HTS221_ONE_SHOT                                             BIT_SHIFT(0b00,0)

/* Power-down control */
#define HTS221_POWER_DOWN_MODE                                      BIT_SHIFT(0b00,7)
#define HTS221_ACTIVE_MODE                                          BIT_SHIFT(0b01,7)

/* Block Data Update (BDU) */
#define HTS221_CONTINUOUS_BDU                                       BIT_SHIFT(0b0,2)
// output registers not updated until MSB and LSB reading
#define HTS221_BLOCKING_BDU                                         BIT_SHIFT(0b1,2)


/* Reboot memory content */
#define HTS221_REBOOT                                              BIT_SHIFT(0b1,7)
#define HTS221_NORMAL_MODE                                         BIT_SHIFT(0b0,7)

/* Heater control */
#define HTS221_HEATER_ON                                           BIT_SHIFT(0b1,1)
#define HTS221_HEATER_OFF                                          BIT_SHIFT(0b0,1)


/* One shot control */
#define HTS221_ONE_SHOT_EN                                            BIT_SHIFT(0b1,0)

/* Data Ready (DRDY) control */
// Output Polarity
#define HTS221_DRDY_ACTIVE_HIGH                                   BIT_SHIFT(0b0,7)
#define HTS221_DRDY_ACTIVE_LOW                                    BIT_SHIFT(0b1,7)

// DRDY push pull (PP) or open drain (OD)
#define HTS221_DRDY_PP                                            BIT_SHIFT(0b0,6)
#define HTS221_DRDY_OD                                            BIT_SHIFT(0b1,6)

// Data ready enable (EN) or disable(DIS)
#define HTS221_DRDY_EN                                            BIT_SHIFT(0b1,2)
#define HTS221_DRDY_DIS                                           BIT_SHIFT(0b0,2)


// Register Masks
#define HTS221_CTRL_REG1_MASK                                   0x87
#define HTS221_CTRL_REG2_MASK                                   0x01
#define HTS221_CTRL_REG3_MASK                                   0xC4;
#define HTS221_A_CONF_MASK                                      0x3F;


#define I2C hi2c1

typedef struct{
	/* ODR, PD, BDU */
	uint8_t ctrl_reg1;
	/* one shot, heater, boot */
	uint8_t ctrl_reg2;
	/* Data ready */
	uint8_t ctrl_reg3;
	/* average samples */
	uint8_t average_config;
	// I2C handle

}HTS221Handle_t;
extern I2C_HandleTypeDef I2C;
HAL_StatusTypeDef xHTS221Init(HTS221Handle_t* pHTS221);
HAL_StatusTypeDef xHTS221WriteReg(uint8_t Register,uint8_t* buffer, uint16_t usSize);
HAL_StatusTypeDef xHTS221ReadReg(uint8_t Register,uint8_t* buffer, uint16_t usSize);
HAL_StatusTypeDef xHTS221OneShotEnable();
HAL_StatusTypeDef xHTS221GetT(float* pfValue);
HAL_StatusTypeDef xHTS221GetH(float* pfValue);
#endif /* INCLUDE_HTS221_H_ */
