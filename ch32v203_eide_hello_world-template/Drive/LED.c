#include "LED.h"

void LED_Init()
{

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1 | GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure); 

    GPIO_SetBits(GPIOA, GPIO_Pin_1 | GPIO_Pin_2);
    RedLED_OFF();
    Green_LED_OFF();
}
void RedLED_OFF(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_1);		
}
void RedLED_ON(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_1);		
}
void RedLED_Turn(void)
{
	if (GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_1) == 0)		//��ȡ����Ĵ�����״̬�������ǰ��������͵�ƽ
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_1);					//������PA1����Ϊ�ߵ�ƽ
	}
	else													//���򣬼���ǰ��������ߵ�ƽ
	{
		GPIO_ResetBits(GPIOA, GPIO_Pin_1);					//������PA1����Ϊ�͵�ƽ
	}
}
void Green_LED_OFF(void)
{
	GPIO_ResetBits(GPIOA, GPIO_Pin_2);		//����PA2����Ϊ�͵�ƽ
}
void Green_LED_ON(void)
{
	GPIO_SetBits(GPIOA, GPIO_Pin_2);		//����PA2����Ϊ�ߵ�ƽ
}
void Green_LED_Turn(void)
{
	if (GPIO_ReadOutputDataBit(GPIOA, GPIO_Pin_2) == 0)		//��ȡ����Ĵ�����״̬�������ǰ��������͵�ƽ
	{                                                  
		GPIO_SetBits(GPIOA, GPIO_Pin_2);               		//������PA2����Ϊ�ߵ�ƽ
	}                                                  
	else                                               		//���򣬼���ǰ��������ߵ�ƽ
	{                                                  
		GPIO_ResetBits(GPIOA, GPIO_Pin_2);             		//������PA2����Ϊ�͵�ƽ
	}
}