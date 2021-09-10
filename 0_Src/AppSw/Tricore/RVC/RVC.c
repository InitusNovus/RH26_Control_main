/*
 * RVC.c
 * Created on 2019. 11. 01
 * Author: Dua
 */


/* 	
TODO: 
	CAN associated functions 
		- R2D entry routine display
		- Steering wheel function
		- Parameter load/save
		- Log Data Broadcasting
	Battery Management
		- Charge consumed calculation
	Analog sensor
		- Steering Wheel Analog: need calibration code
	GPIO
		- Charge Enable
 */


/***************************** Includes ******************************/
#include <math.h>

#include "Beeper_Test_Music.h"
#include "HLD.h"
#include "IfxPort.h"
#include "PedalMap.h"
#include "Gpio_Debounce.h"
#include "AdcSensor.h"
#include "AdcForceStart.h"

#include "RVC.h"
#include "RVC_privateDataStructure.h"
#include "RVC_r2dSound.h"
#include "TorqueVectoring/TorqueVectoring.h"

#include "SteeringWheel.h"

/**************************** Macro **********************************/
#define PWMFREQ 5000 // PWM frequency in Hz
#define PWMVREF 5.0  // PWM reference voltage (On voltage)

/*
#define OUTCAL_LEFT_MUL 1.06
#define OUTCAL_LEFT_OFFSET 0.015
#define OUTCAL_RIGHT_MUL 1.065
#define OUTCAL_RIGHT_OFFSET 0.0125
 */

#define OUTCAL_LEFT_MUL 1.0f
#define OUTCAL_LEFT_OFFSET 0.0f
#define OUTCAL_RIGHT_MUL 1.0f
#define OUTCAL_RIGHT_OFFSET 0.0f

#define R2D_ONHOLD (3*100)			//3 seconds
#define R2D_OFFHOLD (1*100)			//1 seconds	
#define R2D_REL (1*100)			//1 seconds
#define R2D_APPS_TH		95
#define R2D_APPS_LO		5
#define R2D_APPS_HOLD	(5*10)	//0.5 seconds
#define R2D_BPPS_TH		95
#define R2D_BPPS_LO		5
#define R2D_BPPS_HOLD	(5*10)	//0.5 seconds
#define R2D_TEST		//Test: start button set R2D directly

#define PEDAL_BRAKE_ON_THRESHOLD 10
#define REGEN_MUL	1	//2

#define POWER_LIM				40000	//40kW
#define CURRENT_LIM_SET_VAL		10		//10A

#define TV1PGAIN 0.001

#define REGEN_ON_INIT	FALSE	//***** Regen is not abailable now!!! ***** //FIXME //TODO: Regen limit

#define LVBAT_LINCAL_A	1.015f
#define LVBAT_LINCAL_B	0
#define LVBAT_LINCAL_D	0

#define VAR_UPDATE_ERROR_LIM	10

#define THROTLE_5V

#define BRAKE_ON_BP
#define BRAKE_ON_TH_BP1	3.3f
#define BRAKE_ON_TH_BP2 5.6f

#define BMS_PDL_ERROR	FALSE

/*********************** Global Variables ****************************/
RVC_t RVC = 
{
    .readyToDrive = RVC_ReadyToDrive_status_notInitialized,
    .torque.controlled = 0,
    .torque.rearLeft = 0,
    .torque.rearRight = 0,
	.torque.isRegenOn = REGEN_ON_INIT,

	.calibration.leftAcc.mul = 1,
	.calibration.leftAcc.offset = 0,
	.calibration.rightAcc.mul = 1,
	.calibration.rightAcc.offset = 0,
	.calibration.leftDec.mul = 1,
	.calibration.leftDec.offset = 0,
	.calibration.rightDec.mul = 1,
	.calibration.rightDec.offset = 0,

	.power.limit = POWER_LIM,
	.currentLimit.setValue = CURRENT_LIM_SET_VAL,
};

RVC_public_t RVC_public;
/******************* Private Function Prototypes *********************/
IFX_STATIC void RVC_setR2d(void);
IFX_STATIC void RVC_resetR2d(void);
IFX_STATIC void RVC_toggleR2d(void);

IFX_STATIC void RVC_initAdcSensor(void);
IFX_STATIC void RVC_initPwm(void);
IFX_STATIC void RVC_initGpio(void);
IFX_STATIC void RVC_r2d(void);
IFX_STATIC void RVC_pollGpi(RVC_Gpi_t *gpi);

