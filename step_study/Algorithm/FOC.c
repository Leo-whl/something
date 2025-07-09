#include "FOC.h"
#include "FastSin.h"
FastSinToDac PhaseA;
FastSinToDac PhaseB;
FOC_parameter_t foc;
uint16_t Scope[200];
void FOC_InPark_Out(uint32_t _directionIncount, int32_t Iq)
{
    int32_t dac_reg;
    PhaseA.sinMapPtr = (_directionIncount) & (0x000003FF);        // 0x000003FF��1023��������1023��
    PhaseB.sinMapPtr = (PhaseA.sinMapPtr + (256)) & (0x000003FF); // ���Ϊ90�㣬�����ķ�֮һ���ڣ�ͬ��������1023��

    PhaseA.sinMapData = sin_mp[PhaseA.sinMapPtr];
    PhaseB.sinMapData = sin_mp[PhaseB.sinMapPtr];

    dac_reg               = abs(Iq);
    dac_reg               = (int32_t)(dac_reg * 5083) >> 12;                   // ��ͬ��dac_reg=dac_reg/3300*4096    ��ϸ����һ�¶���dac_reg=dac_reg*1.24 ����д����Ч�ʸ� ��0-3300���㵽0-4095�ķ�Χ
    dac_reg               = dac_reg & (0x00000FFF);                             // 0000 1111 1111 1111 ����λȫ������ ����12λ
    PhaseA.dacValue12Bits =  (int32_t)(dac_reg * abs(PhaseA.sinMapData)) >> 12; // ��Id=0���еķ�Park����
    PhaseB.dacValue12Bits =  (int32_t)(dac_reg * abs(PhaseB.sinMapData)) >> 12; // ���� Ia=Iq*sin��  Ib=Iq*cos��

    PWM_INA(PhaseA.dacValue12Bits >> 2); // �������12λֵת��Ϊ10λ��ֵ
    PWM_INB(PhaseB.dacValue12Bits >> 2);

    if (PhaseA.sinMapData > 0)
        TB67H450_SetInputA(true, false);
    else if (PhaseA.sinMapData < 0)
        TB67H450_SetInputA(false, true);
    else
        TB67H450_SetInputA(true, true);

    if (PhaseB.sinMapData > 0)
        TB67H450_SetInputB(true, false);
    else if (PhaseB.sinMapData < 0)
        TB67H450_SetInputB(false, true);
    else
        TB67H450_SetInputB(true, true);
    // Set_Output_I(PhaseA.dacValue12Bits>>2,PhaseB.dacValue12Bits>>2 );
}
void Foc_Init(void)
{
    foc.angle  = 0;
    foc.scope  = 0;
    foc.sector = 0;
    Serial_Printf("%d\n",Flash_Read_SectorAngle(200));
    for (uint8_t i = 0; i < 200; i++) {
        Scope[i] = Flash_Read_SectorAngle(i + 1) - Flash_Read_SectorAngle(i);
        Delay_Ms(10);
        if (Scope[i] > SECTOR_TOTAL_COUNT) {
            Scope[i] += ENCODER_MAX_VALUE;
            foc.offset = i;
        Serial_Printf("%d\n", i);

        }
        // Delay_Ms(100);
        // Serial_Printf("%d\n",Scope[i]);
    }
}
uint16_t Sector_tracking(void)
{
    uint16_t temp = (encoder.angle_data / SECTOR_DIVISION_RESULT + foc.offset) % SECTOR_TOTAL_COUNT; // ��ȡ��������λ��
    for (int i = temp - 1; i <= temp + 1; i++)                                                       // ��������Լ������������һ������һ���Ƚ�һ�¿���ǰ�ĽǶ��ǲ�����������֮��Ӷ�ȷ������
    {
        if (Flash_Read_SectorAngle(i + 1) > Flash_Read_SectorAngle(i)) {
            if (encoder.angle_data >= Flash_Read_SectorAngle(i) && encoder.angle_data <= Flash_Read_SectorAngle(i + 1)) // ���������Ƕ�����֮�� ��ô�������������
            {
                foc.sector = (i + SECTOR_TOTAL_COUNT) % SECTOR_TOTAL_COUNT; // ��������ŵĴ�С��200����
                break;                                                      // �ж���ϣ��˳�FOR
            }
            // ���´������������Ҳ���Ǳ�������Խ����ʱ��
        } else if (Flash_Read_SectorAngle(i + 1) < Flash_Read_SectorAngle(i)) // ��Խ����ʱ��
        {
            if (encoder.angle_data >= Flash_Read_SectorAngle(i) && encoder.angle_data <= (Flash_Read_SectorAngle(i + 1) + ENCODER_MAX_VALUE)) {
                foc.sector = (i + SECTOR_TOTAL_COUNT) % SECTOR_TOTAL_COUNT; // ��������ŵĴ�С��200����
                break;                                                      // �ж���ϣ��˳�ѭ��
            }
        }
    }
    //-----------------------------------------------------------׼ȷ�ҵ���ǰ�Ƕ����ĸ�����------------------------------------------------------

    //-----------------------------------------------------------�������ǰ��ʵ�ʽǶȶ�Ӧ�ĵ�Ƕ�-------------------------------------------------
    foc.scope = Scope[foc.sector];
    if (Flash_Read_SectorAngle(foc.sector + 1) > Flash_Read_SectorAngle(foc.sector)) {
        foc.angle = (encoder.angle_data - Flash_Read_SectorAngle(foc.sector)) * 256 / foc.scope + (foc.sector % 4) * 256;
    } else { // ��������Խ�����
        if (encoder.angle_data > 16200) {
            foc.angle = (encoder.angle_data - Flash_Read_SectorAngle(foc.sector)) * 256 / foc.scope + (foc.sector % 4) * 256;
        } else {
            foc.angle = (16384 - Flash_Read_SectorAngle(foc.sector) + encoder.angle_data) * 256 / foc.scope + (foc.sector % 4) * 256;
        }
    }
    foc.angle=foc.angle & (0x000003FF);
    return foc.angle;
}
void FOC_Ctrl(int16_t Iq, int16_t Id, int x)
{
    int cost, sint;
    int Ia, Ib;
    sint = sin_mp[(x+foc.lead_angle) & (0x000003FF)];
    cost = sin_mp[(x + 256 + foc.lead_angle) & (0x000003FF)];
    Ia = (Id * cost - Iq * sint) >>12; // park���任
    Ib = (Iq * cost + Id * sint) >>12;
    Set_Output_I(Ia, Ib);
    //   Serial_Printf("%d,%d,%d\n",x,Ia,Ib);
}