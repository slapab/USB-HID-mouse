#ifndef _LCD_LAYOUT_
#define _LCD_LAYOUT_

#include <stdint.h>
#include "ts_api_extends.h"

#define LCD_SIZE_Y 320
#define LCD_SIZE_X 240


/* RECTANGLE */
#define RECT_X_POS 113
#define RECT_X_LEN 16
#define RECT_Y_LEN 7


#define LCD_COLOR_ANDROID_BLUE 0xFF42A5F5  // 0xFF34DDDD
#define LCD_COLOR_MY_DARKORANGE 0xFFFF8C00
#define LCD_COLOR_MY_ORANGE LCD_COLOR_WHITE // 0xFFFFA500




typedef enum lcd_draw_action {
	
	LCD_ACT_DRAW_NOTHING = 0ul, 
	LCD_ACT_DRAW_LEFT = 1ul,
	LCD_ACT_DRAW_RIGHT = 2ul,
	LCD_ACT_DRAW_SLIDER_UP = 3ul,
	LCD_ACT_DRAW_SLIDER_DOWN = 4ul
	
} lcd_draw_actionTypeDef ;


/**
	Struktura do rysowania reakcji na akcje uzytkownika
*/
typedef struct lcd_react {
	TS_mouseInputTypeDef curr_action ;
	TS_mouseInputTypeDef prev_action ;
	
	int16_t X ;
  int16_t Y ;
	
	uint8_t penNotify ;
	
} lcd_reactTypeDef ;



void lcd_prepLayers( void ) ;

void Xmirror_points( const Point *in, Point *out, const uint8_t size ) ;

void lcd_drawLeftButton( Point * const right, Point *storage, uint32_t no, uint32_t color ) ;
void lcd_drawRightButton( Point * const right, uint32_t no, uint32_t color ) ;
void lcd_drawSliderRect( uint16_t dist_top, uint32_t fill_color, uint32_t edge_color ) ;
void lcd_drawSlider(void) ;


void lcd_handleMouseNotify( TS_mouseInputTypeDef _state_, const Point * const _inData_ ) ;
void lcd_drawAction(void) ;

#endif
