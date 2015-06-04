
#include "main.h"
#include "ts_api_extends.h"

/**	These functions extens functionality of availabe
		API "stm32f429i_discovery_ts" and driver "stmpe811".
		
		Added:
		
		- Initializing function - it also initialize external interrupts
			for Touch interrupt and FIFO Threshold interrupt. EXTI is
			initializing too.
		
		- Function checking for acrtive interrupt ( touch or fifo threshold )

*/



/** 
	* Private variables for this driver
	*	To get value of that variables from another files use apprioprate functions.
*/

static TS_mouseInputTypeDef mouse_current_state = TS_MOUSE_NONE ;

// table with pointers to function which will be call if mouse 
// state changed
static func_notify_TypeDef p_FuncToNotify[TS_NOTIFIED_FUNCTION_NO] = {NULL} ;






/// 1 if interrupt from Touch Screen driver occured - is setting in EXTI handler
volatile sig_atomic_t ts_int_catched = 0;

// time counters
volatile sig_atomic_t ts_buttons_delay = 0 ;
volatile sig_atomic_t ts_mouse_last_signal_delay = 0 ;
volatile sig_atomic_t ts_slider_hop_time = 0 ;

/** 
	* Enable FIFO Threshold and Touch Detection interrupts
*/
void BSP_TS_Init_extends( uint16_t DeviceAddr ) {
	
	// Disable the TS module
	IOE_Write( DeviceAddr, STMPE811_REG_TSC_CTRL, 0x00);
	

	// Set FIFO Threshold and touch detection interrupts and fifo overflow
	IOE_Write( DeviceAddr, STMPE811_REG_INT_EN, (uint8_t)( STMPE811_GIT_TOUCH | STMPE811_GIT_FTH | STMPE811_GIT_FOV ) ) ;
	
	// Set Innterrupt polarity: falling and type: edge
	// and enable Global interrupts
	IOE_Write(DeviceAddr, STMPE811_REG_INT_CTRL , 0x03 ) ;
	
	// Set FIFO Threshold samples
	IOE_Write( DeviceAddr, STMPE811_REG_FIFO_TH, TS_FIFO_THRESHOLD_VALUE ) ;
	
	// Enable TC module with:
	// Enable XY mode and movement tracking
	IOE_Write( DeviceAddr, STMPE811_REG_TSC_CTRL,   (1<<5) | ( 1<<5 ) | 0x03 );
  
  // Clear all the status pending bits if any
  IOE_Write( DeviceAddr, STMPE811_REG_INT_STA, 0xFF );

  // Wait for 2 ms delay
  IOE_Delay(2);
	
	// Config EXTI
	IOE_ITConfig();
	
}


/**
	* Checking for interrupt flag
*/

