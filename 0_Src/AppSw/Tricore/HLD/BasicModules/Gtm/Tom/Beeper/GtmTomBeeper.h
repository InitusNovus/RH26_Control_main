/*
 * GtmTomBeeper.h.h
 *
 *      Author: Wonjun Youm
 *      Last Modified: 2019.05.29
 */

#ifndef GTMTOMBEEPER_H_
#define GTMTOMBEEPER_H_

/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/
#include "GtmTomBeeper_note.h"
#include "GtmTom.h"

/******************************************************************************/
/*-----------------------------------Macros-----------------------------------*/
/******************************************************************************/
#define BEAT(bpm)			(1000*60/(bpm))
#define VOLUME_DEFAULT		0.2
/******************************************************************************/
/*------------------------------Type Definitions------------------------------*/
/******************************************************************************/

/******************************************************************************/
/*--------------------------------Enumerations--------------------------------*/
/******************************************************************************/
typedef enum
{
	end,
	repeat,
	note_whole,
	note_half,
	note_quarter,
	note_eighth,
	note_sixteenth,
	note_thirtysecond
}noteBeat_t;

typedef enum
{
	rest,
	note_C5,note_Cs5,note_D5,note_Ds5,note_E5,note_F5,note_Fs5,note_G5,note_Gs5,note_A5,note_As5,note_B5,
	note_C6,note_Cs6,note_D6,note_Ds6,note_E6,note_F6, note_Fs6, note_G6, note_Gs6, note_A6, note_C7
}notePitch_t;


/******************************************************************************/
/*-----------------------------Data Structures--------------------------------*/
/******************************************************************************/
typedef struct
{
	noteBeat_t beat;
	notePitch_t note;
	boolean tie;
}note_t;
typedef struct
{
	boolean on;
	float32 volume;
	float32 frequency;
}HLD_Beeper_t;
/******************************************************************************/
/*------------------------------Global variables------------------------------*/
/******************************************************************************/
IFX_EXTERN HLD_Beeper_t HLD_GtmTomBeeper;

/*
 * Beep patterns
 * */
IFX_EXTERN note_t Beep_off[];
IFX_EXTERN note_t Beep_pattern1[];
IFX_EXTERN note_t Beep_pattern2[];
IFX_EXTERN note_t Beep_pattern3[];
IFX_EXTERN note_t Beep_pattern4[];
IFX_EXTERN note_t Beep_pattern5[];
IFX_EXTERN note_t Beep_pattern6[];
IFX_EXTERN note_t Beep_pattern7[];
IFX_EXTERN note_t Beep_pattern8[];
IFX_EXTERN note_t Beep_pattern9[];
IFX_EXTERN note_t Beep_pattern10[];
/******************************************************************************/
/*-------------------------Function Prototypes--------------------------------*/
/******************************************************************************/
/*
 * Bepper initialization function
 * */
IFX_EXTERN void HLD_GtmTomBeeper_init(void);
/*
 * Beeper running function
 * Note: It have to be called every 1ms.
 * */
IFX_EXTERN void HLD_GtmTomBeeper_run_1ms(void);
/*
 * Beep pattern implement function
 * Note: It have to be called every 1ms.
 * */
//IFX_EXTERN void HLD_GtmTomBeeper_play (void);
/*
 * Beep pattern start function
 * Argument: Beep pattern array
 * Note: The new beep pattern overrides the playing one.
 * */
IFX_EXTERN void HLD_GtmTomBeeper_start (note_t* target);
/*
 * Beep pattern start function
 * Argument1: Beep pattern array
 * Argument2: Volume(0~1, real number)
 * Note: The new beep pattern overrides the playing one.
 * */
IFX_EXTERN void HLD_GtmTomBeeper_start_volume(note_t* target, float32 volume);

/******************************************************************************/
/*---------------------Inline Function Implimentations------------------------*/
/******************************************************************************/
IFX_INLINE void HLD_GtmTomBeeper_setVolume(float32 volume)
{
	if(volume > 1)
		volume = 1;
	else if(volume < 0)
		volume = 0;

	HLD_GtmTomBeeper.volume = volume;
}
IFX_INLINE void HLD_GtmTomBeeper_setVolumeDefault(void)
{
	HLD_GtmTomBeeper.volume = VOLUME_DEFAULT;
}

#endif /* BEEPER_TEST_H */
