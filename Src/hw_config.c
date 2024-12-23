#include "hw_config.h"
#include "debug.h"
#include "random.h"
#include "stm32f10x_it.h"
#include "usb_desc.h"
#include "usb_lib.h"
#include "usb_prop.h"
#include "usb_pwr.h"

uint16_t btn1_prev, btn2_prev;
uint8_t Buffer[70];

extern uint8_t image[64];
extern uint8_t Matrix[7];
extern uint8_t buttons;
extern uint8_t buttonsState;

static void IntToUnicode(uint32_t value, uint8_t *pbuf, uint8_t len)
{
	for(uint8_t idx = 0; idx < len; idx++)
	{
		if(((value >> 28)) < 0xA)
		{
			pbuf[2 * idx] = (value >> 28) + '0';
		}
		else
		{
			pbuf[2 * idx] = (value >> 28) + 'A' - 10;
		}
		value = value << 4;
		pbuf[2 * idx + 1] = 0;
	}
}

void hw_init(void)
{
	// RCC Init
	RCC_DeInit();
	RCC_HSEConfig(RCC_HSE_ON);
	ErrorStatus HSEStartUpStatus = RCC_WaitForHSEStartUp();
	if(HSEStartUpStatus == SUCCESS)
	{
		// Enable Prefetch Buffer
		FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
		// Flash 2 wait state
		FLASH_SetLatency(FLASH_Latency_2);

		// HCLK = SYSCLK
		RCC_HCLKConfig(RCC_SYSCLK_Div1);
		// PCLK2 = HCLK
		RCC_PCLK2Config(RCC_HCLK_Div1);
		// PCLK1 = HCLK /2
		RCC_PCLK1Config(RCC_HCLK_Div2);
		// ADC Clock
		RCC_ADCCLKConfig(RCC_PCLK2_Div8);
		// PLL Clock
		RCC_PLLConfig(RCC_PLLSource_HSE_Div1, RCC_PLLMul_9); // 9

		RCC_PLLCmd(ENABLE);
		while(RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET)
			;

		RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
		while(RCC_GetSYSCLKSource() != 0x08)
			;
	}

	// System Clock = (72Mhz) / 72000 = 1000Hz = 1ms reload
	SysTick_Config(72000);

	// Peripheral clocking
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_SPI1 | RCC_APB2Periph_USART1 | RCC_APB2Periph_ADC1 | RCC_APB2Periph_AFIO, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

	AFIO->MAPR |= AFIO_MAPR_SWJ_CFG_JTAGDISABLE;

	// I/O
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_6 | GPIO_Pin_8 | GPIO_Pin_15 | GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

// SPI Pins
#ifdef USE_DMA
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	// SPI Init
	SPI_I2S_DeInit(SPI1);
	SPI_InitTypeDef SPI_InitStructure;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Tx;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_Init(SPI1, &SPI_InitStructure);
	SPI_I2S_DMACmd(SPI1, SPI_I2S_DMAReq_Tx, ENABLE);

	// DMA Init
	DMA_DeInit(DMA1_Channel3);
	DMA_InitTypeDef DMA_InitStructure;
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&SPI1->DR;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&image[0];
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_BufferSize = 8;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(DMA1_Channel3, &DMA_InitStructure);

	// DMA_Cmd(DMA1_Channel3, ENABLE);
	SPI_Cmd(SPI1, ENABLE);
#endif

	// Timer
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	TIM_TimeBaseInitTypeDef TimerInitStructure;
	TimerInitStructure.TIM_Prescaler = 3600;
	TimerInitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TimerInitStructure.TIM_Period = 13;
	TimerInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TimerInitStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM2, &TimerInitStructure);
	TIM_Cmd(TIM2, ENABLE);

	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);

	// USB Init
	RCC_USBCLKConfig(RCC_USBCLKSource_PLLCLK_1Div5);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USB, ENABLE);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

	NVIC_InitStructure.NVIC_IRQChannel = USB_LP_CAN1_RX0_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USB_Init();

	DebugInit();
	RandomInit();
}

void CUBECheckState(void)
{
	static uint8_t packetType = 4;

#if 1
	switch(packetType)
	{
	case 4:
		packetType = 5;
		Buffer[0] = 4;
		for(uint8_t i = 0; i < 32; i++)
		{
			Buffer[i + 1] = image[i];
		}
		USB_SIL_Write(EP1_IN, Buffer, RPT4_COUNT + 1);
		break;
	case 5:
		packetType = 6;
		Buffer[0] = 5;
		for(uint8_t i = 0; i < 32; i++)
		{
			Buffer[i + 1] = image[i + 32];
		}
		USB_SIL_Write(EP1_IN, Buffer, RPT5_COUNT + 1);
		break;
	case 6:
		packetType = 4;
		Buffer[0] = 6;
		Buffer[1] = buttonsState;
		Buffer[2] = Buffer[3] = 0;
		USB_SIL_Write(EP1_IN, Buffer, RPT6_COUNT + 1);
		break;
	default: break;
	}

#else
	Buffer[0] = 4;
	for(uint8_t i = 0; i < 32; i++)
	{
		if(i < 15)
			Buffer[i + 1] = 0;
		else
			Buffer[i + 1] = 255;
	}
	USB_SIL_Write(EP1_IN, Buffer, RPT4_COUNT + 1);
#endif
	/* Buffer[0] = 4;//4;
	 Buffer[1] = 255;//btn1;
	 Buffer[2] = 13;//btn2;
	 Buffer[3] = 150;//Matrix[0];
	 Buffer[4] = 49;
	 Buffer[5] = 48;
	 for(uint8_t i = 1; i<64; i++) Buffer[i] = i;*/

	// for(uint8_t k=0; k<5; k++)Buffer[k] = k+10;

	// USB_SIL_Write(EP1_IN, Buffer, RPT4_COUNT+1); /* Copy mouse position info in ENDP1 Tx Packet Memory Area*/
	SetEPTxValid(ENDP1); /* Enable endpoint for transmission */
}

void CUBECheckState2(void)
{
	uint8_t buff[21] = {05, 56, 57, 58, 100, 101, 102};
	USB_SIL_Write(EP1_IN, buff, 32 + 1);
	SetEPTxValid(ENDP1);
}

void Enter_LowPowerMode(void)
{
	/* Set the device state to suspend */
	bDeviceState = SUSPENDED;
}

void Leave_LowPowerMode(void)
{
	DEVICE_INFO *pInfo = &Device_Info;
	bDeviceState = pInfo->Current_Configuration != 0 ? CONFIGURED : ATTACHED;
}

void USB_Cable_Config(FunctionalState NewState) {}

void Get_SerialNum(void)
{
	uint32_t Device_Serial0, Device_Serial1, Device_Serial2;

	Device_Serial0 = *(volatile uint32_t *)(0x1FFFF7E8);
	Device_Serial1 = *(volatile uint32_t *)(0x1FFFF7EC);
	Device_Serial2 = *(volatile uint32_t *)(0x1FFFF7F0);

	Device_Serial0 += Device_Serial2;

	if(Device_Serial0 != 0)
	{
		IntToUnicode(Device_Serial0, &CUBE_StringSerial[2], 8);
		IntToUnicode(Device_Serial1, &CUBE_StringSerial[18], 4);
	}
}
