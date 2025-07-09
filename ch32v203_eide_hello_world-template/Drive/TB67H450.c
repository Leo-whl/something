#include "TB67H450.h"

#define AP_IN1_PIN GPIO_Pin_2
#define AM_IN2_PIN GPIO_Pin_12
#define BP_IN1_PIN GPIO_Pin_0
#define BM_IN2_PIN GPIO_Pin_1
#define OUT_H      Bit_SET
#define OUT_L      Bit_RESET
void IN_PWM_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure); // B

    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure); // A

    TIM_InternalClockConfig(TIM3);

    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
    TIM_TimeBaseStructure.TIM_ClockDivision     = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode       = TIM_CounterMode_Up;
    TIM_TimeBaseStructure.TIM_Period            = 999;
    TIM_TimeBaseStructure.TIM_Prescaler         = 0;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    TIM_OCInitTypeDef TIM_OCInitStructure;
    TIM_OCStructInit(&TIM_OCInitStructure);

    TIM_OCInitStructure.TIM_OCMode      = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OCPolarity  = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse       = 0;
    TIM_CtrlPWMOutputs(TIM3, ENABLE);
    TIM_OC1Init(TIM3, &TIM_OCInitStructure); // B
    TIM_OC2Init(TIM3, &TIM_OCInitStructure); // A
    TIM_OC1PreloadConfig(TIM3, TIM_OCPreload_Disable);
    TIM_ARRPreloadConfig(TIM3, DISABLE);
    // TIMʹ��
    TIM_Cmd(TIM3, ENABLE);
}
void In_Gpio_Init(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Pin   = AP_IN1_PIN | AM_IN2_PIN | BP_IN1_PIN | BM_IN2_PIN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, AP_IN1_PIN | AM_IN2_PIN | BP_IN1_PIN | BM_IN2_PIN);
}

void TB67H450_Init(void)
{
    IN_PWM_Init();
    In_Gpio_Init();
}

void PWM_INB(uint16_t Compare)//CCR1��B
{
    TIM_SetCompare1(TIM3, Compare);//����CCR1��ֵ  ռ�ձ� = compare/1000
}
void PWM_INA(uint16_t Compare)//CCR2��A
{
    TIM_SetCompare2(TIM3, Compare);//����CCR2��ֵ  ռ�ձ� = compare/1000
}

void TB67H450_SetInputA(bool IN1,bool IN2)
{
    GPIO_WriteBit(GPIOB,AM_IN2_PIN,(BitAction)IN2);
    GPIO_WriteBit(GPIOB,AP_IN1_PIN,(BitAction)IN1);
}

void TB67H450_SetInputB(bool IN1,bool IN2)
{
    GPIO_WriteBit(GPIOB,BM_IN2_PIN,(BitAction)IN2);
    GPIO_WriteBit(GPIOB,BP_IN1_PIN,(BitAction)IN1);
}

void TB67H450_Sleep(void)
{
    PWM_INA(0);
    PWM_INB(0);
    TB67H450_SetInputA(false,false);
    TB67H450_SetInputB(false,false);
}
void TB67H450_Brake(void)
{
    PWM_INA(0);
    PWM_INB(0);
    TB67H450_SetInputA(true,true);
    TB67H450_SetInputB(true,true);
}
//�Ĳ������Ƶ��ת��
void StepCtrl(uint8_t step)//���� ���Ƶ���߶��ٲ� 1��1.8��
{
    static uint8_t last_step = 5;
    if(step != last_step)
    {
        if(step ==0)
        {
            GPIO_WriteBit(GPIOB,AP_IN1_PIN,OUT_H);
            GPIO_WriteBit(GPIOB,AM_IN2_PIN,OUT_L);
            GPIO_WriteBit(GPIOB,BP_IN1_PIN,OUT_L);
            GPIO_WriteBit(GPIOB,BM_IN2_PIN,OUT_L);
        }
        if(step ==1)
        {
            GPIO_WriteBit(GPIOB,AP_IN1_PIN,OUT_L);
            GPIO_WriteBit(GPIOB,AM_IN2_PIN,OUT_L);
            GPIO_WriteBit(GPIOB,BP_IN1_PIN,OUT_H);
            GPIO_WriteBit(GPIOB,BM_IN2_PIN,OUT_L);
        }
        if(step ==2)
        {
            GPIO_WriteBit(GPIOB,AP_IN1_PIN,OUT_L);
            GPIO_WriteBit(GPIOB,AM_IN2_PIN,OUT_H);
            GPIO_WriteBit(GPIOB,BP_IN1_PIN,OUT_L);
            GPIO_WriteBit(GPIOB,BM_IN2_PIN,OUT_L);
        }
        if(step ==3)
        {
            GPIO_WriteBit(GPIOB,AP_IN1_PIN,OUT_L);
            GPIO_WriteBit(GPIOB,AM_IN2_PIN,OUT_L);
            GPIO_WriteBit(GPIOB,BP_IN1_PIN,OUT_L);
            GPIO_WriteBit(GPIOB,BM_IN2_PIN,OUT_H);
        }
        last_step = step;
    }
}

void Set_Output_I(int Ia,int Ib)
{
    if(Ia >0) TB67H450_SetInputA(true,false);
    else 
    {
        Ia = -Ia;
        TB67H450_SetInputA(false,true);
    }
    if(Ib >0) TB67H450_SetInputB(true,false);
    else 
    {
        Ib = -Ib;
        TB67H450_SetInputB(false,true);
    }
    PWM_INA(Ia);
    PWM_INB(Ib);
}