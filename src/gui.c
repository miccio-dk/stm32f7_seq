/*
 * gui.c
 *
 *  Created on: Jun 2, 2016
 *      Author: miccio
 */

#include "gui.h"
#include <stdlib.h>
#include <math.h>

static CRC_HandleTypeDef gui_crc_handle = {};
static Gui_Screen * gui_curr_screen = NULL;
static bool gui_initialized = false;
static uint32_t gui_curr_bgcol = 0xFFFFFFFF;

static bool APP_Gui__UpdateScreenSizes(Gui_Screen * screen);
static bool APP_Gui__SetupElem(Gui_Screen * screen, Gui_Element * parent, uint8_t * elem_idx);
static void APP_Gui__UpdateCRC(Gui_Screen * screen);
static Gui_Element * APP_Gui__FindTarget(uint16_t pos_x, uint16_t pos_y);
static bool  APP_Gui__GetEvents(Gui_Events * events);
static bool APP_Gui__ProcessEvents(Gui_Events * events, Gui_Element * log);

bool APP_Gui_Init() {
	// init periphs
	BSP_LED_Init(LED1);
	if(BSP_LCD_Init() != LCD_OK) {
		return false;
	}

	// init display
	BSP_LCD_LayerDefaultInit(0, LCD_FB_START_ADDRESS);
	BSP_LCD_SelectLayer(0);
	BSP_LCD_DisplayOn();
	BSP_LCD_Clear(0xFFFFFFFF);
	BSP_LCD_SetFont(&APP_GUI_FONT);

	// init touchscreen
	if(BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize()) != TS_OK) {
		APP_View_Log("Touchscreen init error");
		return false;
	}

	// init crc engine
	gui_crc_handle.Instance = CRC;
	gui_crc_handle.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
	gui_crc_handle.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
	gui_crc_handle.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
	gui_crc_handle.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
	gui_crc_handle.InputDataFormat = CRC_INPUTDATA_FORMAT_WORDS;
	if(HAL_CRC_Init(&gui_crc_handle) != HAL_OK) {
		APP_View_Log("CRC Engine init error");
		return false;
	}

	gui_initialized = true;
	return true;
}

Gui_Screen * APP_Gui_GetScreen() {
	return gui_curr_screen;
}

bool APP_Gui_SetScreen(Gui_Screen * screen) {
	// init check
	if(!gui_initialized) {
		return false;
	}

	if(!APP_Gui__UpdateScreenSizes(screen)) {
		return false;
	}

	gui_curr_screen = screen;

	return true;
}

void APP_Gui_InitScreen(Gui_Screen * screen, char * title, Gui_Element * elems_buf, uint8_t elems_buf_len) {
	strncpy(screen->title, title, APP_GUI_MAX_LABELLEN);
	screen->elems_size = elems_buf_len;
	screen->elems_num = 0;
	screen->elems = elems_buf;
	screen->size.w = BSP_LCD_GetXSize();
	screen->size.h = BSP_LCD_GetYSize();
	APP_Gui__UpdateCRC(screen);
}


bool APP_Gui_HandleEvents(Gui_Element * log) {
	Gui_Events events;

	if(!APP_Gui__GetEvents(&events)) {
		return false;
	}
	if(!APP_Gui__ProcessEvents(&events, log)) {
		return false;
	}
	return true;
}

bool APP_Gui_Draw() {
	// init check
	if(!gui_initialized || (gui_curr_screen == NULL)) {
		return false;
	}

	bool res = true;

	// TODO draw title
	for(uint8_t i = 0; i < gui_curr_screen->elems_num; ++i) {
		if(gui_curr_screen->elems[i].redraw) {
			bool res_tmp;
			switch(gui_curr_screen->elems[i].type) {
			case GUI_ELEM_FRAME:
				res_tmp = APP_Gui_DrawFrame(&(gui_curr_screen->elems[i]));
				break;
			case GUI_ELEM_LOG:
				res_tmp = APP_Gui_DrawLog(&(gui_curr_screen->elems[i]));
				break;
			case GUI_ELEM_LABEL:
				res_tmp = APP_Gui_DrawLabel(&(gui_curr_screen->elems[i]));
				break;
			case GUI_ELEM_BUTTON:
				res_tmp = APP_Gui_DrawButton(&(gui_curr_screen->elems[i]));
				break;
			case GUI_ELEM_SLIDER:
				res_tmp = APP_Gui_DrawSlider(&(gui_curr_screen->elems[i]));
				break;
			case GUI_ELEM_KNOB:
				res_tmp = APP_Gui_DrawKnob(&(gui_curr_screen->elems[i]));
				break;
			default:
				res_tmp = false;
				break;
			}
			gui_curr_screen->elems[i].redraw = !res_tmp;
			res = res && res_tmp;
		}
	}

	return res;
}


