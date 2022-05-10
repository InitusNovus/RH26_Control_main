/*
 * kelly8080ips_can.c
 *
 *  Created on: 2018. 8. 8.
 *      Author: bigbi_000
 */


/*----------------------------------Includes----------------------------------*/
#include "HLD.h"
#include "kelly8080ips_can.h"

/*-----------------------------------Macros-----------------------------------*/
const unsigned conMsgId1 = 0x0CF11E05;
const unsigned conMsgId2 = 0x0CF11F05;

const unsigned conMsgId3 = 0x00102F00UL;
const unsigned conMsgId4 = 0x00102F01UL;
/*--------------------------------Enumerations--------------------------------*/

/*-----------------------------Data Structures--------------------------------*/

/*------------------------------Global variables------------------------------*/
kelly8080ips_t kelly8080ips1;
kelly8080ips_t kelly8080ips2;

/*-------------------------Function Prototypes--------------------------------*/
IFX_STATIC void kelly8080ips_canMessage_init(kelly8080ips_t *inverter, IfxMultican_Can_Node *node);
IFX_STATIC void kelly8080ips_receiveMessage(kelly8080ips_t *inverter);
IFX_STATIC void kelly8080ips_broadcasting(kelly8080ips_t *inverter);

/*-------------------------Function Implementations---------------------------*/
void kelly8080ips_can_init(void)
{
	kelly8080ips_canMessage_init(&kelly8080ips1, &CanCommunication_canNode1);
	kelly8080ips_canMessage_init(&kelly8080ips2, &CanCommunication_canNode2);
}

void kelly8080ips_can_run_1ms_c2(void)
{
	kelly8080ips_receiveMessage(&kelly8080ips1);
	kelly8080ips_receiveMessage(&kelly8080ips2);
}

void kelly8080ips_can_run_xms_c2 (void)
{
	kelly8080ips_broadcasting(&kelly8080ips1);
	kelly8080ips_broadcasting(&kelly8080ips2);
}

/*---------------------Private Function Implementations-----------------------*/
IFX_STATIC void kelly8080ips_canMessage_init(kelly8080ips_t *inverter, IfxMultican_Can_Node *node)
{
	{
        CanCommunication_Message_Config config;
        config.messageId		=	conMsgId1;
        config.frameType		=	IfxMultican_Frame_receive;
        config.dataLen			=	IfxMultican_DataLengthCode_8;
        config.node				=	node;
        CanCommunication_initMessage(&inverter->canMsgObj00, &config);
	}
	{
        CanCommunication_Message_Config config;
        config.messageId		=	conMsgId2;
        config.frameType		=	IfxMultican_Frame_receive;
        config.dataLen			=	IfxMultican_DataLengthCode_8;
        config.node				=	node;
		CanCommunication_initMessage(&inverter->canMsgObj01 , &config);
	}
	{
		CanCommunication_Message_Config config;
		config.messageId 		=	conMsgId3;
		config.frameType		=	IfxMultican_Frame_transmit;
		config.dataLen			=	IfxMultican_DataLengthCode_8;
		config.node				=	&CanCommunication_canNode0;
		CanCommunication_initMessage(&inverter->canMsgObj02, &config);
	}
	{
		CanCommunication_Message_Config config;
		config.messageId 		=	conMsgId4;
		config.frameType		=	IfxMultican_Frame_transmit;
		config.dataLen			=	IfxMultican_DataLengthCode_8;
		config.node				=	&CanCommunication_canNode0;
		CanCommunication_initMessage(&inverter->canMsgObj03, &config);
	}
}

IFX_STATIC void kelly8080ips_receiveMessage(kelly8080ips_t *inverter)
{
	if(CanCommunication_receiveMessage(&inverter->canMsgObj00))
	{
		inverter->msg1.rpm = ((inverter->canMsgObj00.msg.data[0] & (0x0000FFFF)) >> 0);				//1rpm/bit
		inverter->msg1.motCurrent = ((inverter->canMsgObj00.msg.data[0] & (0xFFFF0000)) >> 16);		//0.1A/bit
		inverter->msg1.batVoltage = ((inverter->canMsgObj00.msg.data[1] & (0x0000FFFF)) >> 0);		//0.1V/bit
		inverter->msg1.error.U = ((inverter->canMsgObj00.msg.data[1] & (0xFFFF0000)) >> 16);
	}
	if(CanCommunication_receiveMessage(&inverter->canMsgObj01))
	{
		inverter->msg2.tps = ((inverter->canMsgObj01.msg.data[0]&(0x000000FF))	>>	0);					//0~255
		inverter->msg2.conTemp = ((sint8)(((sint16)((inverter->canMsgObj01.msg.data[0] & (0x0000FF00)) >> 8)) - 40));	//1degC/bit
		inverter->msg2.motTemp = ((sint8)(((sint16)((inverter->canMsgObj01.msg.data[0] & (0x00FF0000)) >> 16)) - 30));	//1degC/bit
		inverter->msg2.conStat.U = (((inverter->canMsgObj01.msg.data[1] & (0x000000FF)) >> 0));
		inverter->msg2.swStat.U = (((inverter->canMsgObj01.msg.data[1] & (0x0000FF00)) >> 8));
	}
}

IFX_STATIC void kelly8080ips_broadcasting(kelly8080ips_t *inverter)
{
	inverter->canMsgObj02.msg.data[0] = inverter->canMsgObj00.msg.data[0];
	inverter->canMsgObj02.msg.data[1] = inverter->canMsgObj00.msg.data[1];
	inverter->canMsgObj03.msg.data[0] = inverter->canMsgObj01.msg.data[0];
	inverter->canMsgObj03.msg.data[1] = inverter->canMsgObj01.msg.data[1];

	CanCommunication_transmitMessage(&inverter->canMsgObj02);
	CanCommunication_transmitMessage(&inverter->canMsgObj03);
}