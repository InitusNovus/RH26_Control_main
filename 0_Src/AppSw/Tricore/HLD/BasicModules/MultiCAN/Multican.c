/*
 * Multican.c
 *
 *  Created on: 2018. 8. 4.
 *      Author: bigbi_000
 */

//FIXME Separate communication layer
/******************************************************************************/
/*----------------------------------Includes----------------------------------*/
/******************************************************************************/
#include "HLD.h"
#include "Multican.h"

#include <stdio.h>

#include "IfxMultican_PinMap.h"
#include "Platform_Types.h"
#include "IfxMultican.h"
#include "IfxCan_reg.h"

/******************************************************************************/
/*-----------------------------------Macros-----------------------------------*/
/******************************************************************************/

/******************************************************************************/
/*--------------------------------Enumerations--------------------------------*/
/******************************************************************************/

/******************************************************************************/
/*-----------------------------Data Structures--------------------------------*/
/******************************************************************************/

/******************************************************************************/
/*------------------------------Global variables------------------------------*/
/******************************************************************************/

Multican_t g_Multican;

/******************************************************************************/
/*-------------------------Function Prototypes--------------------------------*/
/******************************************************************************/
IFX_STATIC void HLD_Multican_can0Init(void);
#if HLD_BOARD == HLD_BOARD_AK237
IFX_STATIC void HLD_Multican_can1Init(void);
#endif
/******************************************************************************/
/*------------------------Private Variables/Constants-------------------------*/
/******************************************************************************/

typedef struct{
	sint8					nodeId;
	IfxMultican_Rxd_In		*rxPin;
	IfxMultican_Txd_Out		*txPin;
	uint32					baudrate;
}nodeConfig_t;

/******************************************************************************/
/*-------------------------Function Implementations---------------------------*/
/******************************************************************************/


IFX_STATIC void HLD_Multican_can0Init(void)
{
	IfxMultican_Can_Config canConfig;
	IfxMultican_Can_initModuleConfig(&canConfig, &MODULE_CAN);

	IfxMultican_Can_initModule(&g_Multican.drivers.can0.can, &canConfig);
}


#if HLD_BOARD == HLD_BOARD_AK237
IFX_STATIC void HLD_Multican_can1Init()
{
	IfxMultican_Can_Config canConfig;
	IfxMultican_Can_initModuleConfig(&canConfig, &MODULE_CAN1);

	IfxMultican_Can_initModule(&g_Multican.drivers.can1.can, &canConfig);
}
#endif


IFX_STATIC void HLD_Multican_setNodeConfig(HLD_MultiCan_Can_Node *Node, nodeConfig_t config)
{
	Node->baudrate 	= config.baudrate;
	Node->nodeId 	= config.nodeId;
	Node->rxPin 	= config.rxPin;
	Node->txPin 	= config.txPin;
}


IFX_STATIC void HLD_Multican_initNode(HLD_MultiCan_Can_Node *Node, IfxMultican_Can *mcan)
{
	IfxMultican_Can_NodeConfig canNodeConfig;
	IfxMultican_Can_Node_initConfig(&canNodeConfig, mcan);
	{
		canNodeConfig.baudrate 	= Node->baudrate;
		switch (Node->nodeId){
		case 0:
			canNodeConfig.nodeId	 = (IfxMultican_NodeId)((int)IfxMultican_NodeId_0);
			break;
		case 1:
			canNodeConfig.nodeId	 = (IfxMultican_NodeId)((int)IfxMultican_NodeId_1);
			break;
		case 2:
			canNodeConfig.nodeId	 = (IfxMultican_NodeId)((int)IfxMultican_NodeId_2);
			break;
#if HLD_BOARD == HLD_BOARD_SB275
		case 3:
			canNodeConfig.nodeId	 = (IfxMultican_NodeId)((int)IfxMultican_NodeId_3);
			break;
#endif
		}
		canNodeConfig.rxPin 	 = Node->rxPin;
		canNodeConfig.rxPinMode  = IfxPort_InputMode_pullUp;
		canNodeConfig.txPin      = Node->txPin;
		canNodeConfig.txPinMode  = IfxPort_OutputMode_pushPull;
	}
	IfxMultican_Can_Node_init(&Node->ifxCanNode,&canNodeConfig);
}


