/*
 * usbd_midi.c
 *
 *  Created on: May 3, 2020
 *      Author: vincent
 */

#include "usbd_midi.h"
#include "usbd_ctlreq.h"


static uint8_t USBD_MIDI_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
static uint8_t USBD_MIDI_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx);
// static uint8_t USBD_MIDI_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req);
static uint8_t USBD_MIDI_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum);
static uint8_t USBD_MIDI_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum);
// static uint8_t USBD_MIDI_EP0_RxReady(USBD_HandleTypeDef *pdev);
static uint8_t *USBD_MIDI_GetFSCfgDesc(uint16_t *length);
// static uint8_t *USBD_MIDI_GetHSCfgDesc(uint16_t *length);
// static uint8_t *USBD_MIDI_GetOtherSpeedCfgDesc(uint16_t * length);
// uint8_t *USBD_MIDI_GetDeviceQualifierDescriptor(uint16_t *length);


USBD_ClassTypeDef USBD_MIDI =
{
		USBD_MIDI_Init,
		USBD_MIDI_DeInit,
		NULL, //USBD_MIDI_Setup,
		NULL,
		NULL, //USBD_MIDI_EP0_RxReady,
		USBD_MIDI_DataIn,
		USBD_MIDI_DataOut,
		NULL,
		NULL,
		NULL,
		NULL, //USBD_MIDI_GetHSCfgDesc,
		USBD_MIDI_GetFSCfgDesc,
		NULL, //USBD_MIDI_GetOtherSpeedCfgDesc,
		NULL, //USBD_MIDI_GetDeviceQualifierDescriptor
};

//#define USB_MIDI_CONFIG_DESC_SIZ (67)
//#define MIDI_CMD_EP (0x82U)
//#define MIDI_CMD_PACKET_SIZE (8U)
//#define MIDI_FS_BINTERVAL (0x10U)
//#define MIDI_HS_BINTERVAL (0x10U)
//#define MIDI_DATA_HS_MAX_PACKET_SIZE (512U)

#define MIDI_IN_EP  (0x81)
#define MIDI_OUT_EP (0x01)
#define MIDI_DATA_FS_MAX_PACKET_SIZE (64U)

__ALIGN_BEGIN uint8_t USB_RX_BUFF[MIDI_DATA_FS_MAX_PACKET_SIZE] __ALIGN_END;
__ALIGN_BEGIN uint8_t USB_TX_BUFF[MIDI_DATA_FS_MAX_PACKET_SIZE] __ALIGN_END;
USBD_HandleTypeDef* _handle = NULL;

