#ifndef __HW_CONFIG_H
#define __HW_CONFIG_H

#include "stm32f10x.h"
#include "usb_type.h"

enum WorkState
{
	STANDALONE = 0,
	PC_MONITORING,
	PC_CONTROLS
};

void hw_init(void);
void Enter_LowPowerMode(void);
void Leave_LowPowerMode(void);
void USB_Cable_Config(FunctionalState NewState);
void Get_SerialNum(void);

void CUBECheckState(void);
void CUBECheckState2(void);

#endif /*__HW_CONFIG_H*/
