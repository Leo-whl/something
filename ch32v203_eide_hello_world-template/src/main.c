/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2021/06/06
 * Description        : Main program body.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 *GPIO routine:
 *PA0 push-pull output.
 *
 */

#include "main.h"

void TIM2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM4_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));//WCH必须要加  RIS——V架构中，中断处理函数必须加 __attribute__((interrupt("WCH-Interrupt-fast")));
extern Key key1;
extern Key key2;
void HandleButtonEvent1(Event event);
void HandleButtonEvent2(Event event);//定义回调函数


/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
    uint16_t i = 0;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
    SystemCoreClockUpdate();
    Delay_Init();
    MySPI_Init();
    Serial_Init();
    LED_Init();
    Time2_NVIC_init();
    Time4_NVIC_init();
    OLED_Init();
    OLED_ShowNum(8,8,2,5);
    OLED_Refresh();
    TB67H450_Init();
    Button_Init();
    Button_SetOnEventListener(&key1,HandleButtonEvent1);
    Button_SetOnEventListener(&key2,HandleButtonEvent2);//绑定回调函数
    Encoder_Cali();
    Foc_Init();
    Serial_Printf("%d\n",16213%1024);
    while(1)
    {
    //     Serial_Printf("%d\n",Mt6816_ReadData()); 
        for(i=0;i<200;i++)
        {
            Delay_Ms(10);
            Serial_Printf("%d,%d\n",i,Flash_Read_SectorAngle(i));
        }
    }
}
void TIM2_IRQHandler(void)//频率10k
{
    static uint32_t count;
    if(TIM_GetITStatus(TIM2, TIM_IT_Update) == SET)//标志位置1，进入中断
    {
        count++;
        if(count >= 200)
        {
            Button_Tick(&key1,10);
            Button_Tick(&key2,10);
            count = 0;
        }
        TIM_ClearITPendingBit(TIM2, TIM_IT_Update);//清除TIMx的更新中断标志
    }
}
void TIM4_IRQHandler(void)
{
    if(TIM_GetITStatus(TIM4, TIM_IT_Update) == SET)//标志位置1，进入中断
    {
        TIM_ClearITPendingBit(TIM4, TIM_IT_Update);//清除TIMx的更新中断标志
    }
}
void HandleButtonEvent1(Event event)
{
    switch(event)
    {
        case CLICK:
           RedLED_Turn();
            break;
        case DOWN:
            break;
        case LONG_PRESS:

            break;
        case UP:
            break;
    }
}
void HandleButtonEvent2(Event event)
{
    switch(event)
    {
        case CLICK:
           
            break;
        case DOWN:
            break;
        case LONG_PRESS:
         Green_LED_Turn();
            break;
        case UP:
            break;
    }
}