/*
 * view.c
 *
 *  Created on: Jun 1, 2016
 *      Author: miccio
 */

#include "view.h"

static uint32_t col_bg = 0xFFFFFFFF;
static uint32_t col_txt = 0xFF101010;
static uint32_t col_btn = 0xFF8080FF;
static Gui_Margin view_margins_bg = {0x00000000, 0, 5};
static Gui_Margin view_margins_none = {0x00000000, 0, 0};
static Gui_Margin view_margins_ctrl = {0xFF202020, 1, 10};
static Gui_Margin view_margins_frms = {0xFFD0D0D0, 2, 3};

static Gui_Screen view_screen_test1 = {};
static Gui_Element view_screen_test1_elems[64];

static Gui_Element * log = NULL;
static char view_log1_buff[8][APP_GUI_MAX_LOGSTRLEN];

bool APP_View_Init() {
	if(!APP_Gui_Init()) {
		return false;
	}

	APP_Gui_InitScreen(&view_screen_test1, "Screen 1", view_screen_test1_elems, 64);
	if(!APP_Gui_SetScreen(&view_screen_test1)) {
		return false;
	}

	// root frame
	Gui_Element * frame_bg = APP_Gui_AddFrame(&view_screen_test1, NULL, col_bg, view_margins_bg, 2, 1);
	if(frame_bg == NULL) {
		return false;
	}

	// layout frames
	Gui_Element * frame_inn_up = APP_Gui_AddFrame(&view_screen_test1, frame_bg, col_bg, view_margins_none, 1, 3);
	if(frame_inn_up == NULL) {
		return false;
	}
	Gui_Element * frame_inn_down = APP_Gui_AddFrame(&view_screen_test1, frame_bg, col_bg, view_margins_none, 1, 2);
	if(frame_inn_down == NULL) {
		return false;
	}

	// push-buttons frame
	Gui_Element * frame_bigbtn = APP_Gui_AddFrame(&view_screen_test1, frame_inn_up, col_bg, view_margins_frms, 2, 2);
	if(frame_bigbtn == NULL) {
		return false;
	}
	for(uint8_t i = 0; i <  (frame_bigbtn->frame.rows_num * frame_bigbtn->frame.cols_num); ++i) {
		char str[APP_GUI_MAX_LABELLEN];

		sprintf(str, "Button %d", (i + 1));
		Gui_Element * btn = APP_Gui_AddButton(&view_screen_test1, frame_bigbtn, (col_btn & 0xffffff00) | ((i + 1) * 32),
				view_margins_ctrl, str, GUI_BUTTON_MOMENTARY, false, NULL);
		if(btn == NULL) {
			return false;
		}
	}

	// sliders frame
	Gui_Element * frame_slr = APP_Gui_AddFrame(&view_screen_test1, frame_inn_up, col_bg, view_margins_frms, 1, 3);
	if(frame_slr == NULL) {
		return false;
	}
	for(uint8_t i = 0; i < (frame_slr->frame.rows_num * frame_slr->frame.cols_num); ++i) {
		char str[APP_GUI_MAX_LABELLEN];

		sprintf(str, "Slr%02d", (i + 1));
		Gui_Element * btn = APP_Gui_AddSlider(&view_screen_test1, frame_slr, (col_btn & 0xffffff00) | ((i + 1) * 32),
				view_margins_ctrl, str, -500, 1000, 0);
		if(btn == NULL) {
			return false;
		}
	}

	// big knob frame
	Gui_Element * frame_bigknb = APP_Gui_AddFrame(&view_screen_test1, frame_inn_up, col_bg, view_margins_frms, 1, 1);
	if(frame_bigknb == NULL) {
		return false;
	}
	char str[APP_GUI_MAX_LABELLEN] = "Big Knob";
	Gui_Element * bigknb = APP_Gui_AddKnob(&view_screen_test1, frame_bigknb, (col_btn & 0xffffff00),
			view_margins_ctrl, str, -100, 100, 0);
	if(bigknb == NULL) {
		return false;
	}

	// buttons and knobs frame
	Gui_Element * frame_btn = APP_Gui_AddFrame(&view_screen_test1, frame_inn_down, col_bg, view_margins_frms, 2, 4);
	if(frame_btn == NULL) {
		return false;
	}
	for(uint8_t i = 0; i < (frame_btn->frame.rows_num * frame_btn->frame.cols_num); ++i) {
		char str[APP_GUI_MAX_LABELLEN];

		if(i < frame_btn->frame.cols_num) {
			sprintf(str, "BTN%d", (i % 8) + 1);
			Gui_Element * btn = APP_Gui_AddButton(&view_screen_test1, frame_btn, (col_btn & 0xffffff00) | ((i / 8) * 100 + (i % 8) * 5),
						view_margins_ctrl, str, GUI_BUTTON_TOGGLE, false, NULL);
			if(btn == NULL) {
				return false;
			}

		} else {
			sprintf(str, "KNB%d", (i % 8) + 1);
			Gui_Element * knb = APP_Gui_AddKnob(&view_screen_test1, frame_btn, (col_btn & 0xffffff00) | (i / 8) * 100 + (i % 8) * 5,
						view_margins_ctrl, str, 0, 50, 25);
			if(knb == NULL) {
				return false;
			}
		}
	}

	// log frame
	Gui_Element * frame_log = APP_Gui_AddFrame(&view_screen_test1, frame_inn_down, col_bg, view_margins_frms, 1, 1);
	if(frame_log == NULL) {
		return false;
	}
	log = APP_Gui_AddLog(&view_screen_test1, frame_log, col_txt, view_margins_frms, "Log Frame 1", view_log1_buff, 8);
	if(log == NULL) {
		return false;
	}




/*	for(uint8_t k = 0; k < 4; ++k) {
		Gui_Element * frame_bg_inn = APP_Gui_AddFrame(&view_screen_test1, frame_bg,
				(col_bg & 0xffff00ff) | ((255 - k * 25) << 8), view_margins_frms, 2, 2);
		if(frame_bg_inn == NULL) {
			return false;
		}

		if(k == 0) {

		} else if(k == 1) {

		}
		for(uint8_t i = 0; i < 2; ++i) {
			char str[APP_GUI_MAX_LABELLEN];

			sprintf(str, "Label #%d", (k * 2 + i + 1));
			Gui_Element * label = APP_Gui_AddLabel(&view_screen_test1, frame_bg_inn, col_txt, str);
			if(label == NULL) {
				return false;
			}

			sprintf(str, "Button #%d", (k * 2 + i + 1));
			Gui_Element * btn = APP_Gui_AddButton(&view_screen_test1, frame_bg_inn,
					(col_btn & 0xffffff00) | ((k * 2 + i + 1) * 15), view_margins_ctrl, str, GUI_BUTTON_MOMENTARY, false, NULL);
			if(btn == NULL) {
				return false;
			}
		}
	}
*/
	return true;
}

bool APP_View_Update() {
	bool result = true;

	// handle events and update screen
	result = result && APP_Gui_HandleEvents(log);
	result = result && APP_Gui_Draw();

	return result;
}

void APP_View_Log(char * str, ...) {
	va_list args;
	va_start(args, str);
	printf(str, args);
	printf("\n");
	va_end(args);
}
