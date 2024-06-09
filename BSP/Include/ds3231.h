/*
 * ds3231.h
 *
 *  Created on: May 31, 2024
 *      Author: malef
 */

#ifndef DS3231_H_
#define DS3231_H_
/*Includes*/
#include "main.h"
/* registers */
#define DS3231_ADDR_SEC                                   0x00
#define DS3231_ADDR_MIN                                   0x01
#define DS3231_ADDR_HRS                                   0x02
#define DS3231_ADDR_DAY                                   0x03
#define DS3231_ADDR_DATE                                  0x04
#define DS3231_ADDR_MONTH                                 0x05
#define DS3231_ADDR_YEAR                                  0x06
#define DS3231_ADDR_ALARM1_SEC                            0x07
#define DS3231_ADDR_ALARM1_MIN                            0x08
#define DS3231_ADDR_ALARM1_HRS                            0x09
#define DS3231_ADDR_ALARM1_DAY_DATE                       0x0A
#define DS3231_ADDR_ALARM2_MIN                            0x0B
#define DS3231_ADDR_ALARM2_HRS                            0x0C
#define DS3231_ADDR_ALARM2_DAY_DATE                       0x0D
#define DS3231_ADDR_CONTROL                               0x0E
#define DS3231_ADDR_CONTROL_STATUS                        0x0F
#define DS3231_ADDR_AGING_OFFSET                          0x10
#define DS3231_ADDR_TEMPERATURE_MSB                       0x11
#define DS3231_ADDR_TEMPERATURE_LSB                       0x12

/* @User Time format*/
#define TIME_FORMAT_12HRS_AM       0x40
#define TIME_FORMAT_12HRS_PM       0x60
#define TIME_FORMAT_24HRS          0x00


#define TIME_FORMAT_12HRS_MASK     0xE0
#define TIME_FORMAT_24HRS_MASK     0xC0


#define TIME_FORMAT_12HRS             0x40

#define DS3231_MEM_SIZE 1

#define DS3231_SLAVE_ADDR   0x68

/* Days of the week */
#define SUNDAY          1
#define MONDAY          2
#define TUESDAY         3
#define WEDNESDAY       4
#define THURSDAY        5
#define FRIDAY          6
#define SATURDAY        7
/*@Date or Day enable*/
#define DS3231_DAY_ALARM_ENABLE            1
#define DS3231_DATE_ALARM_ENABLE           0



/*@ALARM1 RATE MASKS*/
#define DS3231_ALARM1_ONCE_PER_SEC                           0x0F
#define DS3231_ALARM1_WHEN_SEC_MATCH                         0x0E
#define DS3231_ALARM1_WHEN_MIN_SEC_MATCH                     0x0C
#define DS3231_ALARM1_WHEN_SEC_MIN_HRS_MATCH                 0x08
#define DS3231_ALARM1_WHEN_SEC__MIN_HRS_DATE_MATCH           0x00
#define DS3231_ALARM1_WHEN_SEC_MIN_HRS_DAY_MATCH             0x10
/*@ALARM2 RATE MASKS*/
#define DS3231_ALARM2_ONCE_PER_MIN                           0x07
#define DS3231_ALARM2_WHEN_MIN_MATCH                         0x06
#define DS3231_ALARM2_WHEN_MIN_HRS_MATCH                     0x04
#define DS3231_ALARM2_WHEN_MIN_HRS_DATE_MATCH                0x00
#define DS3231_ALARM2_WHEN_MIN_HRS_DAY_MATCH                 0x08
/* SQUARE output FREQ */
#define DS3231_SQUARE_WAVE_1Hz                              0x00
#define DS3231_SQUARE_WAVE_1kHz                             0x01
#define DS3231_SQUARE_WAVE_4kHz                             0x02
#define DS3231_SQUARE_WAVE_8kHz                             0x03

#define DS3231_CONTROL_OSC_ENABLE                           0
#define DS3231_CONTROL_OSC_DISABLE                          1
/* @Alarm Interrupt Enable*/
#define DS3231_CONTROL_A1IE_SET                                  0b01
#define DS3231_CONTROL_A2IE_SET                                  0b10
#define DS3231_CONTROL_A1IE_A2IE_SET                             0b11

/*
 * Date Handle structure,
 * */
typedef struct{
	uint8_t ucDate;
	uint8_t ucMonth;
	uint8_t ucYear;
	uint8_t ucDay;
}RTCDate_t;
/*
 * Time Handle structure
 * */
typedef struct{
	uint8_t ucSeconds;
	uint8_t ucMinutes;
	uint8_t ucHours;
	/*See @User Time format*/
	uint8_t ucTimeFormat;
}RTCTime_t;
/*
 * Alarm Handle
 * DS3231 has 2 Alarms so most arrays have two elements
 * Index 0 -> Alarm 1
 * Index 1 -> Alarm 2
 * If User does it other way round, the alarm behavior will be incorrect
 * */
typedef struct{
	RTCDate_t* pxDates[2];
	RTCTime_t* pxTimes[2];
	/* See @Alarm Interrupt Enable, This is used to enable interrupt for either Alarm or both*/
	uint8_t    ucAlarmToEnable;
	/*Specify Alarm frequency, see @ALARM2 RATE MASKS and @ALARM1 RATE MASKS*/
	uint8_t    ucAlarmRates[2];
	/* Both indexes should be either 1 or 0,see @Date or Day enable*/
	uint8_t    ucAlarmDYOrDT[2];
}RTCAlarmHandle_t;

/* Date and Time Set and Get */
HAL_StatusTypeDef xDS3231SetCurrentTime(RTCTime_t* pxTime);
HAL_StatusTypeDef xDS3231GetCurrentTime(RTCTime_t* pxTime);
HAL_StatusTypeDef xDS3231SetCurrentDate(RTCDate_t* pxDate);
HAL_StatusTypeDef xDS3231GetCurrentDate(RTCDate_t* pxDate);

/* SQUARE WAVE Settings */
HAL_StatusTypeDef xDS323132kHzSQWConfig(uint8_t ucStatus);
HAL_StatusTypeDef xDS3231SQWConfig(uint8_t ucFreq);

/* Alarm Config */
HAL_StatusTypeDef xDS3231AlarmConfig(RTCAlarmHandle_t* pxAlarmInfo);

void vClearStatus();
/* Helper functions */
void vFormaToString(RTCTime_t* pxTime, char* pcFormat);
uint8_t ucDecimalToBCD(uint8_t ucDecimal);
uint8_t ucBCDToDecimal(uint8_t ucBCD);

#endif /* DS3231_H_ */