IFX_INLINE void RVC_updateReadyToDriveSignal(void);
IFX_INLINE void RVC_slipComputation(void);
IFX_INLINE void RVC_getTorqueRequired(void);
IFX_INLINE void RVC_powerComputation(void);
IFX_INLINE void RVC_torqueLimit(void);
IFX_INLINE void RVC_torqueSatuation(void);
IFX_INLINE void RVC_torqueDistrobution(void);
IFX_INLINE void RVC_torqueSignalGeneration(void);
IFX_INLINE void RVC_updatePwmSignal(void);
IFX_INLINE void RVC_updateSharedVariable(void);

/********************* Function Implementation ***********************/
void RVC_init(void)
{
	RVC_initAdcSensor();

	RVC.tvMode = RVC_TorqueVectoring_modeOpen;
	RVC.tcMode = RVC_TractionControl_modeNone;
	RVC_PedalMap_lut_setMode(0);

	RVC_initPwm();

	RVC_initGpio();

	RVC.tvMode1.pGain = TV1PGAIN;
	RVC.readyToDrive = RVC_ReadyToDrive_status_initialized;
}

void RVC_run_1ms(void)
{
	RVC_updateReadyToDriveSignal();

	RVC_slipComputation();

	RVC_getTorqueRequired();

	if(RVC.BrakePressure1.value > BRAKE_ON_TH_BP1)
		RVC.brakeOn.bp1 = TRUE;
	else
		RVC.brakeOn.bp1 = FALSE;

	if(RVC.BrakePressure2.value > BRAKE_ON_TH_BP2)
		RVC.brakeOn.bp2 = TRUE;
	else
		RVC.brakeOn.bp2 = FALSE;

	RVC.brakeOn.tot = RVC.brakeOn.bp1 | RVC.brakeOn.bp2 | RVC.brakePressureOn.value;

	/* TODO: Torque limit: Traction control */

	RVC_powerComputation();

	RVC_torqueLimit();

	RVC_torqueSatuation();

	RVC_torqueDistrobution();

	/* TODO: Torque signal check*/

	RVC_torqueSignalGeneration();

	RVC_updatePwmSignal();

	/* TODO: Shared variable update */
	RVC_updateSharedVariable();
}

void RVC_run_10ms(void)
{
	RVC_pollGpi(&RVC.airPositive);
	RVC_pollGpi(&RVC.airNegative);
	RVC_pollGpi(&RVC.brakePressureOn);
	RVC_pollGpi(&RVC.brakeSwitch);
	RVC_r2d();
	AdcSensor_getData(&RVC.LvBattery_Voltage);
	AdcSensor_getData(&RVC.BrakePressure1);
	AdcSensor_getData(&RVC.BrakePressure2);
}

/****************** Private Function Implementation ******************/
IFX_STATIC void RVC_setR2d(void)
{
	if(RVC.readyToDrive == RVC_ReadyToDrive_status_initialized)
	{
		RVC.readyToDrive = RVC_ReadyToDrive_status_run;
		HLD_GtmTomBeeper_start_volume(RVC_r2dSound, 1); // R2D sound	//FIXME: R2D sound
	}
}

IFX_STATIC void RVC_resetR2d(void)
{
	if(RVC.readyToDrive == RVC_ReadyToDrive_status_run)
	{
		RVC.readyToDrive = RVC_ReadyToDrive_status_initialized;
		HLD_GtmTomBeeper_start(RVC_r2dResetSound);
	}
}

IFX_STATIC void RVC_toggleR2d(void)
{
	if(RVC.readyToDrive == RVC_ReadyToDrive_status_initialized)
		RVC.readyToDrive = RVC_ReadyToDrive_status_run;
	else if(RVC.readyToDrive == RVC_ReadyToDrive_status_run)
		RVC.readyToDrive = RVC_ReadyToDrive_status_initialized;
}

