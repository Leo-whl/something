#ifndef __ENCODERCALIBRATION_H
#define __ENCODERCALIBRATION_H

#include "debug.h"
#include "OLED.h"
#include "MT6816.h"
#include "LED.h"
#include "Button.h"
#include "Flash.h"
#include "TB67H450.h"
#include "stdbool.h"

typedef struct
{
  volatile uint16_t angle_data;
  bool rectify_valid;
} Encoder_Typedef;

void Encoder_Get_AngleData(void);
void Encoder_Cali(void);


#endif 