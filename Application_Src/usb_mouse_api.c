#include "usb_mouse_api.h"

extern USBD_HandleTypeDef USBD_Device ;

uint8_t HID_Buffer[4] = {0, 0, 0, 0};	// store cursor movement and buttons action for USB HID
static volatile uint64_t GYRO_Buffer[16] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};  // store data from gyro
volatile uint64_t static *pbuf = GYRO_Buffer;
static float xyz_buff[3];			// store one read from gyro
volatile uint8_t unclick = 0;		// detect mouse unclick


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
				HID_Buffer[3] = 0x01;
				break ;
		case TS_MOUSE_SLIDER_DOWN:
				HID_Buffer[3] = 0xFF ;
				break ;
		default: 
				HID_Buffer[0] = 0x00 ;
				HID_Buffer[3] = 0x00 ;
				unclick = 1;
				break ;
	}
}


/**
  * @brief  Gets Gyro Data.
	* @param  pbuf_x: Pointer to axis data 
  * @retval None
  */
void GetGyroData( void ) {
	
	BSP_GYRO_GetXYZ(xyz_buff);
	if (xyz_buff[0] < -GYRO_SENSITIVITY) {
		*(pbuf + 12) = (*(pbuf + 12) << 1 ) + 1;
		if (*(pbuf + 12) == MAX_UINT64)
			*(pbuf + 13) = (*(pbuf + 13) << 1 ) + 1;
			if (*(pbuf + 13) == MAX_UINT64)
				*(pbuf + 14) = (*(pbuf + 14) << 1 ) + 1;
				if (*(pbuf + 14) == MAX_UINT64)
					*(pbuf + 15) = (*(pbuf + 15) << 1 ) + 1;
	}
	else {
		*(pbuf + 12) = 0;
		*(pbuf + 13) = 0;
		*(pbuf + 14) = 0;
		*(pbuf + 15) = 0;
	}
	
	
	if (xyz_buff[0] > GYRO_SENSITIVITY) {
		*(pbuf + 8) = (*(pbuf + 8) << 1) + 1;
		if (*(pbuf + 8) == MAX_UINT64)
			*(pbuf + 9) = (*(pbuf + 9) << 1 ) + 1;
			if (*(pbuf + 9) == MAX_UINT64)
				*(pbuf + 10) = (*(pbuf + 10) << 1 ) + 1;
				if (*(pbuf + 10) == MAX_UINT64)
					*(pbuf + 11) = (*(pbuf + 11) << 1 ) + 1;
	}
	else {
		*(pbuf + 8) = 0;
		*(pbuf + 9) = 0;
		*(pbuf + 10) = 0;
		*(pbuf + 11) = 0;
	}
	
	
	if (xyz_buff[1] < -GYRO_SENSITIVITY) {
		*pbuf = (*pbuf << 1) + 1;
		if (*pbuf == MAX_UINT64)
			*(pbuf + 1) = (*(pbuf + 1) << 1 ) + 1;
				if (* (pbuf + 1) == MAX_UINT64)
					*(pbuf + 2) = (*(pbuf + 2) << 1 ) + 1;
					if (* (pbuf + 2) == MAX_UINT64)
						*(pbuf + 3) = (*(pbuf + 3) << 1 ) + 1;
	}
	else {
		*pbuf = 0;
		*(pbuf + 1) = 0;
		*(pbuf + 2) = 0;
		*(pbuf + 3) = 0;
	}
	
	
	if (xyz_buff[1] > GYRO_SENSITIVITY) {
		*(pbuf + 4) = (*(pbuf + 4) << 1) + 1;
		if (*(pbuf + 4) == MAX_UINT64)
			*(pbuf + 5) = (*(pbuf + 5) << 1 ) + 1;
			if (* (pbuf + 5) == MAX_UINT64)
				*(pbuf + 6) = (*(pbuf + 6) << 1 ) + 1;
				if (* (pbuf + 6) == MAX_UINT64)
					*(pbuf + 7) = (*(pbuf + 7) << 1 ) + 1;
	}
	
	else {
		*(pbuf + 4) = 0;
		*(pbuf + 5) = 0;
		*(pbuf + 6) = 0;
		*(pbuf + 7) = 0;
	}
}

/**
  * @brief  Gets Pointer Data.
  * @param  pbuf: Pointer to report
  * @retval None
  */
void GetPointerData( void ) {
  int8_t x = 0, y = 0;
	if (* (pbuf + 3) == MAX_UINT64)
		x += CURSOR_STEP;
	if (* (pbuf + 7) == MAX_UINT64)
		x -= CURSOR_STEP;
	if (* (pbuf + 11) == MAX_UINT64)
		y += CURSOR_STEP;
	if (* (pbuf + 15) == MAX_UINT64)
		y -= CURSOR_STEP;
		
	HID_Buffer[1] = x;
	HID_Buffer[2] = y;

}