IFX_STATIC void RVC_initAdcSensor(void)
{
	AdcSensor_Config adcConfig;

	/* LV battery voltage */
	adcConfig.adcConfig.channelIn = &(HLD_Vadc_Channel_In){HLD_Vadc_group2, HLD_Vadc_ChannelId_4};

	adcConfig.adcConfig.lpf.activated = TRUE;
	adcConfig.adcConfig.lpf.config.gain = 1;
	adcConfig.adcConfig.lpf.config.cutOffFrequency = 2;
	adcConfig.adcConfig.lpf.config.samplingTime = 10.0e-3;

	adcConfig.isOvervoltageProtected = FALSE;
	adcConfig.linCalConfig.isAct = TRUE;
	adcConfig.linCalConfig.a = LVBAT_LINCAL_A;
	adcConfig.linCalConfig.b = LVBAT_LINCAL_B;
	adcConfig.linCalConfig.d = LVBAT_LINCAL_D;
	adcConfig.tfConfig.a = (20.0f+8.2f)/8.2f;
	adcConfig.tfConfig.b = 0.0f;

	AdcSensor_initSensor(&RVC.LvBattery_Voltage, &adcConfig);
	HLD_AdcForceStart(RVC.LvBattery_Voltage.adcChannel.channel.group);

	/* Brake Pressure*/
	/* 
	 * 2.5kpsi 
	*/
	adcConfig.adcConfig.lpf.activated = TRUE;
	adcConfig.adcConfig.lpf.config.gain = 1;
	adcConfig.adcConfig.lpf.config.cutOffFrequency = 1/(2*IFX_PI*(1e-2f));
	adcConfig.adcConfig.lpf.config.samplingTime = 10.0e-3;

	adcConfig.isOvervoltageProtected = TRUE;
	adcConfig.linCalConfig.isAct = FALSE;

	adcConfig.tfConfig.a = 43.0925f;
	adcConfig.tfConfig.b = -19.392f;

	adcConfig.adcConfig.channelIn = &(HLD_Vadc_Channel_In){HLD_Vadc_group0, HLD_Vadc_ChannelId_2};
	AdcSensor_initSensor(&RVC.BrakePressure1, &adcConfig);
	adcConfig.adcConfig.channelIn = &(HLD_Vadc_Channel_In){HLD_Vadc_group0, HLD_Vadc_ChannelId_1};
	AdcSensor_initSensor(&RVC.BrakePressure2, &adcConfig);
	HLD_AdcForceStart(RVC.BrakePressure1.adcChannel.channel.group);

	/* Steering Angle Analog (Backup function) */
	//TODO
}

IFX_STATIC void RVC_initPwm(void)
{
	/* PWM output initialzation */
	HLD_GtmTom_Pwm_Config pwmConfig;
	pwmConfig.frequency = PWMFREQ;

	pwmConfig.tomOut = &PWMACCL;
	HLD_GtmTomPwm_initPwm(&RVC.out.accel_rearLeft, &pwmConfig);

	pwmConfig.tomOut = &PWMACCR;
	HLD_GtmTomPwm_initPwm(&RVC.out.accel_rearRight, &pwmConfig);

	pwmConfig.tomOut = &PWMDCCL;
	HLD_GtmTomPwm_initPwm(&RVC.out.decel_rearLeft, &pwmConfig);

	pwmConfig.tomOut = &PWMDCCR;
	HLD_GtmTomPwm_initPwm(&RVC.out.decel_rearRight, &pwmConfig);

	/* PWM output calibration */
	RVC.calibration.leftAcc.mul = OUTCAL_LEFT_MUL;
	RVC.calibration.leftAcc.offset = OUTCAL_LEFT_OFFSET;

	RVC.calibration.rightAcc.mul = OUTCAL_RIGHT_MUL;
	RVC.calibration.rightAcc.offset = OUTCAL_RIGHT_OFFSET;
}

