/*
 * kelly8080ips_can.h
 *
 *  Created on: 2018. 8. 8.
 *      Author: bigbi_000
 */

#ifndef KELLY8080IPS_CAN_H_
#define KELLY8080IPS_CAN_H_

/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/
#include <Ifx_Types.h>
#include "Configuration.h"
#include "ConfigurationIsr.h"

#include "Multican.h"
#include "CanCommunication.h"

/******************************************************************************/
/*-----------------------------------Macros-----------------------------------*/
/******************************************************************************/

/******************************************************************************/
/*------------------------------Type Definitions------------------------------*/
/******************************************************************************/

/******************************************************************************/
/*--------------------------------Enumerations--------------------------------*/
/******************************************************************************/

/******************************************************************************/
/*-----------------------------Data Structures--------------------------------*/
/******************************************************************************/
typedef struct
{
	uint16 rpm;
	uint16 motCurrent;
	uint16 batVoltage;	//10x
	union
	{
		uint16 U;
		struct
		{
			uint16 err0		:1;
			uint16 err1		:1;
			uint16 err2		:1;
			uint16 err3		:1;
			uint16 err4		:1;
			uint16 err5		:1;
			uint16 err6		:1;
			uint16 err7		:1;
			uint16 err8		:1;
			uint16 err9		:1;
			uint16 err10	:1;
			uint16 err11	:1;
			uint16 err12	:1;
			uint16 err13	:1;
			uint16 err14	:1;
			uint16 err15	:1;
		}B;
	}error;

}kelly8080ips_msg1_t;

typedef struct
{
	uint8 tps;		//Raw data
	sint8 conTemp;
	sint8 motTemp;
	union
	{
		uint8 U;
		struct
		{
			uint8 feedback 	:2;
			uint8 command	:2;
		}B;
	}conStat;
	union
	{
		uint8 U;
		struct
		{
			uint8 hallA		:1;
			uint8 hallB		:1;
			uint8 hallC		:1;
			uint8 Brake12v	:1;
			uint8 backward	:1;
			uint8 forward	:1;
			uint8 foot		:1;
			uint8 boost		:1;
		}B;
	}swStat;
}kelly8080ips_msg2_t;

typedef struct
{
	/* Can message data */
	kelly8080ips_msg1_t msg1;
	kelly8080ips_msg2_t msg2;
	
	/* Can message object */
    CanCommunication_Message canMsgObj00;
    CanCommunication_Message canMsgObj01;
	CanCommunication_Message canMsgObj02;
	CanCommunication_Message canMsgObj03;

	uint32 canErrorCount;
	boolean canError;
}kelly8080ips_t;
/******************************************************************************/
/*------------------------------Global variables------------------------------*/
/******************************************************************************/
IFX_EXTERN kelly8080ips_t kelly8080ips1;
IFX_EXTERN kelly8080ips_t kelly8080ips2;
/******************************************************************************/
/*-------------------------Function Prototypes--------------------------------*/
/******************************************************************************/
IFX_EXTERN void kelly8080ips_can_init(void);
IFX_EXTERN void kelly8080ips_can_run_1ms_c2(void);
IFX_EXTERN void kelly8080ips_can_run_xms_c2 (void);
/******************************************************************************/
/*---------------------Inline Function Implementations------------------------*/
/******************************************************************************/


#endif /* KELLY8080IPS_CAN_H_ */
