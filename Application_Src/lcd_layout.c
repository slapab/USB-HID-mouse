#include "main.h"
#include "lcd_layout.h"


// Private arays to contain shapes of layout
static Point button_poly[6] = { { 1, 320 }, { 54, 320 }, { 54, 266 }, { 99, 221 }, { 99, 150 }, { 1, 150 } } ; 
static Point mirr_poly[6] ;

// Timers
volatile sig_atomic_t lcd_draw_button_tmr = 0 ;
volatile sig_atomic_t lcd_draw_slider_tmr = 0 ;
	
// Private variable to contain current state of drawing action
static lcd_reactTypeDef lcd_draw_action = { TS_MOUSE_NONE, TS_MOUSE_NONE, 0, 0 } ;







/* Functions */


void lcd_drawAction(void) {

	if ( lcd_draw_action.penNotify != 0 ) {
		
		// Show unchanged copy of screen:
		BSP_LCD_SetTransparency(LCD_BACKGROUND_LAYER,0xFF) ;
		BSP_LCD_SetTransparency(LCD_FOREGROUND_LAYER,0x00) ;
		//BSP_LCD_SetLayerVisible(LCD_BACKGROUND_LAYER, ENABLE);
		//BSP_LCD_SetLayerVisible(LCD_FOREGROUND_LAYER, DISABLE);
		
		// modify foreground layer
		BSP_LCD_SelectLayer(LCD_FOREGROUND_LAYER);
		/*
		lcd_drawRightButton( &button_poly[0], 6, LCD_COLOR_ANDROID_BLUE ) ;
		lcd_drawLeftButton( &button_poly[0], &mirr_poly[0], 6, LCD_COLOR_ANDROID_BLUE ) ;
		// Draw slider	
		lcd_drawSlider() ;
	*/	
		if ( lcd_draw_action.curr_action == TS_MOUSE_NONE ) {
			switch ( lcd_draw_action.prev_action ) {
				case TS_MOUSE_LEFT : 
						lcd_drawLeftButton( &button_poly[0],  &mirr_poly[0], 6, LCD_COLOR_ANDROID_BLUE ) ;
						break ;
				case TS_MOUSE_RIGHT : 
						lcd_drawRightButton( &button_poly[0], 6, LCD_COLOR_ANDROID_BLUE ) ;
				case TS_MOUSE_NONE : break ;
				default: 
						lcd_drawSlider() ;
						break ;
			}
		}
		else if ( lcd_draw_action.curr_action == TS_MOUSE_LEFT ) {
			lcd_drawLeftButton( &button_poly[0],  &mirr_poly[0], 6, LCD_COLOR_MY_ORANGE ) ;
			//lcd_drawRightButton( &button_poly[0], 6, LCD_COLOR_ANDROID_BLUE ) ;
			// Draw slider	
			//lcd_drawSlider() ;
		}
		else if ( lcd_draw_action.curr_action == TS_MOUSE_RIGHT ) {
			lcd_drawRightButton( &button_poly[0], 6, LCD_COLOR_MY_ORANGE ) ;
			//lcd_drawLeftButton( &button_poly[0],  &mirr_poly[0], 6, LCD_COLOR_ANDROID_BLUE ) ;
			//lcd_drawSlider() ;
		}
		
		
		// Switch layers
		BSP_LCD_SetTransparency(LCD_FOREGROUND_LAYER, 0xFF );
		BSP_LCD_SetTransparency(LCD_BACKGROUND_LAYER, 0x00 );
		//BSP_LCD_SetLayerVisible(LCD_FOREGROUND_LAYER, ENABLE);
		//BSP_LCD_SetLayerVisible(LCD_BACKGROUND_LAYER, DISABLE);
		
		
		
		lcd_draw_action.penNotify = 0 ;
	}
}


/* Function to getting notify from 'mouse driver' ( touchscreen ) */
void lcd_handleMouseNotify( TS_mouseInputTypeDef _state_, const Point * const _inData_ ) {
	
	lcd_draw_action.X = _inData_->X ;
	lcd_draw_action.Y = _inData_->Y ;
	
	lcd_draw_action.prev_action = lcd_draw_action.curr_action ;
	lcd_draw_action.curr_action = _state_ ;
	lcd_draw_action.penNotify = 1 ;
		
}




