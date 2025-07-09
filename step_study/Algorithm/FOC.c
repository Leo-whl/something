#include "FOC.h"
#include "FastSin.h"
FastSinToDac PhaseA;
FastSinToDac PhaseB;
FOC_parameter_t foc;
uint16_t Scope[200];
void FOC_InPark_Out(uint32_t _directionIncount, int32_t Iq)
{
    int32_t dac_reg;
    PhaseA.sinMapPtr = (_directionIncount) & (0x000003FF);        // 0x000003FF是1023，限制在1023内
    PhaseB.sinMapPtr = (PhaseA.sinMapPtr + (256)) & (0x000003FF); // 相差为90°，就是四分之一周期，同样限制在1023内

    PhaseA.sinMapData = sin_mp[PhaseA.sinMapPtr];
    PhaseB.sinMapData = sin_mp[PhaseB.sinMapPtr];

    dac_reg               = abs(Iq);
    dac_reg               = (int32_t)(dac_reg * 5083) >> 12;                   // 等同于dac_reg=dac_reg/3300*4096    详细计算一下都是dac_reg=dac_reg*1.24 这样写计算效率高 把0-3300换算到0-4095的范围
    dac_reg               = dac_reg & (0x00000FFF);                             // 0000 1111 1111 1111 将高位全部清零 保留12位
    PhaseA.dacValue12Bits =  (int32_t)(dac_reg * abs(PhaseA.sinMapData)) >> 12; // 将Id=0进行的反Park运算
    PhaseB.dacValue12Bits =  (int32_t)(dac_reg * abs(PhaseB.sinMapData)) >> 12; // 就是 Ia=Iq*sinθ  Ib=Iq*cosθ

    PWM_INA(PhaseA.dacValue12Bits >> 2); // 讲计算的12位值转化为10位的值
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
    uint16_t temp = (encoder.angle_data / SECTOR_DIVISION_RESULT + foc.offset) % SECTOR_TOTAL_COUNT; // 获取扇区大致位置
    for (int i = temp - 1; i <= temp + 1; i++)                                                       // 在这个粗略计算的扇区的上一个和下一个比较一下看当前的角度是不是在这两个之间从而确定扇区
    {
        if (Flash_Read_SectorAngle(i + 1) > Flash_Read_SectorAngle(i)) {
            if (encoder.angle_data >= Flash_Read_SectorAngle(i) && encoder.angle_data <= Flash_Read_SectorAngle(i + 1)) // 在这两个角度数据之间 那么就是在这个扇区
            {
                foc.sector = (i + SECTOR_TOTAL_COUNT) % SECTOR_TOTAL_COUNT; // 让扇区序号的大小在200以内
                break;                                                      // 判断完毕，退出FOR
            }
            // 以下处理特殊情况，也就是编码器跨越零点的时候
        } else if (Flash_Read_SectorAngle(i + 1) < Flash_Read_SectorAngle(i)) // 跨越零点的时候
        {
            if (encoder.angle_data >= Flash_Read_SectorAngle(i) && encoder.angle_data <= (Flash_Read_SectorAngle(i + 1) + ENCODER_MAX_VALUE)) {
                foc.sector = (i + SECTOR_TOTAL_COUNT) % SECTOR_TOTAL_COUNT; // 让扇区序号的大小在200以内
                break;                                                      // 判断完毕，退出循环
            }
        }
    }
    //-----------------------------------------------------------准确找到当前角度在哪个扇区------------------------------------------------------

    //-----------------------------------------------------------计算出当前的实际角度对应的电角度-------------------------------------------------
    foc.scope = Scope[foc.sector];
    if (Flash_Read_SectorAngle(foc.sector + 1) > Flash_Read_SectorAngle(foc.sector)) {
        foc.angle = (encoder.angle_data - Flash_Read_SectorAngle(foc.sector)) * 256 / foc.scope + (foc.sector % 4) * 256;
    } else { // 处理零点跨越的情况
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
    Ia = (Id * cost - Iq * sint) >>12; // park反变换
    Ib = (Iq * cost + Id * sint) >>12;
    Set_Output_I(Ia, Ib);
    //   Serial_Printf("%d,%d,%d\n",x,Ia,Ib);
}