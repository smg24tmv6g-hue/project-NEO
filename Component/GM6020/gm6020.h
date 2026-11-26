//
// Created by 23146 on 2026/11/26.
//

#ifndef GM6020_H
#define GM6020_H
#include <stdint.h>

typedef struct {
    uint16_t machine_raw_data;
    int16_t speed;
    int16_t current;
    uint8_t temperature;
}GM6020_Feedback_t;


void GM6020_Parse(GM6020_Feedback_t *motor,const uint8_t *data);;
#endif //GM6020_H
