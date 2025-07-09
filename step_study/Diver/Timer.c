#include "Timer.h"

void Time2_NVIC_init(void)
{

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    TIM_TimeBaseStructure.TIM_Period        = 144 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler     = 99;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel                   = TIM2_IRQn; // TIM2 中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;         // 先占优先级 0 级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 3;         // 从优先级 1 级
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;    // IRQ 通道被使能
    NVIC_Init(&NVIC_InitStructure);

    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);

    TIM_Cmd(TIM2, ENABLE);
    // 频率计算：144000000/144/100=10000 10k频率
}
void Time4_NVIC_init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    TIM_TimeBaseStructure.TIM_Period        = 144 - 1;
    TIM_TimeBaseStructure.TIM_Prescaler     = 100 - 1;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel                   = TIM4_IRQn; // TIM2 中断
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;         // 先占优先级 0 级
    NVIC_InitStructure.NVIC_IRQChannelSubPriority        = 4;         // 从优先级 1 级
    NVIC_InitStructure.NVIC_IRQChannelCmd                = ENABLE;    // IRQ 通道被使能
    NVIC_Init(&NVIC_InitStructure);

    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);

    TIM_Cmd(TIM4, ENABLE);
    // 频率计算：144000000/144/100=10000 10k频率
}