Gui_Element * APP_Gui_AddFrame(Gui_Screen * screen, Gui_Element * parent, uint32_t color, Gui_Margin margin, uint8_t rows,
		uint8_t cols) {
	// init check
	if(!gui_initialized || (gui_curr_screen == NULL)) {
		return false;
	}

	uint8_t idx;
	if(!APP_Gui__SetupElem(screen, parent, &idx)) {
		return NULL;
	}

	// apply basic properties
	screen->elems[idx].type = GUI_ELEM_FRAME;
	screen->elems[idx].frame.color = color;
	screen->elems[idx].frame.margin = margin;
	screen->elems[idx].frame.rows_num = rows;
	screen->elems[idx].frame.cols_num = cols;
	APP_Gui__UpdateCRC(screen);
	return &(screen->elems[idx]);
}

Gui_Element * APP_Gui_AddLog(Gui_Screen * screen, Gui_Element * parent, uint32_t color, Gui_Margin margin, char * title, char (*buff)[APP_GUI_MAX_LOGSTRLEN], uint8_t size) {
	// init check
	if(!gui_initialized || (gui_curr_screen == NULL)) {
		return NULL;
	}

	uint8_t idx;
	if(!APP_Gui__SetupElem(screen, parent, &idx)) {
		return NULL;
	}

	// apply basic properties
	screen->elems[idx].type = GUI_ELEM_LOG;
	screen->elems[idx].log.color = color;
	screen->elems[idx].log.margin = margin;
	screen->elems[idx].log.buff = buff;
	screen->elems[idx].log.lines_size = size;
	screen->elems[idx].log.lines_num = 0;
	screen->elems[idx].log.line_idx = 0;
	strncpy(screen->elems[idx].log.txt, title, APP_GUI_MAX_LABELLEN);
	for(uint8_t i = 0; i < size; i++) {
		screen->elems[idx].log.buff[i][0] = 0;
	}

	APP_Gui__UpdateCRC(screen);
	return &(screen->elems[idx]);
}

bool  APP_Gui_AddLogLine(Gui_Element * log, char * txt) {
	// init check
	if(!gui_initialized || (gui_curr_screen == NULL)) {
		return false;
	}

	if(log == NULL) {
		return false;
	}

	log->log.lines_num++;
	strncpy(log->log.buff[log->log.line_idx], txt, APP_GUI_MAX_LOGSTRLEN);
	log->log.line_idx = (log->log.line_idx + 1) % log->log.lines_size;
	log->redraw = true;

	return true;
}

bool APP_Gui_ResetLog(Gui_Element * log) {
	// init check
	if(!gui_initialized || (gui_curr_screen == NULL)) {
		return false;
	}

	if(log == NULL) {
		return false;
	}

	log->log.lines_num = 0;
	log->log.line_idx = 0;
	for(uint8_t i = 0; i < log->log.lines_size; i++) {
		log->log.buff[i][0] = 0;
	}
	log->redraw = true;

	return true;
}

Gui_Element * APP_Gui_AddLabel(Gui_Screen * screen, Gui_Element * parent, uint32_t color, char * txt) {
	// init check
	if(!gui_initialized || (gui_curr_screen == NULL)) {
		return NULL;
	}

	uint8_t idx;
	if(!APP_Gui__SetupElem(screen, parent, &idx)) {
		return NULL;
	}

	// apply basic properties
	screen->elems[idx].type = GUI_ELEM_LABEL;
	strncpy(screen->elems[idx].label.txt, txt, APP_GUI_MAX_LABELLEN);
	screen->elems[idx].label.color = color;

	APP_Gui__UpdateCRC(screen);
	return &(screen->elems[idx]);
}

