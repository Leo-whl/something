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
//来判断按键按下的计数从而确定按下了多长时间
void Button_Tick(Key *key,uint32_t _timeElapseMillis)//_timeElapseMillis为定时器给的计数值
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
    @brief:用于按键按下的回调函数
    @param；void
    @return:none
    @birth: created by whl on 2025/05/03
*/
void Button_SetOnEventListener(Key *key,EventCallback callback)//回调函数的固定写法
{
    key->onEvent =  callback;
}
void Button_Init(void)//总Init
{
    Button_Gpio_Init();
    Button_Sturct_Init();
}