__ALIGN_BEGIN uint8_t USBD_MIDI_CfgFSDesc[] __ALIGN_END =
{
  /*Configuration Descriptor*/
  0x09,   /* bLength: Configuration Descriptor size */
  0x02,   /* bDescriptorType: Configuration */
  LOBYTE(0x0044),   /* wTotalLength: 2nd byte */
  HIBYTE(0x0044),   /* wTotalLength: add total array size here (and on the 2nd byte if needed) */
  0x02,   /* bNumInterfaces: 2 interface */
  0x01,   /* bConfigurationValue: Configuration value */
  0x00,   /* iConfiguration: Index of string descriptor describing the configuration */ /* unused */
  0x80,   /* bmAttributes: self powered */
  0x32,   /* MaxPower 0 mA */

	  /*Interface Descriptor */
	  0x09,   /* bLength: Interface Descriptor size */
	  0x04,  /* bDescriptorType: INTERFACE */
	  0x00,   /* bInterfaceNumber: Number of Interface */
	  0x00,   /* bAlternateSetting: Alternate setting */
	  0x02,   /* bNumEndpoints: MIDI in AND out */
	  0x01,   /* bInterfaceClass: AUDIO */
	  0x03,   /* bInterfaceSubClass: MIDI Streaming */
	  0x00,   /* bInterfaceProtocol: not used*/
	  0x00,   /* iInterface: not used*/

		  /* MIDIStreaming Interface Descriptor */
		  0x07,   /* bLength: Interface Descriptor size */
		  0x24,   /* bDescriptorType: CS_INTERFACE */
		  0x01,   /* bDescriptorSubtype: MS_HEADER */
		  LOBYTE(0x0100),   /* bcdADC: Revision of this class specification */
		  HIBYTE(0x0100),   /* cdADC: 2nd byte */
		  LOBYTE(0x0032),   /* wTotalLength: count size of all sub descriptors*/
		  HIBYTE(0x0032),   /* wTotalLength: count size of all sub descriptors*/

		  	  /* MIDIStreaming Interface Descriptor */
		  	  0x06, /* bLength: Interface Descriptor size */
			  0x24, /* bDescriptorType: CS_INTERFACE */
			  0x02, /* bDescriptorSubType: MIDI_IN_JACK */
			  0x01, /* bJackType: embedded */
			  0x01, /* bJackId: first jack (of 2) */
			  0x00, /* iJack: unused*/

			  /* MIDIStreaming Interface Descriptor*/
			  0x09, /* bLength: Interface Descriptor size */
			  0x24, /* bDescriptorType: CS_INTERFACE */
			  0x03, /* bDescriptorSubType: MIDI_OUT_JACK */
			  0x01, /* bJackType: Embedded */
			  0x02, /* bJackId: second jack (of 2) */
			  0x01, /* bNrInputPins: Number of Input Pins of this Jack */
			  0x02, /* BaSourceID(1): ID of the Entity to which this Pin is connected */
			  0x01, /* BaSourcePin(1): Output Pin number of the Entity to which this Input Pin is connected */
			  0x00, /* iJack: unused */

			  /* Endpoint Descriptor */
			  0x09, /* bLength: Endpoint Descriptor size */
			  0x05, /* bDescriptorType: ENDPOINT */
			  MIDI_OUT_EP, /* bEndpointAddress: OUT endpoint 1 */
			  0x02, /* bmAttributes: BULK, not shared */
			  LOBYTE(MIDI_DATA_FS_MAX_PACKET_SIZE), /* wMaxPacketSize: 64 bytes per packet */
			  HIBYTE(MIDI_DATA_FS_MAX_PACKET_SIZE), /* wMaxPacketSize: */
			  0x00, /* bInterval: ignored for BULK */
			  0x00, /* bRefresh: unused */
			  0x00, /* bSynchAddress: unused */

			  	  /* MIDIStreaming Endpoint Descriptor */
			  	  0x05, /* bLength: MIDIStreaming Endpoint Descriptor size */
				  0x25, /* bDescriptorType: CS_ENDPOINT */
				  0x01, /* bDescriptorSubType: MS_GENERAL */
				  0x01, /* bNumEmbMIDIJack: number of embedded MIDI-IN jacks */
				  0x01, /* baAssocJackID(1): ID of the Embedded MIDI-IN jack */

			  /* Endpoint Descriptor */
			  0x09, /* bLength: Endpoint Descriptor size */
			  0x05, /* bDescriptorType: ENDPOINT */
			  MIDI_IN_EP, /* bEndpointAddress: IN endpoint 1 */
			  0x02, /* bmAttributes: BULK, not shared */
			  LOBYTE(MIDI_DATA_FS_MAX_PACKET_SIZE), /* wMaxPacketSize: 64 bytes */
			  HIBYTE(MIDI_DATA_FS_MAX_PACKET_SIZE), /* wMaxPacketSize: 64 bytes */
			  0x00, /* bInterval: unused */
			  0x00, /* bRefresh: unused */
			  0x00, /* bSynchAddress: unused */
			  	  /* MIDIStreaming Endpoint Descriptor */
			  	  0x05, /* bLength: MIDIStreaming Endpoint Descriptor size */
				  0x25, /* bDescriptorType: CS_ENDPOINT */
				  0x01, /* bDescriptorSubType: MS_GENERAL */
				  0x01, /* bNumEmbMIDIJack: number of embedded MIDI-OUT jacks */
				  0x02, /* BaAssocJackID(1): ID of the embedded MIDI-OUT jack */
};