IFX_STATIC void RVC_initGpio(void)
{
	/* FWD output config */
	IfxPort_setPinMode(FWD_OUT.port, FWD_OUT.pinIndex, IfxPort_OutputMode_pushPull);
	IfxPort_setPinLow(FWD_OUT.port, FWD_OUT.pinIndex);
	/* R2D signal output config */
	IfxPort_setPinMode(R2DOUT.port, R2DOUT.pinIndex, IfxPort_OutputMode_pushPull);
	IfxPort_setPinLow(R2DOUT.port, R2DOUT.pinIndex);

	/* Start button config */
	Gpio_Debounce_inputConfig gpioInputConfig;
	Gpio_Debounce_initInputConfig(&gpioInputConfig);
	gpioInputConfig.bufferLen = Gpio_Debounce_BufferLength_10;
	gpioInputConfig.inputMode = IfxPort_InputMode_noPullDevice;
	gpioInputConfig.port = &START_BTN;
	Gpio_Debounce_initInput(&RVC.startButton, &gpioInputConfig);

	/* AIR Contact signal input config */
	gpioInputConfig.bufferLen = Gpio_Debounce_BufferLength_10;
	gpioInputConfig.inputMode = IfxPort_InputMode_noPullDevice;
	gpioInputConfig.port = &AIR_P_IN;
	Gpio_Debounce_initInput(&RVC.airPositive.debounce, &gpioInputConfig);
	gpioInputConfig.port = &AIR_N_IN;
	Gpio_Debounce_initInput(&RVC.airNegative.debounce, &gpioInputConfig);

	/* Pedalbox signal input config */
	gpioInputConfig.bufferLen = Gpio_Debounce_BufferLength_10;
	gpioInputConfig.inputMode = IfxPort_InputMode_noPullDevice;
	gpioInputConfig.port = &BP_IN;
	Gpio_Debounce_initInput(&RVC.brakePressureOn.debounce, &gpioInputConfig);
	gpioInputConfig.port = &BSW_IN;
	Gpio_Debounce_initInput(&RVC.brakeSwitch.debounce, &gpioInputConfig);
}

/* TODO: 
	Check APPS and BPPS
	R2D sound generation
*/
//FIXME: Using struct - Hi,Lo,None,Error
IFX_STATIC boolean CheckPpsHi(SDP_PedalBox_struct_t *pps)
{
	if(pps->isValueOk)
	{
		if(pps->pps > R2D_APPS_TH)
			return TRUE;
		else
			return FALSE;
	}	
	else
	{
		return FALSE;
	}
}
IFX_STATIC boolean CheckPpsLo(SDP_PedalBox_struct_t *pps)
{
	if(pps->pps < R2D_APPS_LO)
		return TRUE;
	else
		return FALSE;
}
IFX_STATIC void PpsCheck(boolean *isChecked, uint32 *count, boolean isHi, uint32 hold)
{
	if(isHi)
	{
		(*count)++;
		if(*count > hold)
		{
			*isChecked = TRUE;
			*count = 0;
		}
	}
	else 
	{
		*count = 0;
	}
}
IFX_STATIC void RVC_r2d(void)
{
	static boolean risingEdgeFlag = FALSE;	//Ready to drive contorl button hysteresis
	static uint32 pushCount = 0;
	static uint32 releaseCount = 0;
	boolean buttonState = FALSE; //GPIO debounced input result

	boolean buttonOn = FALSE;	//Start button 

	static uint32 appsCount = 0;
	static uint32 bppsCount = 0;
	boolean isAppsHi = FALSE;
	boolean isAppsLo = FALSE;
	boolean isBppsHi = FALSE;
	boolean isBppsLo = FALSE;
	
	/* Poll the button */
	buttonState = Gpio_Debounce_pollInput(&RVC.startButton);

	/* Poll PPS signals */
	isAppsHi = CheckPpsHi(&SDP_PedalBox.apps);
	isAppsLo = CheckPpsLo(&SDP_PedalBox.apps);
	isBppsHi = CheckPpsHi(&SDP_PedalBox.bpps);
	isBppsLo = CheckPpsLo(&SDP_PedalBox.bpps);

	/* Pedal check routine */
	PpsCheck(&RVC.R2d.isAppsChecked, &appsCount, isAppsHi, R2D_APPS_HOLD);
	PpsCheck(&RVC.R2d.isBppsChecked1, &bppsCount, isBppsHi, R2D_BPPS_HOLD);	//FIXME: Using Brake Pressure sensor
	if(RVC.R2d.isBppsChecked1 == TRUE)
		if(isBppsLo)
			RVC.R2d.isBppsChecked2 = TRUE;
	
	/* Start button routine */
	if( (buttonState == TRUE)&&(risingEdgeFlag == FALSE) )
	{	/* The button is Pushed */
		pushCount++;

/* 		if( (RVC.readyToDrive == RVC_ReadyToDrive_status_initialized)&&(pushCount > R2D_ONHOLD) )
		{
			pushCount = 0;
			risingEdgeFlag = TRUE; //Rising edge detected
			RVC_setR2d();
		}
		else if( (RVC.readyToDrive == RVC_ReadyToDrive_status_run)&&(pushCount > R2D_ONHOLD) )	//TODO: RTD off condition: Speed == 0, 
		{
			pushCount = 0;
			risingEdgeFlag = TRUE; //Rising edge detected
			RVC_resetR2d();
			//TODO: R2D off sound
		}
 */		
		if(pushCount > R2D_ONHOLD)
		{
			buttonOn = TRUE;
			pushCount = 0;
			risingEdgeFlag = TRUE;	//Rising edge detected
		}
	}
	else if( (buttonState == FALSE)&&(risingEdgeFlag == TRUE) )
	{	/* The button is released */
		buttonOn = FALSE;
		releaseCount++;
		if( releaseCount > R2D_REL)
		{
			risingEdgeFlag = FALSE;	// The button is released
		}
	}
	else
	{
		buttonOn = FALSE;
		pushCount = 0;
		releaseCount = 0;
	}

	/* R2D routine */
#ifdef R2D_TEST
	/***** Test: start button set/reset R2D directly *****/
	if((RVC.readyToDrive == RVC_ReadyToDrive_status_initialized) && (buttonOn == TRUE) && (RVC.brakeOn.tot == TRUE))
	{
		RVC_setR2d();
	}
	else if((RVC.readyToDrive == RVC_ReadyToDrive_status_run) && (buttonOn == TRUE))
	{
		RVC_resetR2d();
	}
#else
	/* R2D On condition */
	if((RVC.readyToDrive == RVC_ReadyToDrive_status_initialized) && (RVC.R2d.isAppsChecked == TRUE) &&
	    (RVC.R2d.isBppsChecked2 == TRUE) && (isAppsLo == TRUE) && (isBppsHi == TRUE))
	{
		if((buttonOn == TRUE))
		{
			RVC_setR2d();
		}
	}
	else if(RVC.readyToDrive == RVC_ReadyToDrive_status_run)
	{
		if(buttonOn == TRUE || (RVC.airPositive.value == FALSE) || (RVC.airNegative.value == FALSE))
		{
			RVC_resetR2d();
		}
	}

#endif // R2D_TEST
}