Gui_Element * APP_Gui_AddButton(Gui_Screen * screen, Gui_Element * parent, uint32_t color, Gui_Margin margin, char * txt,
		Gui_Button_Type type, bool state, button_cb_t callback) {
	// init check
	if(!gui_initialized || (gui_curr_screen == NULL)) {
		return NULL;
	}

	uint8_t idx;
	if(!APP_Gui__SetupElem(screen, parent, &idx)) {
		return NULL;
	}

	// apply basic properties
	screen->elems[idx].type = GUI_ELEM_BUTTON;
	screen->elems[idx].button.color = color;
	screen->elems[idx].button.margin = margin;
	strncpy(screen->elems[idx].button.txt, txt, APP_GUI_MAX_LABELLEN);
	screen->elems[idx].button.type = type;
	screen->elems[idx].button.state = state;
	screen->elems[idx].button.callback = callback;

	APP_Gui__UpdateCRC(screen);
	return &(screen->elems[idx]);
}

Gui_Element * APP_Gui_AddSlider(Gui_Screen * screen, Gui_Element * parent, uint32_t color, Gui_Margin margin, char * txt,
		int16_t min, int16_t max, int16_t val) {
	// init check
	if(!gui_initialized || (gui_curr_screen == NULL)) {
		return NULL;
	}

	uint8_t idx;
	if(!APP_Gui__SetupElem(screen, parent, &idx)) {
		return NULL;
	}

	// apply basic properties
	screen->elems[idx].type = GUI_ELEM_SLIDER;
	screen->elems[idx].slider.color = color;
	screen->elems[idx].slider.margin = margin;
	strncpy(screen->elems[idx].slider.txt, txt, APP_GUI_MAX_LABELLEN);
	screen->elems[idx].slider.min = min;
	screen->elems[idx].slider.max = max;
	screen->elems[idx].slider.val = val;

	APP_Gui__UpdateCRC(screen);
	return &(screen->elems[idx]);
}

Gui_Element * APP_Gui_AddKnob(Gui_Screen * screen, Gui_Element * parent, uint32_t color, Gui_Margin margin, char * txt,
		int16_t min, int16_t max, int16_t val) {
	// init check
	if(!gui_initialized || (gui_curr_screen == NULL)) {
		return NULL;
	}

	uint8_t idx;
	if(!APP_Gui__SetupElem(screen, parent, &idx)) {
		return NULL;
	}

	// apply basic properties
	screen->elems[idx].type = GUI_ELEM_KNOB;
	screen->elems[idx].knob.color = color;
	screen->elems[idx].knob.margin = margin;
	strncpy(screen->elems[idx].knob.txt, txt, APP_GUI_MAX_LABELLEN);
	screen->elems[idx].knob.min = min;
	screen->elems[idx].knob.max = max;
	screen->elems[idx].knob.val = val;

	APP_Gui__UpdateCRC(screen);
	return &(screen->elems[idx]);
}

bool APP_Gui_DrawFrame(Gui_Element * elem) {
	// init check
	if(!gui_initialized || (gui_curr_screen == NULL)) {
		return false;
	}

	// visibility check
	if(!elem->visible) {
		return true;
	}

	// draw margins
	BSP_LCD_SetTextColor(elem->frame.margin.color);
	BSP_LCD_FillRect(elem->box.pos.x, elem->box.pos.y, elem->box.size.w, elem->box.size.h);
	// draw background
	BSP_LCD_SetTextColor(elem->frame.color);
	BSP_LCD_FillRect(elem->box.pos.x + elem->frame.margin.thickness, elem->box.pos.y + elem->frame.margin.thickness,
			elem->box.size.w - (elem->frame.margin.thickness * 2), elem->box.size.h - (elem->frame.margin.thickness * 2));
	gui_curr_bgcol = elem->frame.color;

	return true;
}