void HLD_Multican_setMsgConfig(HLD_MultiCan_Message *msg, HLD_Multican_msgConfig config)
{
	msg->canType 	= config.canType;
	msg->canId		= config.canId;
	msg->nodeId		= config.nodeId;
	msg->msgId		= config.msgId;
}


void HLD_Multican_initMessage(HLD_MultiCan_Message *msg)
{
	IfxMultican_Can_MsgObjConfig canMsgObjConfig;
#if HLD_BOARD == HLD_BOARD_AK237
	if(msg->canId == 0)
#endif
	{
		switch (msg->nodeId)
		{
		case 0:
			IfxMultican_Can_MsgObj_initConfig(&canMsgObjConfig, &g_Multican.drivers.can0.node0.ifxCanNode);
			break;
		case 1:
			IfxMultican_Can_MsgObj_initConfig(&canMsgObjConfig, &g_Multican.drivers.can0.node1.ifxCanNode);
			break;
		case 2:
			IfxMultican_Can_MsgObj_initConfig(&canMsgObjConfig, &g_Multican.drivers.can0.node2.ifxCanNode);
			break;
#if HLD_BOARD == HLD_BOARD_SB275
		case 3:
			IfxMultican_Can_MsgObj_initConfig(&canMsgObjConfig, &g_Multican.drivers.can0.node3.ifxCanNode);
			break;
#endif
		}
		canMsgObjConfig.msgObjId		= g_Multican.drivers.can0.msgCnt;
		g_Multican.drivers.can0.msgCnt++;

	}
#if HLD_BOARD == HLD_BOARD_AK237
	else
	{
		switch (msg->nodeId){
		case 0:
			IfxMultican_Can_MsgObj_initConfig(&canMsgObjConfig, &g_Multican.drivers.can1.node0.ifxCanNode);
			break;
		case 1:
			IfxMultican_Can_MsgObj_initConfig(&canMsgObjConfig, &g_Multican.drivers.can1.node1.ifxCanNode);
			break;
		case 2:
			IfxMultican_Can_MsgObj_initConfig(&canMsgObjConfig, &g_Multican.drivers.can1.node2.ifxCanNode);
			break;
		}
		canMsgObjConfig.msgObjId		= g_Multican.drivers.can1.msgCnt;
		g_Multican.drivers.can1.msgCnt++;

	}
#endif
	msg->msgObjId = canMsgObjConfig.msgObjId;
	canMsgObjConfig.messageId		= msg->msgId;
	canMsgObjConfig.acceptanceMask  = 0x7FFFFFFFUL;
	if(msg->canType == CANTYPERX)
	{canMsgObjConfig.frame = IfxMultican_Frame_receive;}
	else
	{canMsgObjConfig.frame = IfxMultican_Frame_transmit;}
	canMsgObjConfig.control.messageLen    = IfxMultican_DataLengthCode_8;
	canMsgObjConfig.control.extendedFrame = TRUE;
	canMsgObjConfig.control.matchingId    = TRUE;

	IfxMultican_Can_MsgObj_init(&msg->msgObj, &canMsgObjConfig);
	IfxMultican_Message_init(&msg->message, msg->msgId, 0xdeadbeef, 0xdeadbeef, IfxMultican_DataLengthCode_8);
}



