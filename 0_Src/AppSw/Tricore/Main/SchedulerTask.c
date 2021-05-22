/*
 * SchedulerTask.c
 *
 *      Author: Dua
 */


/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/
#include "SchedulerTask.h"

#include "HLD.h"
#include "User.h"
#include "SDP.h"

#include "CanCommunication.h"
#include "AccumulatorManager_master.h"
#include "RVC.h"
#include "kelly8080ips_can.h"
#include "OrionBms2.h"
#include "SteeringWheel.h"

#include "SharedPinFix.h"
#include "AdcForceStart.h"


/******************************************************************************/
/*-----------------------------------Macros-----------------------------------*/
/******************************************************************************/
#define TASK_MODE_IC		0
#define TASK_MODE_RH		1

#define TASK_MODE			TASK_MODE_RH
/******************************************************************************/
/*--------------------------------Enumerations--------------------------------*/
/******************************************************************************/

/******************************************************************************/
/*-----------------------------Data Structures--------------------------------*/
/******************************************************************************/

typedef enum
{
	typeBoolean,
	typeUint8,
	typeUint16,
	typeUint32,
	typeUint64,
	typeSint8,
	typeSint16,
	typeSint32,
	typeSint64,
	typeFloat32,
	typeFloat64,
	typePointer,

}Task_dataTypes;

/*typedef struct
{
	void* val;
	Task_dataTypes type;

}Tsak_softReset_t;*/
/******************************************************************************/
/*------------------------------Global variables------------------------------*/
/******************************************************************************/
Task_data Task =
{
		.counter_1ms = 0,
		.counter_10ms = 0,
		.counter_100ms = 0,
		.counter_1000ms = 0,

		.systemState = initialized,

		//		.core1.start = FALSE,
		//		.core1.flag = FALSE,
		.core1.test = 0,
};

note_t readySound[] = {EC6L, EG5L, ENDING};



uint64 stm_buf = 0;
uint64 stm_buf_1ms = 0;
uint64 ticToc_1ms_flag = 0;
uint64 ticToc_1ms = 0;
uint64 ticToc_10ms_s0 = 0;
uint64 ticToc_10ms_s1 = 0;
uint64 ticToc_100ms_s4 = 0;
uint64 ticToc_100ms_s14 = 0;



/******************************************************************************/
/*-------------------------Function Prototypes--------------------------------*/
/******************************************************************************/
IFX_STATIC void Task_counter_service_1ms (void);
IFX_STATIC void Task_counter_service_10ms (void);
IFX_STATIC void Task_counter_service_100ms (void);
IFX_STATIC void Task_counter_service_1000ms (void);
IFX_STATIC void Task_startButtonRoutine(void)
{
	switch(Task.systemState)
	{
#if TASK_MODE == TASK_MODE_RH

	case initialized:
		break;
	case tsActivated:
		break;
	case ppsChecked:
		break;
	case readyToDrive:
		break;
	default:
		break;
#else
	case initialized:
		Task.systemState = ready;
		HLD_GtmTomBeeper_start(readySound);
		break;
	case ready:
		Task.systemState = run;
		break;
	case run:
		Task.systemState = ready;
		break;
#endif
	}
}

/******************************************************************************/
/*------------------------Private Variables/Constants-------------------------*/
/******************************************************************************/

/******************************************************************************/
/*-------------------------Function Implementations---------------------------*/
/******************************************************************************/
void Task_init (void)
{
	/*HLD initialization*/
	{
		HLD_GtmTom_init();
		HLD_GtmTim_init();
		HLD_Qspi_init();
		// HLD_Multican_init();
		HLD_Vadc_init();
	}
	/*HLD_AbstractionLayer initialization*/
	{
		HLD_Imu_init();
	}
	/*HLD_Userinterface initialization*/
	{
		// HLD_UserInterface_init(Task_startButtonRoutine);
	}	
	/* UHM initialization */
	{
		CanCommunication_init();
	}
	/*SDP initialization*/
	{
		SDP_PedalBox_init();
		SDP_SteeringAngle_init();
		SDP_WheelSpeed_init();
	}
	/* Hmm... */
	{
		// AccumulatorManager_master_init();
		kelly8080ips_can_init();
		OrionBms2_init();
		RVC_init();
		SteeringWheel_init();
	}

	/*HLD initialization finished*/
	{
		while(IfxCpu_acquireMutex(&Task_core1.mutex));
		{
			Task_core1.start = TRUE;	//starting-up Cpu1
			IfxCpu_releaseMutex(&Task_core1.mutex);
		}

		while(IfxCpu_acquireMutex(&Task_core2.mutex));
		{
			Task_core2.start = TRUE;	//starting-up Cpu2
			IfxCpu_releaseMutex(&Task_core2.mutex);
		}

		HLD_GtmTomBeeper_start(Beep_pattern4);
		//HLD_GtmTomBeeper_start(KartRider);
		//HLD_GtmTomBeeper_start(GrandfathersElevenMonth);
	}

}
/**********************************************************************/
/**********************************1ms*********************************/
/**********************************************************************/
/*
 * Don't use this function, use Task_IsrCb_1ms() instead.
 * */
