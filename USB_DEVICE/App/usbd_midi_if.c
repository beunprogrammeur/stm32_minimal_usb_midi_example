/*
 * usbd_midi_if.c
 *
 *  Created on: May 3, 2020
 *      Author: vincent
 */

#include "usbd_midi_if.h"
#include "usbd_midi.h"

//static int8_t MIDI_Init_FS(void);
//static int8_t MIDI_DeInit_FS(void);
//static int8_t MIDI_Transmit_FS(uint8_t* buff, uint8_t length);
//static int8_t MIDI_Receive_FS(uint8_t* buff, uint8_t length);

USBD_MIDI_ItfTypeDef USBD_Interface_fops_FS =
{
		//MIDI_Init_FS,
		//MIDI_DeInit_FS,
		MIDI_Transmit,
		MIDI_Receive,
};


uint8_t MIDI_Receive(uint8_t* buff, uint8_t length)
{
	// TODO: add your incoming message handling here

	return 0;
}

uint8_t MIDI_Transmit(uint8_t* buff, uint8_t length)
{
	return MIDI_TransmitData(buff, length);
}


//static int8_t MIDI_Init_FS(void)
//{
//	return 0;
//}
//
//static int8_t MIDI_DeInit_FS(void)
//{
//	return 0;
//}
//
//static int8_t MIDI_Transmit_FS(uint8_t* buff, uint8_t length)
//{
//	return 0;
//}
//
//static int8_t MIDI_Receive_FS(uint8_t* buff, uint8_t length)
//{
//	return 0;
//}