void HLD_Multican_init (void)
{
	HLD_Multican_can0Init();
#if HLD_BOARD == HLD_BOARD_AK237
	HLD_Multican_can1Init();
#endif
	nodeConfig_t nodeConfig;

	nodeConfig.nodeId = 0;
	nodeConfig.baudrate = 1000000;
	nodeConfig.rxPin =  &CAN0NODE0IN;
	nodeConfig.txPin = &CAN0NODE0OUT;
	HLD_Multican_setNodeConfig(&g_Multican.drivers.can0.node0, nodeConfig);
	HLD_Multican_initNode(&g_Multican.drivers.can0.node0,&g_Multican.drivers.can0.can);

	nodeConfig.nodeId = 1;
	nodeConfig.baudrate = 1000000;
	nodeConfig.rxPin =  &CAN0NODE1IN;
	nodeConfig.txPin = &CAN0NODE1OUT;
	HLD_Multican_setNodeConfig(&g_Multican.drivers.can0.node1, nodeConfig);
	HLD_Multican_initNode(&g_Multican.drivers.can0.node1,&g_Multican.drivers.can0.can);

	nodeConfig.nodeId = 2;
	nodeConfig.baudrate = 1000000;
	nodeConfig.rxPin =  &CAN0NODE2IN;
	nodeConfig.txPin = 	&CAN0NODE2OUT;
	HLD_Multican_setNodeConfig(&g_Multican.drivers.can0.node2, nodeConfig);
	HLD_Multican_initNode(&g_Multican.drivers.can0.node2,&g_Multican.drivers.can0.can);
#if  ((HLD_BOARD == HLD_BOARD_SB275) && (HLD_FOURTH_CANNODE == TRUE))
	nodeConfig.nodeId = 3;
	nodeConfig.baudrate = 1000000;
	nodeConfig.rxPin =  &CAN0NODE3IN;
	nodeConfig.txPin = 	&CAN0NODE3OUT;
	HLD_Multican_setNodeConfig(&g_Multican.drivers.can0.node3, nodeConfig);
	HLD_Multican_initNode(&g_Multican.drivers.can0.node3,&g_Multican.drivers.can0.can);
#endif
#if HLD_BOARD == HLD_BOARD_AK237
	nodeConfig.nodeId = 0;
	nodeConfig.baudrate = 1000000;
	nodeConfig.rxPin =  &CAN1NODE0IN;
	nodeConfig.txPin = &CAN1NODE0OUT;
	HLD_Multican_setNodeConfig(&g_Multican.drivers.can1.node0, nodeConfig);
	HLD_Multican_initNode(&g_Multican.drivers.can1.node0,&g_Multican.drivers.can1.can);

	nodeConfig.nodeId = 1;
	nodeConfig.baudrate = 1000000;
	nodeConfig.rxPin =  &CAN1NODE1IN;
	nodeConfig.txPin = &CAN1NODE1OUT;
	HLD_Multican_setNodeConfig(&g_Multican.drivers.can1.node1, nodeConfig);
	HLD_Multican_initNode(&g_Multican.drivers.can1.node1,&g_Multican.drivers.can1.can);

	nodeConfig.nodeId = 2;
	nodeConfig.baudrate = 1000000;
	nodeConfig.rxPin =  &CAN1NODE2IN;
	nodeConfig.txPin = &CAN1NODE2OUT;
	HLD_Multican_setNodeConfig(&g_Multican.drivers.can1.node2, nodeConfig);
	HLD_Multican_initNode(&g_Multican.drivers.can1.node2,&g_Multican.drivers.can1.can);
#endif
}


void HLD_Multican_receiveMessage(HLD_MultiCan_Message *msg)
{
	IfxMultican_Status  readStatus;
	if(IfxMultican_Can_MsgObj_isRxPending(&msg->msgObj))
	{
		//IfxMultican_Message_init(&msg->message, 0xdead, 0xdeadbeef, 0xdeadbeef, IfxMultican_DataLengthCode_8);	//Dummy msg.

		readStatus = IfxMultican_Can_MsgObj_readMessage(&msg->msgObj, &msg->message);

		/* if no new data is been received report an error */
		if (!(readStatus & IfxMultican_Status_newData))
		{
			printf("ERROR: IfxMultican_Can_MsgObj_readMessage for canMsgObj00 returned 0x%04x\n", readStatus);
		}

		/* if a new data is been received but one lost, report the status */
		if (readStatus == IfxMultican_Status_newDataButOneLost)
		{
			printf(" IfxMultican_Can_MsgObj_readMessage for canMsgObj00 returned 0x%04x\n", readStatus);
		}

		if (readStatus == IfxMultican_Status_newData)
		{
			msg->isUpdated = TRUE;
		}

	}
}


void HLD_Multican_transmitMessage(HLD_MultiCan_Message *msg)
{
/*
	if((IfxMultican_Can_MsgObj_sendMessage(&msg->msgObj, &msg->message)) == IfxMultican_Status_ok)
	{
		msg->isUpdated = TRUE;
	}
*/

	while(IfxMultican_Can_MsgObj_sendMessage(&msg->msgObj, &msg->message) == IfxMultican_Status_notSentBusy );
	msg->isUpdated = TRUE;
}
