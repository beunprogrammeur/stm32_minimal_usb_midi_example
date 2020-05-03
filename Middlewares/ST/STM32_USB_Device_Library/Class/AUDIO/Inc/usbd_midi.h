/*
 * usbd_midi.h
 *
 *  Created on: May 3, 2020
 *      Author: vincent
 */

#ifndef ST_STM32_USB_DEVICE_LIBRARY_CLASS_AUDIO_INC_USBD_MIDI_H_
#define ST_STM32_USB_DEVICE_LIBRARY_CLASS_AUDIO_INC_USBD_MIDI_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "usbd_ioreq.h"


typedef struct _USBD_MIDI_Itf
{
	int8_t (* Transmit)(uint8_t* buff, uint8_t length);
	int8_t (* Receive)(uint8_t* buff, uint8_t length);
} USBD_MIDI_ItfTypeDef;



extern USBD_ClassTypeDef USBD_MIDI;
#define USBD_MIDI_CLASS &USBD_MIDI


uint8_t USBD_MIDI_RegisterInterface(USBD_HandleTypeDef *pdev, USBD_MIDI_ItfTypeDef *fops);
/* used internally */
uint8_t MIDI_TransmitData(uint8_t* buff, uint8_t length);
#ifdef __cplusplus
}
#endif

#endif /* ST_STM32_USB_DEVICE_LIBRARY_CLASS_AUDIO_INC_USBD_MIDI_H_ */
