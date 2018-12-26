/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_DESC_H
#define __USB_DESC_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported define -----------------------------------------------------------*/
#define USB_DEVICE_DESCRIPTOR_TYPE              0x01
#define USB_CONFIGURATION_DESCRIPTOR_TYPE       0x02
#define USB_STRING_DESCRIPTOR_TYPE              0x03
#define USB_INTERFACE_DESCRIPTOR_TYPE           0x04
#define USB_ENDPOINT_DESCRIPTOR_TYPE            0x05

#define HID_DESCRIPTOR_TYPE                     0x21
#define HID_REPORT_DESCRIPTOR_TYPE				0x22

#define CUBE_SIZ_HID_DESC                   0x09
#define CUBE_OFF_HID_DESC                   0x12

#define CUBE_SIZ_DEVICE_DESC                18
#define CUBE_SIZ_CONFIG_DESC                41
#define CUBE_SIZ_REPORT_DESC                101
#define CUBE_SIZ_STRING_LANGID              4
#define CUBE_SIZ_STRING_VENDOR              10
#define CUBE_SIZ_STRING_PRODUCT             22
#define CUBE_SIZ_STRING_SERIAL              26

#define STANDARD_ENDPOINT_DESC_SIZE             0x09

//HID Maximum packet size in bytes
#define wMaxPacketSize  0x40
#define EP1TxCount wMaxPacketSize
#define EP1RxCount 2

#define RPT1_COUNT 32 //PC sends CUBE/2
#define RPT2_COUNT 32 //PC sends CUBE/2
#define RPT3_COUNT 3  //PC sends???
#define RPT4_COUNT 32 //STM32 sends CUBE/2
#define RPT5_COUNT 32 //STM32 sends CUBE/2
#define RPT6_COUNT 3  //STM32 sends buttons + info

/* Exported functions ------------------------------------------------------- */
extern const uint8_t CUBE_DeviceDescriptor[CUBE_SIZ_DEVICE_DESC];
extern const uint8_t CUBE_ConfigDescriptor[CUBE_SIZ_CONFIG_DESC];
extern const uint8_t CUBE_ReportDescriptor[CUBE_SIZ_REPORT_DESC];
extern const uint8_t CUBE_StringLangID[CUBE_SIZ_STRING_LANGID];
extern const uint8_t CUBE_StringVendor[CUBE_SIZ_STRING_VENDOR];
extern const uint8_t CUBE_StringProduct[CUBE_SIZ_STRING_PRODUCT];
extern uint8_t CUBE_StringSerial[CUBE_SIZ_STRING_SERIAL];

#endif /* __USB_DESC_H */
