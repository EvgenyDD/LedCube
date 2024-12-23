#include "usb_prop.h"
#include "hw_config.h"
#include "usb_conf.h"
#include "usb_desc.h"
#include "usb_lib.h"
#include "usb_pwr.h"

uint32_t ProtocolValue;
volatile uint8_t EXTI_Enable;
volatile uint8_t Request = 0;
uint8_t Report_Buf[wMaxPacketSize];

extern uint8_t Matrix[7];
extern uint8_t image[64];
extern uint8_t USBmode;

DEVICE Device_Table = {EP_NUM, 1};

DEVICE_PROP Device_Property = {
	HID_init, HID_Reset, HID_Status_In, HID_Status_Out, HID_Data_Setup,
	HID_NoData_Setup, HID_Get_Interface_Setting, HID_GetDeviceDescriptor,
	HID_GetConfigDescriptor, HID_GetStringDescriptor, 0, 0x40 /*MAX PACKET SIZE*/
};
USER_STANDARD_REQUESTS User_Standard_Requests = {
	HID_GetConfiguration, HID_SetConfiguration, HID_GetInterface,
	HID_SetInterface, HID_GetStatus, HID_ClearFeature,
	HID_SetEndPointFeature, HID_SetDeviceFeature, HID_SetDeviceAddress};

ONE_DESCRIPTOR Device_Descriptor = {(uint8_t *)CUBE_DeviceDescriptor, CUBE_SIZ_DEVICE_DESC};
ONE_DESCRIPTOR Config_Descriptor = {(uint8_t *)CUBE_ConfigDescriptor, CUBE_SIZ_CONFIG_DESC};
ONE_DESCRIPTOR CUBE_Report_Descriptor = {(uint8_t *)CUBE_ReportDescriptor, CUBE_SIZ_REPORT_DESC};
ONE_DESCRIPTOR CUBE_Hid_Descriptor = {(uint8_t *)CUBE_ConfigDescriptor + CUBE_OFF_HID_DESC, CUBE_SIZ_HID_DESC};

ONE_DESCRIPTOR String_Descriptor[4] = {
	{(uint8_t *)CUBE_StringLangID, CUBE_SIZ_STRING_LANGID},
	{(uint8_t *)CUBE_StringVendor, CUBE_SIZ_STRING_VENDOR},
	{(uint8_t *)CUBE_StringProduct, CUBE_SIZ_STRING_PRODUCT},
	{(uint8_t *)CUBE_StringSerial, CUBE_SIZ_STRING_SERIAL}};

uint8_t *HID_SetReport_Feature(uint16_t Length);

void HID_init(void)
{
	Get_SerialNum();
	pInformation->Current_Configuration = 0;
	PowerOn();
	USB_SIL_Init();
	bDeviceState = UNCONNECTED;
}

void HID_Reset(void)
{
	/* Set HID_DEVICE as not configured */
	pInformation->Current_Configuration = 0;
	pInformation->Current_Interface = 0; /*the default Interface*/

	/* Current Feature initialization */
	pInformation->Current_Feature = CUBE_ConfigDescriptor[7];
	SetBTABLE(BTABLE_ADDRESS);
	/* Initialize Endpoint 0 */
	SetEPType(ENDP0, EP_CONTROL);
	SetEPTxStatus(ENDP0, EP_TX_STALL);
	SetEPRxAddr(ENDP0, ENDP0_RXADDR);
	SetEPTxAddr(ENDP0, ENDP0_TXADDR);
	Clear_Status_Out(ENDP0);
	SetEPRxCount(ENDP0, Device_Property.MaxPacketSize);
	SetEPRxValid(ENDP0);

	/* Initialize Endpoint 1 */
	SetEPType(ENDP1, EP_INTERRUPT);
	SetEPTxAddr(ENDP1, ENDP1_TXADDR);
	SetEPRxAddr(ENDP1, ENDP1_RXADDR);
	SetEPTxCount(ENDP1, EP1TxCount);
	SetEPRxCount(ENDP1, EP1RxCount);
	SetEPRxStatus(ENDP1, EP_RX_VALID);
	SetEPTxStatus(ENDP1, EP_TX_NAK);

	/* Set this device to response on default address */
	SetDeviceAddress(0);
	bDeviceState = ATTACHED;
}

void HID_SetConfiguration(void)
{
	DEVICE_INFO *pInfo = &Device_Info;
	if(pInfo->Current_Configuration != 0) bDeviceState = CONFIGURED;
}

