/**
 * \file Configuration.h
 * \brief Global configuration
 *
 * \version iLLD_Demos_1_0_1_4_0
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
 *
 * \defgroup IfxLld_Demo_StmDemo_SrcDoc_Config Application configuration
 * \ingroup IfxLld_Demo_StmDemo_SrcDoc
 *
 *
 */

#ifndef CONFIGURATION_H
#define CONFIGURATION_H
/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/
#include "Ifx_Cfg.h"
#include "ConfigurationIsr.h"
#include "_Impl/IfxGlobal_cfg.h"
#include "IfxPort_PinMap.h"

/******************************************************************************/
/*-----------------------------------Macros-----------------------------------*/
/******************************************************************************/
#define HLD_BOARD_AK237		0
#define HLD_BOARD_SB275		1

#define HLD_BOARD 			HLD_BOARD_SB275
#define HLD_FOURTH_CANNODE	FALSE

/*GPIO macros*/		//FIXME: Specify Port Init source file
#define START_BTN					IfxPort_P11_9
#define R2DOUT 						IfxPort_P21_0
#define FWD_OUT						IfxPort_P21_3
#define AIR_P_IN					IfxPort_P00_6
#define AIR_N_IN					IfxPort_P00_7
#define BP_IN						IfxPort_P14_1
#define BSW_IN						IfxPort_P00_0

/*Qspi2 macros*/
#define QSPI2_SCLK					IfxQspi2_SCLK_P15_6_OUT
#define QSPI2_MTSR					IfxQspi2_MTSR_P15_5_OUT
#define QSPI2_MRST					IfxQspi2_MRSTA_P15_4_IN
#define QSPI2_SLSO9					IfxQspi2_SLSO9_P20_3_OUT

/*GtmTom macros*/
#define BEEPER                      IfxGtm_TOM0_3_TOUT80_P14_0_OUT//IfxGtm_TOM0_4_TOUT22_P33_0_OUT
#define PWMACCR						IfxGtm_TOM0_15_TOUT17_P00_8_OUT
#define PWMACCL						IfxGtm_TOM0_9_TOUT10_P00_1_OUT
#define PWMDCCR						IfxGtm_TOM1_0_TOUT18_P00_9_OUT
#define PWMDCCL						IfxGtm_TOM1_1_TOUT11_P00_2_OUT

/*GtmTim macros*/
#define GTMTIM0						IfxGtm_TIM0_6_TIN24_P33_2_IN	//APPS0
#define GTMTIM1						IfxGtm_TIM2_4_TIN14_P00_5_IN	//APPS1		//IfxGtm_TIM2_3_TIN13_P00_4_IN
#define GTMTIM2						IfxGtm_TIM0_3_TIN21_P00_12_IN	//APPS2
#define GTMTIM3						IfxGtm_TIM3_3_TIN13_P00_4_IN	//BPPS0
#define GTMTIM4						IfxGtm_TIM0_2_TIN20_P00_11_IN	//BPPS1
#define GTMTIM5						IfxGtm_TIM0_5_TIN23_P33_1_IN	//STA		//IfxGtm_TIM1_4_TIN22_P33_0_IN//IfxGtm_TIM0_4_TIN22_P33_0_IN
#define GTMTIM6						IfxGtm_TIM2_3_TIN97_P11_6_IN	//WSS0
#define GTMTIM7						IfxGtm_TIM2_2_TIN96_P11_3_IN	//WSS1
#define GTMTIM8						IfxGtm_TIM2_1_TIN95_P11_2_IN	//WSS2
#define GTMTIM9						IfxGtm_TIM2_6_TIN100_P11_11_IN	//WSS3


/*Led port macros*/
#define LED_ONBOARD					IfxPort_P10_2


/*MultiCAN macros*/
#define CAN0NODE0IN					IfxMultican_RXD0D_P02_4_IN//IfxMultican_RXD0A_P02_1_IN
#define CAN0NODE0OUT				IfxMultican_TXD0_P02_5_OUT//IfxMultican_TXD0_P02_0_OUT

#define CAN0NODE1IN					IfxMultican_RXD1A_P15_3_IN
#define CAN0NODE1OUT				IfxMultican_TXD1_P15_2_OUT

#define CAN0NODE2IN					IfxMultican_RXD2B_P02_3_IN
#define CAN0NODE2OUT				IfxMultican_TXD2_P15_0_OUT//IfxMultican_TXD2_P02_2_OUT

