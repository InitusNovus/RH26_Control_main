/*
 * SteeringWheel.h
 * Created on: 2020.08.05
 * Author: Dua
 */


/* TODO
 * SteeringWheel Can Transmition
 * -
 */

/***************************** Includes ******************************/
#include "SteeringWheel.h"


/**************************** Macro **********************************/


/************************* Data Structures ***************************/
typedef struct 
{
	CanCommunication_Message msgObj1;
	CanCommunication_Message msgObj2;
	CanCommunication_Message msgObj3;
	SteeringWheel_canMsg1_t canMsg1;
	SteeringWheel_canMsg2_t canMsg2;
	SteeringWheel_canMsg3_t canMsg3;
}SteeringWheel_t;

/*********************** Global Variables ****************************/
const uint32 StWhlMsgId1 = 0x00101F00UL;
const uint32 StWhlMsgId2 = 0x00101F01UL;
const uint32 StWhlMsgId3 = 0x00101F02UL;

SteeringWheel_t SteeringWheel;
SteeringWheel_public_t SteeringWheel_public;

/******************* Private Function Prototypes *********************/


/********************* Function Implementation ***********************/

void SteeringWheel_init(void)
{
	{
		CanCommunication_Message_Config config;
		config.messageId		=	StWhlMsgId1;
        config.frameType		=	IfxMultican_Frame_transmit;
        config.dataLen			=	IfxMultican_DataLengthCode_8;
        config.node				=	&CanCommunication_canNode0;
		CanCommunication_initMessage(&SteeringWheel.msgObj1, &config);
	}
	{
		CanCommunication_Message_Config config;
		config.messageId 		= 	StWhlMsgId2;
		config.frameType		=	IfxMultican_Frame_transmit;
        config.dataLen			=	IfxMultican_DataLengthCode_8;
        config.node				=	&CanCommunication_canNode0;
        CanCommunication_initMessage(&SteeringWheel.msgObj2, &config);
	}
	{
		CanCommunication_Message_Config config;
		config.messageId 		= 	StWhlMsgId3;
		config.frameType		=	IfxMultican_Frame_transmit;
        config.dataLen			=	IfxMultican_DataLengthCode_4;
        config.node				=	&CanCommunication_canNode0;
        CanCommunication_initMessage(&SteeringWheel.msgObj3, &config);
	}
}

void SteeringWheel_run_xms_c2(void)
{
	/* Shared variable update */
	while(IfxCpu_acquireMutex(&SteeringWheel_public.shared.mutex));	//Wait for mutex
	{
		SteeringWheel_public.data = SteeringWheel_public.shared.data;
		IfxCpu_releaseMutex(&SteeringWheel_public.shared.mutex);
	}

	/* Data parsing */
	SteeringWheel.canMsg1.S.vehicleSpeed = (uint8)SteeringWheel_public.data.vehicleSpeed;
	SteeringWheel.canMsg1.S.lowestVoltage = OrionBms2.msg3.lowVoltage;
	SteeringWheel.canMsg1.S.highestTemp = OrionBms2.msg3.highTemp;
	SteeringWheel.canMsg1.S.bmsTemp = OrionBms2.msg3.bmsTemp;
	SteeringWheel.canMsg1.S.soc = OrionBms2.msg1.packSoc;
	SteeringWheel.canMsg1.S.averageTemp = OrionBms2.msg3.avgTemp;
	SteeringWheel.canMsg1.S.status.S.r2d = ((SteeringWheel_public.data.isReadyToDrive & 0x1) << 3) |
	                                       ((SteeringWheel_public.data.isBppsChecked2 & 0x1) << 2) |
	                                       ((SteeringWheel_public.data.isBppsChecked1 & 0x1) << 1) |
	                                       ((SteeringWheel_public.data.isAppsChecked & 0x1) << 0);
	SteeringWheel.canMsg1.S.status.S.appsError = SteeringWheel_public.data.appsError;
	SteeringWheel.canMsg1.S.status.S.bppsError = SteeringWheel_public.data.bppsError;

	SteeringWheel.canMsg2.S.apps = (uint16)(SteeringWheel_public.data.apps*100*10);
	SteeringWheel.canMsg2.S.bpps = (uint16)(SteeringWheel_public.data.bpps*100*10);
	SteeringWheel.canMsg2.S.lvBatteryVoltage = (uint16)(SteeringWheel_public.data.lvBatteryVoltage*100);
	SteeringWheel.canMsg2.S.accumulatorVoltage = OrionBms2.msg1.packVoltage;

	SteeringWheel.canMsg3.S.inverter1Temp = kelly8080ips1.msg2.conTemp;
	SteeringWheel.canMsg3.S.motor1Temp = kelly8080ips1.msg2.motTemp;
	SteeringWheel.canMsg3.S.inverter2Temp = kelly8080ips2.msg2.conTemp;
	SteeringWheel.canMsg3.S.motor2Temp = kelly8080ips2.msg2.motTemp;

	/* Set the messages */
	CanCommunication_setMessageData(SteeringWheel.canMsg1.U[0], SteeringWheel.canMsg1.U[1], &SteeringWheel.msgObj1);
	CanCommunication_setMessageData(SteeringWheel.canMsg2.U[0], SteeringWheel.canMsg2.U[1], &SteeringWheel.msgObj2);
	CanCommunication_setMessageData(SteeringWheel.canMsg3.U, 0, &SteeringWheel.msgObj3);

	/* Transmit the messages */
	CanCommunication_transmitMessage(&SteeringWheel.msgObj1);
	CanCommunication_transmitMessage(&SteeringWheel.msgObj2);
	CanCommunication_transmitMessage(&SteeringWheel.msgObj3);
}