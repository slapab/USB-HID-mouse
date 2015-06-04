#ifndef _LCD_LAYOUT_
#define _LCD_LAYOUT_

#include <stdint.h>


#define LCD_COLOR_ANDROID_BLUE 0xFF34DDDD

void lcd_prepLayers( void ) ;

void Xmirror_points( const Point *in, Point *out, const uint8_t size ) ;



#endif
