
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


/// 1 if interrupt from Touch Screen driver occured - is setting in EXTI handler
volatile sig_atomic_t ts_int_catched = 0;

volatile sig_atomic_t ts_buttons_delay = 0 ;

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
	IOE_Write( DeviceAddr, STMPE811_REG_TSC_CTRL,   (1<<4) | ( 1<<6 ) | 0x03 );
  
  // Clear all the status pending bits if any
  IOE_Write( DeviceAddr, STMPE811_REG_INT_STA, 0xFF );

  // Wait for 2 ms delay
  IOE_Delay(2);
	
	// Config EXTI
	IOE_ITConfig();
	
}


/**
	* Check for interrupt flag
*/

void TS_checkEvent( uint16_t DeviceAddr ) {
	
	uint8_t status = 0 ;
	static uint8_t fifo_values[ TS_FIFO_THRESHOLD_VALUE * 3 ] ;
	static uint16_t pX[ TS_FIFO_THRESHOLD_VALUE ] ;
	static uint16_t pY[ TS_FIFO_THRESHOLD_VALUE ] ;

	
	if ( ts_int_catched == 1 ) {
		
		// Clear flag
		ts_int_catched = 0 ;
		
		// Read status from TS driver
		status = stmpe811_TS_ITStatus( DeviceAddr ) ;
		
		// Mask for Touch detect and FIFO THRESHOLD flags and FIFO OVERFOLOW only
		status &= ( STMPE811_GIT_TOUCH | STMPE811_GIT_FTH | STMPE811_GIT_FOV ) ;
		
		
		
		
		if ( (status & STMPE811_GIT_TOUCH) && 
				 !(status & STMPE811_GIT_FTH)
		) {
			
			if ( ts_buttons_delay > 10 ) {
				ts_buttons_delay = 0 ;
				
				switch( TS_check_button( DeviceAddr, &fifo_values[0], &pX[0], &pY[0]  ) ) {
					case TS_MOUSE_LEFT : BSP_LED_Toggle( LED4 ) ; break ;
					case TS_MOUSE_RIGHT : BSP_LED_Toggle( LED3 ) ; break ;
					default: break ; //BSP_LED_Off( LED3 ) ; BSP_LED_Off( LED3 ) ; 
				}
			}	// some delay is needed
			
		
		} // interrupt from touch detected only
		
		else if ( (status & STMPE811_GIT_FTH) ) {
			
			
			switch( TS_check_slider(DeviceAddr, &fifo_values[0], &pX[0], &pY[0]) ) {
				case TS_MOUSE_SLIDER_UP : BSP_LED_On( LED4 ); BSP_LED_On( LED3 );  break ;
					
				case TS_MOUSE_SLIDER_DOWN : BSP_LED_Off( LED4 ); BSP_LED_Off( LED3 ); break ;
				default : break ;
			}

			
		} // FIFO threshold detected
		
		
		// Flush FIFO
		TS_flushFIFO( DeviceAddr ) ;
		
		// Clear flags in driver's register
		stmpe811_ClearGlobalIT( DeviceAddr, status ) ;
		
	}
	
}



// Sprawdza w ktora strone przesuwano palec
TS_mouseInputTypeDef TS_check_slider( uint16_t DeviceAddr, uint8_t *t_smpl, uint16_t *pX, uint16_t *pY ) {
	
	uint8_t fifo_size = 0 ;
	uint8_t i ;
	int16_t change ;
	uint16_t xavg ;
	
	
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
		for ( i = 0 ; i < fifo_size - 1 ; ++i ) {
				change = pY[i] - pY[i+1] ;
				if ( change <= 0 ) {
					++t_smpl[0] ;		// Count Up direction
				} else {
					++t_smpl[1] ;		// Count Down direction
				}
				
				xavg += pX[i] ;
		}
		xavg /= fifo_size - 1 ;
		
		// If X is outsite of buttons
		if ( (xavg < 2496) &&
				 (xavg > 1600)
		) {
			 
			if( t_smpl[0] > t_smpl[1] ) {
				return TS_MOUSE_SLIDER_UP ;
			}
			else {
				return TS_MOUSE_SLIDER_DOWN ;
			}
		} 

	}
	
	
	return TS_MOUSE_NONE ;
}


// zwraca czy kliknieto na prawy / lewy przycisk myszy
// tylko x,y - 3bajty
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
		if ( (pX[0] <= 1600) && ( pY[0] >= 2458 ) ) {
			return TS_MOUSE_LEFT;
		}// For mouse's left button
		else if ( (pX[0] >= 2496 ) && ( pY[0] >= 2458) ) {
			return TS_MOUSE_RIGHT ;
		} // For mouse's right button
		
		
	}
	
	// No action
	return TS_MOUSE_NONE ;
}



/*
void TS_toDisplaysXY( uint16_t * pX, uint16_t * pY ) {
	*pX = *pX * 
}
*/


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
  IOE_Write(DeviceAddr, STMPE811_REG_FIFO_STA, 0x00);
	
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
