#ifndef _TS_API_EXTENDS_
#define _TS_API_EXTENDS_

#include <stdint.h>


#define TS_CHECKING_PERIOD 20			// in ms
#define TS_FIFO_THRESHOLD_VALUE 12
#define TS_SLIDER_HOP_TIME 120		// in ms


#define TS_NOTIFIED_FUNCTION_NO 4


#define TS_EDGE_LEFT_BUTTON_X 1600
#define TS_EDGE_LEFT_BUTTON_Y 2458
#define TS_EDGE_RIGHT_BUTTON_X 2496
#define TS_EDGE_RIGHT_BUTTON_Y 2458




/**
	Values of possible mouse action from touchpanel
*/
typedef enum 
{
	TS_MOUSE_NONE  = 0x00,
  TS_MOUSE_LEFT  = 0x01,
  TS_MOUSE_RIGHT = 0x02,
	TS_MOUSE_SLIDER_UP = 0x03,
	TS_MOUSE_SLIDER_DOWN = 0x04,

} TS_mouseInputTypeDef ;



typedef void (*func_notify_TypeDef) ( const TS_mouseInputTypeDef, const Point * const ) ;



void BSP_TS_Init_extends( uint16_t DeviceAddr ) ;
void TS_checkEvent( uint16_t DeviceAddr );

void TS_flushFIFO(  uint16_t DeviceAddr ) ;
uint8_t TS_getFIFO_size ( uint16_t DeviceAddr ) ;
uint8_t TS_readFIFO ( uint16_t DeviceAddr, uint16_t no, uint8_t *pTab ) ;


TS_mouseInputTypeDef TS_check_button( uint16_t DeviceAddr, uint8_t *t_smpl, uint16_t *pX, uint16_t *pY ) ;
TS_mouseInputTypeDef TS_match_input( uint16_t DeviceAddr, uint8_t *t_smpl, uint16_t *pX, uint16_t *pY ) ;
uint8_t TS_isHoldDownButton( uint16_t DeviceAddr, uint8_t *t_smpl, uint16_t *pX, uint16_t *pY ) ;

void TS_convertXY( uint16_t DeviceAddr, uint16_t no, uint8_t *pTab, uint16_t *pX, uint16_t *pY ) ;

static void TS_setCurrMouseState( const TS_mouseInputTypeDef _state_ ) ;
static void TS_setCurrMouseStateNoNotify( const TS_mouseInputTypeDef _state_ ) ;
TS_mouseInputTypeDef TS_getCurrMouseState(void) ;

uint8_t TS_registerNotifyFunc( func_notify_TypeDef _in_ ) ;


#endif