void HID_SetDeviceAddress(void)
{
	bDeviceState = ADDRESSED;
}

void HID_Status_In(void)
{
	Matrix[0] <<= 1;
	if(Matrix[0] >= 32) Matrix[0] = 1;

	switch(Report_Buf[0])
	{
	case 1:
		for(uint8_t i = 0; i < 32; i++)
			image[i] = Report_Buf[i + 1];
		Matrix[5]++;
		USBmode = PC_CONTROLS;
		break;

	case 2:
		for(uint8_t i = 0; i < 32; i++)
			image[i + 32] = Report_Buf[i + 1];
		Matrix[6]++;
		USBmode = PC_CONTROLS;
		break;

	case 3:
		USBmode = Report_Buf[1];
		break;

	default:
		break;
	}
}

void HID_Status_Out(void)
{
	Matrix[1] <<= 1;
	if(Matrix[1] >= 32) Matrix[1] = 1;
}

RESULT HID_Data_Setup(uint8_t RequestNo)
{
	uint8_t *(*CopyRoutine)(uint16_t);

	CopyRoutine = NULL;
	if((RequestNo == GET_DESCRIPTOR) && (Type_Recipient == (STANDARD_REQUEST | INTERFACE_RECIPIENT)) && (pInformation->USBwIndex0 == 0))
	{
		if(pInformation->USBwValue1 == REPORT_DESCRIPTOR)
		{
			CopyRoutine = HID_GetReportDescriptor;
		}
		else if(pInformation->USBwValue1 == HID_DESCRIPTOR_TYPE)
		{
			CopyRoutine = HID_GetHIDDescriptor;
		}
	}
	else if((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT)))
	{
		switch(RequestNo)
		{
		case GET_PROTOCOL: CopyRoutine = HID_GetProtocolValue; break;
		case SET_REPORT:
			CopyRoutine = HID_SetReport_Feature;
			Request = SET_REPORT;
			break;
		default: break;
		}
	}

	if(CopyRoutine == NULL) return USB_UNSUPPORT;
	pInformation->Ctrl_Info.CopyData = CopyRoutine;
	pInformation->Ctrl_Info.Usb_wOffset = 0;
	(*CopyRoutine)(0);

	return USB_SUCCESS;
}

uint8_t *HID_SetReport_Feature(uint16_t Length)
{
	if(Length == 0)
	{
		pInformation->Ctrl_Info.Usb_wLength = wMaxPacketSize;
		return NULL;
	}
	return Report_Buf;
}

RESULT HID_NoData_Setup(uint8_t RequestNo)
{
	if((Type_Recipient == (CLASS_REQUEST | INTERFACE_RECIPIENT)) && (RequestNo == SET_PROTOCOL)) return HID_SetProtocol();
	return USB_UNSUPPORT;
}

uint8_t *HID_GetDeviceDescriptor(uint16_t Length) { return Standard_GetDescriptorData(Length, &Device_Descriptor); }
uint8_t *HID_GetConfigDescriptor(uint16_t Length) { return Standard_GetDescriptorData(Length, &Config_Descriptor); }
uint8_t *HID_GetReportDescriptor(uint16_t Length) { return Standard_GetDescriptorData(Length, &CUBE_Report_Descriptor); }
uint8_t *HID_GetHIDDescriptor(uint16_t Length) { return Standard_GetDescriptorData(Length, &CUBE_Hid_Descriptor); }
uint8_t *HID_GetStringDescriptor(uint16_t Length)
{
	uint8_t wValue0 = pInformation->USBwValue0;
	if(wValue0 > 4) return NULL;
	return Standard_GetDescriptorData(Length, &String_Descriptor[wValue0]);
}

RESULT HID_Get_Interface_Setting(uint8_t Interface, uint8_t AlternateSetting)
{
	if(AlternateSetting > 0)
		return USB_UNSUPPORT;
	else if(Interface > 0)
		return USB_UNSUPPORT;
	return USB_SUCCESS;
}

RESULT HID_SetProtocol(void)
{
	uint8_t wValue0 = pInformation->USBwValue0;
	ProtocolValue = wValue0;
	return USB_SUCCESS;
}

uint8_t *HID_GetProtocolValue(uint16_t Length)
{
	if(Length == 0)
	{
		pInformation->Ctrl_Info.Usb_wLength = 1;
		return NULL;
	}
	return (uint8_t *)(&ProtocolValue);
}
