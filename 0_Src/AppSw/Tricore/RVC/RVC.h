/*
 * RVC.h
 * Created on 2019. 11. 01
 * Author: Dua
 */

#ifndef RVC_H_
#define RVC_H_

/* Includes */
#include "SDP.h"

/* Data Structures */
typedef struct
{
	float32 current;
	float32 voltage;
	float32 soc;
	uint16 chargeLimit;
	uint16 dischargeLimit;
	float32 lowestVoltage;
	sint8 highestTemp;
	sint8 averageTemp;
	sint8 bmsTemp;

	IfxCpu_mutexLock mutex;
	boolean isUpdated;
} RVC_public_bms_t;

typedef struct
{
	uint32 rpm;
	float32 current;
	float32 voltage;
	uint32 inveterTemp;
	uint32 controllerTemp;
} RVC_public_inverter_t;

typedef struct
{
	struct
	{
		RVC_public_bms_t data;

		struct
		{
			RVC_public_bms_t data;
			IfxCpu_mutexLock mutex;
			boolean isUpdated;
		} shared;
	} bms;

	struct
	{
		RVC_public_inverter_t data;

		struct
		{
			RVC_public_inverter_t data;
			IfxCpu_mutexLock mutex;
			boolean isUpdated;
		} shared;
	} inverter1, inverter2;
} RVC_public_t;

/* Global Variables */
IFX_EXTERN RVC_public_t RVC_public;

/* Function Prototypes */
IFX_EXTERN void RVC_init(void);
IFX_EXTERN void RVC_run_1ms(void);
IFX_EXTERN void RVC_run_10ms(void);
IFX_EXTERN void RVC_run_100ms(void);

#endif