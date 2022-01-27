/*
 * TorqueVectoring.c
 * Created on 2019. 12. 10.
 * Author: Dua
 */


/* Includes */
#include "TorqueVectoring.h"
#include "SteeringAngle.h"
#include <math.h>

/* Macros */
#define TVOPEN_LSD_ON		FALSE
#define TVOPEN_LSD_GAIN		1.0f

/* Global Variables */
IFX_EXTERN RVC_t RVC;

/* Function Implementation */
void RVC_TorqueVectoring_run_modeOpen(void)
{
	if(TVOPEN_LSD_ON == TRUE)
	{
		// RVC.slip.axle
		
	}
	else
	{
		RVC.torque.rearLeft = RVC.torque.controlled;
		RVC.torque.rearRight = RVC.torque.controlled;
	}
}

void RVC_TorqueVectoring_run_mode1(void)
{
	// TODO: TV algorithm
	const float32 maxSteering = 90.0;
	if(SDP_SteeringAngle.stas0.direction == SteerLeft)
	{
		RVC.torque.rearLeft = RVC.torque.controlled * (1 - fabs(SDP_SteeringAngle.stas0.degSteeringAngle)/maxSteering);
		RVC.torque.rearRight = RVC.torque.controlled;
	}
	else if(SDP_SteeringAngle.stas0.direction == SteerRight)
	{
		RVC.torque.rearLeft = RVC.torque.controlled;
		RVC.torque.rearRight = RVC.torque.controlled * (1 - fabs(SDP_SteeringAngle.stas0.degSteeringAngle)/maxSteering);
	}
	else
	{
		RVC.torque.rearLeft = RVC.torque.controlled;
		RVC.torque.rearRight = RVC.torque.controlled;
	}
}