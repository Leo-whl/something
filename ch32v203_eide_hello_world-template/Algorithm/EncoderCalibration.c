#include "EncoderCalibration.h"

uint16_t SampleDataAverage[201];
Encoder_Typedef  encoder;

void Encoder_Get_AngleData(void)
{
    encoder.angle_data = Mt6816_ReadData();
}
void Encoder_Cali(void)
{
    if(!GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_8))
    {
        uint16_t i =0;
        PWM_INB(400);
        PWM_INA(400);
        Encoder_Get_AngleData();
        while (i<201)
        {
            StepCtrl(i%4);
            Delay_Ms(100);
            Encoder_Get_AngleData();
            SampleDataAverage[i] = encoder.angle_data;
            Serial_Printf("%d\n",encoder.angle_data);
            Green_LED_Turn();
            RedLED_Turn();
            i++;
        }
        PWM_INB(0);
        PWM_INA(0);
        Write_CaliData_Array(SampleDataAverage,200);
        FLASH_Write_Original_Point(SampleDataAverage[199]);
    }
}