/* 
 * CanCommunication.h
 * Created on 2019. 10. 24.
 * Author: Dua
 * 
 * For RH26_Control_main
 */

#ifndef CANCOMMUNICATION_H_
#define CANCOMMUNICATION_H_


/* Includes */
#include <Ifx_Types.h>
#include <IfxMultican.h>
#include <IfxMultican_Can.h>
#include "Configuration.h"



/* DataStructures */
typedef struct
{
	IfxMultican_Can_Node* 	node;
	IfxMultican_Can_MsgObj	obj;
	IfxMultican_Message		msg;
	boolean					isUpdated;
	struct 
	{
		uint32 Success;
		uint32 Failed;
	}testCount;
}CanCommunication_Message;

typedef struct 
{
	IfxMultican_Can_Node* node;
	uint32 messageId;
	IfxMultican_Frame frameType;
	IfxMultican_DataLengthCode dataLen;
}CanCommunication_Message_Config;


/* Global Variables */
IFX_EXTERN IfxMultican_Can_Node	CanCommunication_canNode0;
IFX_EXTERN IfxMultican_Can_Node	CanCommunication_canNode1;
IFX_EXTERN IfxMultican_Can_Node	CanCommunication_canNode2;
// IFX_EXTERN CanCommunication_Message CanCommunication_message0;


/* Function Prototypes */
IFX_EXTERN void CanCommunication_init(void);

IFX_EXTERN void CanCommunication_initMessage(CanCommunication_Message* ccMsg, CanCommunication_Message_Config* config);

IFX_EXTERN boolean CanCommunication_receiveMessage(CanCommunication_Message* msg);
IFX_EXTERN void CanCommunication_resetUpdateState(CanCommunication_Message* msg);

IFX_EXTERN void CanCommunication_transmitMessage(CanCommunication_Message *msg);
IFX_EXTERN void CanCommunication_setMessageData(uint32 data0, uint32 data1, CanCommunication_Message *msg);



#endif