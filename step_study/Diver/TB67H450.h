#ifndef __TB67H450_H
#define __TB67H450_H
#include "stdbool.h"
#include "debug.h"
void IN_PWM_Init(void);
void In_Gpio_Init(void);
void TB67H450_Init(void);
void TB67H450_Sleep(void);
void TB67H450_Brake(void);
void Set_Output_I(int Ia, int Ib);
void TB67H450_SetInputA(bool IN1, bool IN2);
void TB67H450_SetInputB(bool IN1, bool IN2);
void StepCtrl(uint8_t step);
void PWM_INB(uint16_t Compare);
void PWM_INA(uint16_t Compare);

#endif
