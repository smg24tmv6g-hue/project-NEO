//
// Created by 23146 on 2026/11/26.
//
#include "gm6020.h"

GM6020_Feedback_t gm6020_feedback_t;

void GM6020_Parse(GM6020_Feedback_t *motor,const uint8_t *data){
    motor->machine_raw_data=((uint16_t)data[0]<<8) | (uint16_t)data[1];
    motor->speed=(int16_t)((uint16_t)data[2]<<8) | (uint16_t)data[3];
    motor->current=(int16_t)((uint16_t)data[4]<<8) | (uint16_t)data[5];
    motor->temperature=data[6];
}