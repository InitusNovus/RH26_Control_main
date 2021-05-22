/*
 * Beeper_Test.c
 *
 *  Created on: 2018. 5. 20.
 *      Author: bigbi_000
 */

/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/
#include "../../../Gtm/Tom/Beeper/GtmTomBeeper.h"

#include "HLD.h"
#include <stdio.h>

/******************************************************************************/
/*-----------------------------------Macros-----------------------------------*/
/******************************************************************************/
//#define BPM 120
#define BPM 140
#define GAP 50

/******************************************************************************/
/*--------------------------------Enumerations--------------------------------*/
/******************************************************************************/

/******************************************************************************/
/*-----------------------------Data Structures--------------------------------*/
/******************************************************************************/
typedef struct
{
	boolean 			on;
	float32 			frequency;
	Ifx_TimerValue 		triggerPoint;
	IfxGtm_Tom_Timer 	driver;
} GtmTomBeeper_t;
typedef struct
{
	boolean isend;
	uint32 note_counter;
	uint32 beat_counter;
	uint32 beats;
	float32 pitch;
	boolean tie;
	note_t* piece;
	float32 volume;
}music_t;
/******************************************************************************/
/*------------------------------Global variables------------------------------*/
/******************************************************************************/
GtmTomBeeper_t g_GtmTomBeeper =
{
		.on = FALSE,
		.triggerPoint = 300,
		.frequency = 440
};


music_t g_Music =
{
		.isend = TRUE,
		.note_counter = 0,
		.beat_counter = 0,
		.beats = 0,
		.pitch = 0,
		.tie = 0,
		.piece = 0,	//NULL
		.volume = 0.2
};

HLD_Beeper_t HLD_GtmTomBeeper =
{
		.volume = 0.15,
		.frequency = 440,
		.on = FALSE,
};

note_t Beep_off[] = {ENDING};
note_t Beep_pattern1[] = {TC6L, ENDING};
note_t Beep_pattern2[] = {TC5L, ENDING};
note_t Beep_pattern3[] = {SE6,SE6,SE6, ENDING};
note_t Beep_pattern4[] = {TG5L,TA5L,TB5L,TC6L,TD6L,ENDING};
note_t Beep_pattern5[] = {TD5L,TC5L,TB5L,TA6L,TG6L,ENDING};
note_t Beep_pattern6[] = {TG5L,TB5L,TD6L,ENDING};
note_t Beep_pattern7[] = {TD5L,TB5L,TG6L,ENDING};
note_t Beep_pattern8[] = {SA6L,ENDING};
note_t Beep_pattern9[] = {SAS5L,ENDING};
note_t Beep_pattern10[] = {SF5L,ENDING};

/******************************************************************************/
/*-------------------------Function Prototypes--------------------------------*/
/******************************************************************************/
IFX_STATIC void getBeats (note_t note);
IFX_STATIC void getPitch (note_t note);

/******************************************************************************/
/*------------------------Private Variables/Constants-------------------------*/
/******************************************************************************/

/******************************************************************************/
/*-------------------------Function Prototypes--------------------------------*/
/******************************************************************************/
IFX_STATIC void HLD_GtmTomBeeper_runInternal(void);
IFX_STATIC void HLD_GtmTomBeeper_play(void);
/******************************************************************************/
/*-------------------------Function Implementations---------------------------*/
/******************************************************************************/