IFX_STATIC void RVC_pollGpi(RVC_Gpi_t *gpi)
{
	gpi->value = Gpio_Debounce_pollInput(&gpi->debounce);
}

/***************** Inline Function Implementation ******************/
IFX_INLINE void RVC_updateReadyToDriveSignal(void)
{
	if(RVC.readyToDrive == RVC_ReadyToDrive_status_run)
	{
		IfxPort_setPinHigh(R2DOUT.port, R2DOUT.pinIndex);
		IfxPort_setPinHigh(FWD_OUT.port, FWD_OUT.pinIndex);
	}
	else
	{
		IfxPort_setPinLow(R2DOUT.port, R2DOUT.pinIndex);
		IfxPort_setPinLow(FWD_OUT.port, FWD_OUT.pinIndex);
	}
}

IFX_INLINE void RVC_slipComputation(void)
{
	RVC.slip.axle = SDP_WheelSpeed.velocity.rearAxle/SDP_WheelSpeed.velocity.frontAxle;
	RVC.slip.left = SDP_WheelSpeed.wssRL.wheelLinearVelocity/SDP_WheelSpeed.wssFL.wheelLinearVelocity;
	RVC.slip.right = SDP_WheelSpeed.wssRR.wheelLinearVelocity/SDP_WheelSpeed.wssFR.wheelLinearVelocity;
	// RVC.diff.rear
	if(isnan(RVC.slip.axle)||isnan(RVC.slip.left)||isnan(RVC.slip.right)) 
	{
		RVC.slip.error = TRUE;
	}
	else
	{
		RVC.slip.error = FALSE;
	}
}

