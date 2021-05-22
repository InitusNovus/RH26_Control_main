/*
 * SchedulerTask_Cpu2.h
 *
 *  Created on: 2019. 10. 30.
 *      Author: Dua
 */

#ifndef SRC_APPSW_TRICORE_MAIN_SCHEDULERTASK_CPU2_H_
#define SRC_APPSW_TRICORE_MAIN_SCHEDULERTASK_CPU2_H_

/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/
#include "SchedulerTask.h"
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
	IfxCpu_mutexLock mutex;
	boolean start;
	boolean flag;
	volatile uint32 count10ms;
}Task_cpu2;


/******************************************************************************/
/*------------------------------Global variables------------------------------*/
/******************************************************************************/
IFX_EXTERN Task_cpu2 Task_core2;

/******************************************************************************/
/*-------------------------Function Prototypes--------------------------------*/
/******************************************************************************/


/******************************************************************************/
/*---------------------Inline Function Implementations------------------------*/
/******************************************************************************/
void Task_core2_1ms (void);


#endif