bool APP_Gui_DrawLog(Gui_Element * elem) {
	// init check
	if(!gui_initialized || (gui_curr_screen == NULL)) {
		return false;
	}

	// visibility check
	if(!elem->visible) {
		return true;
	}

	// erase area
	BSP_LCD_SetTextColor(gui_curr_bgcol);
	BSP_LCD_FillRect(elem->box.pos.x, elem->box.pos.y, elem->box.size.w, elem->box.size.h);

	// draw title
	BSP_LCD_SetTextColor(APP_GUI_FONTCOL);
	BSP_LCD_FillRect(elem->box.pos.x, elem->box.pos.y, elem->box.size.w, APP_GUI_FONT.Height);
	BSP_LCD_SetBackColor(APP_GUI_FONTCOL);
	BSP_LCD_SetTextColor(gui_curr_bgcol);
	BSP_LCD_DisplayStringAt(elem->box.pos.x, elem->box.pos.y,
			(uint8_t *)(elem->log.txt), LEFT_MODE);

	// draw log lines
	BSP_LCD_SetTextColor(APP_GUI_FONTCOL);
	BSP_LCD_SetBackColor(gui_curr_bgcol);
	char buff[APP_GUI_MAX_LABELLEN];
	uint8_t lines_realnum = min(elem->log.lines_size, elem->log.lines_num);
	for(uint8_t i = 0; i < lines_realnum; i++) {
		uint8_t j = lines_realnum - i - 1;
		// TODO print line correctly
		snprintf(buff, APP_GUI_MAX_LABELLEN, "%4d", elem->log.lines_num - lines_realnum + j);
		BSP_LCD_DisplayStringAt(elem->box.pos.x,
				elem->box.pos.y + APP_GUI_FONT.Height * (j + 1),
				(uint8_t *)(buff), LEFT_MODE);

		BSP_LCD_DisplayStringAt(elem->box.pos.x + APP_GUI_FONT.Width * 5,
				elem->box.pos.y + APP_GUI_FONT.Height * (j + 1),
//				(uint8_t *)(elem->log.buff[(elem->log.line_idx + elem->log.lines_size - j - 2) % elem->log.lines_size]), LEFT_MODE);
				(uint8_t *)(elem->log.buff[(elem->log.lines_size + elem->log.line_idx - 1 - i) % elem->log.lines_size]), LEFT_MODE);
	}

	return true;
}

bool APP_Gui_DrawLabel(Gui_Element * elem) {
	// init check
	if(!gui_initialized || (gui_curr_screen == NULL)) {
		return false;
	}

	// visibility check
	if(!elem->visible) {
		return true;
	}

	// draw text
	BSP_LCD_SetTextColor(elem->label.color);
	BSP_LCD_SetBackColor(gui_curr_bgcol);
	BSP_LCD_DisplayStringAt(elem->box.pos.x + (elem->box.size.w / 2) - gui_curr_screen->size.w / 2,
			elem->box.pos.y + (elem->box.size.h - APP_GUI_FONT.Height) / 2, (uint8_t *)(elem->label.txt), CENTER_MODE);

	return true;
}

bool APP_Gui_DrawButton(Gui_Element * elem) {
	// init check
	if(!gui_initialized || (gui_curr_screen == NULL)) {
		return false;
	}

	// visibility check
	if(!elem->visible) {
		return true;
	}

	// draw margins
	BSP_LCD_SetTextColor(elem->button.margin.color);
	BSP_LCD_FillRect(elem->box.pos.x, elem->box.pos.y, elem->box.size.w, elem->box.size.h);
	// draw background
	if(elem->button.state) {
		BSP_LCD_SetTextColor(elem->button.color);
	} else {
		BSP_LCD_SetTextColor(APP_GUI_DISABLEDCOL);
	}
	BSP_LCD_FillRect(elem->box.pos.x + elem->button.margin.thickness, elem->box.pos.y + elem->button.margin.thickness,
			elem->box.size.w - (elem->button.margin.thickness * 2), elem->box.size.h - (elem->button.margin.thickness * 2));
	// draw text
	BSP_LCD_SetTextColor(APP_GUI_FONTCOL);
	BSP_LCD_SetBackColor(elem->button.color);
	BSP_LCD_DisplayStringAt(elem->box.pos.x + (elem->box.size.w - APP_GUI_FONT.Width * strlen(elem->button.txt)) / 2,
			elem->box.pos.y + (elem->box.size.h - APP_GUI_FONT.Height) / 2, (uint8_t *)(elem->button.txt), LEFT_MODE);

	return true;
}

