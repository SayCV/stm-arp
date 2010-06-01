#include <stm32f10x.h>

#define DACCLK_CNT 479
#define ADCCLK_CNT 479

void initrcc(void);
void initgpio(void);
void initdac(void);
void initadc(void);
void initdacdma(void);
void initadcdma(void);
void inittim6(void);
void inittim3(void);

// volatile uint16_t adcbuf;
volatile uint16_t adcbuf;
volatile uint16_t dacbuf;

void main (void) {
	initrcc();
	initgpio();
	initdacdma();
	
	initadcdma();
	initdac();
	initadc();
	inittim6();
	inittim3();
	
	while (1)
	{
		dacbuf = adcbuf;
	}
}

void initrcc(void)
{
	//enable peripheral clocks
	
	//apb2 max clock 72mhz
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	
	//apb1 max clock 36mhz
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
	
	//ahb max clock 72mhz
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
}

void initgpio(void)
{
	//configure port a pin 4 (dac out 1) as an analog input
	//to avoid parasitic consumption (ref. stdperiph example)
	GPIO_InitTypeDef porta;
	porta.GPIO_Pin = GPIO_Pin_4;
	porta.GPIO_Mode = GPIO_Mode_AIN;
	
	GPIO_Init(GPIOA, &porta);
	
	//configure port c pin 0 (adc in 10) as an analog input
	//to avoid parasitic consumption (ref. stdperiph example)
	GPIO_InitTypeDef portc4;
	portc4.GPIO_Pin = GPIO_Pin_0;
	portc4.GPIO_Mode = GPIO_Mode_AIN;
	
	GPIO_Init(GPIOC, &portc4);
	
	//populate structure to define pins connected to LEDs
	//to be 10MHz open-drain outputs
	GPIO_InitTypeDef portc;
	portc.GPIO_Pin = (GPIO_Pin_1 |
	GPIO_Pin_4 |
	GPIO_Pin_5 |
	GPIO_Pin_8 |
	GPIO_Pin_9);
	portc.GPIO_Speed = GPIO_Speed_10MHz;
	portc.GPIO_Mode = GPIO_Mode_Out_OD;
	
	//Apply the settings to port c
	GPIO_Init(GPIOC, &portc);
}

void initdac(void)
{
	//populate dac structure
	DAC_InitTypeDef dacinfo;
	dacinfo.DAC_Trigger = DAC_Trigger_T6_TRGO;
	dacinfo.DAC_WaveGeneration = DAC_WaveGeneration_None;
	dacinfo.DAC_LFSRUnmask_TriangleAmplitude = DAC_LFSRUnmask_Bit0;
	dacinfo.DAC_OutputBuffer = DAC_OutputBuffer_Enable;
	
	//apply settings to dac channel 1
	DAC_Init(DAC_Channel_1, &dacinfo);
	
	DAC_DMACmd(DAC_Channel_1, ENABLE);
	
	//enable dac
	DAC_Cmd(DAC_Channel_1, ENABLE);
}

