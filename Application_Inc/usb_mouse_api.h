#ifndef _USB_MOUSE_API_H_
#define _USB_MOUSE_API_H_

#include "main.h"

#define CURSOR_STEP     1
#define ABS(x) (((x) > 0) ? (x) : (-x))


void usb_checkPoolInterval( void ) ;
void usb_handleMouseNotify( TS_mouseInputTypeDef _state_, const Point * const _inData_ ) ;	


static void GetPointerData(uint8_t *pbuf);


#endif