void HLD_GtmTomBeeper_start(note_t* target)
{
	g_Music.piece = target;
	g_Music.note_counter = 0;
	g_Music.beat_counter = 0;
	g_Music.beats = 0;
	g_Music.pitch = 0;
	g_Music.isend = FALSE;
	g_Music.volume = VOLUME_DEFAULT;
	HLD_GtmTomBeeper.on = TRUE;
}
void HLD_GtmTomBeeper_start_volume(note_t* target, float32 volume)
{
	g_Music.piece = target;
	g_Music.note_counter = 0;
	g_Music.beat_counter = 0;
	g_Music.beats = 0;
	g_Music.pitch = 0;
	g_Music.isend = FALSE;
	g_Music.volume = volume;
	HLD_GtmTomBeeper.on = TRUE;
}
void HLD_GtmTomBeeper_play (void)
{

	if((g_Music.isend == FALSE))
	{
		if(g_Music.beat_counter == 0)
		{
			getBeats(g_Music.piece[g_Music.note_counter]);
			getPitch(g_Music.piece[g_Music.note_counter]);
			if(g_Music.pitch != 0)
			{
				HLD_GtmTomBeeper.frequency = g_Music.pitch;
			}
		}
		g_Music.beat_counter ++;

		if((g_Music.pitch < 0)||(g_Music.isend == TRUE))
		{
			HLD_GtmTomBeeper.volume = 0;
		}
		else
		{
			if((g_Music.beat_counter > g_Music.beats - GAP)&&(g_Music.tie == FALSE))
			{
				HLD_GtmTomBeeper.volume = 0;
			}
			else
			{
				HLD_GtmTomBeeper.volume = g_Music.volume;
			}
		}

		if(g_Music.beat_counter >= g_Music.beats)
		{
			g_Music.beat_counter = 0;
			g_Music.note_counter ++;
		}
	}
}

IFX_STATIC void getBeats (note_t note)
{
	switch(note.beat)
	{
	case note_whole:		g_Music.beats = (4*BEAT(BPM)); 	break;
	case note_half:			g_Music.beats = (2*BEAT(BPM)); 	break;
	case note_quarter:		g_Music.beats = (BEAT(BPM));	break;
	case note_eighth:		g_Music.beats = (BEAT(BPM)/2); 	break;
	case note_sixteenth:	g_Music.beats = (BEAT(BPM)/4); 	break;
	case note_thirtysecond:	g_Music.beats = (BEAT(BPM)/8); 	break;
	case end: 				g_Music.isend = TRUE ;			break;
	case repeat:			g_Music.note_counter = 1;
	g_Music.beat_counter = 1;
	break;
	default: 				g_Music.beats = 0;
	}
	if(note.tie == TRUE)
	{
		g_Music.tie = TRUE;
	}
	else
	{
		g_Music.tie = FALSE;
	}
}
IFX_STATIC void getPitch (note_t note)
{
	switch(note.note)
	{
	case rest: 				g_Music.pitch = -1;			break;
	case note_C5:			g_Music.pitch = NOTE_C5;	break;
	case note_Cs5:			g_Music.pitch = NOTE_CS5;	break;
	case note_D5:			g_Music.pitch = NOTE_D5;	break;
	case note_Ds5:			g_Music.pitch = NOTE_DS5;	break;
	case note_E5:			g_Music.pitch = NOTE_E5;	break;
	case note_F5:			g_Music.pitch = NOTE_F5;	break;
	case note_Fs5:			g_Music.pitch = NOTE_FS5;	break;
	case note_G5:			g_Music.pitch = NOTE_G5;	break;
	case note_Gs5:			g_Music.pitch = NOTE_GS5;	break;
	case note_A5:			g_Music.pitch = NOTE_A5;	break;
	case note_As5:			g_Music.pitch = NOTE_AS5;	break;
	case note_B5:			g_Music.pitch = NOTE_B5;	break;
	case note_C6:			g_Music.pitch = NOTE_C6;	break;
	case note_Cs6:			g_Music.pitch = NOTE_CS6;	break;
	case note_D6:			g_Music.pitch = NOTE_D6;	break;
	case note_Ds6:			g_Music.pitch = NOTE_DS6;	break;
	case note_E6:			g_Music.pitch = NOTE_E6;	break;
	case note_F6:			g_Music.pitch = NOTE_F6;	break;
	case note_Fs6:			g_Music.pitch = NOTE_FS6;	break;
	case note_G6:			g_Music.pitch = NOTE_G6;	break;
	case note_Gs6:			g_Music.pitch = NOTE_GS6;	break;
	case note_A6:			g_Music.pitch = NOTE_A6;	break;
	case note_C7:			g_Music.pitch = NOTE_C7;	break;
	}
}