void Task_1ms (void)
{
	/*
	stm_buf = IfxStm_get(&MODULE_STM0);
	Task_counter_service_1ms();
	ticToc_1ms_flag = (IfxStm_get(&MODULE_STM0) - stm_buf)*1000000/(IfxStm_getFrequency(&MODULE_STM0));
	 */
}
void Task_IsrCb_1ms (void)
{
	stm_buf_1ms = IfxStm_get(&MODULE_STM0);

	while(IfxCpu_acquireMutex(&Task_core1.mutex));
	{
		Task_core1.flag = Task_core1.start ? TRUE : FALSE;
		IfxCpu_releaseMutex(&Task_core1.mutex);
	}
		while(IfxCpu_acquireMutex(&Task_core1.mutex));
	{
		Task_core2.flag = Task_core2.start ? TRUE : FALSE;
		IfxCpu_releaseMutex(&Task_core2.mutex);
	}

	{
		HLD_GtmTim_run_1ms();
	}
	{
		SDP_PedalBox_run_1ms();
		SDP_SteeringAngle_run_1ms();
		SDP_WheelSpeed_run_1ms();
	}
	{
		RVC_run_1ms();
	}
	HLD_GtmTomBeeper_run_1ms();

	ticToc_1ms = (IfxStm_get(&MODULE_STM0) - stm_buf_1ms)*1000000/(IfxStm_getFrequency(&MODULE_STM0));
}
/**********************************************************************/
/**********************************10ms********************************/
/**********************************************************************/
void Task_10ms (void)			//Slot 0
{
	stm_buf = IfxStm_get(&MODULE_STM0);
	Task_counter_service_10ms();

	RVC_run_10ms();

	// HLD_UserInterface_run_10ms();

	ticToc_10ms_s0 = (IfxStm_get(&MODULE_STM0) - stm_buf)*1000000/(IfxStm_getFrequency(&MODULE_STM0));
}
void Task_10ms_slot1 (void)
{
	stm_buf = IfxStm_get(&MODULE_STM0);

	//	HLD_Imu_run_10ms_s1();

	ticToc_10ms_s1 = (IfxStm_get(&MODULE_STM0) - stm_buf)*1000000/(IfxStm_getFrequency(&MODULE_STM0));
}
/**********************************************************************/
/*********************************100ms********************************/
/**********************************************************************/
void Task_100ms (void)
{
	Task_counter_service_100ms();
	if(Task.counter_100ms%2 == 0)
	{

	}
}
void Task_100ms_slot4(void)
{
	stm_buf = IfxStm_get(&MODULE_STM0);
	//	HLD_UserInterface_run_100ms_s4();
	ticToc_100ms_s4 = (IfxStm_get(&MODULE_STM0) - stm_buf)*1000000/(IfxStm_getFrequency(&MODULE_STM0));
}
void Task_100ms_slot14(void)
{
	stm_buf = IfxStm_get(&MODULE_STM0);
	//	HLD_UserInterface_run_100ms_s14();
	ticToc_100ms_s14 = (IfxStm_get(&MODULE_STM0) - stm_buf)*1000000/(IfxStm_getFrequency(&MODULE_STM0));
}
void Task_100ms_slot24(void)
{
	//	HLD_UserInterface_run_100ms_s24();
}
void Task_100ms_slot34(void)
{
	//	HLD_UserInterface_run_100ms_s34();
}
void Task_100ms_slot44(void)
{
	//	HLD_UserInterface_run_100ms_s44();
}
void Task_100ms_slot5(void)
{

}
/**********************************************************************/
/********************************1000ms********************************/
/**********************************************************************/
void Task_1000ms (void)
{
	Task_counter_service_1000ms();
}
void Task_1000ms_slot3 (void)
{

}




void Task_counter_service_1ms (void)
{
	Task.counter_1ms ++;
	if(Task.counter_1ms == 1000)
	{
		Task.counter_1ms = 0;
	}
}
void Task_counter_service_10ms (void)
{
	Task.counter_10ms ++;
	if(Task.counter_10ms == 1000)
	{
		Task.counter_10ms = 0;
	}
}
void Task_counter_service_100ms (void)
{
	Task.counter_100ms ++;
	if(Task.counter_100ms == 1000)
	{
		Task.counter_100ms = 0;
	}
}
void Task_counter_service_1000ms (void)
{
	Task.counter_1000ms ++;
	if(Task.counter_1000ms == 1000)
	{
		Task.counter_1000ms = 0;
	}
}
