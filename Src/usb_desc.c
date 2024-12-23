#include "usb_desc.h"
#include "usb_lib.h"

/* USB Standard Device Descriptor */
uint8_t CUBE_DeviceDescriptor[CUBE_SIZ_DEVICE_DESC] = {
	CUBE_SIZ_DEVICE_DESC,		// = 18 ����� ����� ����������� ���������� � ������
	USB_DEVICE_DESCRIPTOR_TYPE, // bDescriptorType - ����������, ��� ��� �� ����������. � ������ ������ - Device descriptor
	0x00, 0x02,					// bcdUSB - ����� ������ ��������� USB ������������ ����������. 2.0

	// �����, �������� ���������� � ��������, �� ��������� USB. � ��� ����, �������� ������ ��������� ��� �� ����
	0x00, // bDeviceClass
	0x00, // bDeviceSubClass
	0x00, // bDeviceProtocol

	0x40, // bMaxPacketSize - ������������ ������ ������� ��� Endpoint 0 (��� ����������������)

	// �� ����� ����������� VID � PID,  �� ������� � ������������, ��� �� ��� �� ����������.
	// � �������� ����������� ���� �������� VID, ����� ���������� ����� ���� ��������� � ����������� ������ ��������
	0x83, 0x04, // idVendor (0x0483)
	0x11, 0x57, // idProduct (0x5711)

	0x00, 0x01, // bcdDevice rel. DEVICE_VER_H.DEVICE_VER_L  ����� ������ ����������

	// ������ ���� ������� �����, ����������� �������������, ���������� � �������� �����.
	// ������������ � ��������� ���������� � ���������� ���������
	// � �� ��������� ������ ������������ ���������� � ���������� VID/PID ����������� ��������.
	1,	 // Index of string descriptor describing manufacturer
	2,	 // Index of string descriptor describing product
	3,	 // Index of string descriptor describing the device serial number
	0x01 // bNumConfigurations - ���������� ��������� ������������. � ��� ����.
}; /* CustomHID_DeviceDescriptor */

/* USB Configuration Descriptor */
/*   All Descriptors (Configuration, Interface, Endpoint, Class, Vendor */
uint8_t CUBE_ConfigDescriptor[CUBE_SIZ_CONFIG_DESC] = {
	0x09,							   // bLength: ����� ����������� ������������
	USB_CONFIGURATION_DESCRIPTOR_TYPE, // bDescriptorType: ��� ����������� - ������������
	CUBE_SIZ_CONFIG_DESC, 0x00,		   // wTotalLength: ����� ������ ����� ������ ��� ������ ������������� � ������

	0x01, // bNumInterfaces: � ������������ ����� ���� ���������
	0x01, // bConfigurationValue: ������ ������ ������������
	0x00, // iConfiguration: ������ ������, ������� ��������� ��� ������������
	0xE0, // bmAttributes: ������� ����, ��� ���������� ����� �������� �� ���� USB
	0xFA, // MaxPower 500 mA

	/************** ���������� ���������� ****************/
	0x09,						   // bLength: ������ ����������� ����������
	USB_INTERFACE_DESCRIPTOR_TYPE, // bDescriptorType: ��� ����������� - ���������
	0x00,						   // bInterfaceNumber: ���������� ����� ���������� - 0
	0x00,						   // bAlternateSetting: ������� ��������������� ����������, � ��� �� ������������
	/*----------->*/ 0x02,		   // bNumEndpoints - ���������� ����������.

	0x03, // bInterfaceClass: ����� ��������� - HID
	// ���� �� �� ������ ��� ����������� ����������, �������� ���������� ��� ����, �� ���� ���� �� ������� ��������� ����� � ��������
	// � ��� � ��� ����� HID-����������
	0x00, // bInterfaceSubClass : �������� ����������.
	0x00, // nInterfaceProtocol : �������� ����������

	0, // iInterface: ������ ������, ����������� ���������

	// ������ ��������� ���������� ��� ��������� ����, ��� ������ ��������� - ��� HID ����������
	/******************** HID ���������� ********************/
	0x09,						// bLength: ����� HID-�����������
	HID_DESCRIPTOR_TYPE,		// bDescriptorType: ��� ����������� - HID
	0x01, 0x01,					// bcdHID: ����� ������ HID 1.1
	/*--------------->*/ 0x33,	// bCountryCode: ��� ������ (���� �����)
	/*===============>*/ 0x01,	// bNumDescriptors: ������� ������ ����� report ������������
	HID_REPORT_DESCRIPTOR_TYPE, // bDescriptorType: ��� ����������� - report
	CUBE_SIZ_REPORT_DESC, 0x00, // wItemLength: ����� report-�����������

	/******************** ���������� �������� ����� (endpoints) ********************/
	0x07,						  // bLength: ����� �����������
	USB_ENDPOINT_DESCRIPTOR_TYPE, // ��� ����������� - endpoints

	0x81,						  // bEndpointAddress: ����� �������� ����� � ����������� 1(IN)
	0x03,						  // bmAttributes: ��� �������� ����� - Interrupt endpoint
	wMaxPacketSize, 0x00,		  // wMaxPacketSize:  Bytes max
	0x20,						  // bInterval: Polling Interval (32 ms)=0x20
								  /*==^==^==^==^==^==^==^==^==^==^==^==^*/
	0x07,						  /* bLength: Endpoint Descriptor size */
	USB_ENDPOINT_DESCRIPTOR_TYPE, /* bDescriptorType: */
								  /*	Endpoint descriptor type */
	0x01,						  /* bEndpointAddress: */
								  /*	Endpoint Address (OUT) */
	0x03,						  /* bmAttributes: Interrupt endpoint */
	wMaxPacketSize,				  /* wMaxPacketSize:  Bytes max  */
	0x00,
	0x20, /* bInterval: Polling Interval (32 ms) */
}; /* CUBE_ConfigDescriptor */

