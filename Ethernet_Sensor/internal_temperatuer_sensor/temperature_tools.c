/*
 * temperature_tools.c
 *
 *  Created on: Jun 18, 2020
 *      Author: johnm
 */

#include "temperature_tools.h"
#include "board/ethernet.h"


void temp_init(void) {
	ADC_temp_setup();
	uDMA_temp_setup();
	gADCBufferIndex = -1;
}
void uDMA_temp_setup(void) {
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UDMA);
	uDMAEnable();
	uDMAControlBaseSet(gDMAControlTable);
	uDMAChannelAssign(UDMA_CH24_ADC1_0); // assign DMA channel 24 to ADC1 sequence 0
	uDMAChannelAttributeDisable(UDMA_SEC_CHANNEL_ADC10, UDMA_ATTR_ALL);
	// primary DMA channel = first half of the ADC buffer
	uDMAChannelControlSet(UDMA_SEC_CHANNEL_ADC10 | UDMA_PRI_SELECT,
	UDMA_SIZE_16 | UDMA_SRC_INC_NONE | UDMA_DST_INC_16 | UDMA_ARB_4);
	uDMAChannelTransferSet(UDMA_SEC_CHANNEL_ADC10 | UDMA_PRI_SELECT,
	UDMA_MODE_PINGPONG, (void*) &ADC1_SSFIFO0_R, (void*) &gADCBuffer[0],
	ADC_BUFFER_SIZE / 2);
	// alternate DMA channel = second half of the ADC buffer
	uDMAChannelControlSet(UDMA_SEC_CHANNEL_ADC10 | UDMA_ALT_SELECT,
	UDMA_SIZE_16 | UDMA_SRC_INC_NONE | UDMA_DST_INC_16 | UDMA_ARB_4);
	uDMAChannelTransferSet(UDMA_SEC_CHANNEL_ADC10 | UDMA_ALT_SELECT,
	UDMA_MODE_PINGPONG, (void*) &ADC1_SSFIFO0_R,
			(void*) &gADCBuffer[ADC_BUFFER_SIZE / 2], ADC_BUFFER_SIZE / 2);
	uDMAChannelEnable(UDMA_SEC_CHANNEL_ADC10);

	//ADCSequenceDMAEnable(ADC1_BASE, 0); // enable DMA for ADC1 sequence 0
	//ADCIntEnableEx(ADC1_BASE, ADC_INT_DMA_SS0); // enable ADC1 sequen
}
void ADC_temp_setup(void) {

	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);
	// ADC clock
	uint32_t pll_frequency = SysCtlFrequencyGet(CRYSTAL_FREQUENCY);
	uint32_t pll_divisor = (pll_frequency - 1) / (16 * ADC_SAMPLING_RATE_2X)
			+ 1; //round up
	ADCClockConfigSet(ADC1_BASE, ADC_CLOCK_SRC_PLL | ADC_CLOCK_RATE_FULL,
			pll_divisor);
	ADCSequenceDisable(ADC1_BASE, 0); // choose ADC1 sequence 0; disable before configuring

	ADCSequenceConfigure(ADC1_BASE, 0, ADC_TRIGGER_ALWAYS, 0);
	ADCSequenceStepConfigure(ADC1_BASE, 0, 0,
	ADC_CTL_TS | ADC_CTL_IE | ADC_CTL_END);
	ADCSequenceEnable(ADC1_BASE, 0);
	ADCSequenceDMAEnable(ADC1_BASE, 0);
	ADCIntEnableEx(ADC1_BASE, ADC_INT_DMA_SS0);

}

