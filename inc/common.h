/*
 * common.h
 *
 *  Created on: Jun 1, 2016
 *      Author: miccio
 */

#ifndef COMMON_H_
#define COMMON_H_

// system
#include <stdarg.h>
#include <stdbool.h>

// board
#include "stm32f7xx.h"
#include "stm32746g_discovery.h"
#include "stm32746g_discovery_sdram.h"
#include "stm32746g_discovery_audio.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_ts.h"


static void inline APP_Err(char * str, ...) {
	va_list args;
	va_start(args, str);
	printf("System error!! \n");
	printf(str, args);
	printf("\n");
	va_end (args);
	while(true) {
		BSP_LED_Toggle(LED1);
		HAL_Delay(250);
	}
}

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })


 #define min(a,b) \
	({ __typeof__ (a) _a = (a); \
		__typeof__ (b) _b = (b); \
	  _a < _b ? _a : _b; })





#endif /* COMMON_H_ */
