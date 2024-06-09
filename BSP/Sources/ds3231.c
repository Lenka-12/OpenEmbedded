/*
 * ds3131.c
 *
 *  Created on: May 31, 2024
 *      Author: malef
 */
#include "ds3231.h"
/* Will make it user configurable soon*/
extern I2C_HandleTypeDef hi2c1;
static uint8_t ucExtractBit(uint8_t ucValue, uint8_t ucBitPos){
	uint8_t ucMask = 1 << ucBitPos;
	return (((ucValue & ucMask) >> ucBitPos)&0x01);
}
uint8_t ucDecimalToBCD(uint8_t ucDecimal){
  if (ucDecimal < 10) return ucDecimal;
  uint8_t ucBCD;
  ucBCD = ((ucDecimal/10)<<4) | ucDecimal%10;
  return ucBCD;
}

uint8_t ucBCDToDecimal(uint8_t ucBCD){
	if (ucBCD < 10) return ucBCD;
	uint8_t ucDecimal;
	ucDecimal = (ucBCD>>4)*10 + (ucBCD&0x0F);
	return ucDecimal;
}

static void vDateToArray(uint8_t ucDate[], RTCDate_t* pxDate){
	ucDate[0] = ucDecimalToBCD(pxDate->ucDay);
	ucDate[1] = ucDecimalToBCD(pxDate->ucDate);
	ucDate[2] = ucDecimalToBCD(pxDate->ucMonth);
	ucDate[3] = ucDecimalToBCD(pxDate->ucYear);
}

static void vTimeToArray(uint8_t ucTime[], RTCTime_t* pxTime){
	ucTime[0] = ucDecimalToBCD(pxTime->ucSeconds);
	ucTime[1] = ucDecimalToBCD(pxTime->ucMinutes);
	ucTime[2] = pxTime->ucTimeFormat|ucDecimalToBCD(pxTime->ucHours);
}

static void vArrayToTime(uint8_t ucTime[], RTCTime_t* pxTime){
	pxTime->ucSeconds    = ucBCDToDecimal(ucTime[0]);
	pxTime->ucMinutes    = ucBCDToDecimal(ucTime[1]);
	uint8_t ucHours;
	// check time format bit: 12 vs 24
	if ((ucTime[2]&(1<<6)) == TIME_FORMAT_12HRS){
		ucHours      = ucTime[2]&(~TIME_FORMAT_12HRS_MASK);
		pxTime->ucTimeFormat = ucTime[2]&(TIME_FORMAT_12HRS_MASK);
	}
	else {
		// 24 hrs
		ucHours      = ucTime[2]&(~TIME_FORMAT_24HRS_MASK);
		pxTime->ucTimeFormat = TIME_FORMAT_24HRS;
	}
	pxTime->ucHours      = ucBCDToDecimal(ucHours);
}

static void vArrayToDate(uint8_t ucDate[], RTCDate_t* pxDate){
	pxDate->ucDay    = ucBCDToDecimal(ucDate[0]);
	pxDate->ucDate   = ucBCDToDecimal(ucDate[1]);
	pxDate->ucMonth  = ucBCDToDecimal(ucDate[2]);
	pxDate->ucYear   = ucBCDToDecimal(ucDate[3]);
}

static HAL_StatusTypeDef xDS3231_SetAlarm(RTCAlarmHandle_t* pxAlarm, uint8_t ucStartBit, uint8_t ucStartAddr){
	HAL_StatusTypeDef xStatus;
	// arrays declaration
	uint8_t ucAlarm[4];
	vTimeToArray(ucAlarm, pxAlarm->pxTimes[ucStartBit]);
	// Date and Day Alarm configuration
	switch (pxAlarm->ucAlarmRates[ucStartBit]){
	case DS3231_ALARM1_WHEN_SEC_MIN_HRS_DAY_MATCH:
		// SET DY/!DT bit 6
		ucAlarm[3] |= (ucDecimalToBCD(pxAlarm->pxDates[ucStartBit]->ucDay) | (DS3231_DAY_ALARM_ENABLE<6));
		break;
	case DS3231_ALARM1_WHEN_SEC__MIN_HRS_DATE_MATCH:
		ucAlarm[3] |= ucDecimalToBCD(pxAlarm->pxDates[ucStartBit]->ucDate);
		break;
	default:
		if (pxAlarm->ucAlarmDYOrDT[ucStartBit] == DS3231_DAY_ALARM_ENABLE){
			// Day of the week so set up DY/!DT
			ucAlarm[3] = (ucDecimalToBCD(pxAlarm->pxDates[ucStartBit]->ucDay) | DS3231_DAY_ALARM_ENABLE<<6);
		}
		else {
			// DY/!DT low
			ucAlarm[3] = ucDecimalToBCD(pxAlarm->pxDates[ucStartBit]->ucDate);
		}
		for (uint8_t i = ucStartBit; i < 4; i++){
			// set up AxMx bit i.e MSB
			ucAlarm[i] |= ((ucExtractBit(pxAlarm->ucAlarmRates[ucStartBit], (i-ucStartBit)))<<7);
		}
		break;
	}

	/* Set Alarm
	 * If its Alarm2, ucStartbit should be 1 and 0 for Alarm1
	 * */
	xStatus = HAL_I2C_Mem_Write(&hi2c1, DS3231_SLAVE_ADDR<<1, ucStartAddr, DS3231_MEM_SIZE, &ucAlarm[ucStartBit], 4-ucStartBit, 1000);

	return xStatus;
}

HAL_StatusTypeDef xDS3231SetCurrentTime(RTCTime_t* pxTime){
	HAL_StatusTypeDef xStatus;
	uint8_t ucTime[3];
	vTimeToArray(ucTime, pxTime);
	xStatus = HAL_I2C_Mem_Write(&hi2c1, DS3231_SLAVE_ADDR<<1, DS3231_ADDR_SEC, DS3231_MEM_SIZE, ucTime, 3, 1000);
	return xStatus;
}

