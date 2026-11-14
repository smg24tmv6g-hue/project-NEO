//
// Created by 23146 on 2026/11/14.
//
#include "bsp_buzzer.h"
#include "main.h"
extern TIM_HandleTypeDef htim12;
static uint32_t beep_until=0;
static uint8_t tone=0;
void bsp_buzzer_init(void) {
    HAL_TIM_PWM_Start(&htim12, TIM_CHANNEL_2);//开通道
}
void bsp_buzzer_beep(void) {
    tone= tone^1;//两种音调互换
    if (tone) {
        __HAL_TIM_SET_AUTORELOAD(&htim12,1999);//2kHz=4mHz/2000
        __HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_2, 1000);//占空比50%
    }else {
        __HAL_TIM_SET_AUTORELOAD(&htim12,999);//4kHz=4mHz/1000
        __HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_2, 700);//占空比70%
    }
    beep_until = HAL_GetTick()+200;
}//调音调种类
void bsp_buzzer_stop(void) {
    __HAL_TIM_SET_COMPARE(&htim12, TIM_CHANNEL_2, 0);
    beep_until = 0;
}//清空CRR
void bsp_buzzer_tick(void) {
    if (beep_until!=0&&HAL_GetTick()>=beep_until)
        bsp_buzzer_stop();
}//调完静音