bool APP_Gui_DrawSlider(Gui_Element * elem) {
	// init check
	if(!gui_initialized || (gui_curr_screen == NULL)) {
		return false;
	}

	// visibility check
	if(!elem->visible) {
		return true;
	}

	// TODO show vertical or horizontal
	if(elem->box.size.h >= elem->box.size.w) {
		;
	} else {
		;
	}

	// empty area
	BSP_LCD_SetTextColor(gui_curr_bgcol);
	BSP_LCD_FillRect(elem->box.pos.x, elem->box.pos.y,
			elem->box.size.w, elem->box.size.h);
	// draw margins
	uint16_t bar_width = elem->slider.margin.space;
	BSP_LCD_SetTextColor(elem->slider.margin.color);
	BSP_LCD_FillRect(elem->box.pos.x + (elem->box.size.w - bar_width) / 2,
			elem->box.pos.y + APP_GUI_FONT.Height * 2,
			bar_width,
			elem->box.size.h - APP_GUI_FONT.Height * 3);
	// draw background
	BSP_LCD_SetTextColor(elem->slider.color);
	BSP_LCD_FillRect(elem->box.pos.x + (elem->box.size.w - bar_width) / 2 + elem->slider.margin.thickness,
			elem->box.pos.y + APP_GUI_FONT.Height * 2 + elem->slider.margin.thickness,
			bar_width - (elem->slider.margin.thickness * 2),
			elem->box.size.h - APP_GUI_FONT.Height * 3 - (elem->slider.margin.thickness * 2));

	// draw text: name, val, min, max
	BSP_LCD_SetTextColor(APP_GUI_FONTCOL);
	BSP_LCD_SetBackColor(elem->slider.color);
	char buff[APP_GUI_MAX_LABELLEN];

	// name
	BSP_LCD_DisplayStringAt(elem->box.pos.x + (elem->box.size.w - APP_GUI_FONT.Width * strlen(elem->slider.txt)) / 2,
			elem->box.pos.y, (uint8_t *)(elem->slider.txt), LEFT_MODE);

	// val
	snprintf(buff, APP_GUI_MAX_LABELLEN, "%d", elem->slider.val);
	float pos_perc = 1 - (float)(elem->slider.val - elem->slider.min) / (elem->slider.max - elem->slider.min);
	uint16_t pos_x = elem->box.pos.x + (elem->box.size.w - APP_GUI_FONT.Width * strlen(buff)) / 2;
	uint16_t pos_y = elem->box.pos.y + APP_GUI_FONT.Height * 2 + (elem->box.size.h - APP_GUI_FONT.Height * 4) * pos_perc;
	BSP_LCD_DisplayStringAt(pos_x, pos_y,
			(uint8_t *)(buff), LEFT_MODE);



	BSP_LCD_SetBackColor(gui_curr_bgcol);
	// min
	snprintf(buff, APP_GUI_MAX_LABELLEN, "%d", elem->slider.min);
	BSP_LCD_DisplayStringAt(elem->box.pos.x + (elem->box.size.w - APP_GUI_FONT.Width * strlen(buff)) / 2,
			elem->box.pos.y + elem->box.size.h - APP_GUI_FONT.Height, (uint8_t *)(buff), LEFT_MODE);
	// max
	snprintf(buff, APP_GUI_MAX_LABELLEN, "%d", elem->slider.max);
	BSP_LCD_DisplayStringAt(elem->box.pos.x + (elem->box.size.w - APP_GUI_FONT.Width * strlen(buff)) / 2,
			elem->box.pos.y +  APP_GUI_FONT.Height, (uint8_t *)(buff), LEFT_MODE);

	return true;
}

