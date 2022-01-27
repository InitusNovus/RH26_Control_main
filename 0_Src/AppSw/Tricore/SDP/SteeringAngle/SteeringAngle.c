/*
 * SteeringAngle.c
 *
 *  Created on: 2019. 10. 9.
 *      Author: Dua
 */


/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/
#include "SteeringAngle.h"


/******************************************************************************/
/*-----------------------------------Macros-----------------------------------*/
/******************************************************************************/
#define STAS0	HLD_GtmTim.ch[5].data

/*FIXME Temporary values*/
#define STCEN	58.12
#define STDDZ	0.5
#define STRAN	(2*90)
/**************************************************h****************************/
/*--------------------------------Enumerations--------------------------------*/
/******************************************************************************/

/******************************************************************************/
/*-----------------------------Data Structures--------------------------------*/
/******************************************************************************/
typedef struct
{
	float32 percentCenter;
	float32 degRange;
	float32 percentDeadzone;
	boolean reversed;
}SDP_SteeringAngle_sensorConfig_config;


/******************************************************************************/
/*------------------------------Global variables------------------------------*/
/******************************************************************************/
/*
SDP_SteeringAngle_sensor_t SDP_SteeringAngle =
{
		.degSteeringAngle = 0,
		.direction = SteerNeutral,
};
*/

SDP_SteeringAngle_t SDP_SteeringAngle =
{
	.stas0.degSteeringAngle = 0,
	.stas0.direction = SteerNeutral,
};

/******************************************************************************/
/*-------------------------Function Prototypes--------------------------------*/
/******************************************************************************/
IFX_STATIC void SDP_SteeringAngle_initSensor(SDP_SteeringAngle_sensor_t* sensor, SDP_SteeringAngle_sensorConfig_config* config);
IFX_STATIC void SDP_SteeringAngle_initSensorConfig(SDP_SteeringAngle_sensorConfig_config* config);
IFX_STATIC float32 SDP_SteeringAngle_getAngleLinear(SDP_SteeringAngle_sensor_t* sensor, HLD_GtmTim_dataPulse_t *tim);
IFX_STATIC void SDP_SteeringAngle_update(void);

/******************************************************************************/
/*------------------------Private Variables/Constants-------------------------*/
/******************************************************************************/


/******************************************************************************/
/*-------------------------Function Implementations---------------------------*/
/******************************************************************************/
void SDP_SteeringAngle_init(void)
{
	SDP_SteeringAngle_sensorConfig_config config;
	SDP_SteeringAngle_initSensorConfig(&config);
	config.percentCenter = STCEN;
	config.degRange = STRAN;
	config.percentDeadzone = STDDZ;
	config.reversed = TRUE;

	SDP_SteeringAngle_initSensor(&SDP_SteeringAngle.stas0, &config);
}

void SDP_SteeringAngle_run_1ms(void)
{
	SDP_SteeringAngle_update();
}

IFX_STATIC void SDP_SteeringAngle_update(void)
{
	SDP_SteeringAngle_getAngleLinear(&SDP_SteeringAngle.stas0, &STAS0);
	SDP_SteeringAngle.degSteeringAngle = SDP_SteeringAngle.stas0.degSteeringAngle;
	SDP_SteeringAngle.degSteeringAngle = SDP_SteeringAngle.stas0.direction;
}

