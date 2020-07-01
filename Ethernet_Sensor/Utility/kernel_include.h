/*
 * kernel_include.h
 *
 *  Created on: Jun 18, 2020
 *      Author: johnm
 */

#ifndef KERNEL_INCLUDE_H_
#define KERNEL_INCLUDE_H_

#include <stdbool.h>
#include <stdint.h>
#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <xdc/cfg/global.h>

/* BIOS Header files */
#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <ti/sysbios/gates/GateTask.h>

// Driver libraries from TivaWare
#include "board/Crystalfontz128x128_ST7735.h"
#include "inc/hw_memmap.h"
/* Driver Header files */
#include "inc/tm4c1294ncpdt.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/adc.h"
#include "driverlib/pin_map.h"
#include "driverlib/udma.h"
#include "board/sysctl_pll.h"



#define ADC_SAMPLING_RATE 1000000   // [samples/sec] desired ADC sampling rate
#define ADC_SAMPLING_RATE_2X 2000000 // 2x normal adc sampling rate
#define CRYSTAL_FREQUENCY 25000000  // [Hz] crystal oscillator frequency used to calculate clock rates
#endif /* KERNEL_INCLUDE_H_ */
