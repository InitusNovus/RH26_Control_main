/*
 * WheelSpeed.h
 *
 *  Created on: 2019. 10. 11.
 *      Author: Dua
 */

#ifndef SRC_APPSW_TRICORE_SDP_WHEELSPEED_WHEELSPEED_H_
#define SRC_APPSW_TRICORE_SDP_WHEELSPEED_WHEELSPEED_H_

/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/
#include "Ifx_LowPassPt1F32.h"

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
	float32 gearRatio;
	float32 wheelDiameter;
	float32 sensorResolution;
}SDP_WheelSpeed_sensorConfig_config;

typedef struct
{
	float32 gearRatio;
	float32 wheelDiameter;
	float32 resolution;

	float32 wheelRadius;
	float32 freqToSpeed;
	float32 speedToVelocity;	//in m/s
}SDP_WheelSpeed_sensorConfig_t;

typedef struct
{
	SDP_WheelSpeed_sensorConfig_t config;
	float32 sensorFrequencyRaw;
	float32 sensorAngularSpeed;
	float32 wheelLinearVelocity;

	Ifx_LowPassPt1F32 filter;

}SDP_WheelSpeed_sensor_t;

typedef struct
{
	SDP_WheelSpeed_sensor_t wssFL;
	SDP_WheelSpeed_sensor_t wssFR;
	SDP_WheelSpeed_sensor_t wssRL;
	SDP_WheelSpeed_sensor_t wssRR;

	struct
	{
		float32 frontAxle;
		float32 rearAxle;
		float32 chassis;
	}velocity;
}SDP_WheelSpeed_t;

/******************************************************************************/
/*------------------------------Global variables------------------------------*/
/******************************************************************************/
IFX_EXTERN SDP_WheelSpeed_t SDP_WheelSpeed;

/******************************************************************************/
/*-------------------------Function Prototypes--------------------------------*/
/******************************************************************************/
IFX_EXTERN void SDP_WheelSpeed_init(void);
IFX_EXTERN void SDP_WheelSpeed_run_1ms(void);

/******************************************************************************/
/*---------------------Inline Function Implementations------------------------*/
/******************************************************************************/



#endif
