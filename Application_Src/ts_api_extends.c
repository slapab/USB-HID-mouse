
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

/** 
	* Enable FIFO Threshold and Touch Detection interrupts
*/
void BSP_TS_Init_extends( uint16_t DeviceAddr, uint8_t FIFO_Threshold ) {
	
	
	// Disable the TS module
	IOE_Write( DeviceAddr, STMPE811_REG_TSC_CTRL, 0x00);
	

	// Set FIFO Threshold and touch detection interrupts
	// also FIFO full interrupt - STMPE811_GIT_FF
	IOE_Write( DeviceAddr, STMPE811_REG_INT_EN, (uint8_t)(STMPE811_GIT_TOUCH | STMPE811_GIT_FTH) ) ;
	
	// Set Innterrupt polarity: falling and type: edge
	// and enable Global interrupts
	IOE_Write(DeviceAddr, STMPE811_REG_INT_CTRL , 0x03 ) ;
	
	// Set FIFO Threshold samples
	IOE_Write( DeviceAddr, STMPE811_REG_FIFO_TH, FIFO_Threshold ) ;
	
	// Enable TC module with:
	// Enable XY mode and movement tracking for 0b100 ( square: 32points )
	IOE_Write( DeviceAddr, STMPE811_REG_TSC_CTRL,   ( 1<<6 ) | 0x03 );
  
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
	static uint8_t status = 0 ;
	static uint8_t fifo_size = 0;
	
	static uint8_t fifo_values[30] ;
	
	static uint8_t double_tap = 0 ;
	static uint8_t togg = 0 ;
	
	if ( ts_int_catched == 1 ) {
		
		// Clear flag
		ts_int_catched = 0 ;
		
		// Read status from TS driver
		status = stmpe811_TS_ITStatus( DeviceAddr ) ;
		
		// Mask for Touch detect and FIFO THRESHOLD flags only
		status &= ( STMPE811_GIT_TOUCH | STMPE811_GIT_FTH ) ;
		
		// get current fifo size
		fifo_size = TS_getFIFO_size( DeviceAddr ) ;
		if( fifo_size > 30 ) 
			fifo_size = 30 ;
		
		TS_readFIFO( DeviceAddr, fifo_size*3, &fifo_values[0] ) ;
		
		// Read data from driver
		if ( status & STMPE811_GIT_TOUCH ) {
			// to do - read from which register if fifo is using too?
			++double_tap ;
			if ( double_tap >= 3 ){ 
				double_tap = 0 ;
				togg ^= 0xFF ;
			}
			
			if( togg ) {
				BSP_LCD_SetLayerVisible(LCD_BACKGROUND_LAYER, ENABLE);
				BSP_LCD_SetLayerVisible(LCD_FOREGROUND_LAYER, DISABLE);
			} else {
				BSP_LCD_SetLayerVisible(LCD_FOREGROUND_LAYER, ENABLE);
				BSP_LCD_SetLayerVisible(LCD_BACKGROUND_LAYER, DISABLE);
			}
		}
		
		if ( status & STMPE811_GIT_FTH ) {
			// to do - read fifo
			// read fifo becouse need to went down bellow threshold
		}
		
		// Clear flags in driver's register
		stmpe811_ClearGlobalIT( DeviceAddr, status ) ;
		
	}
	
}


uint8_t TS_getFIFO_size ( uint16_t DeviceAddr ) {
	 return IOE_Read( DeviceAddr, STMPE811_REG_FIFO_SIZE ) ;
}

uint8_t TS_readFIFO ( uint16_t DeviceAddr, uint16_t no, uint8_t *pTab ) {

	//STMPE811_REG_TSC_DATA_XYZ;
	return (uint8_t)IOE_ReadMultiple( DeviceAddr, STMPE811_REG_TSC_DATA_NON_INC , pTab, no ) ;
	
}