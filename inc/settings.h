/*
 * settings.h
 *
 *  Created on: Jun 1, 2016
 *      Author: miccio
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

#include "common.h"

// audio
#define APP_AUDIO_INITVOL		50
#define APP_AUDIO_FREQ			I2S_AUDIOFREQ_44K
#define APP_AUDIO_DEV_IN		INPUT_DEVICE_DIGITAL_MICROPHONE_2
#define APP_AUDIO_DEV_OUT		OUTPUT_DEVICE_AUTO
#define APP_AUDIO_BUFLEN_4		0x200
#define APP_AUDIO_BUFLEN_IN		(4 * APP_AUDIO_BUFLEN_4)
#define APP_AUDIO_BUFLEN_OUT	(4 * APP_AUDIO_BUFLEN_4)

// gui
#define APP_GUI_MAX_EVENTS			TS_MAX_NB_TOUCH
#define APP_GUI_MAX_CHILDR			16
#define APP_GUI_MAX_LABELLEN		24
#define APP_GUI_MAX_LOGSTRLEN	48
#define APP_GUI_FONT						Font12
#define APP_GUI_FONTCOL					((uint32_t)0xFF000000)
#define APP_GUI_DISABLEDCOL			((uint32_t)0xFFA0A0A0)


// view
#define APP_VIEW_TXT_HEADER		"Sample audio test"
#define APP_VIEW_TXT_FOOTER		"Riccardo Miccini - 2016"

#endif /* SETTINGS_H_ */