bool APP_Gui_DrawKnob(Gui_Element * elem) {
	// init check
	if(!gui_initialized || (gui_curr_screen == NULL)) {
		return false;
	}

	// visibility check
	if(!elem->visible) {
		return true;
	}

	uint16_t radius = min(elem->box.size.w, elem->box.size.h) / 2 - 1;
	uint16_t center_x = elem->box.pos.x + elem->box.size.w / 2;
	uint16_t center_y = elem->box.pos.y + elem->box.size.h / 2;

	// draw circle
	BSP_LCD_SetTextColor(elem->knob.margin.color);
	BSP_LCD_FillCircle(center_x, center_y, radius);
	uint16_t radius_inn = radius - elem->knob.margin.thickness;
	BSP_LCD_SetTextColor(elem->knob.color);
	BSP_LCD_FillCircle(center_x, center_y, radius_inn);

	// draw text: name, val, min, max
	// TODO draw labels based on circle position
	BSP_LCD_SetTextColor(APP_GUI_FONTCOL);
	BSP_LCD_SetBackColor(elem->knob.color);
	char buff[APP_GUI_MAX_LABELLEN];

	// name
	BSP_LCD_DisplayStringAt(center_x - (APP_GUI_FONT.Width * strlen(elem->knob.txt)) / 2,
			elem->box.pos.y,
			(uint8_t *)(elem->knob.txt), LEFT_MODE);

	// val
	snprintf(buff, APP_GUI_MAX_LABELLEN, "%d", elem->knob.val);
	BSP_LCD_DisplayStringAt(center_x - (APP_GUI_FONT.Width * strlen(buff)) / 2,
			elem->box.pos.y + (elem->box.size.h - APP_GUI_FONT.Height) / 2,
			(uint8_t *)(buff), LEFT_MODE);

	if(elem->box.size.h >= elem->box.size.w) {
		;
	} else {
		;
	}
	BSP_LCD_SetBackColor(gui_curr_bgcol);

	// min
	snprintf(buff, APP_GUI_MAX_LABELLEN, "%d", elem->knob.min);
	BSP_LCD_DisplayStringAt(center_x - radius,
			elem->box.pos.y + elem->box.size.h - APP_GUI_FONT.Height,
			(uint8_t *)(buff), LEFT_MODE);

	// max
	snprintf(buff, APP_GUI_MAX_LABELLEN, "%d", elem->knob.max);
	BSP_LCD_DisplayStringAt(center_x + radius - APP_GUI_FONT.Width * strlen(buff),
			elem->box.pos.y + elem->box.size.h - APP_GUI_FONT.Height,
			(uint8_t *)(buff), LEFT_MODE);

	return true;
}

bool APP_Gui__UpdateScreenSizes(Gui_Screen * screen) {
	// loop through elements
	for(uint8_t i = 0; i < screen->elems_num; i++) {
		/*
		 if() {
		 } else {
		 }
		 screen->elems[i]->active
		 screen->elems[i]->
		 */
	}

	APP_Gui__UpdateCRC(screen);
	return true;
}

