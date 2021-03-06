/*
 * GtmTomPWM.h
 *
 *  Created on: 2019. 4. 24.
 *      Author: mains
 */

#ifndef _GTMTOMPWM_H_
#define _GTMTOMPWM_H_


/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/
#include <Ifx_Types.h>
#include "Configuration.h"
#include "ConfigurationIsr.h"
#include "GtmTom.h"

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
	IfxGtm_Tom_Timer	driver;
	Ifx_TimerValue		triggerPoints;
	float32 			frequency;
	Ifx_TimerValue 		period;
}HLD_GtmTom_Pwm;

typedef struct
{
	IfxGtm_Tom_ToutMap* tomOut;
	float32 frequency;
}HLD_GtmTom_Pwm_Config;
/******************************************************************************/
/*------------------------------Global variables------------------------------*/
/******************************************************************************/

/******************************************************************************/
/*-------------------------Function Prototypes--------------------------------*/
/******************************************************************************/

/*
 *
 */
IFX_EXTERN void HLD_GtmTomPwm_initPwm(HLD_GtmTom_Pwm* pwm, HLD_GtmTom_Pwm_Config* config);

IFX_EXTERN void HLD_GtmTomPwm_setTriggerPoint(HLD_GtmTom_Pwm* pwm, Ifx_TimerValue timVal);

/*
 * PWM triggerPoint setting function
 * input type : float (between 0 and 1)
 */
IFX_EXTERN void HLD_GtmTomPwm_setTriggerPointFloat(HLD_GtmTom_Pwm* pwm, float32 floatDuty);

/******************************************************************************/

/******************************************************************************/
/*---------------------Inline Function Implementations------------------------*/
/******************************************************************************/



#endif /* 0_SRC_APPSW_TRICORE_HLD_BASICMODULES_GTMTOMPWM_H_ */
