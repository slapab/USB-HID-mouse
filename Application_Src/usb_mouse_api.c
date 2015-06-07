#include "usb_mouse_api.h"

extern USBD_HandleTypeDef USBD_Device ;

uint8_t HID_Buffer[4];	// store cursor movement and buttons action for USB HID
float xyz_buff[3];			// store data from gyro
uint8_t unclick = 0;		// detect mouse unclick


void usb_handleMouseNotify( TS_mouseInputTypeDef _state_, const Point * const _inData_ ) {
	
	switch( _state_ ) {
		case TS_MOUSE_LEFT: 
				HID_Buffer[0] = 0x01 ;
				HID_Buffer[3] = 0x00 ; // slider off
				break ;
		case TS_MOUSE_RIGHT: 
				HID_Buffer[0] = 0x02 ;
				HID_Buffer[3] = 0x00 ;
				break ;
		case TS_MOUSE_SLIDER_UP: 
				HID_Buffer[3] = 0x01 ;
				//HID_Buffer[3] = 0x00 ;
				break ;
		case TS_MOUSE_SLIDER_DOWN:
				HID_Buffer[3] = 0xFF ;
				//HID_Buffer[3] = 0x00 ; // ???? -1???
				break ;
		default: 
				HID_Buffer[0] = 0x00 ;
				HID_Buffer[3] = 0x00 ;
				unclick = 1;
				break ;
	}
}

/**
  * @brief  Gets Pointer Data.
  * @param  pbuf: Pointer to report
  * @retval None
  */
void GetPointerData(uint8_t *pbuf) {
  int8_t x = 0, y = 0;
	
	BSP_GYRO_GetXYZ(xyz_buff);
	
	if (xyz_buff[0] > GYRO_SENSITIVITY)
		y += CURSOR_STEP;
	if (xyz_buff[0] < -GYRO_SENSITIVITY)
		y -= CURSOR_STEP;
	if (xyz_buff[1] > GYRO_SENSITIVITY)
		x -= CURSOR_STEP;
	if (xyz_buff[1] < -GYRO_SENSITIVITY)
		x += CURSOR_STEP;
		
	pbuf[0] = 0;
	pbuf[1] = x;
	pbuf[2] = y;
	pbuf[3] = 0;
}
