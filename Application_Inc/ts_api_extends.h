#ifndef _TS_API_EXTENDS_
#define _TS_API_EXTENDS_


void BSP_TS_Init_extends( uint16_t DeviceAddr, uint8_t FIFO_Threshold ) ;
void TS_checkEvent( uint16_t DeviceAddr );
uint8_t TS_getFIFO_size ( uint16_t DeviceAddr ) ;
uint8_t TS_readFIFO ( uint16_t DeviceAddr, uint16_t no, uint8_t *pTab ) ;
#endif
