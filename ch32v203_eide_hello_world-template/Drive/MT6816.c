#include "MT6816.h"

void SPI_W_SS(uint8_t Bit)
{
    GPIO_WriteBit(GPIOA, GPIO_Pin_4, (BitAction)Bit);
}
void MySPI_Init(void)
{
    SPI_InitTypeDef SPI_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE); // 开启SPI2的时钟

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13 | GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	//主机模式				 
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //全双工模式 
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;		//每次传输16位数据	
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;			//高位先发送	   
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;  //预分频为8
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;						 //时钟极性高
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;					 //数据在第二个边沿采集
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;						 //软件NSS片选
	SPI_InitStructure.SPI_CRCPolynomial = 10;						  //CRC多项式，用于校验，不常用 

    SPI_Init(SPI2, &SPI_InitStructure);

    SPI_Cmd(SPI2, ENABLE);

    SPI_W_SS(1); // 初始情况拉高SPI的片选信号
}
void SPI_Start(void)
{
    SPI_W_SS(0);
}

void SPI_Stop(void)
{
    SPI_W_SS(1);
}
uint16_t SPI_SwapByte(uint16_t Byte) // 调用库函数来发送和接收
{
    SPI_Start();
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) != SET);

    SPI_I2S_SendData(SPI2, Byte);

    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) != SET);
    return SPI_I2S_ReceiveData(SPI2);
}
/**
 * @brief  通过SPI读取MT6816传感器的角度数据
 * @retval 处理后的14位角度数据
 */
uint16_t Mt6816_ReadData(void)
{
    uint16_t high_byte_data, low_byte_data;
    uint16_t high_byte_cmd, low_byte_cmd;
    uint16_t angledata;
    uint8_t h_count;
    uint8_t r_valid;
    high_byte_cmd =(0x80 | 0x03) << 8;
    low_byte_cmd =(0x80 | 0x04) << 8;
    for (uint8_t i = 0; i < 3; i++) {
        SPI_Start();
        high_byte_data = SPI_SwapByte(high_byte_cmd);
        SPI_Stop();
        SPI_Start();
        low_byte_data = SPI_SwapByte(low_byte_cmd);
        SPI_Stop();

        angledata = (((high_byte_data & 0x00FF ) << 8) | (low_byte_data & 0XFF));
        h_count=0;
        for (uint8_t bit_pos = 0; bit_pos < 16; bit_pos++) {
            if (angledata & (0x0001 << bit_pos)) {
                h_count++;
            }
        }
        if (h_count & 0x01) {
            r_valid = 0;
        } else {
            r_valid = 1;
        }
        if (r_valid) {
            angledata = angledata >> 2;
        }
    }
    return angledata;
}
