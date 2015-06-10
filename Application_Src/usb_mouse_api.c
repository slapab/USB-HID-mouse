#include "usb_mouse_api.h"

extern USBD_HandleTypeDef USBD_Device ;

uint8_t HID_Buffer[4] = {0, 0, 0, 0};	// store cursor movement and buttons action for USB HID
static volatile uint32_t GYRO_Buffer[4] = {0, 0, 0, 0};  // store data from gyro
volatile uint32_t static *pbuf = GYRO_Buffer;
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
	
	if (xyz_buff[0] < -GYRO_SENSITIVITY)
		++ (* (pbuf + 3) );
	else 
		*(pbuf + 3) = 0;
	
	if (xyz_buff[0] > GYRO_SENSITIVITY) 
		++( * (pbuf + 2) );
	else 
		*(pbuf + 2) = 0;
	
	if (xyz_buff[1] < -GYRO_SENSITIVITY)
		++(*pbuf);
	else 
		*pbuf = 0;
	
	if (xyz_buff[1] > GYRO_SENSITIVITY) 
		++( * (pbuf + 1) );
	else
		*(pbuf + 1) = 0;

}

/**
  * @brief  Gets Pointer Data.
  * @param  pbuf: Pointer to report
  * @retval None
  */
void GetPointerData( void ) {
  int8_t x = 0, y = 0;
	if (*pbuf >= GYRO_AVG)
		x += CURSOR_STEP;
	if (* (pbuf + 1) >= GYRO_AVG)
		x -= CURSOR_STEP;
	if (* (pbuf + 2) >= GYRO_AVG)
		y += CURSOR_STEP;
	if (* (pbuf + 3) >= GYRO_AVG)
		y -= CURSOR_STEP;
		
	HID_Buffer[1] = x;
	HID_Buffer[2] = y;

}
