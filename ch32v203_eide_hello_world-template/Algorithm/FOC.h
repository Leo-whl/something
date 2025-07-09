#ifndef __FOC_H__
#define __FOC_H__
#include "main.h"
#define ENCODER_MAX_VALUE 16384
#define SECTOR_TOTAL_COUNT 200
#define SECTOR_DIVISION_RESULT 82
typedef struct
{
    uint16_t sinMapPtr;
    int16_t sinMapData;
    uint16_t dacValue12Bits;
} FastSinToDac;
typedef struct{
	volatile uint8_t	sector;	
	volatile int	scope;	
	volatile int16_t angle;
	volatile int lead_angle;
	volatile float speed_error;
    uint8_t offset;
}FOC_parameter_t;
void FOC_InPark_Out(uint32_t _directionIncount, int32_t Iq);
void Foc_Init(void);
uint16_t Sector_tracking(void);
void FOC_Ctrl(int16_t Iq, int16_t Id,int x);
#endif