void initadc(void)
{
	//populate adc structure
	ADC_InitTypeDef adcinfo;
	adcinfo.ADC_Mode = ADC_Mode_Independent;
	adcinfo.ADC_ScanConvMode = DISABLE;
	adcinfo.ADC_ContinuousConvMode = ENABLE;
	adcinfo.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T3_TRGO;
// 	adcinfo.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	adcinfo.ADC_DataAlign = ADC_DataAlign_Right;
	adcinfo.ADC_NbrOfChannel = 1;
	
	// 	void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct);	
	// 	void DAC_Init(uint32_t DAC_Channel, DAC_InitTypeDef* DAC_InitStruct);
	// 	void ADC_Init(ADC_TypeDef* ADCx, ADC_InitTypeDef* ADC_InitStruct);
	//apply settings to adc 1
	ADC_Init(ADC1, &adcinfo);
	
	// 	void ADC_RegularChannelConfig(ADC_TypeDef* ADCx, uint8_t ADC_Channel, uint8_t Rank, uint8_t ADC_SampleTime)
	ADC_RegularChannelConfig(ADC1, ADC_Channel_10, 1, ADC_SampleTime_1Cycles5);
	
	// 	void ADC_Cmd(ADC_TypeDef* ADCx, FunctionalState NewState);
	ADC_Cmd(ADC1, ENABLE);
	
	//Do an ADC reset calibration. block until finished.
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	
	//Do an ADC start calibration. block until finished.
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	
	ADC_DMACmd(ADC1, ENABLE);
	
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

void initadcdma(void)
{
	DMA_DeInit(DMA1_Channel1);
	
	DMA_InitTypeDef dmainfo;
// 	dmainfo.DMA_PeripheralBaseAddr = DR_ADDRESS;
	dmainfo.DMA_PeripheralBaseAddr = (uint32_t) (0x4001244c);
	dmainfo.DMA_MemoryBaseAddr = (uint32_t) &adcbuf;
	dmainfo.DMA_DIR = DMA_DIR_PeripheralSRC;
	dmainfo.DMA_BufferSize = 1;
	dmainfo.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dmainfo.DMA_MemoryInc = DMA_MemoryInc_Disable;
	dmainfo.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	dmainfo.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	dmainfo.DMA_Mode = DMA_Mode_Circular;
	dmainfo.DMA_Priority = DMA_Priority_High;
	dmainfo.DMA_M2M = DMA_M2M_Disable;
	
	DMA_Init(DMA1_Channel1, &dmainfo);
	
	DMA_Cmd(DMA1_Channel1, ENABLE);
}

void initdacdma(void)
{
	DMA_DeInit(DMA2_Channel3);
	
	DMA_InitTypeDef dmainfo;
// 	dmainfo.DMA_PeripheralBaseAddr = (uint32_t) (DAC_BASE + DHR12R1_Offset);
	dmainfo.DMA_PeripheralBaseAddr = (uint32_t) (0x40007408);
	dmainfo.DMA_MemoryBaseAddr = (uint32_t) &dacbuf;
	dmainfo.DMA_DIR = DMA_DIR_PeripheralDST;
	dmainfo.DMA_BufferSize = 1;
	dmainfo.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	dmainfo.DMA_MemoryInc = DMA_MemoryInc_Disable;
	dmainfo.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	dmainfo.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	dmainfo.DMA_Mode = DMA_Mode_Circular;
	dmainfo.DMA_Priority = DMA_Priority_High;
	dmainfo.DMA_M2M = DMA_M2M_Disable;
	
	DMA_Init(DMA2_Channel3, &dmainfo);
	
	DMA_Cmd(DMA2_Channel3, ENABLE);
}

void inittim6(void)
{
// 	TIM_SetAutoreload(TIM6, 0xf);
// 	TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
// 	TIM_Cmd(TIM6, ENABLE);

	TIM_TimeBaseInitTypeDef timinfo;
	timinfo.TIM_Prescaler = TIM_CKD_DIV1;
	timinfo.TIM_CounterMode = TIM_CounterMode_Down;
	timinfo.TIM_Period = DACCLK_CNT;
	timinfo.TIM_ClockDivision = 0;
	//repetition counter isn't used for tim6
	timinfo.TIM_RepetitionCounter = 0;
	
	TIM_TimeBaseInit(TIM6, &timinfo);
	
	TIM_SelectOutputTrigger(TIM6, TIM_TRGOSource_Update);
	
	TIM_Cmd(TIM6, ENABLE);
}

void inittim3(void)
{
	TIM_TimeBaseInitTypeDef timinfo;
	timinfo.TIM_Prescaler = TIM_CKD_DIV1;
	timinfo.TIM_CounterMode = TIM_CounterMode_Down;
	timinfo.TIM_Period = ADCCLK_CNT;
	timinfo.TIM_ClockDivision = 0;
	//repetition counter isn't used for tim3
	timinfo.TIM_RepetitionCounter = 0;
	
	TIM_TimeBaseInit(TIM3, &timinfo);
	
	TIM_SelectOutputTrigger(TIM3, TIM_TRGOSource_Update);
	
	TIM_Cmd(TIM3, ENABLE);
}