IFX_STATIC void SDP_SteeringAngle_initSensor(SDP_SteeringAngle_sensor_t* sensor, SDP_SteeringAngle_sensorConfig_config* config)
{
	float32 center = config->percentCenter;
	float32 opposite = center + 50;

	float32 percentRange = config->degRange * 100/360;
	float32 start = center - percentRange/2;
	float32 end = center + percentRange/2;
	float32 deadzone = config->percentDeadzone;

	sensor->config.center	= center;

	if(opposite < 0)
	{
		opposite = opposite + 100;
	}
	sensor->config.opposite = opposite;

	sensor->config.deadzone	= deadzone;
	sensor->config.reversed	= config->reversed;

	sensor->config.start = start;
	sensor->config.end = end;

	float32 actLenRaw = end - start;
	float32 actLenRawCorner = actLenRaw/2;
	float32 dzLen = actLenRaw*(deadzone)/100;
	float32 actLenCorner = (actLenRaw - dzLen)/2;
	float32 dzLeftEnd = center - dzLen/2;
	float32 dzRightEnd = center + dzLen/2;

	sensor->config.actLenRaw = actLenRaw;
	sensor->config.actLenRawCorner = actLenRawCorner;
	sensor->config.dzLen = dzLen;
	sensor->config.actLenCorner = actLenCorner;
	sensor->config.dzLeftEnd = dzLeftEnd;
	sensor->config.dzRightEnd = dzRightEnd;

}
IFX_STATIC void SDP_SteeringAngle_initSensorConfig(SDP_SteeringAngle_sensorConfig_config* config)
{
	config->degRange = 180;
	config->percentCenter = 0;
	config->percentDeadzone = 0;
	config->reversed = FALSE;
}

IFX_STATIC float32 SDP_SteeringAngle_getAngleLinear(SDP_SteeringAngle_sensor_t* sensor, HLD_GtmTim_dataPulse_t *tim)
{
	float32 value = tim->dutyRatio_percent;
	float32 result;

	float32 center = sensor->config.center;
	float32 opposite = sensor->config.opposite;
	float32 start = sensor->config.start;
	float32 end = sensor->config.end;
//	float32 deadzone = sensor->config.deadzone;

	boolean reversed = sensor->config.reversed;

	if(opposite >= 100)
	{
		if(value < opposite - 100)
		{
			value = value + 100;
		}
	}
	else
	{
		if(value > opposite)
		{
			value = value - 100;
		}
	}


//	float32 actLenRaw = sensor->config.actLenRaw;
	float32 actLenRawCorner = sensor->config.actLenRawCorner;
//	float32 dzLen = sensor->config.dzLen;
	float32 actLenCorner = sensor->config.actLenCorner;
	float32 dzLeftEnd = sensor->config.dzLeftEnd;
	float32 dzRightEnd = sensor->config.dzRightEnd;


	sensor->degSteeringAngle_raw = (value - center)*360/100;

	if((value > start)&&(value < dzLeftEnd))
	{
		result = reversed
				?(sensor->degSteeringAngle = -(value - dzLeftEnd)/actLenCorner*actLenRawCorner*360/100)
						:(sensor->degSteeringAngle = (value - dzLeftEnd)/actLenCorner*actLenRawCorner*360/100);
		sensor->direction = SteerLeft;
	}
	else if((value > dzRightEnd)&&(value < end))
	{
		result = reversed
				?(sensor->degSteeringAngle = -(value - dzRightEnd)/actLenCorner*actLenRawCorner*360/100)
						:(sensor->degSteeringAngle = (value - dzRightEnd)/actLenCorner*actLenRawCorner*360/100);
		sensor->direction = SteerRight;
	}
	else if (value < start)
	{
		result = reversed
				?(sensor->degSteeringAngle = -(start - dzLeftEnd)/actLenCorner*actLenRawCorner*360/100)
						:(sensor->degSteeringAngle = (start - dzLeftEnd)/actLenCorner*actLenRawCorner*360/100);
		sensor->direction = SteerLeft;
	}
	else if (value > end)
	{
		result = reversed
				?(sensor->degSteeringAngle = -(end - dzRightEnd)/actLenCorner*actLenRawCorner*360/100)
						:(sensor->degSteeringAngle = (end - dzRightEnd)/actLenCorner*actLenRawCorner*360/100);
		sensor->direction = SteerRight;
	}
	else
	{
		result = (sensor->degSteeringAngle = 0);
		sensor->direction = SteerNeutral;
	}

	return result;
}