HAL_StatusTypeDef xDS3231GetCurrentTime(RTCTime_t* pxTime){
	HAL_StatusTypeDef xStatus;
	uint8_t ucTime[3];
	xStatus = HAL_I2C_Mem_Read(&hi2c1, DS3231_SLAVE_ADDR<<1, DS3231_ADDR_SEC, DS3231_MEM_SIZE, ucTime, 3, 1000);
	if(xStatus != HAL_OK) return xStatus;
	vArrayToTime(ucTime, pxTime);
	return xStatus;
}

HAL_StatusTypeDef xDS3231SetCurrentDate(RTCDate_t* pxDate){
	HAL_StatusTypeDef xStatus;
	uint8_t ucDate[4];
	vDateToArray(ucDate, pxDate);
	xStatus = HAL_I2C_Mem_Write(&hi2c1, DS3231_SLAVE_ADDR<<1, DS3231_ADDR_DAY, DS3231_MEM_SIZE, ucDate, 4, 1000);
	return xStatus;
}

HAL_StatusTypeDef xDS3231GetCurrentDate(RTCDate_t* pxDate){
	HAL_StatusTypeDef xStatus;
	uint8_t ucDate[4];
	xStatus = HAL_I2C_Mem_Read(&hi2c1, DS3231_SLAVE_ADDR<<1, DS3231_ADDR_DAY, DS3231_MEM_SIZE, ucDate, 4, 1000);
	if(xStatus != HAL_OK) return xStatus;
	vArrayToDate(ucDate, pxDate);
	return xStatus;
}

HAL_StatusTypeDef xDS323132kHzSQWConfig(uint8_t ucStatus){
	uint8_t ucData = 0x80 | (ucStatus<<3);
	HAL_StatusTypeDef xStatus;
	xStatus = HAL_I2C_Mem_Write(&hi2c1, DS3231_SLAVE_ADDR<<1, DS3231_ADDR_CONTROL_STATUS, DS3231_MEM_SIZE, &ucData, 1, HAL_MAX_DELAY);

	return xStatus;
}

void vFormaToString(RTCTime_t* pxTime, char* pcFormat){
	switch(pxTime->ucTimeFormat){
	case TIME_FORMAT_12HRS_AM:
		pcFormat[0] = 'A';
		pcFormat[1] = 'M';
		break;
	case TIME_FORMAT_12HRS_PM:
		pcFormat[0] = 'P';
		pcFormat[1] = 'M';
		break;
	case TIME_FORMAT_24HRS:
		pcFormat[0] = '\0';
		pcFormat[1] = '\0';
		break;
	default:
		pcFormat[0] = '\0';
		pcFormat[1] = '\0';
		break;
	}
}

HAL_StatusTypeDef xDS3231SQWConfig(uint8_t ucFreq){
	HAL_StatusTypeDef xStatus;
	uint8_t ucValue = 0x00;
	ucValue |= (ucFreq << 3);
	xStatus = HAL_I2C_Mem_Write(&hi2c1, DS3231_SLAVE_ADDR<<1, DS3231_ADDR_CONTROL, DS3231_MEM_SIZE, &ucValue, 1, 1000);

	return xStatus;
}

HAL_StatusTypeDef xDS3231AlarmConfig(RTCAlarmHandle_t* pxAlarmInfo){
	HAL_StatusTypeDef xStatus;
	switch (pxAlarmInfo->ucAlarmToEnable){
	case DS3231_CONTROL_A1IE_A2IE_SET:
		// enable both alarms
		xStatus = xDS3231_SetAlarm(pxAlarmInfo, 0, DS3231_ADDR_ALARM1_SEC);
		if (xStatus != HAL_OK) return xStatus;
		HAL_Delay(100);
		xStatus = xDS3231_SetAlarm(pxAlarmInfo, 1, DS3231_ADDR_ALARM2_MIN);
		break;
	case DS3231_CONTROL_A1IE_SET:
		// ENABLE ALARM1
		xStatus = xDS3231_SetAlarm(pxAlarmInfo, 0, DS3231_ADDR_ALARM1_SEC);
		break;
	case DS3231_CONTROL_A2IE_SET:
		// ENABLE ALARM2
		xStatus = xDS3231_SetAlarm(pxAlarmInfo, 1, DS3231_ADDR_ALARM2_MIN);
		break;
	default:
		// Make sure that we have 00 or 01 or 11
		xStatus = HAL_ERROR;
		break;
	}
	if (xStatus != HAL_OK){
		return xStatus;
	}
	HAL_Delay(100);
	// configure control register
	uint8_t ucControlReg;
	xStatus = HAL_I2C_Mem_Read(&hi2c1, DS3231_SLAVE_ADDR<<1, DS3231_ADDR_CONTROL, DS3231_MEM_SIZE ,&ucControlReg, 1, 1000);
	if (xStatus != HAL_OK ) return xStatus;
	HAL_Delay(100);
	ucControlReg |= ((1<<2) | pxAlarmInfo->ucAlarmToEnable);
	xStatus = HAL_I2C_Mem_Write(&hi2c1, DS3231_SLAVE_ADDR<<1, DS3231_ADDR_CONTROL, DS3231_MEM_SIZE ,&ucControlReg, 1, 1000);
	return xStatus;
}

void vClearStatus(){
	/* clear status bits*/
	unsigned char value = 0x80;
	HAL_I2C_Mem_Write(&hi2c1, DS3231_SLAVE_ADDR<<1, DS3231_ADDR_CONTROL_STATUS, DS3231_MEM_SIZE ,&value, 1, 1000);
}

