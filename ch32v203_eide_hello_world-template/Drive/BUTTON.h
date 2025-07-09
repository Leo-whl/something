#ifndef __BUTTON_H
#define __BUTTON_H


#include "debug.h"
#include "stdbool.h"
// ���尴���¼�ö��
typedef enum 
{
    UP,         // ̧��
    DOWN,       // ����
    LONG_PRESS, // ����
    CLICK       // ���
} Event;

typedef void (*EventCallback)(Event event);//�ص�������

typedef struct
{
    uint8_t id;             // ������
    bool lastPinIO;         // �ϸ�״̬
    uint32_t timer;         // ����
    uint32_t pressTime;     // ���µ�ʱ��
    uint32_t longPressTime; // �ж�ʱ��
    EventCallback onEvent;  // �¼��ص�����

} Key;

void Button_Gpio_Init(void);
void Button_Sturct_Init(void);
bool Button_ReadButtonPin(uint8_t _id);
void Button_Tick(Key *key,uint32_t _timeElapseMillis);
void Button_SetOnEventListener(Key *key,EventCallback callback);
void Button_Init(void);

#endif //