uint8_t USBD_MIDI_RegisterInterface(USBD_HandleTypeDef *pdev, USBD_MIDI_ItfTypeDef *fops)
{

	uint8_t ret = USBD_FAIL;

	if(fops != NULL)
	{
		pdev->pUserData = fops;
		ret = USBD_OK;
	}

	return ret;
}

static uint8_t USBD_MIDI_Init(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
	_handle = pdev;
	USBD_LL_OpenEP(pdev, MIDI_IN_EP, USBD_EP_TYPE_BULK, MIDI_DATA_FS_MAX_PACKET_SIZE);
	USBD_LL_OpenEP(pdev, MIDI_OUT_EP, USBD_EP_TYPE_BULK, MIDI_DATA_FS_MAX_PACKET_SIZE);
	USBD_LL_PrepareReceive(pdev, MIDI_OUT_EP, (uint8_t*)USB_RX_BUFF, MIDI_DATA_FS_MAX_PACKET_SIZE);
	return 0;
}
static uint8_t USBD_MIDI_DeInit(USBD_HandleTypeDef *pdev, uint8_t cfgidx)
{
	_handle = NULL;
	USBD_LL_CloseEP(pdev, MIDI_IN_EP);
	USBD_LL_CloseEP(pdev, MIDI_OUT_EP);
	return 0;
}

/* In the PC, so outbound from the microcontroller */
static uint8_t USBD_MIDI_DataIn(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
	return USBD_OK;
}

/* Out the PC, so inbound towards the microcontroller */
static uint8_t USBD_MIDI_DataOut(USBD_HandleTypeDef *pdev, uint8_t epnum)
{
	uint8_t length = ((PCD_HandleTypeDef*)pdev->pData)->OUT_ep[epnum].xfer_count;
	USBD_MIDI_ItfTypeDef* interface = (USBD_MIDI_ItfTypeDef *)(pdev->pUserData);
	interface->Receive(USB_RX_BUFF, length);

	USBD_LL_PrepareReceive(pdev, epnum, USB_RX_BUFF, MIDI_DATA_FS_MAX_PACKET_SIZE);
	return USBD_OK;
}

static uint8_t *USBD_MIDI_GetFSCfgDesc(uint16_t *length)
{
	*length = sizeof(USBD_MIDI_CfgFSDesc);
	return USBD_MIDI_CfgFSDesc;
}

uint8_t MIDI_TransmitData(uint8_t* buff, uint8_t length)
{
	if(_handle == NULL)
	{
		return;
	}

	int i = 0;
	for(i = 0; i < length; i++)
	{
		USB_TX_BUFF[i] = buff[i];
	}

	for(; i < MIDI_DATA_FS_MAX_PACKET_SIZE; i++)
	{
		USB_TX_BUFF[i] = 0;
	}


	while(USBD_LL_Transmit(_handle, MIDI_IN_EP, USB_TX_BUFF, MIDI_DATA_FS_MAX_PACKET_SIZE) != USBD_OK);
	return USBD_OK;
}

//static uint8_t USBD_MIDI_Setup(USBD_HandleTypeDef *pdev, USBD_SetupReqTypedef *req)
//{
//	return 0;
//}

//static uint8_t USBD_MIDI_EP0_RxReady(USBD_HandleTypeDef *pdev)
//{
//	return 0;
//}
//static uint8_t *USBD_MIDI_GetHSCfgDesc(uint16_t *length)
//{
//	*length = sizeof(USBD_MIDI_CfgHSDesc);
//	return USBD_MIDI_CfgHSDesc;
//}
//static uint8_t *USBD_MIDI_GetOtherSpeedCfgDesc(uint16_t * length)
//{
//	*length = sizeof(USBD_MIDI_OtherSpeedCfgDesc);
//	return USBD_MIDI_OtherSpeedCfgDesc;
//}
//uint8_t *USBD_MIDI_GetDeviceQualifierDescriptor(uint16_t *length)
//{
//	*length = sizeof(USBD_MIDI_DeviceQualifierDesc);
//	return USBD_MIDI_DeviceQualifierDesc;
//}