#define DRIVER (&g_GtmTomBeeper.driver)
IFX_STATIC void HLD_GtmTomBeeper_runInternal(void)
{
	g_GtmTomBeeper.on = HLD_GtmTomBeeper.on;
	g_GtmTomBeeper.frequency = HLD_GtmTomBeeper.frequency;
	g_GtmTomBeeper.triggerPoint = g_GtmTomBeeper.driver.base.period * HLD_GtmTomBeeper.volume/2;

	if(g_GtmTomBeeper.on == TRUE)
	{
		IfxGtm_Tom_Timer_disableUpdate(DRIVER);
		IfxGtm_Tom_Timer_setFrequency(DRIVER, g_GtmTomBeeper.frequency);
		IfxGtm_Tom_Timer_setTrigger(DRIVER, g_GtmTomBeeper.triggerPoint);
		IfxGtm_Tom_Timer_applyUpdate(DRIVER);
		/*Output port on*/
		IfxPort_setPinModeOutput(BEEPER.pin.port, BEEPER.pin.pinIndex, IfxPort_OutputMode_pushPull, IfxPort_OutputIdx_alt1);
	}
	else
	{
		/*Output port off*/
		IfxPort_setPinModeInput(BEEPER.pin.port, BEEPER.pin.pinIndex, IfxPort_Mode_inputPullDown);
	}
}
#undef DRIVER

void HLD_GtmTomBeeper_init(void)
{
	IfxGtm_Tom_Timer_Config timerConfig;
	IfxGtm_Tom_Timer_initConfig (&timerConfig, &MODULE_GTM);
	timerConfig.tom = BEEPER.tom;
	timerConfig.timerChannel = BEEPER.channel;
	timerConfig.triggerOut = &BEEPER;
	timerConfig.base.frequency = g_GtmTomBeeper.frequency;
	timerConfig.base.minResolution = 0;
	timerConfig.clock = IfxGtm_Tom_Ch_ClkSrc_cmuFxclk1;

	timerConfig.base.trigger.enabled = TRUE;
	timerConfig.base.trigger.outputEnabled = TRUE;
	timerConfig.base.trigger.outputMode = IfxPort_OutputMode_pushPull;
	timerConfig.base.trigger.outputDriver = IfxPort_PadDriver_cmosAutomotiveSpeed1;
	timerConfig.base.trigger.triggerPoint =  g_GtmTomBeeper.triggerPoint;
	timerConfig.base.trigger.risingEdgeAtPeriod = TRUE;
	IfxGtm_Tom_Timer_init (&g_GtmTomBeeper.driver, &timerConfig);

	/*switch off the beeper*/
	IfxPort_setPinModeInput(BEEPER.pin.port, BEEPER.pin.pinIndex, IfxPort_Mode_inputPullDown);
	/*start the 2048Hz for Beeper*/
	IfxGtm_Tom_Timer_run(&g_GtmTomBeeper.driver);

	g_GtmTomBeeper.frequency = HLD_GtmTomBeeper.frequency;
	g_GtmTomBeeper.triggerPoint = g_GtmTomBeeper.driver.base.period * HLD_GtmTomBeeper.on;
	IfxGtm_Tom_Timer_setFrequency(&g_GtmTomBeeper.driver, g_GtmTomBeeper.frequency);
	IfxGtm_Tom_Timer_setTrigger(&g_GtmTomBeeper.driver, g_GtmTomBeeper.triggerPoint);
}

void HLD_GtmTomBeeper_run_1ms(void)
{
	HLD_GtmTomBeeper_play();
	HLD_GtmTomBeeper_runInternal();
}