uint8_t CUBE_ReportDescriptor[CUBE_SIZ_REPORT_DESC] = {
	0x06, 0x00, 0xff, // USAGE_PAGE (Generic Desktop)
	0x09, 0x01,		  // USAGE (Vendor Usage 1)
	0xa1, 0x01,		  // COLLECTION (Application)

	// REPORT ID 1 - PC sends first CUBE/2
	0x85, 0x01,		  // REPORT_ID (1)
	0x09, 0x01,		  // USAGE (Vendor Usage 1)
	0x15, 0x00,		  //   LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x00, //   LOGICAL_MAXIMUM (255)
	0x75, 0x08,		  //   REPORT_SIZE (8)
	0x95, RPT1_COUNT, // REPORT_COUNT (N)
	0xb1, 0x82,		  //   FEATURE (Data,Var,Abs,Vol)
	0x85, 0x01,		  // REPORT_ID (1)
	0x09, 0x01,		  // USAGE (Vendor Usage 1)
	0x91, 0x82,		  //   OUTPUT (Data,Var,Abs,Vol)

	// REPORT ID 2 - PC sends second CUBE/2
	0x85, 0x02,		  // REPORT_ID (2)
	0x09, 0x02,		  // USAGE (Vendor Usage 2)
	0x15, 0x00,		  //   LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x00, //   LOGICAL_MAXIMUM (255)
	0x75, 0x08,		  //   REPORT_SIZE (8)
	0x95, RPT2_COUNT, // REPORT_COUNT (N)
	0xb1, 0x82,		  //   FEATURE (Data,Var,Abs,Vol)
	0x85, 0x02,		  // REPORT_ID (2)
	0x09, 0x02,		  // USAGE (Vendor Usage 2)
	0x91, 0x82,		  //   OUTPUT (Data,Var,Abs,Vol)

	// REPORT ID 3 - PC sends ???
	0x85, 0x03,		  // REPORT_ID (3)
	0x09, 0x03,		  // USAGE (Vendor Usage 3)
	0x15, 0x00,		  //   LOGICAL_MINIMUM (0)
	0x26, 0xff, 0x00, //   LOGICAL_MAXIMUM (255)
	0x75, 0x08,		  //   REPORT_SIZE (8)
	0x95, RPT3_COUNT, // REPORT_COUNT (N)
	0xb1, 0x82,		  //   FEATURE (Data,Var,Abs,Vol)
	0x85, 0x03,		  // REPORT_ID (3)
	0x09, 0x03,		  // USAGE (Vendor Usage 3)
	0x91, 0x82,		  //   OUTPUT (Data,Var,Abs,Vol)

	// REPORT ID 4 - STM32 sends CUBE/2
	0x85, 0x04,		  // REPORT_ID (4)
	0x09, 0x04,		  // USAGE (Vendor Usage 4)
	0x75, 0x08,		  //   REPORT_SIZE (8)
	0x95, RPT4_COUNT, // REPORT_COUNT (N)
	0x81, 0x82,		  //   INPUT (Data,Var,Abs,Vol)

	// REPORT ID 5 - STM32 sends CUBE/2
	0x85, 0x05,		  // REPORT_ID (5)
	0x09, 0x05,		  // USAGE (Vendor Usage 5)
	0x75, 0x08,		  //   REPORT_SIZE (8)
	0x95, RPT5_COUNT, // REPORT_COUNT (N)
	0x81, 0x82,		  //   INPUT (Data,Var,Abs,Vol)

	// REPORT ID 6 - STM32 sends buttons + info
	0x85, 0x06,		  // REPORT_ID (6)
	0x09, 0x06,		  // USAGE (Vendor Usage 6)
	0x75, 0x08,		  //   REPORT_SIZE (8)
	0x95, RPT6_COUNT, // REPORT_COUNT (N)
	0x81, 0x82,		  //   INPUT (Data,Var,Abs,Vol)

	0xc0 // END_COLLECTION
}; /* CUBE_ReportDescriptor */

/* USB String Descriptors (optional) */
uint8_t CUBE_StringLangID[CUBE_SIZ_STRING_LANGID] = {
	CUBE_SIZ_STRING_LANGID, /* =4*/
	USB_STRING_DESCRIPTOR_TYPE,
	0x09,
	0x04}; /* LangID = 0x0409: U.S. English */

uint8_t CUBE_StringVendor[CUBE_SIZ_STRING_VENDOR] = {
	CUBE_SIZ_STRING_VENDOR,		/* Size of Vendor string = 10*/
	USB_STRING_DESCRIPTOR_TYPE, /* bDescriptorType*/
	/* Manufacturer: "STMicroelectronics" */
	'E', 0, '.', 0, 'D', 0, '.', 0};

uint8_t CUBE_StringProduct[CUBE_SIZ_STRING_PRODUCT] = {
	CUBE_SIZ_STRING_PRODUCT,	/* bLength = 22*/
	USB_STRING_DESCRIPTOR_TYPE, /* bDescriptorType */
	'C', 0, 'U', 0, 'B', 0, 'E', 0, ' ', 0, 'S', 0, 'T', 0,
	'M', 0, '3', 0, '2', 0};
uint8_t CUBE_StringSerial[CUBE_SIZ_STRING_SERIAL] = {
	CUBE_SIZ_STRING_SERIAL,		/* bLength = 26 */
	USB_STRING_DESCRIPTOR_TYPE, /* bDescriptorType */
	'U', 0, 'S', 0, 'B', 0, ' ', 0, 'C', 0, 'U', 0, 'B', 0, 'E'};
