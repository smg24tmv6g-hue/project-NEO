//
// Created by 23146 on 2026/11/19.
//
#ifndef BSP_CAN_H
#define BSP_CAN_H
#include <stdint.h>

typedef void (*bsp_can_handler_t)(uint32_t id, const uint8_t *data, uint8_t len);

int bsp_can_init(void);
int bsp_can_send(uint32_t id, const uint8_t *data, uint8_t len);
int bsp_can_register(uint32_t id, bsp_can_handler_t handler);

#endif