void TS_checkEvent( uint16_t DeviceAddr ) {
	
	uint8_t status = 0 ;
	TS_mouseInputTypeDef state ;
	
	static uint8_t fifo_values[ TS_FIFO_THRESHOLD_VALUE * 3 ] ;
	static uint16_t pX[ TS_FIFO_THRESHOLD_VALUE ] ;
	static uint16_t pY[ TS_FIFO_THRESHOLD_VALUE ] ;
	
	static uint8_t button_hold_cnt = 0;
	
	// It is used for discarding some events:
	if ( ts_buttons_delay > TS_CHECKING_PERIOD ) {
		ts_buttons_delay = 0 ;
		
		state = TS_getCurrMouseState() ;
		
		
		if ( (state == TS_MOUSE_LEFT) || 
				 (state == TS_MOUSE_RIGHT) 
		) {
			
					if ( 
							0 == TS_isHoldDownButton( DeviceAddr, &fifo_values[0], &pX[0], &pY[0] )
					) {
						++button_hold_cnt;
						if ( button_hold_cnt >= 3 ) {
							button_hold_cnt = 0 ;
							TS_setCurrMouseState( TS_MOUSE_NONE );
						}
					} else {
						button_hold_cnt = 0 ;
					}	

		} // When button was pressed and held - need wait for release
		
		
		
		else if ( ts_slider_hop_time > 0 ) {
			// Block driver for some time before next reading ...
			
			if ( ts_slider_hop_time > TS_SLIDER_HOP_TIME ) {
				// stop counter
				ts_slider_hop_time = 0 ;
				
				// Flush fifo
				TS_flushFIFO( DeviceAddr ) ;
				// Clear flags in driver's register
				stmpe811_ClearGlobalIT( DeviceAddr, status ) ;
			}
				
		} // Slider updating frequency
		
		
		
		else if ( ts_int_catched == 1 ) {
			
			// Clear flag
			ts_int_catched = 0 ;

			// Read status from TS driver
			status = stmpe811_TS_ITStatus( DeviceAddr ) ;

			// Mask for Touch detect and FIFO THRESHOLD flags and FIFO OVERFOLOW only
			status &= ( STMPE811_GIT_TOUCH | STMPE811_GIT_FTH | STMPE811_GIT_FOV ) ;
			
			
						
			if ( (status & STMPE811_GIT_FTH) || (status & STMPE811_GIT_TOUCH) ) {


				state = TS_match_input(DeviceAddr, &fifo_values[0], &pX[0], &pY[0]) ;
				if ( state != TS_MOUSE_NONE ) {
					TS_setCurrMouseState( state ) ;
					if ( (state == TS_MOUSE_LEFT) || (state == TS_MOUSE_RIGHT) ) {
						ts_mouse_last_signal_delay = 0 ;	// reset delay timer for that state
					}
					else {
						ts_slider_hop_time = 1 ;					// start counting
						// that is only one impoulse, so change state to TS_MOUSE_NONE
						// but no notify registered functions - they should know about that
						TS_setCurrMouseStateNoNotify( TS_MOUSE_NONE ) ;

					}
				}		

			} // FIFO threshold detected

			// Flush FIFO
			TS_flushFIFO( DeviceAddr ) ;
			
			// Clear flags in driver's register
			stmpe811_ClearGlobalIT( DeviceAddr, status ) ;
			
		} // event from touch driver
		
		else {
			if ( (TS_getCurrMouseState() != TS_MOUSE_NONE) &&
					 (ts_mouse_last_signal_delay > 80 )
			) {
				// clear state
				TS_setCurrMouseState( TS_MOUSE_NONE ) ;
				ts_mouse_last_signal_delay = 0 ;
				
				// Flush FIFO
				TS_flushFIFO( DeviceAddr ) ;
			
				// Clear flags in driver's register
				stmpe811_ClearGlobalIT( DeviceAddr, status ) ;
					
				}
		} // hold for some time last event state

	} // some delay is needed

}



// Sprawdza w ktora strone przesuwano palec
TS_mouseInputTypeDef TS_match_input( uint16_t DeviceAddr, uint8_t *t_smpl, uint16_t *pX, uint16_t *pY ) {
	
	uint8_t fifo_size = 0 ;
	uint8_t i ;
	int16_t change ;
	uint16_t xavg ;
	uint16_t yavg ;
	
	
	// Read FIFO size
	fifo_size = TS_getFIFO_size( DeviceAddr ) ;
	
	if ( fifo_size > 0 ) {
		
		if ( fifo_size > TS_FIFO_THRESHOLD_VALUE ) {
				fifo_size = TS_FIFO_THRESHOLD_VALUE ;
		}
		
		//Read data
		TS_readFIFO( DeviceAddr, fifo_size*3, &t_smpl[0] ) ;
		// Convert to X,Y
		TS_convertXY( DeviceAddr, fifo_size, &t_smpl[0], &pX[0], &pY[0] ) ;
	
		// Check Y direction's change
		
		t_smpl[0] = t_smpl[1]  = 0 ;	// Y up counter and Y down counter
		xavg = 0 ;	// X average
		yavg = 0 ; 	// Y average
		for ( i = 0 ; i < fifo_size - 1 ; ++i ) {
				change = pY[i] - pY[i+1] ;
				if ( change <= 0 ) {
					++t_smpl[0] ;		// Count Up direction
				} else {
					++t_smpl[1] ;		// Count Down direction
				}
				
				xavg += pX[i] ;
				yavg += pY[i] ;
				
		}
		xavg /= fifo_size - 1 ;
		yavg /= fifo_size - 1 ;
		
		// If X is outsite of buttons
		if ( (xavg < 2496) &&
				 (xavg > 1600)
		) {
			 
			t_smpl[3] = fifo_size/4;
			if( t_smpl[0] >= t_smpl[1]+t_smpl[3] ) {
				return TS_MOUSE_SLIDER_UP ;
			}
			else if ( t_smpl[1] >= t_smpl[0]+t_smpl[3] ) {
				return TS_MOUSE_SLIDER_DOWN ;
			}
		} // for slide
		else if ( (xavg < TS_EDGE_LEFT_BUTTON_X-5) && 
							(yavg > TS_EDGE_LEFT_BUTTON_Y)
		) {
				return TS_MOUSE_LEFT ;
		} // for left button pressed and is holding
		else if ( (xavg > TS_EDGE_RIGHT_BUTTON_X+5) && 
							(yavg > TS_EDGE_RIGHT_BUTTON_Y)
		) {
				return TS_MOUSE_RIGHT ;
		}	// for right mouse button 
	}
	
	
	return TS_MOUSE_NONE ;
}


