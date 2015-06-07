#ifndef _USB_MOUSE_API_H_
#define _USB_MOUSE_API_H_

#include "main.h"
#include "ts_api_extends.h"

#define CURSOR_STEP 1
#define GYRO_SENSITIVITY 30000.0f
void usb_handleMouseNotify( TS_mouseInputTypeDef _state_, const Point * const _inData_ ) ;	

void GetPointerData(uint8_t *pbuf);


#endif
