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
	CanCommunication_Message msgObj4;
	CanCommunication_Message msgObj5;
	CanCommunication_Message msgObj6;
	SteeringWheel_canMsg1_t canMsg1;
	SteeringWheel_canMsg2_t canMsg2;
	SteeringWheel_canMsg3_t canMsg3;
	SteeringWheel_canMsg4_t canMsg4;
	SteeringWheel_canMsg5_t canMsg5;
	SteeringWheel_canMsg6_t canMsg6;
}SteeringWheel_t;

/*********************** Global Variables ****************************/
const uint32 StWhlMsgId1 = 0x00101F00UL;
const uint32 StWhlMsgId2 = 0x00101F01UL;
const uint32 StWhlMsgId3 = 0x00101F02UL;

const uint32 StWhlMsgId4 = 0x00101F03UL;
const uint32 StWhlMsgId5 = 0x00101F04UL;
const uint32 StWhlMsgId6 = 0x00101F05UL;

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
	{
		CanCommunication_Message_Config config;
		config.messageId 		= 	StWhlMsgId4;
		config.frameType		=	IfxMultican_Frame_transmit;
        config.dataLen			=	IfxMultican_DataLengthCode_8;
        config.node				=	&CanCommunication_canNode0;
        CanCommunication_initMessage(&SteeringWheel.msgObj4, &config);
	}
	{
		CanCommunication_Message_Config config;
		config.messageId 		= 	StWhlMsgId5;
		config.frameType		=	IfxMultican_Frame_transmit;
        config.dataLen			=	IfxMultican_DataLengthCode_8;
        config.node				=	&CanCommunication_canNode0;
        CanCommunication_initMessage(&SteeringWheel.msgObj5, &config);
	}
	{
		CanCommunication_Message_Config config;
		config.messageId 		= 	StWhlMsgId6;
		config.frameType		=	IfxMultican_Frame_transmit;
        config.dataLen			=	IfxMultican_DataLengthCode_8;
        config.node				=	&CanCommunication_canNode0;
        CanCommunication_initMessage(&SteeringWheel.msgObj6, &config);
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

	SteeringWheel.canMsg2.S.apps = (uint16)(SteeringWheel_public.data.apps*100*10);			//FP 0.1 percent
	SteeringWheel.canMsg2.S.bpps = (uint16)(SteeringWheel_public.data.bpps*100*10);			//FP 0.1 percent
	SteeringWheel.canMsg2.S.lvBatteryVoltage = (uint16)(SteeringWheel_public.data.lvBatteryVoltage*100);
	SteeringWheel.canMsg2.S.accumulatorVoltage = OrionBms2.msg1.packVoltage;

	SteeringWheel.canMsg3.S.inverter1Temp = kelly8080ips1.msg2.conTemp;
	SteeringWheel.canMsg3.S.motor1Temp = kelly8080ips1.msg2.motTemp;
	SteeringWheel.canMsg3.S.inverter2Temp = kelly8080ips2.msg2.conTemp;
	SteeringWheel.canMsg3.S.motor2Temp = kelly8080ips2.msg2.motTemp;

	SteeringWheel.canMsg4.S.brakePressure1 = (uint16)(SteeringWheel_public.data.brakePressure1*10);	//FP 0.1 bar
	SteeringWheel.canMsg4.S.brakePressure2 = (uint16)(SteeringWheel_public.data.brakePressure2*10);	//FP 0.1 bar
	SteeringWheel.canMsg4.S.steeringAngle_raw = (uint16)(SteeringWheel_public.data.sta_raw*10);		//FP 0.1 deg
	SteeringWheel.canMsg4.S.steeringAngle = (uint16)(SteeringWheel_public.data.sta*10);				//FP 0.1 deg

	SteeringWheel.canMsg5.S.wssFL = (uint16)(SteeringWheel_public.data.wssFL*10);	//FP 0.1 m/s
	SteeringWheel.canMsg5.S.wssFR = (uint16)(SteeringWheel_public.data.wssFR*10);	//FP 0.1 m/s
	SteeringWheel.canMsg5.S.wssRL = (uint16)(SteeringWheel_public.data.wssRL*10);	//FP 0.1 m/s
	SteeringWheel.canMsg5.S.wssRR = (uint16)(SteeringWheel_public.data.wssRR*10);	//FP 0.1 m/s

	SteeringWheel.canMsg6.S.torqueFL = (uint16)(SteeringWheel_public.data.torqueFL*10);	//FP 0.1 percent
	SteeringWheel.canMsg6.S.torqueFR = (uint16)(SteeringWheel_public.data.torqueFR*10);	//FP 0.1 percent
	SteeringWheel.canMsg6.S.torqueRL = (uint16)(SteeringWheel_public.data.torqueRL*10);	//FP 0.1 percent
	SteeringWheel.canMsg6.S.torqueRR = (uint16)(SteeringWheel_public.data.torqueRR*10);	//FP 0.1 percent

	/* Set the messages */
	CanCommunication_setMessageData(SteeringWheel.canMsg1.U[0], SteeringWheel.canMsg1.U[1], &SteeringWheel.msgObj1);
	CanCommunication_setMessageData(SteeringWheel.canMsg2.U[0], SteeringWheel.canMsg2.U[1], &SteeringWheel.msgObj2);
	CanCommunication_setMessageData(SteeringWheel.canMsg3.U, 0, &SteeringWheel.msgObj3);
	CanCommunication_setMessageData(SteeringWheel.canMsg4.U[0], SteeringWheel.canMsg4.U[1], &SteeringWheel.msgObj4);
	CanCommunication_setMessageData(SteeringWheel.canMsg5.U[0], SteeringWheel.canMsg5.U[1], &SteeringWheel.msgObj5);
	CanCommunication_setMessageData(SteeringWheel.canMsg6.U[0], SteeringWheel.canMsg6.U[1], &SteeringWheel.msgObj6);

	/* Transmit the messages */
	CanCommunication_transmitMessage(&SteeringWheel.msgObj1);
	CanCommunication_transmitMessage(&SteeringWheel.msgObj2);
	CanCommunication_transmitMessage(&SteeringWheel.msgObj3);
	CanCommunication_transmitMessage(&SteeringWheel.msgObj4);
	CanCommunication_transmitMessage(&SteeringWheel.msgObj5);
	CanCommunication_transmitMessage(&SteeringWheel.msgObj6);
}