#ifndef __FLASH_H
#define __FLASH_H

#include "debug.h"


void Write_CaliData_Array(uint16_t *data, uint32_t num);
uint16_t Flash_Read_SectorAngle(int addr);
void FLASH_Write_Original_Point(uint16_t num);
uint16_t Flash_Read_Original_Point(void);
#endif