// insert element into screen and set up size and generic properties
// return true if succesfull,
bool APP_Gui__SetupElem(Gui_Screen * screen, Gui_Element * parent, uint8_t * elem_idx) {
	// get index of new element
	uint8_t idx_elem = screen->elems_num;

	// boundary check
	if((idx_elem + 1) > screen->elems_size) {
		return false;
	}

	// if parent is NULL
	if(parent == NULL) {
		// and if the current element is the first (its parent is root)
		if(idx_elem == 0) {
			// set box to screen size
			screen->elems[idx_elem].box.size.w = screen->size.w;
			screen->elems[idx_elem].box.size.h = screen->size.h;
			screen->elems[idx_elem].box.pos.x = 0;
			screen->elems[idx_elem].box.pos.y = 0;
		// if parent is not root
		 } else {
			return false;
		}
	// if parent is not NULL
	} else {
		bool parent_found = false;
		// for each element in the screen
		for(uint i = 0; i < screen->elems_num; ++i) {
			// if it's the parent
			if(&(screen->elems[i]) == parent) {
				// if it's a frame
				if(parent->type == GUI_ELEM_FRAME) {
					parent_found = true;

					// add children to element
					uint8_t idx_child = parent->childr_num;
					if((idx_child + 1) > (parent->frame.cols_num * parent->frame.rows_num)) {
						return false;
					}
					if((idx_child + 1) > APP_GUI_MAX_CHILDR) {
						return false;
					}
					parent->childr[idx_child] = &(screen->elems[screen->elems_num]);

					// calculate position and size - inner frame
					// TODO allow manual positioning, maybe with some indexing at element level?
					// TODO optimize by calculating cell size at frame level
					uint8_t space = parent->frame.margin.space;
					uint8_t thickness = parent->frame.margin.thickness;
					float size_w = (float)(parent->box.size.w
							- (thickness * 2)
							- (space * (parent->frame.cols_num + 1)))
							/ parent->frame.cols_num;
					float size_h = (float)(parent->box.size.h
							- (thickness * 2)
							- (space * (parent->frame.rows_num + 1)))
							/ parent->frame.rows_num;

					screen->elems[idx_elem].box.size.w = size_w;
					screen->elems[idx_elem].box.size.h = size_h;

					screen->elems[idx_elem].box.pos.x = parent->box.pos.x
							+ thickness
							+ (space * (idx_child % parent->frame.cols_num + 1))
							+ (size_w * (idx_child % parent->frame.cols_num));
					screen->elems[idx_elem].box.pos.y = parent->box.pos.y
							+ thickness
							+ (space * (idx_child / parent->frame.cols_num + 1))
							+ (size_h * (idx_child / parent->frame.cols_num));
					// break loop
					break;
				// if parent is not frame
				} else {
					return false;
				}
			}
		}
		// if parent frame not found
		if(!parent_found) {
			return false;
		}
	 }

	parent->childr_num++;

	screen->elems[idx_elem].active = true;
	screen->elems[idx_elem].visible = true;
	screen->elems[idx_elem].redraw = true;
	screen->elems[idx_elem].childr_num = 0;
	*elem_idx = idx_elem;
	// increment global element count (has to be done here because the function uses it)
	screen->elems_num++;
	return true;
}

void APP_Gui__UpdateCRC(Gui_Screen * screen) {
	uint32_t crc = 0;
	// apply crc to screen
	crc = HAL_CRC_Accumulate(&gui_crc_handle, (uint32_t *)screen, sizeof(Gui_Screen));

	// loop through elements
	for(uint8_t i = 0; i < screen->elems_num; i++) {
		crc += 0;
		// TODO implement above
	}

	screen->crc = crc;
}

Gui_Element * APP_Gui__FindTarget(uint16_t pos_x, uint16_t pos_y) {
	// check screen boundaries
	if((pos_x > gui_curr_screen->size.w) || (pos_y > gui_curr_screen->size.h)) {
		return NULL;
	}

	// init check
	if(!gui_initialized || (gui_curr_screen == NULL)) {
		return false;
	}

	// loop through elements
	for(uint8_t i = 0; i < gui_curr_screen->elems_num; i++) {
		uint8_t j = gui_curr_screen->elems_num - i - 1;

		// check element type
		if((gui_curr_screen->elems[j].type == GUI_ELEM_BUTTON)
				|| (gui_curr_screen->elems[j].type == GUI_ELEM_KNOB)
				|| (gui_curr_screen->elems[j].type == GUI_ELEM_SLIDER)
				|| (gui_curr_screen->elems[j].type == GUI_ELEM_LOG)) {
			// check position
			if((pos_x > gui_curr_screen->elems[j].box.pos.x)
					&& (pos_x < (gui_curr_screen->elems[j].box.pos.x + gui_curr_screen->elems[j].box.size.w))
					&& (pos_y > gui_curr_screen->elems[j].box.pos.y)
					&& (pos_y < (gui_curr_screen->elems[j].box.pos.y + gui_curr_screen->elems[j].box.size.h))) {
				// return effected element
				return &gui_curr_screen->elems[j];
			}
		}
	}

	return NULL;
}

