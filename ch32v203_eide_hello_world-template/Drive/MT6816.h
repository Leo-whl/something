#ifndef __MT6816_H
#define __MT6816_H
#include "debug.h"

void SPI_W_SS(uint8_t Bit);
void MySPI_Init(void);
void SPI_Start(void);
void SPI_Stop(void);
uint16_t SPI_SwapByte(uint16_t Byte);
uint16_t Mt6816_ReadData();

#endif 