void scale_samples(void) {
	float fScale = (VREF * 20) / ((1 << ADC_BITS) * 1.0);
	for (uint16_t i = 0; i < LCD_HORIZONTAL_MAX; i++) {

		scaled_samples_buffer[i].c = LCD_VERTICAL_MAX / 2
				- (int16_t) roundf(
						fScale
								* ((int16_t) local_sample_buffer[i].c
										- ADC_OFFSET));
		scaled_samples_buffer[i].f = LCD_VERTICAL_MAX / 2
				- (int16_t) roundf(
						fScale
								* ((int16_t) local_sample_buffer[i].f
										- ADC_OFFSET));
	}
}
void find_avg_temp(tContext *sContext) {
	GrContextForegroundSet(sContext, ClrWhite);
	int32_t avg_c = 0;
	int32_t avg_f = 0;
	for (size_t i = 0; i < 128; i++) {
		avg_c += local_sample_buffer[i].c;
		avg_f += local_sample_buffer[i].f;
	}
	avg_c /= 128;
	avg_f /= 128;

	sprintf(buffer, "Avg F*: %d  Avg C*: %d\n", avg_f, avg_c);
	GrStringDraw(sContext, buffer, -1, 0, 5, false);

}
void temp_draw_function(tContext *sContext, size_t i1, size_t i2, void *p1,
		void *p2) {
	Temp_Reading *point_1 = (Temp_Reading *) p1;
	Temp_Reading *point_2 = (Temp_Reading *) p2;

	GrLineDraw(sContext, i1, point_1->c, i2, point_2->c);
	GrLineDraw(sContext, i1, point_1->f, i2, point_2->f);

}
void temp_access_function(void *reading_struct, void *data) {
	Temp_Reading *tr = (Temp_Reading *) reading_struct;
	uint16_t u16_data = *((uint16_t *) data);
	float reading_c = (float) (147.5f - ((75 * (3.3f) * u16_data) / 4096));
	float reading_f = reading_c * (9.0f / 5.0f) + 32;
	tr->f = reading_f;
	tr->c = reading_c;
}
void record_temp(UArg arg1, UArg arg2) {
	IntMasterEnable();
	//Semaphore_pend(main_loop,BIOS_WAIT_FOREVER);

	while (1) {
		Semaphore_pend(Access_Data_Sem, BIOS_WAIT_FOREVER);

		size_t retval = copy_buffer(&local_sample_buffer, &gADCBuffer, 128,
				sizeof(Temp_Reading), sizeof(uint16_t), &temp_access_function);
		if (retval != 128) {
			gADCErrors++;
		}
		scale_samples();

		Semaphore_post(Access_Data_Sem);

		Semaphore_post(SEM_DISP_TEMP);
	}

}

void display_temp(UArg arg1, UArg arg2) {
	tContext sContext;
	while (1) {
		Semaphore_pend(SEM_DISP_TEMP, BIOS_WAIT_FOREVER);
		Semaphore_pend(Access_Data_Sem, BIOS_WAIT_FOREVER);

		draw(&sContext, &scaled_samples_buffer, 128, sizeof(Temp_Reading),
				&temp_draw_function);
		draw_grid(&sContext);
		draw_text(&sContext, &find_avg_temp);
		GrFlush(&sContext);
		Semaphore_post(Access_Data_Sem);

	}
}
void TEMP_ISR(void) {
	ADCIntClearEx(ADC1_BASE, ADC_INT_DMA_SS0);
// Check the primary DMA channel for end of transfer, and restart if needed.
	if (uDMAChannelModeGet(
	UDMA_SEC_CHANNEL_ADC10 | UDMA_PRI_SELECT) == UDMA_MODE_STOP) {

		uDMAChannelTransferSet(UDMA_SEC_CHANNEL_ADC10 | UDMA_PRI_SELECT,
		UDMA_MODE_PINGPONG, (void*) &ADC1_SSFIFO0_R, (void*) &gADCBuffer[0],
		ADC_BUFFER_SIZE / 2); // restart the primary channel (same as setup)
		gDMAPrimary = false; // DMA is currently occurring in the alternate buffer
	}
	if (uDMAChannelModeGet(
	UDMA_SEC_CHANNEL_ADC10 | UDMA_ALT_SELECT) == UDMA_MODE_STOP) {
		uDMAChannelTransferSet(UDMA_SEC_CHANNEL_ADC10 | UDMA_ALT_SELECT,
		UDMA_MODE_PINGPONG, (void*) &ADC1_SSFIFO0_R,
				(void*) &gADCBuffer[ADC_BUFFER_SIZE / 2], ADC_BUFFER_SIZE / 2); // restart the alternate channel (same as setup)
		gDMAPrimary = true; // DMA is currently occurring in the primary buffer
	}
// The DMA channel may be disabled if the CPU is paused by the debugger.
	if (!uDMAChannelIsEnabled(UDMA_SEC_CHANNEL_ADC10)) {
		uDMAChannelEnable(UDMA_SEC_CHANNEL_ADC10); // re-enable the DMA channel
	}

	/*
	 //Clear interrupt vector
	 ADC1_ISC_R = ADC_ISC_IN0;
	 if (ADC1_OSTAT_R & ADC_OSTAT_OV0) {
	 gADCErrors++;
	 ADC1_OSTAT_R = ADC_OSTAT_OV0;
	 }
	 gADCBuffer[gADCBufferIndex = ADC_BUFFER_WRAP(gADCBufferIndex + 1)] =
	 ADC1_SSFIFO0_R;
	 */
}
