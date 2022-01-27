/*
 * SteeringAngle.h
 *
 *  Created on: 2019. 10. 9.
 *      Author: Dua
 */

#ifndef SRC_APPSW_TRICORE_SDP_STEERINGANGLE_STEERINGANGLE_H_
#define SRC_APPSW_TRICORE_SDP_STEERINGANGLE_STEERINGANGLE_H_

/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/
#include "SDP.h"
#include "GtmTim.h"

/******************************************************************************/
/*-----------------------------------Macros-----------------------------------*/
/******************************************************************************/


/******************************************************************************/
/*------------------------------Type Definitions------------------------------*/
/******************************************************************************/


/******************************************************************************/
/*--------------------------------Enumerations--------------------------------*/
/******************************************************************************/
typedef enum
{
	SteerNeutral,
	SteerLeft,
	SteerRight,
}SDP_SteeringAngle_steerDir;


/******************************************************************************/
/*-----------------------------Data Structures--------------------------------*/
/******************************************************************************/
typedef struct
{
	float32 start;
	float32 end;
	float32 deadzone;
	boolean reversed;

	float32 center;
	float32 opposite;

	float32 actLenRaw;
	float32 actLenRawCorner;
	float32 dzLen;
	float32 actLenCorner;
	float32 dzLeftEnd;
	float32 dzRightEnd;
}SDP_SteeringAngle_sensorConfig;

typedef struct
{
	float32 degSteeringAngle;
	float32 degSteeringAngle_raw;
	SDP_SteeringAngle_steerDir direction;
	SDP_SteeringAngle_sensorConfig config;
}SDP_SteeringAngle_sensor_t;

typedef struct
{
	SDP_SteeringAngle_sensor_t stas0;
	float32 degSteeringAngle;
	SDP_SteeringAngle_steerDir direction;
}SDP_SteeringAngle_t;


/******************************************************************************/
/*------------------------------Global variables------------------------------*/
/******************************************************************************/
IFX_EXTERN SDP_SteeringAngle_t SDP_SteeringAngle;

/******************************************************************************/
/*-------------------------Function Prototypes--------------------------------*/
/******************************************************************************/
IFX_EXTERN void SDP_SteeringAngle_init(void);
IFX_EXTERN void SDP_SteeringAngle_run_1ms(void);
/******************************************************************************/
/*---------------------Inline Function Implementations------------------------*/
/******************************************************************************/



#endif
