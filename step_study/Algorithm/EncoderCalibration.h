#ifndef __ENCODERCALIBRATION_H
#define __ENCODERCALIBRATION_H

#include "debug.h"
#include "main.h"
typedef struct
{
    volatile uint16_t angle_data;  
    bool rectify_valid;
} Encoder_Typedef;
extern Encoder_Typedef encoder;
void Encoder_Get_AngleData(void);
void Encoder_Cali(void);

#endif
