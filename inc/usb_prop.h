#ifndef __USB_PROP_H
#define __USB_PROP_H

#include "usb_core.h"
#include <stdint.h>

typedef enum _HID_REQUESTS
{
	GET_REPORT = 1,
	GET_IDLE,
	GET_PROTOCOL,

	SET_REPORT = 9,
	SET_IDLE,
	SET_PROTOCOL
} HID_REQUESTS;

void HID_init(void);
void HID_Reset(void);
void HID_Status_In(void);
void HID_Status_Out(void);
RESULT HID_Data_Setup(uint8_t);
RESULT HID_NoData_Setup(uint8_t);
RESULT HID_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting);
uint8_t *HID_GetDeviceDescriptor(uint16_t);
uint8_t *HID_GetConfigDescriptor(uint16_t);
uint8_t *HID_GetStringDescriptor(uint16_t);
RESULT HID_SetProtocol(void);
uint8_t *HID_GetProtocolValue(uint16_t Length);
uint8_t *HID_GetReportDescriptor(uint16_t Length);
uint8_t *HID_GetHIDDescriptor(uint16_t Length);

void HID_GetConfiguration(void);
void HID_SetConfiguration(void);
void HID_GetInterface(void);
void HID_SetInterface(void);
void HID_GetStatus(void);
void HID_ClearFeature(void);
void HID_SetEndPointFeature(void);
void HID_SetDeviceFeature(void);
void HID_SetDeviceAddress(void);

#define REPORT_DESCRIPTOR 0x22

#endif /* __USB_PROP_H */
