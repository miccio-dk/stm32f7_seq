/*
 * gui.h
 *
 *  Created on: Jun 2, 2016
 *      Author: miccio
 */

#ifndef GUI_H_
#define GUI_H_

#include "common.h"
#include "settings.h"
#include <string.h>


typedef enum {
	GUI_ELEM_FRAME,
	GUI_ELEM_LOG,
	GUI_ELEM_LABEL,
	GUI_ELEM_MENUBAR,
	GUI_ELEM_BUTTON,
	GUI_ELEM_SLIDER,
	GUI_ELEM_KNOB,
} Gui_Element_Type;

typedef enum {
	GUI_LAYOUT_ROWS,
	GUI_LAYOUT_COLS,
	GUI_LAYOUT_GRID,
} Gui_Layout_Type;

typedef enum {
	GUI_BUTTON_MOMENTARY,
	GUI_BUTTON_TOGGLE,
} Gui_Button_Type;

typedef struct Gui_Element Gui_Element;
typedef bool (*button_cb_t)(Gui_Element *);

typedef struct {
	uint32_t	color;
	uint8_t		thickness;
	uint8_t		space;
} Gui_Margin;

typedef struct {
	uint16_t	x;
	uint16_t	y;
} Gui_Elem_Position;

typedef struct {
	uint16_t		w;
	uint16_t		h;
} Gui_Elem_Size;

typedef struct {
	Gui_Elem_Position	pos;
	Gui_Elem_Size		size;
} Gui_Elem_BoundingBox;

typedef struct {
	uint32_t	color;
	Gui_Margin	margin;
	uint8_t rows_num;
	uint8_t cols_num;
} Gui_Element_Frame;

typedef char gui_log_line[APP_GUI_MAX_LOGSTRLEN];

typedef struct {
	uint32_t	color;
	Gui_Margin	margin;
	char		txt[APP_GUI_MAX_LABELLEN];
	gui_log_line * buff;
	uint16_t lines_num;
	uint8_t lines_size;
	uint8_t line_idx;
} Gui_Element_Log;

typedef struct {
	uint32_t	color;
	char		txt[APP_GUI_MAX_LABELLEN];
} Gui_Element_Label;

typedef struct {
	uint32_t	color;
	Gui_Margin	margin;
	char		txt[APP_GUI_MAX_LABELLEN];
	Gui_Button_Type type;
	bool		state;
	button_cb_t callback;
} Gui_Element_Button;

typedef struct {
	uint32_t	color;
	Gui_Margin	margin;
	char		txt[APP_GUI_MAX_LABELLEN];
	int16_t		min;
	int16_t		max;
	int16_t		val;
} Gui_Element_Numeric;

// inheritance in C?
typedef Gui_Element_Numeric  Gui_Element_Knob;
typedef Gui_Element_Numeric  Gui_Element_Slider;

struct Gui_Element {
	Gui_Element_Type		type;
	union {
		Gui_Element_Frame 	frame;
		Gui_Element_Log		log;
		Gui_Element_Label		label;
		Gui_Element_Button	button;
		Gui_Element_Numeric numeric;
		Gui_Element_Slider		slider;
		Gui_Element_Knob		knob;
	};
	Gui_Elem_BoundingBox	box;
	bool					active;
	bool					visible;
	bool					redraw;
	uint8_t				childr_num;
	Gui_Element *	childr[APP_GUI_MAX_CHILDR];
};

typedef struct {
	char					title[APP_GUI_MAX_LABELLEN];
	uint32_t				crc;
	uint8_t					elems_size;
	uint8_t					elems_num;
	Gui_Element *			elems;
	Gui_Elem_Size			size;
} Gui_Screen;

typedef enum {
	GUI_EVENT_PRESS,
	GUI_EVENT_RELEASE,
	GUI_EVENT_DRAG,
} Gui_Event_Type;

typedef struct {
	Gui_Event_Type type;
	Gui_Element * target;
	Gui_Elem_Position pos;
} Gui_Event;

typedef struct {
	uint8_t		events_num;
	Gui_Event	events[APP_GUI_MAX_EVENTS];
} Gui_Events;


// init functions
bool APP_Gui_Init();

// screen setup functions
Gui_Screen *  APP_Gui_GetScreen();
bool APP_Gui_SetScreen(Gui_Screen * screen);
void APP_Gui_InitScreen(Gui_Screen * screen, char * title, Gui_Element * elems_buf, uint8_t elems_buf_len);

// gui operation functions
bool APP_Gui_HandleEvents(Gui_Element * log);
bool APP_Gui_Draw();

// widget- related functions
Gui_Element * APP_Gui_AddFrame(Gui_Screen * screen, Gui_Element * parent, uint32_t color, Gui_Margin margin, uint8_t rows, uint8_t cols);
bool APP_Gui_DrawFrame(Gui_Element * elem);

Gui_Element * APP_Gui_AddLog(Gui_Screen * screen, Gui_Element * parent, uint32_t color, Gui_Margin margin, char * title, char (*buff)[APP_GUI_MAX_LOGSTRLEN], uint8_t size);
bool APP_Gui_AddLogLine(Gui_Element * log, char * txt);
bool APP_Gui_ResetLog(Gui_Element * log);
bool APP_Gui_DrawLog(Gui_Element * elem);

Gui_Element * APP_Gui_AddLabel(Gui_Screen * screen, Gui_Element * parent, uint32_t color, char * txt);
bool APP_Gui_DrawLabel(Gui_Element * elem);

Gui_Element * APP_Gui_AddButton(Gui_Screen * screen, Gui_Element * parent, uint32_t color, Gui_Margin margin, char * txt, Gui_Button_Type type, bool state, button_cb_t callback);
bool APP_Gui_DrawButton(Gui_Element * elem);

Gui_Element * APP_Gui_AddSlider(Gui_Screen * screen, Gui_Element * parent, uint32_t color, Gui_Margin margin, char * txt, int16_t min, int16_t max, int16_t val);
bool APP_Gui_DrawSlider(Gui_Element * elem);

Gui_Element * APP_Gui_AddKnob(Gui_Screen * screen, Gui_Element * parent, uint32_t color, Gui_Margin margin, char * txt, int16_t min, int16_t max, int16_t val);
bool APP_Gui_DrawKnob(Gui_Element * elem);


#endif /* GUI_H_ */