IFX_INLINE void RVC_getTorqueRequired(void)
{
	if(SDP_PedalBox.apps.isValueOk)		//APPS Plausibility check
	{
		RVC.torque.controlled = (RVC.torque.desired = RVC_PedalMap_lut_getResult(SDP_PedalBox.apps.pps));
	}
	else
	{
		RVC.torque.controlled = (RVC.torque.desired = 0);		//APPS Fail
	}
#ifdef BRAKE_ON_BP	//No Regen!	//TODO
	if(RVC.brakeOn.tot == TRUE)
	{
		RVC.torque.controlled = (RVC.torque.desired = 0);	//Zero torque signal when brake on.
	}
#else
	if(SDP_PedalBox.bpps.isValueOk)		//BPPS Plausibility check
	{
		if(SDP_PedalBox.bpps.pps > PEDAL_BRAKE_ON_THRESHOLD)
		{
			RVC.torque.desired = -(SDP_PedalBox.bpps.pps);		//BPPS overide
			if(RVC.torque.isRegenOn)							//Regen
			{
				RVC.torque.controlled = RVC.torque.desired;
			}
			else 
			{
				RVC.torque.controlled = (RVC.torque.desired = 0);	//Regen off: Zero torque signal when brake on.
			}	
		}
	}
	else //FIXME: BSPD control using Brake Pressure Analog signal. Failsafe for BPPS.
	{
		RVC.torque.controlled = 0;		//BPPS Fail
	}
#endif
}

IFX_INLINE void RVC_powerComputation(void)
{
	RVC.power.value = RVC_public.bms.data.current * RVC_public.bms.data.voltage;
	RVC.power.currentLimit = RVC.power.limit / RVC_public.bms.data.voltage;
}

IFX_INLINE void RVC_torqueLimit(void)
{
	uint16 dischargeLimit ;

	if(BMS_PDL_ERROR == TRUE)
	{
		dischargeLimit = 400;
		if(RVC_public.bms.data.highestTemp > 45)
		{
			dischargeLimit = 200;
		}
		else if(RVC_public.bms.data.highestTemp > 50)
		{
			dischargeLimit = 100;
		}
		else if(RVC_public.bms.data.highestTemp > 55)
		{
			dischargeLimit = 50;
		}
		if(RVC_public.bms.data.lowestVoltage < 3.0f)
		{
			dischargeLimit = 50;
		}
	}
	else
	{
		dischargeLimit = RVC_public.bms.data.dischargeLimit;
	}

	float32 currentLimitByPower = RVC.power.currentLimit;

	RVC.currentLimit.value = (dischargeLimit > currentLimitByPower) ? currentLimitByPower : dischargeLimit;

	RVC.currentLimit.margin = RVC.currentLimit.value - RVC_public.bms.data.current;

	if(RVC.currentLimit.margin < RVC.currentLimit.setValue)
	{
		RVC.torque.controlled = RVC.torque.controlled * RVC.currentLimit.margin / RVC.currentLimit.setValue;
		RVC.currentLimit.isLimited = TRUE;
	}
	else
	{
		RVC.currentLimit.isLimited = FALSE;
	}
}

IFX_INLINE void RVC_torqueSatuation(void)
{
	if(RVC.torque.controlled > 100)
	{
		RVC.torque.controlled = 100;
	}
	else if(RVC.torque.controlled < -100)
	{
		RVC.torque.controlled = -100;
	}
	else if(RVC.torque.desired < RVC.torque.controlled)
	{
		RVC.torque.controlled = RVC.torque.desired;
	}
}

IFX_INLINE void RVC_torqueDistrobution(void)
{
	/* Traction Control Calculation */
	switch(RVC.tcMode)
	{
	case RVC_TractionControl_mode1:
		break;
	default:
		break;
	}

	/* Torque distribution */
	switch(RVC.tvMode)
	{
	case RVC_TorqueVectoring_mode1:
		RVC_TorqueVectoring_run_mode1();
		break;
	default:
		RVC_TorqueVectoring_run_modeOpen();
		break;
	}
}

