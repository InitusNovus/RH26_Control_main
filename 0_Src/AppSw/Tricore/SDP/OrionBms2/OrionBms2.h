/*
 * OrionBms2.h
 * Created on: 2020.08.04
 * Author: Dua
 */

#ifndef ORIONBMS2_H
#define ORIONBMS2_H
/**************************** Includes *******************************/
#include "CanCommunication.h"

/***************************** Macro *********************************/


/************************* Data Structures ***************************/
typedef struct 
{
	uint16 packCurrent;
	uint16 packVoltage;
	uint8 packSoc;
}OrionBms2_msg1_t;

typedef struct 
{
	uint16 packChargeLimit;
	uint16 packDischargeLimit;
}OrionBms2_msg2_t;

typedef struct
{
	uint8 highTemp;
	uint8 highCell;
	uint8 avgTemp;
	uint8 bmsTemp;
	uint16 lowVoltage;
}OrionBms2_msg3_t;

typedef struct 
{
	OrionBms2_msg1_t msg1;
	OrionBms2_msg2_t msg2;
	OrionBms2_msg3_t msg3;

	CanCommunication_Message msgObj1;
	CanCommunication_Message msgObj2;
	CanCommunication_Message msgObj3;

	uint32 canErrorCount;
	boolean canError;
}OrionBms2_t;

/************************ Global Variables ***************************/
IFX_EXTERN OrionBms2_t OrionBms2;

/*********************** Function Prototypes *************************/
IFX_EXTERN void OrionBms2_init(void);
IFX_EXTERN void OrionBms2_run_1ms_c2(void);

#endif