// zwraca czy kliknieto na prawy / lewy przycisk myszy
// tylko x,y - 3bajty
/*
TS_mouseInputTypeDef TS_check_button( uint16_t DeviceAddr, uint8_t *t_smpl, uint16_t *pX, uint16_t *pY ) {
	
	/// Maximum samples use to average
	const uint32_t Smpl2Avg = 3 ;
	
	uint8_t i ;
	uint8_t fifo_size = 0 ;
	//static uint8_t t_smpl[ Smpl2Avg*3 ] ; // because 3 bytes for each XY data
	
	
	// Read size of fifo
	fifo_size = TS_getFIFO_size( DeviceAddr ) ;
	if ( fifo_size > 0 ) {
		if ( fifo_size > Smpl2Avg ) {
			fifo_size = Smpl2Avg ;
		}
		
		//Read data
		TS_readFIFO( DeviceAddr, fifo_size*3, &t_smpl[0] ) ;
		// Convert to X,Y
		TS_convertXY( DeviceAddr, fifo_size, &t_smpl[0], &pX[0], &pY[0] ) ;
		// Average
		for ( i = 1 ; i < fifo_size ; ++i ) {
			pX[0] += pX[i] ;
			pY[0] += pY[i] ;
		}
		pX[0] /= fifo_size ;
		pY[0] /= fifo_size ;
		
		// compare with button's boundaries
		if ( (pX[0] <= TS_EDGE_LEFT_BUTTON_X ) && ( pY[0] >= TS_EDGE_LEFT_BUTTON_Y ) ) {
			return TS_MOUSE_LEFT;
		}// For mouse's left button
		else if ( (pX[0] >= TS_EDGE_RIGHT_BUTTON_X ) && ( pY[0] >= TS_EDGE_RIGHT_BUTTON_Y ) ) {
			return TS_MOUSE_RIGHT ;
		} // For mouse's right button
		
		
	}
	
	// No action
	return TS_MOUSE_NONE ;
}
*/