IFX_INLINE void RVC_torqueSignalGeneration(void)
{
#ifdef THROTLE_5V
	// 0~100% maped to 0~5V ( = 0.0~1.0 duty)
	if(RVC.readyToDrive == RVC_ReadyToDrive_status_run)
	{
		if(RVC.torque.rearLeft > 0) 	//Accelertation
		{
			RVC.pwmDuty.rearLeftAcc =
				(RVC.torque.rearLeft) * 0.01f * RVC.calibration.leftAcc.mul + RVC.calibration.leftAcc.offset;
			RVC.pwmDuty.rearLeftDec = 
				(0) * 0.01f * RVC.calibration.leftDec.mul + RVC.calibration.leftDec.offset;
		}
		else 
		{
			RVC.pwmDuty.rearLeftAcc =
				(0) * 0.01f * RVC.calibration.leftAcc.mul + RVC.calibration.leftAcc.offset;
			RVC.pwmDuty.rearLeftDec = 
				(-(RVC.torque.rearLeft)*REGEN_MUL) * 0.01f * RVC.calibration.leftDec.mul + RVC.calibration.leftDec.offset;
		}

		if(RVC.torque.rearRight > 0)
		{
			RVC.pwmDuty.rearRightAcc =
				(RVC.torque.rearRight) * 0.01f * RVC.calibration.rightAcc.mul + RVC.calibration.rightAcc.offset;			
			RVC.pwmDuty.rearRightDec = 
				(0) * 0.01f * RVC.calibration.rightDec.mul + RVC.calibration.rightDec.offset;
		}
		else 
		{
			RVC.pwmDuty.rearRightAcc =
				(0) * 0.01f * RVC.calibration.rightAcc.mul + RVC.calibration.rightAcc.offset;
			RVC.pwmDuty.rearRightDec = 
				(-(RVC.torque.rearRight)*REGEN_MUL) * 0.01f * RVC.calibration.rightDec.mul + RVC.calibration.rightDec.offset;			
		}
	}
	else
	{
		RVC.pwmDuty.rearLeftAcc = (0) * 0.01f * RVC.calibration.leftAcc.mul + RVC.calibration.leftAcc.offset;
		RVC.pwmDuty.rearRightAcc = (0) * 0.01f * RVC.calibration.rightAcc.mul + RVC.calibration.rightAcc.offset;
		RVC.pwmDuty.rearLeftDec = (0) * 0.01f * RVC.calibration.leftDec.mul + RVC.calibration.leftDec.offset;
		RVC.pwmDuty.rearRightDec = (0) * 0.01f * RVC.calibration.rightDec.mul + RVC.calibration.rightDec.offset;
	}
#else
	// 0~100% maped to 1~4V ( = 0.2~0.8 duty)
	if(RVC.readyToDrive == RVC_ReadyToDrive_status_run)
	{
		if(RVC.torque.rearLeft > 0) 	//Accelertation
		{
			RVC.pwmDuty.rearLeftAcc =
				(RVC.torque.rearLeft) * 0.006f * RVC.calibration.leftAcc.mul + 0.2f + RVC.calibration.leftAcc.offset;
			RVC.pwmDuty.rearLeftDec = 
				(0) * 0.006f * RVC.calibration.leftDec.mul + 0.2f + RVC.calibration.leftDec.offset;
		}
		else 
		{
			RVC.pwmDuty.rearLeftAcc =
				(0) * 0.006f * RVC.calibration.leftAcc.mul + 0.2f + RVC.calibration.leftAcc.offset;
			RVC.pwmDuty.rearLeftDec = 
				(-(RVC.torque.rearLeft)*REGEN_MUL) * 0.006f * RVC.calibration.leftDec.mul + 0.2f + RVC.calibration.leftDec.offset;
		}

		if(RVC.torque.rearRight > 0)
		{
			RVC.pwmDuty.rearRightAcc =
				(RVC.torque.rearRight) * 0.006f * RVC.calibration.rightAcc.mul + 0.2f + RVC.calibration.rightAcc.offset;			
			RVC.pwmDuty.rearRightDec = 
				(0) * 0.006f * RVC.calibration.rightDec.mul + 0.2f + RVC.calibration.rightDec.offset;
		}
		else 
		{
			RVC.pwmDuty.rearRightAcc =
				(0) * 0.006f * RVC.calibration.rightAcc.mul + 0.2f + RVC.calibration.rightAcc.offset;
			RVC.pwmDuty.rearRightDec = 
				(-(RVC.torque.rearRight)*REGEN_MUL) * 0.006f * RVC.calibration.rightDec.mul + 0.2f + RVC.calibration.rightDec.offset;			
		}
	}
	else
	{
		RVC.pwmDuty.rearLeftAcc = (0) * 0.006f * RVC.calibration.leftAcc.mul + 0.2f + RVC.calibration.leftAcc.offset;
		RVC.pwmDuty.rearRightAcc = (0) * 0.006f * RVC.calibration.rightAcc.mul + 0.2f + RVC.calibration.rightAcc.offset;
		RVC.pwmDuty.rearLeftDec = (0) * 0.006f * RVC.calibration.leftDec.mul + 0.2f + RVC.calibration.leftDec.offset;
		RVC.pwmDuty.rearRightDec = (0) * 0.006f * RVC.calibration.rightDec.mul + 0.2f + RVC.calibration.rightDec.offset;
	}
#endif 
}

