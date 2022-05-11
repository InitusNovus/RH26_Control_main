/*
 * TorqueVectoring.c
 * Created on 2019. 12. 10.
 * Author: Dua
 */


/* Includes */
#include "TorqueVectoring.h"
#include <math.h>

/* Macros */
// #define TVOPEN_LSD_ON		TRUE
// #define TVOPEN_LSD_GAIN		1.0f

/* Global Variables */
boolean TVOPEN_LSD_ON = TRUE;
IFX_EXTERN RVC_t RVC;

/* Function Implementation */
void RVC_TorqueVectoring_run_modeOpen(void)
{
	if(TVOPEN_LSD_ON == TRUE && RVC.diff.error == FALSE && SDP_WheelSpeed.velocity.chassis > RVC.lsd.speedLow )	//TODO: Diff deadzone
	{
		if(RVC.diff.rear > 0)
			RVC.lsd.faster = RVC_Lsd_Faster_left;
		else if(RVC.diff.rear < 0)
			RVC.lsd.faster = RVC_Lsd_Faster_right;
		else
			RVC.lsd.faster = RVC_Lsd_Faster_none;

		float absDiffRear = fabs(RVC.diff.rear);
		float slowerTorque = 0;
		float fasterTorque = 0;
		if(absDiffRear > RVC.lsd.diffLimit)
		{
			slowerTorque = RVC.torque.controlled * (RVC.lsd.lGain * (absDiffRear - RVC.lsd.diffLimit) +
			                                           (1 + RVC.lsd.diffLimit * RVC.lsd.kGain * RVC.lsd.mGain));
			fasterTorque = RVC.torque.controlled * (RVC.lsd.lGain * (-absDiffRear + RVC.lsd.diffLimit) +
			                                           (1 - RVC.lsd.diffLimit * RVC.lsd.kGain));
		}
		else
		{
			slowerTorque = RVC.torque.controlled * (1.0 + absDiffRear * RVC.lsd.kGain * RVC.lsd.mGain);
			fasterTorque = RVC.torque.controlled * (1.0 - absDiffRear * RVC.lsd.kGain);
		}

		if(slowerTorque > 100)
			slowerTorque = 100;
		if(fasterTorque < 0)
			fasterTorque = 0;

		if(RVC.lsd.faster == RVC_Lsd_Faster_left)
		{
			RVC.torque.rearLeft = fasterTorque;
			RVC.torque.rearRight = slowerTorque;
		}
		else if(RVC.lsd.faster == RVC_Lsd_Faster_right)
		{
			RVC.torque.rearLeft = slowerTorque;
			RVC.torque.rearRight = fasterTorque;
		}
		else
		{
			RVC.torque.rearLeft = RVC.torque.controlled;
			RVC.torque.rearRight = RVC.torque.controlled;
		}
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
	/*Default*/
	RVC_TorqueVectoring_run_modeOpen();
}