void lcd_prepLayers( void ) {
	
	// Init LCD buffers for layers
	BSP_LCD_LayerDefaultInit(LCD_BACKGROUND_LAYER, LCD_FRAME_BUFFER);
	BSP_LCD_LayerDefaultInit(LCD_FOREGROUND_LAYER, LCD_FRAME_BUFFER+BUFFER_OFFSET);
	// Enable Layer
	BSP_LCD_SetLayerVisible(LCD_FOREGROUND_LAYER, ENABLE);
	BSP_LCD_SetLayerVisible(LCD_BACKGROUND_LAYER, DISABLE);
	
	
	// Select Foreground Layer
	BSP_LCD_SelectLayer(LCD_FOREGROUND_LAYER);
	BSP_LCD_Clear(LCD_COLOR_BLACK);

	lcd_drawRightButton( &button_poly[0], 6, LCD_COLOR_ANDROID_BLUE ) ;
	lcd_drawLeftButton( &button_poly[0], &mirr_poly[0], 6, LCD_COLOR_ANDROID_BLUE ) ;
	// Draw slider	
	lcd_drawSlider() ;
	
  /* Display LCD messages */
	//BSP_LCD_SetFont(&LCD_DEFAULT_FONT);
  //BSP_LCD_SetBackColor(LCD_COLOR_WHITE); 
  //BSP_LCD_DisplayStringAt(0, 10, (uint8_t*)"MOUSE USB HID", CENTER_MODE);
	
	
	//############# 2 warstawa - test
	
	// Select Background Layer
	BSP_LCD_SelectLayer(LCD_BACKGROUND_LAYER);
	BSP_LCD_Clear(LCD_COLOR_BLACK);
	
	lcd_drawRightButton( &button_poly[0], 6, LCD_COLOR_ANDROID_BLUE ) ;
	lcd_drawLeftButton( &button_poly[0], &mirr_poly[0], 6, LCD_COLOR_ANDROID_BLUE ) ;
	// Draw slider	
	lcd_drawSlider() ;
	
}


void Xmirror_points( const Point *in, Point *out, const uint8_t size ) {
	uint8_t i ;
	for( i = 0 ; i < size ; ++i ) {
		out[i].X  = 240 - in[i].X ;
		out[i].Y  = in[i].Y ;
	}
}

void lcd_drawSlider(void) {
	lcd_drawSliderRect( 40 , LCD_COLOR_ANDROID_BLUE, LCD_COLOR_WHITE ) ;
	lcd_drawSliderRect( 100 , LCD_COLOR_ANDROID_BLUE, LCD_COLOR_WHITE ) ;
	lcd_drawSliderRect( 160 , LCD_COLOR_ANDROID_BLUE, LCD_COLOR_WHITE ) ;
}
void lcd_drawSliderRect( uint16_t dist_top, uint32_t fill_color, uint32_t edge_color ) {
	
	uint32_t cback ;
	
	if ( dist_top >= (uint16_t)LCD_SIZE_Y )
		dist_top = 0 ;

	cback = BSP_LCD_GetTextColor() ;
	BSP_LCD_SetTextColor(fill_color) ;
	BSP_LCD_FillRect( RECT_X_POS, ((uint16_t)LCD_SIZE_Y - dist_top) , RECT_X_LEN, RECT_Y_LEN ) ;
	
	// Draw edge
	BSP_LCD_SetTextColor(edge_color) ;
	BSP_LCD_DrawRect( RECT_X_POS-2, ((uint16_t)LCD_SIZE_Y - dist_top -2) , RECT_X_LEN+3, RECT_Y_LEN+3 ) ;
	
	BSP_LCD_SetTextColor(cback) ;
}



void lcd_drawRightButton( Point * const right, uint32_t no, uint32_t color ) {
	
	uint32_t cback ;
	cback = BSP_LCD_GetTextColor() ;
	
	BSP_LCD_SetTextColor( color ) ;
	BSP_LCD_FillPolygon( &right[0], no ) ;
	
	
	
	BSP_LCD_SetTextColor( cback ) ;
}


void lcd_drawLeftButton( Point * const right, Point *storage, uint32_t no, uint32_t color ) {
	
	uint32_t cback ;
	cback = BSP_LCD_GetTextColor() ;
	
	Xmirror_points( &right[0], &storage[0], no ) ;
	BSP_LCD_SetTextColor( color ) ;
	BSP_LCD_FillPolygon( &storage[0], no ) ;
	
	BSP_LCD_SetTextColor( cback ) ;
}