uint8_t TS_isHoldDownButton( uint16_t DeviceAddr, uint8_t *t_smpl, uint16_t *pX, uint16_t *pY ) {
	uint8_t fifo_size = 0;
	uint8_t i ;
	uint16_t xavg, yavg ;
	const uint8_t avg_smpl = 5;
	
	// Read size of fifo
	fifo_size = TS_getFIFO_size( DeviceAddr ) ;
	
	// check if is any data in fifo
	if ( fifo_size > 0 ) {
		if ( fifo_size > avg_smpl ) {
			fifo_size = avg_smpl ;
		}
		
		// Read fifo
		TS_readFIFO( DeviceAddr, fifo_size*3, &t_smpl[0] ) ;
				
		// Clear flags in driver's register
		stmpe811_ClearGlobalIT( DeviceAddr, STMPE811_GIT_TOUCH | STMPE811_GIT_FTH | STMPE811_GIT_FOV ) ;
		
		// Convert to XY
		TS_convertXY( DeviceAddr, fifo_size, &t_smpl[0], &pX[0], &pY[0] ) ;
		
		xavg = pX[0] ;
		yavg = pY[0] ;
		for( i = 1 ; i < avg_smpl ; ++i ) {
			xavg += pX[i] ;
			yavg += pY[i] ;
		}
		xavg /= avg_smpl ;
		yavg /= avg_smpl ;
		
		// Check if is pressed still in boundaries of left button
		if ( (xavg < TS_EDGE_LEFT_BUTTON_X) && (yavg > TS_EDGE_LEFT_BUTTON_Y) ) {
			return 1 ;
		} // hold the left button
		else if ( (xavg > TS_EDGE_RIGHT_BUTTON_X) && (yavg > TS_EDGE_RIGHT_BUTTON_Y) ) {
			return 1 ;
		}
		
	}
	
	
	return 0 ;
	
}




uint8_t TS_getFIFO_size ( uint16_t DeviceAddr ) {
	 return IOE_Read( DeviceAddr, STMPE811_REG_FIFO_SIZE ) ;
}

uint8_t TS_readFIFO ( uint16_t DeviceAddr, uint16_t no, uint8_t *pTab ) {

	//STMPE811_REG_TSC_DATA_XYZ;
	return (uint8_t)IOE_ReadMultiple( DeviceAddr, STMPE811_REG_TSC_DATA_NON_INC , pTab, no ) ;
	
}

void TS_flushFIFO(  uint16_t DeviceAddr ) {
	/* Reset FIFO */
  IOE_Write(DeviceAddr, STMPE811_REG_FIFO_STA, 0x01);
  /* Enable the FIFO again */
	//HAL_Delay(1) ;
  IOE_Write(DeviceAddr, STMPE811_REG_FIFO_STA, 0x00);
	
}


TS_mouseInputTypeDef TS_getCurrMouseState(void) {
	return mouse_current_state ;
}

static void TS_setCurrMouseStateNoNotify( const TS_mouseInputTypeDef _state_ ) {
	mouse_current_state = _state_ ;
}
static void TS_setCurrMouseState( const TS_mouseInputTypeDef _state_ ) {
	uint8_t i ;
	
	mouse_current_state = _state_ ;
	
	//Call user's defined funtion to sent notification to them
	for ( i = 0 ; i < TS_NOTIFIED_FUNCTION_NO ; ++i ) {
			if ( NULL != p_FuncToNotify[i] ) {
				// send notify
				p_FuncToNotify[i]( _state_ ) ; 	
			}
	}
	
}

uint8_t TS_registerNotifyFunc( func_notify_TypeDef _in_ ) {
	uint8_t i ;
	
	for ( i = 0 ; i < TS_NOTIFIED_FUNCTION_NO ; ++i ) {
		if ( NULL == p_FuncToNotify[i] ) {
			p_FuncToNotify[i] = _in_ ;
			return 1 ;
		}
	}
	return 0;
}

// Konwertuje na dane x, y - 3bajtowe dane wejsciowe ( tryb XY )
// no - wielokrotne liczby 3
void TS_convertXY( uint16_t DeviceAddr, uint16_t no, uint8_t *pTab, uint16_t *pX, uint16_t *pY ) {
	uint16_t i, j ;
	
	for( i = 0, j = 0 ; i < no ; ++i, j+=3 ) {
		
		pX[i] = pY[i] = 0 ;
		
		pX[i] = (uint16_t)( pTab[j] ) << 4 ;
		pX[i] |= ( (uint16_t)( pTab[j+1] ) & 0x00F0u ) >> 4 ;
		
		pY[i] = ( (uint16_t)( pTab[j+1] ) & 0x000Fu ) << 8  ;
		pY[i] |= pTab[j+2] ;
		
	}
	
}
