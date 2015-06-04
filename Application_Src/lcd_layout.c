#include "main.h"
#include "lcd_layout.h" 

static Point button_poly[6] = { { 1, 360 }, { 54, 360 }, { 54, 306 }, { 99, 261 }, { 99, 190 }, { 1, 190 } } ; 
static Point mirr_poly[6] ;


void lcd_prepLayers( void ) {
	
	// Init LCD buffers for layers
	BSP_LCD_LayerDefaultInit(LCD_BACKGROUND_LAYER, LCD_FRAME_BUFFER);
	BSP_LCD_LayerDefaultInit(LCD_FOREGROUND_LAYER, LCD_FRAME_BUFFER+BUFFER_OFFSET);
	// Enable Layer
	BSP_LCD_SetLayerVisible(LCD_FOREGROUND_LAYER, DISABLE);
	BSP_LCD_SetLayerVisible(LCD_BACKGROUND_LAYER, ENABLE);
	
	// Select Foreground Layer
	BSP_LCD_SelectLayer(LCD_FOREGROUND_LAYER);
	BSP_LCD_SetFont(&LCD_DEFAULT_FONT);

	/* Clear the LCD */ 
  BSP_LCD_SetBackColor(LCD_COLOR_WHITE); 
  BSP_LCD_Clear(LCD_COLOR_LIGHTBLUE);
  
  /* Set the LCD Text Color */
  BSP_LCD_SetTextColor(LCD_COLOR_DARKBLUE);  
  
  /* Display LCD messages */
  BSP_LCD_DisplayStringAt(0, 10, (uint8_t*)"MOUSE USB HID", CENTER_MODE);
	
	
	//############# 2 warstawa - test
	
	// Select Background Layer
	BSP_LCD_SelectLayer(LCD_BACKGROUND_LAYER);
	BSP_LCD_Clear(LCD_COLOR_BLACK);
	
	/* Clear the LCD */ 
  /*BSP_LCD_SetBackColor(LCD_COLOR_LIGHTGREEN); 
  	
	BSP_LCD_SetTextColor(LCD_COLOR_DARKBLUE);  
  BSP_LCD_SetFont(&Font16);
  //BSP_LCD_DisplayStringAt(0, 35, (uint8_t*)"Drivers examples", CENTER_MODE);
	*/
	
	BSP_LCD_SetBackColor( LCD_COLOR_ANDROID_BLUE ) ;
	Xmirror_points( &button_poly[0], &mirr_poly[0], 6 ) ;

	
	BSP_LCD_SetTextColor(LCD_COLOR_ANDROID_BLUE); 
	BSP_LCD_FillPolygon( &button_poly[0], 6 );
	BSP_LCD_FillPolygon( &mirr_poly[0], 6 );
		
	//rectangle
		BSP_LCD_FillRect( 112, 290, 16, 7 ) ;
		//BSP_LCD_FillRect( 112, 260, 16, 7 ) ;
		BSP_LCD_FillRect( 112, 230, 16, 7 ) ;
		//BSP_LCD_FillRect( 112, 200, 16, 7 ) ;
		BSP_LCD_FillRect( 112, 170, 16, 7 ) ;
		//BSP_LCD_FillRect( 112, 140, 16, 7 ) ;
		//BSP_LCD_DrawRect( 103, 300, 32, 7 );
}


void Xmirror_points( const Point *in, Point *out, const uint8_t size ) {
	uint8_t i ;
	for( i = 0 ; i < size ; ++i ) {
		out[i].X  = 240 - in[i].X ;
		out[i].Y  = in[i].Y ;
	}
}
