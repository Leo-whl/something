#include "EncoderCalibration.h"
uint16_t SampleDataAverage[201];
Encoder_Typedef encoder;
void Encoder_Get_AngleData(void)
{
    encoder.angle_data = Mt6816_ReadData();
}

void Encoder_Cali(void)
{
    if (!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8)) {

        uint8_t i = 0;
        PWM_INB(400);
        PWM_INA(400);
        StepCtrl(0);
        Encoder_Get_AngleData();
        Delay_Ms(100);
        StepCtrl(1);
        Encoder_Get_AngleData();
        Delay_Ms(100);
        StepCtrl(2);
        Encoder_Get_AngleData();
        Delay_Ms(100);
        StepCtrl(3);
        Encoder_Get_AngleData();
        Delay_Ms(100);
        while (i < 201) {
        StepCtrl(i % 4);
        Delay_Ms(100);
        Encoder_Get_AngleData();
        SampleDataAverage[i] = encoder.angle_data;
        Serial_Printf("%d,%d\n", i, SampleDataAverage[i]);
        Green_LED_Turn();
        RedLED_Turn();
        i++;
        }
        PWM_INB(0);
        PWM_INA(0);
        Write_CaliData_Array(SampleDataAverage, 200);
        FLASH_Write_Original_Point(SampleDataAverage[199]);
    }
}