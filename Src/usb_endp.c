/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_mem.h"
#include "hw_config.h"
#include "usb_istr.h"
#include "usb_pwr.h"


/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t Receive_Buffer[wMaxPacketSize];


/* Extern variables ----------------------------------------------------------*/
extern uint8_t Buffer[RPT4_COUNT+1];
extern uint8_t Matrix[7];


/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : EP1_IN_Callback.
* Description    : EP1 IN Callback Routine.
*******************************************************************************/
void EP1_IN_Callback (void)
{
	/* Set the transfer complete token to inform upper layer that the current
	  transfer has been complete */
	/*Matrix[3] <<= 1;
	if(Matrix[3] >= 32) Matrix[3] = 1;*/


	  /*for(uint8_t k=0; k<64; k++)
	  			Buffer[k] = k+10;*/


#if 0
	uint16_t USB_Tx_ptr;
	uint16_t USB_Tx_length;

	if (USB_Tx_State == 1)
	{
		if (USART_Rx_length == 0)
		{
			USB_Tx_State = 0;
		}
		else
		{
			// assumption: always transmitting multiply of endpoint size
			if (USART_Rx_ptr_out == USART_RX_DATA_SIZE)
				USART_Rx_ptr_out = 0;
			USB_Tx_ptr = USART_Rx_ptr_out;
			USB_Tx_length = VIRTUAL_COM_PORT_DATA_SIZE;

			USART_Rx_ptr_out += VIRTUAL_COM_PORT_DATA_SIZE;
			USART_Rx_length -= VIRTUAL_COM_PORT_DATA_SIZE;

			UserToPMABufferCopy(&USART_Rx_Buffer[USB_Tx_ptr], ENDP1_TXADDR, USB_Tx_length);
			SetEPTxCount(ENDP1, USB_Tx_length);
			SetEPTxValid(ENDP1);
		}
	}
#endif
}


/*******************************************************************************
* Function Name  : EP1_OUT_Callback.
* Description    : EP1 OUT Callback Routine.
*******************************************************************************/
void EP1_OUT_Callback(void)	//Данные, отправленные методом SET_REPORT
{
	Matrix[2] <<= 1;
	if(Matrix[2] >= 32) Matrix[2] = 1;

	/* Read received data (2 bytes) */
	USB_SIL_Read(EP1_OUT, Receive_Buffer);

	Matrix[3] ++;
	Matrix[4] ++;

#if 0
	switch (Receive_Buffer[0])
	{

		case 1: /* Led 1 */
			if (Led_State != 0)
			{
				Matrix[4] |= (1<<2);//GPIO_SetBits(LED_PORT,LED1_PIN);
			}
			else
			{
				Matrix[4] &= ~(1<<2);//GPIO_ResetBits(LED_PORT,LED1_PIN);
			}
			break;

		case 2: /* Led 2 */
			if (Led_State != 0)
			{
				Matrix[4] |= (1<<3);//GPIO_SetBits(LED_PORT,LED2_PIN);
			}
			else
			{
				Matrix[4] &= ~(1<<3);//GPIO_ResetBits(LED_PORT,LED2_PIN);
			}
			break;

		case 3: /* Led 1&2 */
			Buffer[4] = Receive_Buffer[1];
			break;
	}
#endif


	SetEPRxStatus(ENDP1, EP_RX_VALID);
}
