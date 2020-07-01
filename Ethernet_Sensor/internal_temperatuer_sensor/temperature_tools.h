/*
 * temperature_tools.h
 *
 *  Created on: Jun 18, 2020
 *      Author: johnm
 */

#ifndef TEMPERATURE_TOOLS_H_
#define TEMPERATURE_TOOLS_H_
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include "Utility/kernel_include.h"
#include <Utility/utility.h>
#define ADC_BUFFER_SIZE 2048
#define ADC_BUFFER_WRAP(i) ((i) & (ADC_BUFFER_SIZE -1))
#define ADC_OFFSET 2048
#define ADC_BITS 12
#define TEMP_CONV_FORM (125.0f / 4096.0f)
#define VREF 3.3f
/**
 * Structs
 */

typedef struct _temp_reading {
	float f;
	float c;
} Temp_Reading;


void temp_access_function(void *reading_struct,void *data);
void temp_draw_function(tContext *sContext, size_t i1,size_t i2,void *p1,void *p2);
/**
 * Struct end
 */

volatile int32_t gADCBufferIndex; // = ADC_BUFFER_SIZE -1;
volatile uint16_t gADCBuffer[ADC_BUFFER_SIZE];
Temp_Reading local_sample_buffer[LCD_HORIZONTAL_MAX];
Temp_Reading scaled_samples_buffer[LCD_HORIZONTAL_MAX];
volatile uint32_t gADCErrors; // = 0;
extern uint32_t gSystemClock;
bool gDMAPrimary;
#pragma DATA_ALIGN(gDMAControlTable,1024) //address alignment required
tDMAControlTable gDMAControlTable[64];
char buffer[64];


void temp_init();
void uDMA_temp_setup(void);
void ADC_temp_setup(void);
void TEMP_ISR(void);
void find_avg_temp(tContext *sContext);
void scale_samples(void);
void record_temp(UArg arg1, UArg arg2);
#endif /* TEMPERATURE_TOOLS_H_ */