#if HLD_BOARD == (HLD_BOARD_SB275 && (HLD_FOURTH_CANNODE == TRUE))
#define CAN0NODE3IN					IfxMultican_RXD2B_P02_3_IN
#define CAN0NODE3OUT				IfxMultican_TXD2_P02_2_OUT
#endif

#if HLD_BOARD == HLD_BOARD_AK237
/* CAN0NODE2 has lower priority */
#define CAN1NODE0IN					IfxMultican1_RXD0G_P00_5_IN
#define CAN1NODE0OUT				IfxMultican1_TXD0_P00_4_OUT

#define CAN1NODE1IN					IfxMultican1_RXD1F_P02_7_IN
#define CAN1NODE1OUT				IfxMultican1_TXD1_P02_5_OUT

#define CAN1NODE2IN					IfxMultican1_RXD2E_P20_9_IN
#define CAN1NODE2OUT				IfxMultican1_TXD2_P20_10_OUT
#endif

/********************* Unused macro s*********************/

/*Ccu6 macros*/
/*Compatibility layer*/
#define MOTOR_U5					MOTOR_U//IfxCcu60_COUT60_P02_1_OUT
#define MOTOR_U6					MOTOR_W//IfxCcu60_COUT61_P02_3_OUT
#define MOTOR_U5_INH				MOTOR_U_INH//IfxPort_P02_0
#define MOTOR_U6_INH				MOTOR_W_INH//IfxPort_P02_2

#define MOTOR_U						IfxCcu60_CC61_P11_11_OUT	//DIG43//Temporary setting
#define MOTOR_V
#define MOTOR_W						IfxCcu60_CC62_P11_10_OUT	//DIG42//Temporary setting
#define MOTOR_U_INH					IfxPort_P02_4				//DIG6
#define MOTOR_V_INH					IfxPort_P02_3				//DIG5
#define MOTOR_W_INH					IfxPort_P02_1				//DIG3



/*Gpt Encoder macros*/
#define ENCODER_A					IfxGpt120_T2INA_P00_7_IN
#define ENCODER_B					IfxGpt120_T3EUDA_P02_7_IN

/*Line scan macros*/
#define TSL1401_SI					IfxPort_P14_0
#define TSL1401_CLK					IfxPort_P14_1
#define TSL1401_AO_1				3
#define TSL1401_AO_2				2
#define TSL1401_AO_3				1

/*Infra red macros*/
#define IR_OBSTACLE					4
#define IR_HILL						5


/*UserInterface macros*/
//#define START_BTN					IfxPort_P00_1
#define UI_BTN1						IfxPort_P00_9
#define UI_BTN2						IfxPort_P00_2
#define UI_BTN3						IfxPort_P00_3
#define UI_BTN4						IfxPort_P00_10
#define UI_BTN5						IfxPort_P00_11
#define UI_BTN6						IfxPort_P00_4
#define UI_BTN7						IfxPort_P00_5


#define LCD_EN						IfxPort_P15_0		//DIG14
#define LCD_B4						IfxPort_P15_1		//DIG15
#define	LCD_B5						IfxPort_P33_9		//DIG16
#define LCD_B6						IfxPort_P33_8		//DIG17
#define LCD_B7						IfxPort_P20_0		//DIG18

#define LCD_RS						IfxPort_P33_4		//DIG50
#define LCD_RW						IfxPort_P33_12		//DIG51

#define UI_LED1						IfxPort_P33_2
#define UI_LED2						IfxPort_P00_6
#define UI_LED3						IfxPort_P33_1



/*GPT Cnt macros*/
#define GPT12T3IN					IfxGpt120_T3INA_P02_6_IN
#define GPT12T4IN					IfxGpt120_T4INA_P02_8_IN


#define SERVO						IfxGtm_TOM0_7_TOUT25_P33_3_OUT//IfxGtm_TOM0_7_TOUT84_P14_4_OUT
#define SERVO_IR					IfxGtm_TOM2_1_TOUT95_P11_2_OUT//IfxGtm_TOM2_1_TOUT95_P11_2_OUTIfxGtm_TOM2_1_TOUT120_P11_1_OUT





/** \addtogroup IfxLld_Demo_Template_SrcDoc_Config
 * \{ */
/*______________________________________________________________________________
** Help Macros
**____________________________________________________________________________*/
/**
 * \name Macros for Regression Runs
 * \{
 */
#ifndef REGRESSION_RUN_STOP_PASS
#define REGRESSION_RUN_STOP_PASS
#endif

#ifndef REGRESSION_RUN_STOP_FAIL
#define REGRESSION_RUN_STOP_FAIL
#endif
/** \} */

/** \} */
#endif
