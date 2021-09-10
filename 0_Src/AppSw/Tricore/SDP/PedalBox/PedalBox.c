 /*
 * PedalBox.c
 *
 *  Created on: 2019. 10. 6.
 *      Author: Dua
 */


/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/
#include "SDP.h"
#include "PedalBox.h"
#include "GtmTim.h"
#include <math.h>

/******************************************************************************/
/*-----------------------------------Macros-----------------------------------*/
/******************************************************************************/
/* 36.74 45.03
83.20 92.04 */
/*FIXME Temporary values*/
#define A0END		83.42f
#define A0STT		74.53f
#define A0DDZ		5

#define A1END		92.0f	//FIXME: Temporary value
#define A1STT		82.8f	//FIXME: Temparary value
#define A1DDZ		5

#define A2END		92.50f
#define A2STT		83.03f
#define A2DDZ		5

#define B0STT		78.2f	//FIXME
#define B0END		83.7f	//FIXME
#define B0DDZ		5

#define B1STT		42.6f	//FIXME
#define B1END		47.6f	//FIXME
#define B1DDZ		5

#define PBERRORLIMIT	10



#define	APPS0		HLD_GtmTim.ch[0].data
#define	APPS1		HLD_GtmTim.ch[1].data
#define	APPS2		HLD_GtmTim.ch[2].data
#define	BPPS0		HLD_GtmTim.ch[3].data
#define	BPPS1		HLD_GtmTim.ch[4].data

#define ERRLIM		SDP_PedalBox_errorLimit


/******************************************************************************/
/*--------------------------------Enumerations--------------------------------*/
/******************************************************************************/


/******************************************************************************/
/*-----------------------------Data Structures--------------------------------*/
/******************************************************************************/
typedef struct
{
	float32 start;
	float32 end;
	float32 percentDeadzone;
	boolean reversed;
}SDP_PedalBox_sensorConfig_config;

/******************************************************************************/
/*------------------------------Global variables------------------------------*/
/******************************************************************************/
float32				SDP_PedalBox_errorLimit = PBERRORLIMIT;

SDP_PedalBox_pps_t	SDP_PedalBox_pps;
SDP_PedalBox_t		SDP_PedalBox;

/******************************************************************************/
/*-------------------------Function Prototypes--------------------------------*/
/******************************************************************************/
IFX_STATIC float32 SDP_PedalBox_getPedalLinear(SDP_PedalBox_sensor_t *sensor, HLD_GtmTim_dataPulse_t *tim);
IFX_STATIC void SDP_PedalBox_initSensorConfig(SDP_PedalBox_sensorConfig_config* config);
IFX_STATIC void SDP_PedalBox_initSensor(SDP_PedalBox_sensor_t* sensor, SDP_PedalBox_sensorConfig_config* config);

/******************************************************************************/
/*------------------------Private Variables/Constants-------------------------*/
/******************************************************************************/


/******************************************************************************/
/*-------------------------Function Implementations---------------------------*/
/******************************************************************************/
void SDP_PedalBox_init(void)
{
	SDP_PedalBox_sensorConfig_config config;

	SDP_PedalBox_initSensorConfig(&config);
	config.start = A0STT;
	config.end = A0END;
	config.percentDeadzone = A0DDZ;
	config.reversed = FALSE;
	SDP_PedalBox_initSensor(&SDP_PedalBox_pps.apps0, &config);

	SDP_PedalBox_initSensorConfig(&config);
	config.start = A1STT;
	config.end = A1END;
	config.percentDeadzone = A1DDZ;
	config.reversed = FALSE;
	SDP_PedalBox_initSensor(&SDP_PedalBox_pps.apps1, &config);

	SDP_PedalBox_initSensorConfig(&config);
	config.start = A2STT;
	config.end = A2END;
	config.percentDeadzone = A2DDZ;
	config.reversed = FALSE;
	SDP_PedalBox_initSensor(&SDP_PedalBox_pps.apps2, &config);

	SDP_PedalBox_initSensorConfig(&config);
	config.start = B0STT;
	config.end = B0END;
	config.percentDeadzone = B0DDZ;
	config.reversed = FALSE;
	SDP_PedalBox_initSensor(&SDP_PedalBox_pps.bpps0, &config);

	SDP_PedalBox_initSensorConfig(&config);
	config.start = B1STT;
	config.end = B1END;
	config.percentDeadzone = B1DDZ;
	config.reversed = FALSE;
	SDP_PedalBox_initSensor(&SDP_PedalBox_pps.bpps1, &config);
}

