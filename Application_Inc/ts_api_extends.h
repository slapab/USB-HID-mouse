#ifndef _TS_API_EXTENDS_
#define _TS_API_EXTENDS_

#include <stdint.h>


#define TS_FIFO_THRESHOLD_VALUE 10


/**
	Values of possible mouse action from touchpanel
*/
typedef enum 
{
	TS_MOUSE_NONE  = 0x00,
  TS_MOUSE_LEFT  = 0x01,
  TS_MOUSE_RIGHT = 0x02,
	TS_MOUSE_SLIDER_UP = 0x03,
	TS_MOUSE_SLIDER_DOWN = 0x04
} TS_mouseInputTypeDef ;


void BSP_TS_Init_extends( uint16_t DeviceAddr ) ;
void TS_checkEvent( uint16_t DeviceAddr );

void TS_flushFIFO(  uint16_t DeviceAddr ) ;
uint8_t TS_getFIFO_size ( uint16_t DeviceAddr ) ;
uint8_t TS_readFIFO ( uint16_t DeviceAddr, uint16_t no, uint8_t *pTab ) ;

void TS_convertXY( uint16_t DeviceAddr, uint16_t no, uint8_t *pTab, uint16_t *pX, uint16_t *pY ) ;

TS_mouseInputTypeDef TS_check_button( uint16_t DeviceAddr, uint8_t *t_smpl, uint16_t *pX, uint16_t *pY ) ;
TS_mouseInputTypeDef TS_check_slider( uint16_t DeviceAddr, uint8_t *t_smpl, uint16_t *pX, uint16_t *pY ) ;

void Xmirror_points( const Point *in, Point *out, const uint8_t size ) ;

#endif