IFX_INLINE void RVC_updatePwmSignal(void)
{
	HLD_GtmTomPwm_setTriggerPointFloat(&RVC.out.accel_rearLeft, RVC.pwmDuty.rearLeftAcc);
	HLD_GtmTomPwm_setTriggerPointFloat(&RVC.out.accel_rearRight, RVC.pwmDuty.rearRightAcc);
	HLD_GtmTomPwm_setTriggerPointFloat(&RVC.out.decel_rearLeft, RVC.pwmDuty.rearLeftDec);
	HLD_GtmTomPwm_setTriggerPointFloat(&RVC.out.decel_rearRight, RVC.pwmDuty.rearRightDec);
}

IFX_INLINE void VariableUpdateRoutine(void)
{
	SteeringWheel_public.shared.data.vehicleSpeed = SDP_WheelSpeed.velocity.chassis;
	SteeringWheel_public.shared.data.apps = SDP_PedalBox.apps.pps;
	SteeringWheel_public.shared.data.bpps = SDP_PedalBox.bpps.pps;
	if(RVC.readyToDrive == RVC_ReadyToDrive_status_run)
		SteeringWheel_public.shared.data.isReadyToDrive = TRUE;
	else
		SteeringWheel_public.shared.data.isReadyToDrive = FALSE;
	SteeringWheel_public.shared.data.isAppsChecked = RVC.R2d.isAppsChecked;
	SteeringWheel_public.shared.data.isBppsChecked1 = RVC.R2d.isBppsChecked1;
	SteeringWheel_public.shared.data.isBppsChecked2 = RVC.R2d.isBppsChecked2;
	if(SDP_PedalBox.apps.isValueOk == TRUE)
		SteeringWheel_public.shared.data.appsError = FALSE;
	else
		SteeringWheel_public.shared.data.appsError = TRUE;
	if(SDP_PedalBox.bpps.isValueOk == TRUE)
		SteeringWheel_public.shared.data.bppsError = FALSE;
	else 
		SteeringWheel_public.shared.data.bppsError = TRUE;
	SteeringWheel_public.shared.data.lvBatteryVoltage = RVC.LvBattery_Voltage.value;
}
volatile uint32 updateErrorCount = 0;
IFX_INLINE void RVC_updateSharedVariable(void)
{
	// static uint32 updateErrorCount = 0;
	if(IfxCpu_acquireMutex(&SteeringWheel_public.shared.mutex))	//Do not wait.
	{
		VariableUpdateRoutine();
		IfxCpu_releaseMutex(&SteeringWheel_public.shared.mutex);
	}
	else if(updateErrorCount < VAR_UPDATE_ERROR_LIM)
	{
		updateErrorCount++;
	}
	else
	{
		while(IfxCpu_acquireMutex(&SteeringWheel_public.shared.mutex));
		{
			VariableUpdateRoutine();
			IfxCpu_releaseMutex(&SteeringWheel_public.shared.mutex);
		}
		updateErrorCount = 0;
	}
	if(IfxCpu_acquireMutex(&RVC_public.bms.shared.mutex))
	{
		if(RVC_public.bms.shared.isUpdated)
		{
			RVC_public.bms.data.isUpdated = TRUE;
			RVC_public.bms.shared.isUpdated = FALSE;
			RVC_public.bms.data.averageTemp = RVC_public.bms.shared.data.averageTemp;
			RVC_public.bms.data.bmsTemp = RVC_public.bms.shared.data.bmsTemp;
			RVC_public.bms.data.chargeLimit = RVC_public.bms.shared.data.chargeLimit;
			RVC_public.bms.data.current = RVC_public.bms.shared.data.current;
			RVC_public.bms.data.dischargeLimit = RVC_public.bms.shared.data.dischargeLimit;
			RVC_public.bms.data.highestTemp = RVC_public.bms.shared.data.highestTemp;
			RVC_public.bms.data.lowestVoltage = RVC_public.bms.shared.data.lowestVoltage;
			RVC_public.bms.data.soc = RVC_public.bms.shared.data.soc;
			RVC_public.bms.data.voltage = RVC_public.bms.shared.data.voltage;
			
		}
		IfxCpu_releaseMutex(&RVC_public.bms.shared.mutex);
	}
}