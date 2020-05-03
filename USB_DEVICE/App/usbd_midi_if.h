/*
 * usbd_midi_if.h
 *
 *  Created on: May 3, 2020
 *      Author: vincent
 */

#ifndef APP_USBD_MIDI_IF_H_
#define APP_USBD_MIDI_IF_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "usbd_midi.h"

extern USBD_MIDI_ItfTypeDef USBD_Interface_fops_FS;


 uint8_t MIDI_Transmit(uint8_t* buff, uint8_t length);
 uint8_t MIDI_Receive(uint8_t* buff, uint8_t length);

#ifdef __cplusplus
}
#endif
#endif /* APP_USBD_MIDI_IF_H_ */
