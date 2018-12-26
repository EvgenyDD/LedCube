/* Includes ------------------------------------------------------------------*/
#include "random.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Extern variables ----------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : RandomInit
* Description    : Initialize random number generator
*******************************************************************************/
void RandomInit()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_InitTypeDef  ADC_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_ADC1, ENABLE );

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE; // Single Channel
	ADC_InitStructure.ADC_ContinuousConvMode = DISABLE; // Scan on Demand
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);

	//ADC1->CR2 |= ((uint32_t)0x00800000); //Enable temperature sensor

	/* ADC1 regular channel1 configuration */
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_1Cycles5);

	/* Enable ADC1 */
	ADC_Cmd(ADC1, ENABLE);

#if 0
	/* Enable ADC1 reset calibration register */
	ADC_ResetCalibration(ADC1);

	/* Check the end of ADC1 reset calibration register */
	while(ADC_GetResetCalibrationStatus(ADC1));

	/* Start ADC1 calibaration */
	ADC_StartCalibration(ADC1);

	/* Check the end of ADC1 calibration */
	while(ADC_GetCalibrationStatus(ADC1));
#endif

	/* Start ADC1 Software Conversion */
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}


/*******************************************************************************
* Function Name  : GetRandom
* Description    : Generates a random number
* Output	     : 12-bit random number
*******************************************************************************/
uint16_t GetRandom()
{
uint32_t adc;

	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);

	adc = ADC_GetConversionValue(ADC1);

	/* Probably overkill */
	ADC_ClearFlag(ADC1, ADC_FLAG_EOC);

	/* Start ADC1 Software Conversion */
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);

	return adc;
}

uint16_t GetRandomBig()
{
uint32_t adc[4];

	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	adc[0] = ADC_GetConversionValue(ADC1);
	ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);

	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	adc[1] = ADC_GetConversionValue(ADC1);
	ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);

	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	adc[2] = ADC_GetConversionValue(ADC1);
	ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);

	while(ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET);
	adc[3] = ADC_GetConversionValue(ADC1);
	ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
	adc[0] = adc[0] %10;
	adc[1] = adc[1] %10;
	adc[2] = adc[2] %10;
	adc[3] = adc[3] %10;

	return adc[0]*1000+adc[2]*100+adc[1]*10+adc[0];
}



