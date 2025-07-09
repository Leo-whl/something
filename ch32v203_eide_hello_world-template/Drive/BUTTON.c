#include "BUTTON.h"

Key key1;
Key key2;
Event event;
void Button_Gpio_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void Button_Sturct_Init(void)
{
    key1.id            = 1;
    key1.longPressTime = 500;
    key1.lastPinIO     = Button_ReadButtonPin(1);

    key2.id            = 2;
    key2.longPressTime = 500;
    key2.lastPinIO     = Button_ReadButtonPin(2);
}

bool Button_ReadButtonPin(uint8_t _id)
{
    switch (_id) {
        case 1:
            return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_8) == Bit_SET;

        case 2:
            return GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_10) == Bit_SET;

        default:
            return false;
    }
}
//���жϰ������µļ����Ӷ�ȷ�������˶೤ʱ��
void Button_Tick(Key *key,uint32_t _timeElapseMillis)//_timeElapseMillisΪ��ʱ�����ļ���ֵ
{
    key->timer += _timeElapseMillis;
    bool pinIO = Button_ReadButtonPin(key->id);
    if(key->lastPinIO != pinIO)
    {
        if(pinIO)
        {
            key->onEvent(UP);
            if(key->timer -key->pressTime > key->longPressTime)
            {
                key->onEvent(LONG_PRESS);
            }
            else
            {
                key->onEvent(CLICK);
            }
        }
        else
        {
            key->onEvent(DOWN);
            key->pressTime = key->timer;
        }
    key->lastPinIO = pinIO;
    }
}

/*
    @brief:���ڰ������µĻص�����
    @param��void
    @return:none
    @birth: created by whl on 2025/05/03
*/
void Button_SetOnEventListener(Key *key,EventCallback callback)//�ص������Ĺ̶�д��
{
    key->onEvent =  callback;
}
void Button_Init(void)//��Init
{
    Button_Gpio_Init();
    Button_Sturct_Init();
}