IFX_STATIC void SDP_PedalBox_initSensorConfig(SDP_PedalBox_sensorConfig_config* config)
{
	config->start = 0;
	config->end = 100;
	config->percentDeadzone = 0;
	config->reversed = FALSE;
}

IFX_STATIC void SDP_PedalBox_initSensor(SDP_PedalBox_sensor_t* sensor, SDP_PedalBox_sensorConfig_config* config)
{
	float32 start = config->start;
	float32 end = config->end;
	float32 deadzone = config->percentDeadzone;

	float32 len = end - start;

	float32 center;
	float32 opposite;

	if(len<0)
	{
		opposite = (end+start)/2;
		center = opposite + 50;
		if(center < 0)
		{
			center = center + 100;
		}
		end = end+100;
	}
	else
	{
		center = (end+start)/2;
		opposite = center + 50;
		if(opposite < 0)
		{
			opposite = opposite + 100;
		}
	}

	float32 dzstart = start+(len*deadzone/100);
	float32 dzend = end-(len*deadzone/100);
	float32 dzlen = dzend-dzstart;


	sensor->config.start = start;
	sensor->config.end = end;
	sensor->config.deadzone = deadzone;
	sensor->config.reversed = config->reversed;
	sensor->config.center = center;
	sensor->config.opposite = opposite;
	sensor->config.len = len;
	sensor->config.dzstart = dzstart;
	sensor->config.dzend = dzend;
	sensor->config.dzlen = dzlen;

}

