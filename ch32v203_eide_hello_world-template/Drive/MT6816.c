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
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE); // ����SPI2��ʱ��

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

	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;	//����ģʽ				 
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  //ȫ˫��ģʽ 
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;		//ÿ�δ���16λ����	
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;			//��λ�ȷ���	   
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;  //Ԥ��ƵΪ8
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;						 //ʱ�Ӽ��Ը�
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;					 //�����ڵڶ������زɼ�
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;						 //���NSSƬѡ
	SPI_InitStructure.SPI_CRCPolynomial = 10;						  //CRC����ʽ������У�飬������ 

    SPI_Init(SPI2, &SPI_InitStructure);

    SPI_Cmd(SPI2, ENABLE);

    SPI_W_SS(1); // ��ʼ�������SPI��Ƭѡ�ź�
}
void SPI_Start(void)
{
    SPI_W_SS(0);
}

void SPI_Stop(void)
{
    SPI_W_SS(1);
}
uint16_t SPI_SwapByte(uint16_t Byte) // ���ÿ⺯�������ͺͽ���
{
    SPI_Start();
    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_TXE) != SET);

    SPI_I2S_SendData(SPI2, Byte);

    while (SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) != SET);
    return SPI_I2S_ReceiveData(SPI2);
}
/**
 * @brief  ͨ��SPI��ȡMT6816�������ĽǶ�����
 * @retval ������14λ�Ƕ�����
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
