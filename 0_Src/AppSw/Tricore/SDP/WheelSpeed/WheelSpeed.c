/*
 * WheelSpeed.c
 *
 *  Created on: 2019. 10. 11.
 *      Author: Dua
 */


/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/
#include "SDP.h"
#include "WheelSpeed.h"
#include "GtmTim.h"
#include <math.h>
#include "Ifx_LowPassPt1F32.h"

/******************************************************************************/
/*-----------------------------------Macros-----------------------------------*/
/******************************************************************************/
#define WSSFL		HLD_GtmTim.ch[6].data
#define WSSFR		HLD_GtmTim.ch[7].data
#define WSSRL		HLD_GtmTim.ch[8].data
#define WSSRR		HLD_GtmTim.ch[9].data

#define GR			1.00
#define WDIA		0.52324f
#define	WSSRES		20

#define AVG2(x,y)	((x + y)/2)
/******************************************************************************/
/*--------------------------------Enumerations--------------------------------*/
/******************************************************************************/


/******************************************************************************/
/*-----------------------------Data Structures--------------------------------*/
/******************************************************************************/


/******************************************************************************/
/*------------------------------Global variables------------------------------*/
/******************************************************************************/
SDP_WheelSpeed_t SDP_WheelSpeed;

/******************************************************************************/
/*-------------------------Function Prototypes--------------------------------*/
/******************************************************************************/
IFX_STATIC void SDP_WheelSpeed_initSensorConfig(SDP_WheelSpeed_sensorConfig_config* config);
IFX_STATIC void SDP_WheelSpeed_initSensor(SDP_WheelSpeed_sensor_t* sensor, SDP_WheelSpeed_sensorConfig_config* config);
IFX_STATIC void SDP_WheelSpeed_update(void);
IFX_STATIC void SDP_WheelSpeed_getSensorValue(SDP_WheelSpeed_sensor_t* sensor, HLD_GtmTim_dataPulse_t* tim);

/******************************************************************************/
/*------------------------Private Variables/Constants-------------------------*/
/******************************************************************************/


/******************************************************************************/
/*-------------------------Function Implementations---------------------------*/
/******************************************************************************/
void SDP_WheelSpeed_init(void)
{
	SDP_WheelSpeed_sensorConfig_config config;
	SDP_WheelSpeed_initSensorConfig(&config);
	config.gearRatio = GR;
	config.sensorResolution = WSSRES;
	config.wheelDiameter = WDIA;
	SDP_WheelSpeed_initSensor(&SDP_WheelSpeed.wssFL, &config);
	SDP_WheelSpeed_initSensor(&SDP_WheelSpeed.wssFR, &config);
	SDP_WheelSpeed_initSensor(&SDP_WheelSpeed.wssRL, &config);
	SDP_WheelSpeed_initSensor(&SDP_WheelSpeed.wssRR, &config);
}

void SDP_WheelSpeed_run_1ms(void)
{
	SDP_WheelSpeed_update();
}

IFX_STATIC void SDP_WheelSpeed_initSensorConfig(SDP_WheelSpeed_sensorConfig_config* config)
{
	config->gearRatio = 1.00;
	config->wheelDiameter = 1.00;
	config->sensorResolution = 0;
}

IFX_STATIC void SDP_WheelSpeed_initSensor(SDP_WheelSpeed_sensor_t* sensor, SDP_WheelSpeed_sensorConfig_config* config)
{
	float32 gearRatio = config->gearRatio;
	float32 wheelDiameter = config->wheelDiameter;
	float32 resolution = config->sensorResolution;

	float32 freqToSpeed = 2*IFX_PI/resolution;
	float32 SpeedToVelocity = wheelDiameter/2 /gearRatio;


	sensor->config.gearRatio = gearRatio;
	sensor->config.resolution = resolution;
	sensor->config.wheelDiameter = wheelDiameter;

	sensor->config.wheelRadius = wheelDiameter/2;
	sensor->config.freqToSpeed = freqToSpeed;
	sensor->config.speedToVelocity = SpeedToVelocity;

	//TODO: WheelSpeed LPF

}

IFX_STATIC void SDP_WheelSpeed_update(void)
{
	SDP_WheelSpeed_getSensorValue(&SDP_WheelSpeed.wssFL, &WSSFL);
	SDP_WheelSpeed_getSensorValue(&SDP_WheelSpeed.wssFR, &WSSFR);
	SDP_WheelSpeed_getSensorValue(&SDP_WheelSpeed.wssRL, &WSSRL);
	SDP_WheelSpeed_getSensorValue(&SDP_WheelSpeed.wssRR, &WSSRR);
	SDP_WheelSpeed.velocity.frontAxle = AVG2(SDP_WheelSpeed.wssFL.wheelLinearVelocity, SDP_WheelSpeed.wssFR.wheelLinearVelocity);
	SDP_WheelSpeed.velocity.rearAxle = AVG2(SDP_WheelSpeed.wssRL.wheelLinearVelocity, SDP_WheelSpeed.wssRR.wheelLinearVelocity);
	SDP_WheelSpeed.velocity.chassis = AVG2(SDP_WheelSpeed.velocity.frontAxle, SDP_WheelSpeed.velocity.rearAxle);
}

IFX_STATIC void SDP_WheelSpeed_getSensorValue(SDP_WheelSpeed_sensor_t* sensor, HLD_GtmTim_dataPulse_t* tim)
{
	float32 freq = tim->pulseHz;
	if(isinf(freq))
	{
		freq = 0;
	}
	sensor->wheelLinearVelocity =
			sensor->config.speedToVelocity *(sensor->sensorAngularSpeed =
					sensor->config.freqToSpeed *(sensor->sensorFrequencyRaw = freq));
	

}

