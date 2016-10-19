/*
 * audio_out.c
 *
 *  Created on: Jun 1, 2016
 *      Author: miccio
 */

#include "audio_out.h"


static uint16_t audio_out_buf[APP_AUDIO_BUFLEN_OUT];
static uint32_t t = 0;

bool APP_AudioOut_Init() {
	if(BSP_AUDIO_OUT_Init(APP_AUDIO_DEV_OUT, APP_AUDIO_INITVOL, APP_AUDIO_FREQ) != AUDIO_OK) {
		APP_View_Log("Audio Out init error");
		return false;
	}
	APP_View_Log("Audio Out init");

	if(BSP_AUDIO_OUT_Play(audio_out_buf,
	        APP_AUDIO_BUFLEN_OUT * sizeof(audio_out_buf[0])) != AUDIO_OK) {
		APP_View_Log("Audio Out play error");
		return false;
	}
	APP_View_Log("Audio Out play");

	return true;
}

// callbacks

// Manages the DMA Half Transfer complete interrupt.
void BSP_AUDIO_OUT_HalfTransfer_CallBack(void) {
	BSP_LED_Off(LED1);
	// TODO refill buffer
	// make_sound((uint16_t *)audiobuff, BUFF_LEN_DIV4);
	for(int i = 0; i < APP_AUDIO_BUFLEN_4; ++i) {
		audio_out_buf[i] = t++ % 2500;
	}
	BSP_LED_On(LED1);
}

// Calculates the remaining file size and new position of the pointer.
void BSP_AUDIO_OUT_TransferComplete_CallBack() {
	BSP_LED_Off(LED1);
	// TODO refill buffer
	// make_sound((uint16_t *)(audiobuff + BUFF_LEN_DIV2), BUFF_LEN_DIV4);
	for(int i = 0; i < APP_AUDIO_BUFLEN_4; ++i) {
		(audio_out_buf + (APP_AUDIO_BUFLEN_4 * sizeof(audio_out_buf[0])))[i] = t++ % 2500;
	}
	BSP_LED_On(LED1);
}

// Manages the DMA FIFO error interrupt.
void BSP_AUDIO_OUT_Error_CallBack(void) {
	APP_Err("BSP AUDIO OUT Error!");
}