bool APP_Gui__GetEvents(Gui_Events * events) {
	if(events  == NULL) {
		return false;
	}

	// read touchscreen state
	TS_StateTypeDef ts_state;
	if(BSP_TS_GetState(&ts_state) != TS_OK) {
		return false;
	}

	// loop through touchscreen events
	for(uint8_t touch_idx = 0; touch_idx < ts_state.touchDetected; touch_idx++) {
		Gui_Event curr_event;
		// find affected element
		curr_event.target = APP_Gui__FindTarget(ts_state.touchX[touch_idx], ts_state.touchY[touch_idx]);

		// set event type
		switch(ts_state.touchEventId[touch_idx]) {
		case TOUCH_EVENT_PRESS_DOWN:
			curr_event.type = GUI_EVENT_PRESS;
			break;
		case TOUCH_EVENT_LIFT_UP:
			curr_event.type = GUI_EVENT_RELEASE;
			break;
		case TOUCH_EVENT_CONTACT:
			curr_event.type = GUI_EVENT_DRAG;
			break;
		case TOUCH_EVENT_NO_EVT:
			curr_event.type = GUI_EVENT_RELEASE;
			break;
		default:
			break;
		}

		// add properties
		curr_event.pos.x = ts_state.touchX[touch_idx];
		curr_event.pos.y = ts_state.touchY[touch_idx];

		events->events[touch_idx] = curr_event;
	}

	events->events_num = ts_state.touchDetected;
	return true;
}

bool APP_Gui__ProcessEvents(Gui_Events * events, Gui_Element * log) {
	static Gui_Events events_old;
	bool done_something = false;

	if(events  == NULL) {
		return false;
	}

	// string arrays for printing enums: event type, elem type
	char *event_type_str[] = {"PRESS", "RELEASE", "DRAG"};

	// process events
	for(uint8_t i = 0; i < events->events_num; ++i) {
		Gui_Event * curr_event = &events-> events[i];

		switch(curr_event->type) {
		case GUI_EVENT_PRESS:
			if(curr_event->target != NULL) {
				if(curr_event->target->type == GUI_ELEM_BUTTON) {
					if(curr_event->target->button.type == GUI_BUTTON_MOMENTARY) {
						curr_event->target->button.state = true;
						curr_event->target->redraw = true;
					} else if(curr_event->target->button.type == GUI_BUTTON_TOGGLE) {
						curr_event->target->button.state =  !curr_event->target->button.state;
						curr_event->target->redraw = true;
						if(curr_event->target->button.callback != NULL) {
							curr_event->target->button.callback(curr_event->target);
						}
					}
					done_something = true;
				} else if(curr_event->target->type == GUI_ELEM_LOG) {
					APP_Gui_ResetLog(curr_event->target);
					// done_something = true;
				} else if(curr_event->target->type == GUI_ELEM_MENUBAR) {
					// TODO implement MENUBAR :D
					done_something = true;
				}
			}
			break;
		case GUI_EVENT_RELEASE:
			if(curr_event->target != NULL) {
				if(curr_event->target->type == GUI_ELEM_BUTTON) {
					if(curr_event->target->button.type == GUI_BUTTON_MOMENTARY) {
						curr_event->target->button.state = false;
						curr_event->target->redraw = true;
						done_something = true;
					}
				}
			}
			break;
		case GUI_EVENT_DRAG:
			if(curr_event->target != NULL) {
				if((curr_event->target->type == GUI_ELEM_SLIDER)
						|| (curr_event->target->type == GUI_ELEM_KNOB)) {
					// TODO use correct axis for position calculation, implement multitouch suport
					// calculate change in position and clamp value
					int16_t delta_pos = events_old.events[i].pos.y - curr_event->pos.y;
					curr_event->target->numeric.val += (delta_pos * abs(delta_pos + 1)) / 2;
					if(curr_event->target->numeric.val > curr_event->target->numeric.max) {
						curr_event->target->numeric.val = curr_event->target->numeric.max;
					} else if(curr_event->target->numeric.val < curr_event->target->numeric.min) {
						curr_event->target->numeric.val = curr_event->target->numeric.min;
					}
					curr_event->target->redraw = true;
					done_something = true;
				}
			}
		default:
			break;
		}

		// add log line
		if(done_something) {
			char buff[APP_GUI_MAX_LOGSTRLEN];
			snprintf(buff, APP_GUI_MAX_LOGSTRLEN, "Event %s @ %s", event_type_str[curr_event->type], (curr_event->target == NULL ? "NULL" : curr_event->target->button.txt));
			APP_Gui_AddLogLine(log, buff);
		}
	}


	events_old = *events;
	return true;
}