IFX_STATIC float32 SDP_PedalBox_getPedalLinear(SDP_PedalBox_sensor_t *sensor, HLD_GtmTim_dataPulse_t *tim)
{
/*FIXME unnecessary local variables -> to data which are initialized once at first.*/
	float32 value = tim->dutyRatio_percent;
	float32 result;


	const float32 start = sensor->config.start;
	const float32 end = sensor->config.end;
	const float32 deadzone = sensor->config.deadzone;
	float32 len = sensor->config.len;
	float32 opposite = sensor->config.opposite;
	float32 center = sensor->config.center;


	if(len <0)
	{
		len = -len;
//		end = end+100;
		if(value < opposite)
		{
			value = value + 100;
		}
	}
	else if(opposite >= 100)
	{
		opposite = opposite - 100;
		if(value < opposite)
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

	float32 dzstart = sensor->config.dzstart;
	float32 dzend = sensor->config.dzend;
	float32 dzlen = sensor->config.dzlen;

	if((value > dzstart)&&(value < dzend))
	{
		result = (value-dzstart)/dzlen*100;
	}
	else if(value<dzstart)
	{
		result = 0;
	}
	else
	{
		result = 100;
	}
	return result;
}


IFX_STATIC void SDP_PedalBox_updatePPS(SDP_PedalBox_sensor_t *data_out, HLD_GtmTim_dataPulse_t *data_in)
{
	data_out->pedalPercent = data_out->config.reversed
			?(float32)100.0 - SDP_PedalBox_getPedalLinear(data_out, data_in)
					:SDP_PedalBox_getPedalLinear(data_out, data_in);
}


IFX_STATIC void SDP_PedalBox_checkErrorState_fromTwo(SDP_PedalBox_sensor_t *data1, SDP_PedalBox_sensor_t *data2)
{
	float32 diff = (data1->pedalPercent) - (data2->pedalPercent);
	float32 absDiff = fabs(diff);

	if (absDiff>ERRLIM)
	{
		data1 -> isValueOk = FALSE;
		data2 -> isValueOk = FALSE;
	}
	else
	{
		data1 -> isValueOk = TRUE;
		data2 -> isValueOk = TRUE;
	}
}


IFX_STATIC void SDP_PedalBox_checkErrorState_fromThree(SDP_PedalBox_sensor_t *data1, SDP_PedalBox_sensor_t *data2, SDP_PedalBox_sensor_t *data3)
{
	float32 diff12 = (data1->pedalPercent) - (data2->pedalPercent);
	float32 diff23 = (data2->pedalPercent) - (data3->pedalPercent);
	float32 diff31 = (data3->pedalPercent) - (data1->pedalPercent);

	float32 absDiff12 = fabs(diff12);
	float32 absDiff23 = fabs(diff23);
	float32 absDiff31 = fabs(diff31);

	if((absDiff12 < ERRLIM ) && (absDiff23 < ERRLIM) && (absDiff31 < ERRLIM))
	{
		data1 -> isValueOk = TRUE;
		data2 -> isValueOk = TRUE;
		data3 -> isValueOk = TRUE;
	}

	else if((absDiff12 < ERRLIM ) && (absDiff23 < ERRLIM) && (absDiff31 >= ERRLIM))
	{
		data1 -> isValueOk = TRUE;
		data2 -> isValueOk = TRUE;
		data3 -> isValueOk = TRUE;
	}
	else if((absDiff12 < ERRLIM ) && (absDiff23 >= ERRLIM) && (absDiff31 < ERRLIM))
	{
		data1 -> isValueOk = TRUE;
		data2 -> isValueOk = TRUE;
		data3 -> isValueOk = TRUE;
	}
	else if((absDiff12 >= ERRLIM ) && (absDiff23 < ERRLIM) && (absDiff31 < ERRLIM))
	{
		data1 -> isValueOk = TRUE;
		data2 -> isValueOk = TRUE;
		data3 -> isValueOk = TRUE;
	}
	else if((absDiff12 >= ERRLIM ) && (absDiff23 >= ERRLIM) && (absDiff31 < ERRLIM))
	{
		data1 -> isValueOk = TRUE;
		data2 -> isValueOk = FALSE;
		data3 -> isValueOk = TRUE;
	}
	else if((absDiff12 >= ERRLIM ) && (absDiff23 < ERRLIM) && (absDiff31 >= ERRLIM))
	{
		data1 -> isValueOk = FALSE;
		data2 -> isValueOk = TRUE;
		data3 -> isValueOk = TRUE;
	}
	else if((absDiff12 < ERRLIM ) && (absDiff23 >= ERRLIM) && (absDiff31 >= ERRLIM))
	{
		data1 -> isValueOk = TRUE;
		data2 -> isValueOk = TRUE;
		data3 -> isValueOk = FALSE;
	}
	else
	{
		data1 -> isValueOk = FALSE;
		data2 -> isValueOk = FALSE;
		data3 -> isValueOk = FALSE;
	}
}


IFX_STATIC void SDP_PedalBox_updateBPPS(void)
{
	uint8 	okCount = 0;
	float32 sum = 0;

	SDP_PedalBox_updatePPS(&SDP_PedalBox_pps.bpps0, &BPPS0);
	SDP_PedalBox_updatePPS(&SDP_PedalBox_pps.bpps1, &BPPS1);

	SDP_PedalBox_checkErrorState_fromTwo(&SDP_PedalBox_pps.bpps0, &SDP_PedalBox_pps.bpps1);

	if(SDP_PedalBox_pps.bpps0.isValueOk)
	{
		sum += SDP_PedalBox_pps.bpps0.pedalPercent;
		okCount++;
	}
	if(SDP_PedalBox_pps.bpps1.isValueOk)
	{
		sum += SDP_PedalBox_pps.bpps1.pedalPercent;
		okCount++;
	}

	if(okCount>=2)
	{
		SDP_PedalBox.bpps.pps = (float32)sum/(float32)okCount;
		SDP_PedalBox.bpps.isValueOk = TRUE;
	}
	else
	{
		SDP_PedalBox.bpps.pps = 0;
		SDP_PedalBox.bpps.isValueOk = FALSE;
	}
}


IFX_STATIC void SDP_PedalBox_updateAPPS(void)
{
	uint8 	okCount = 0;
	float32 sum = 0;

	SDP_PedalBox_updatePPS(&SDP_PedalBox_pps.apps0, &APPS0);
	SDP_PedalBox_updatePPS(&SDP_PedalBox_pps.apps1, &APPS1);
	SDP_PedalBox_updatePPS(&SDP_PedalBox_pps.apps2, &APPS2);

	SDP_PedalBox_checkErrorState_fromThree(&SDP_PedalBox_pps.apps0, &SDP_PedalBox_pps.apps1, &SDP_PedalBox_pps.apps2);


	if(SDP_PedalBox_pps.apps0.isValueOk)
	{
		sum += SDP_PedalBox_pps.apps0.pedalPercent;
		okCount++;
	}
	if(SDP_PedalBox_pps.apps1.isValueOk)
	{
		sum += SDP_PedalBox_pps.apps1.pedalPercent;
		okCount++;
	}
	if(SDP_PedalBox_pps.apps2.isValueOk)
	{
		sum += SDP_PedalBox_pps.apps2.pedalPercent;
		okCount++;
	}


	if(okCount>=2)
	{
		SDP_PedalBox.apps.pps = (float32)sum/(float32)okCount;
		SDP_PedalBox.apps.isValueOk = TRUE;
	}
	else
	{
		SDP_PedalBox.apps.pps = 0;
		SDP_PedalBox.apps.isValueOk = FALSE;
	}
}


void SDP_PedalBox_run_1ms(void)
{
	SDP_PedalBox_updateBPPS();
	SDP_PedalBox_updateAPPS();
}


