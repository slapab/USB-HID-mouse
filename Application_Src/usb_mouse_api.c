
#include "usb_mouse_api.h"


volatile uint8_t usb_poolInterval = 0 ; 	// in systick
extern USBD_HandleTypeDef USBD_Device ;

uint8_t HID_Buffer[4];	// store cursor movement and buttons action for USB HID
static float xyz_buff[3];			// store data from gyro


void usb_checkPoolInterval( void ) {
	
	if ( usb_poolInterval >= USBD_HID_GetPollingInterval(&USBD_Device) ) {
		usb_poolInterval = 0 ;
		
		// get current status of gyro position
		GetPointerData(&HID_Buffer[0]);
		

		// send data though IN endpoint
    if((HID_Buffer[0] != 0) || (HID_Buffer[1] != 0) || HID_Buffer[2] != 0 || HID_Buffer[3] != 0 )
    {
			
      USBD_HID_SendReport(&USBD_Device, &HID_Buffer[0], 4);
			// clear slider event - ts driver do not sent notify of TS_MOUSE_NONE after slide has been detected
			//HID_Buffer[0] = 0x00 ;
			//HID_Buffer[1] = 0x00 ;
			
			HID_Buffer[2] = 0x00 ; // clear mouse scroll event
			//HID_Buffer[3] = 0x00 ;
    }
	}
}


void usb_handleMouseNotify( TS_mouseInputTypeDef _state_, const Point * const _inData_ ) {
	switch( _state_ ) {
		case TS_MOUSE_LEFT: 
				HID_Buffer[0] = 0x02 ;
				HID_Buffer[2] = 0x00 ; // slider off
				break ;
		case TS_MOUSE_RIGHT: 
				HID_Buffer[1] = 0x02 ;
				HID_Buffer[2] = 0x00 ;
				break ;
		case TS_MOUSE_SLIDER_UP: 
				HID_Buffer[2] = 0x01 ;
				//HID_Buffer[3] = 0x00 ;
				break ;
		case TS_MOUSE_SLIDER_DOWN:
				HID_Buffer[2] = 0xFF ;
				//HID_Buffer[3] = 0x00 ; // ???? -1???
				break ;
		default: 
				
				HID_Buffer[0] = 0x00 ;
				HID_Buffer[1] = 0x00 ;
				HID_Buffer[3] = 0x00 ;
				break ;
	}
}



static void GetPointerData(uint8_t *pbuf)
{
  int8_t x = 0, y = 0;
	/*
	BSP_GYRO_GetXYZ(xyz_buff);
		if (xyz_buff[0] > 10000.0f)
			y += CURSOR_STEP;
		if (xyz_buff[0] < -10000.0f)
			y -= CURSOR_STEP;
		if (xyz_buff[1] > 10000.0f)
			x -= CURSOR_STEP;
		if (xyz_buff[1] < -10000.0f)
			x += CURSOR_STEP;
	*/
		//0 - x + -> w prawo
		//1 - y 
		//2 - s up dla dodatnich
		//pbuf[0] = 3 ;
		
		//HID_Buffer[0] = 1 ;//x;
		//pbuf[2] = 2 ;// y;
		
	/* mouse_state = TS_getCurrMouseState();
	if (mouse_state == TS_MOUSE_LEFT)
		pbuf[0] = 0x01;
	else if (mouse_state == TS_MOUSE_RIGHT)
		pbuf[0] = 0x02;
	else
		pbuf[0] = 0;
	
	if (mouse_state == TS_MOUSE_SLIDER_UP)
    pbuf[3] = 1;
	else if (mouse_state == TS_MOUSE_SLIDER_DOWN)
		pbuf[3] = -1;
	else
		pbuf[3] = 0;
	*/
	
}
