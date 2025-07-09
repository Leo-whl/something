#ifndef __BUTTON_H
#define __BUTTON_H
#include "stdbool.h"
#include "debug.h"
// 定义按键事件枚举
typedef enum {
    UP,  //抬起
    DOWN,//按下
    LONG_PRESS,//长按
    CLICK//点击
} Event;
typedef void (*EventCallback)(Event);  //回调类声明
typedef struct 
{
    uint8_t id;  //按键号
    bool lastPinIO; 
    uint32_t timer ; //计数
    uint32_t pressTime; //按下时长
    uint32_t longPressTime ; //长按的时长
    EventCallback onEvent; // 事件回调函数
}Key;
void Button_Gpio_Init(void);
void Button_Sturct_Init(void);
bool Button_ReadButtonPinIO(uint8_t _id);
void Button_Tick(Key *key, uint32_t _timeElapseMillis) ;
void Button_SetOnEventListener(Key *key, EventCallback callback);
void Button_Init(void);

#endif
