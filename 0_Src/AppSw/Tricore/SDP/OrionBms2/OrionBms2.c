/*
 * OrionBms2.c
 * Created on: 2020.08.04
 * Author: Dua
 */

/***************************** Includes ******************************/
#include <IfxCpu.h>

#include "OrionBms2.h"
#include "RVC.h"
/**************************** Macro **********************************/

/*********************** Global Variables ****************************/
const uint32 OrionMsgId1 = 0x00001F00UL;
const uint32 OrionMsgId2 = 0x00001F01UL;
const uint32 OrionMsgId3 = 0x00001F02UL;

OrionBms2_t OrionBms2;

/******************* Private Function Prototypes *********************/
IFX_STATIC void OrionBms2_receiveMessage(void);

/********************* Function Implementation ***********************/
void OrionBms2_init(void)
{
	{
		CanCommunication_Message_Config config;
		config.messageId = OrionMsgId1;
		config.frameType = IfxMultican_Frame_receive;
		config.dataLen = IfxMultican_DataLengthCode_6;
		config.node = &CanCommunication_canNode0;
		CanCommunication_initMessage(&OrionBms2.msgObj1, &config);
	}
	{
		CanCommunication_Message_Config config;
		config.messageId = OrionMsgId2;
		config.frameType = IfxMultican_Frame_receive;
		config.dataLen = IfxMultican_DataLengthCode_4;
		config.node = &CanCommunication_canNode0;
		CanCommunication_initMessage(&OrionBms2.msgObj2, &config);
	}
	{
		CanCommunication_Message_Config config;
		config.messageId = OrionMsgId3;
		config.frameType = IfxMultican_Frame_receive;
		config.dataLen = IfxMultican_DataLengthCode_6;
		config.node = &CanCommunication_canNode0;
		CanCommunication_initMessage(&OrionBms2.msgObj3, &config);
	}
}

void OrionBms2_run_1ms_c2(void)
{
	OrionBms2_receiveMessage();
}

/****************** Private Function Implementation ******************/

/* TODO:
 * CRC check
 * Update flag
 */
IFX_STATIC void OrionBms2_receiveMessage(void)
{
	uint32 updateFlag;
	if(CanCommunication_receiveMessage(&OrionBms2.msgObj1))
	{
		OrionBms2.msg1.packCurrent = ((OrionBms2.msgObj1.msg.data[0] & 0x0000FFFF) >> 0);
		OrionBms2.msg1.packVoltage = ((OrionBms2.msgObj1.msg.data[0] & 0xFFFF0000) >> 16);
		OrionBms2.msg1.packSoc = ((OrionBms2.msgObj1.msg.data[1] & 0x000000FF) >> 0);
		while(IfxCpu_acquireMutex(&RVC_public.bms.shared.mutex))
			; // Wait for mutex
		{
			RVC_public.bms.shared.data.current = (float32)OrionBms2.msg1.packCurrent / 10;
			RVC_public.bms.shared.data.voltage = (float32)OrionBms2.msg1.packVoltage / 10;
			RVC_public.bms.shared.data.soc = (float32)OrionBms2.msg1.packSoc / 2;
			// RVC_public.bms.shared.isUpdated = TRUE;
			IfxCpu_releaseMutex(&RVC_public.bms.shared.mutex);
		}
	}
	if(CanCommunication_receiveMessage(&OrionBms2.msgObj2))
	{
		OrionBms2.msg2.packChargeLimit = ((OrionBms2.msgObj2.msg.data[0] & 0x0000FFFF) >> 0);
		OrionBms2.msg2.packDischargeLimit = ((OrionBms2.msgObj2.msg.data[0] & 0xFFFF0000) >> 16);
		while(IfxCpu_acquireMutex(&RVC_public.bms.shared.mutex))
			; // Wait for mutex
		{
			RVC_public.bms.shared.data.chargeLimit = OrionBms2.msg2.packChargeLimit;
			RVC_public.bms.shared.data.dischargeLimit = OrionBms2.msg2.packDischargeLimit;
			// RVC_public.bms.shared.isUpdated = TRUE;
			IfxCpu_releaseMutex(&RVC_public.bms.shared.mutex);
		}
	}
	if(CanCommunication_receiveMessage(&OrionBms2.msgObj3))
	{
		OrionBms2.msg3.highTemp = ((OrionBms2.msgObj3.msg.data[0] & 0x000000FF) >> 0);
		OrionBms2.msg3.highCell = ((OrionBms2.msgObj3.msg.data[0] & 0x0000FF00) >> 8);
		OrionBms2.msg3.avgTemp = ((OrionBms2.msgObj3.msg.data[0] & 0x00FF0000) >> 16);
		OrionBms2.msg3.bmsTemp = ((OrionBms2.msgObj3.msg.data[0] & 0xFF000000) >> 24);
		OrionBms2.msg3.lowVoltage = ((OrionBms2.msgObj3.msg.data[1] & 0x0000FFFF) >> 0);
		while(IfxCpu_acquireMutex(&RVC_public.bms.shared.mutex))
			; // Wait for mutex
		{
			RVC_public.bms.shared.data.highestTemp = (sint8)OrionBms2.msg3.highTemp;
			RVC_public.bms.shared.data.averageTemp = (sint8)OrionBms2.msg3.avgTemp;
			RVC_public.bms.shared.data.bmsTemp = (sint8)OrionBms2.msg3.bmsTemp;
			RVC_public.bms.shared.data.lowestVoltage = (float32)OrionBms2.msg3.lowVoltage/10000;
			// RVC_public.bms.shared.isUpdated = TRUE;
			IfxCpu_releaseMutex(&RVC_public.bms.shared.mutex);
		}
	}
}
