/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USB_CONF_H
#define __USB_CONF_H

/* Includes ------------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/* External variables --------------------------------------------------------*/

/*-------------------------------------------------------------*/
/* EP_NUM */
/* defines how many endpoints are used by the device */
/*-------------------------------------------------------------*/
#define EP_NUM (2)

/*-------------------------------------------------------------*/
/* --------------   Buffer Description Table  -----------------*/
/*-------------------------------------------------------------*/
/* buffer table base address */
#define BTABLE_ADDRESS (0x00)

/* EP0  */
/* rx/tx buffer base address */
#define ENDP0_RXADDR (0x18)
#define ENDP0_TXADDR (0x58)

/* EP1  */
/* tx buffer base address */
#define ENDP1_TXADDR (0x100)
#define ENDP1_RXADDR (0x140)

#if 0
/* buffer table base address */
#define BTABLE_ADDRESS (0x00)

/* EP0  */
/* rx/tx buffer base address */
#define ENDP0_RXADDR (0x40)
#define ENDP0_TXADDR (0x80)

/* EP1  */
/* tx buffer base address */
#define ENDP1_TXADDR (0xC0)
#define ENDP1_RXADDR (0xC1)
#define ENDP2_TXADDR (0x100)
#define ENDP3_RXADDR (0x110)
#endif

/*-------------------------------------------------------------*/
/* -------------------   ISTR events  -------------------------*/
/*-------------------------------------------------------------*/
/* IMR_MSK */
/* mask defining which events has to be handled */
/* by the device application software */
#define IMR_MSK (CNTR_CTRM | CNTR_WKUPM | CNTR_SUSPM | CNTR_ERRM | CNTR_SOFM | CNTR_ESOFM | CNTR_RESETM)

#endif /* __USB_CONF_H */
