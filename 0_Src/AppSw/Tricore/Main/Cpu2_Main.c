/**
 * \file Cpu2_Main.c
 * \brief CPU2 functions.
 *
 * \version iLLD_Demos_1_0_1_8_0
 * \copyright Copyright (c) 2014 Infineon Technologies AG. All rights reserved.
 *
 *
 *                                 IMPORTANT NOTICE
 *
 *
 * Infineon Technologies AG (Infineon) is supplying this file for use
 * exclusively with Infineon's microcontroller products. This file can be freely
 * distributed within development tools that are supporting such microcontroller
 * products.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * INFINEON SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 * OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 */

/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/

#include "Cpu0_Main.h"
#include "IfxScuWdt.h"

#include "SchedulerTask.h"
/** \brief Main entry point for CPU1 */
void core2_main(void)
{
    /*
     * !!WATCHDOG2 IS DISABLED HERE!!
     * Enable the watchdog in the demo if it is required and also service the watchdog periodically
     * */
    IfxScuWdt_disableCpuWatchdog(IfxScuWdt_getCpuWatchdogPassword());

	while(TRUE)	//waiting for the start-up sign.
	{
		boolean start;
		while(IfxCpu_acquireMutex(&Task_core2.mutex));
		{
			start = Task_core2.start;
			IfxCpu_releaseMutex(&Task_core2.mutex);
		}
		if(start)
		{
			break;
		}
	}

    /** - Background loop */
    while (TRUE)
    {
		boolean flag;
		boolean flag_100ms = FALSE;
		uint8 	counter_100ms = 0;
		while(IfxCpu_acquireMutex(&Task_core2.mutex));
		{
			flag = Task_core2.flag;
			IfxCpu_releaseMutex(&Task_core2.mutex);
		}
		if(flag)
		{
			Task_core2_1ms();

			counter_100ms++;
			if(counter_100ms%100 == 0)
			{
				Task_core2_100ms();
				counter_100ms = 0;
			}

			while(IfxCpu_acquireMutex(&Task_core2.mutex));
			{
				Task_core2.flag = FALSE;
				IfxCpu_releaseMutex(&Task_core2.mutex);
            }
		}
	}
}
