/*
 * RVC_privateDataStructure.h
 * Created on 2019. 12. 10.
 * Author: Dua
 */

#ifndef RVC_PRIVATEDATASTRUCTURE_H_
#define RVC_PRIVATEDATASTRUCTURE_H_

/******************************** Includes ***********************************/
#include "HLD.h"
#include "Gpio_Debounce.h"
#include "AdcSensor.h"

/****************************** Enumerations *********************************/
typedef enum
{
	RVC_ReadyToDrive_status_notInitialized = 0,
	RVC_ReadyToDrive_status_initialized = 1,
	RVC_ReadyToDrive_status_run = 2,
} RVC_ReadyToDrive_status;

typedef enum
{
	RVC_TorqueVectoring_modeOpen = 0,
	RVC_TorqueVectoring_mode1 = 1,
	RVC_TorqueVectoring_diff = 2,
} RVC_TorqueVectoring_mode_t;

typedef enum
{
	RVC_TractionControl_modeNone = 0,
	RVC_TractionControl_mode1 = 1,
} RVC_TractionControl_mode_t;

typedef struct
{
	float32 mul;
	float32 offset;
} RVC_pwmCalibration;

typedef struct 
{
	Gpio_Debounce_input debounce;
	boolean value;
} RVC_Gpi_t;

/*************************** Data Structures *********************************/
typedef struct
{
	RVC_ReadyToDrive_status readyToDrive;

	AdcSensor LvBattery_Voltage;
	AdcSensor BrakePressure1;
	AdcSensor BrakePressure2;

	RVC_TorqueVectoring_mode_t tvMode;
	RVC_TractionControl_mode_t tcMode;
	
	Gpio_Debounce_input startButton;

	RVC_Gpi_t airPositive;
	RVC_Gpi_t airNegative;
	RVC_Gpi_t 	brakePressureOn;
	RVC_Gpi_t brakeSwitch;

	struct 
	{
		boolean bp1;
		boolean bp2;
		boolean tot;
	} brakeOn;

	struct 
	{
		float32 value;
		float32 limit;
		float32 currentLimit;
	}power;

	struct 
	{
		float32 value;
		float32 margin;
		float32 setValue;
		boolean isLimited;
	}currentLimit;


	struct 
	{
		boolean isAppsChecked;
		boolean isBppsChecked1;
		boolean isBppsChecked2;
	}R2d;

	struct
	{
		float32 desired;
		float32 controlled;

		float32 rearLeft;
		float32 rearRight;

		float32 predeterminedLimit;

		boolean isRegenOn;
	} torque;

	struct
	{
		HLD_GtmTom_Pwm accel_rearLeft;
		HLD_GtmTom_Pwm accel_rearRight;
		HLD_GtmTom_Pwm decel_rearLeft;
		HLD_GtmTom_Pwm decel_rearRight;
	} out;

	struct
	{
		RVC_pwmCalibration leftAcc;
		RVC_pwmCalibration rightAcc;
		RVC_pwmCalibration leftDec;
		RVC_pwmCalibration rightDec;
	} calibration;				//FIXME: To be suitable for LTC2645

	struct
	{
		float32 rearLeftAcc;
		float32 rearRightAcc;
		float32 rearLeftDec;
		float32 rearRightDec;
	} pwmDuty;

	struct
	{
		float32 axle;
		float32 left;
		float32 right;
		boolean error;
	} slip;

	struct 
	{
		float32 rear;
	} diff;

	struct 
	{
		float32 slipLimit;
	} tcMode1;

	struct
	{
		float32 pGain;
	} tvMode1;

} RVC_t;

#endif
