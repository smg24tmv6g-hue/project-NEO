//
// Created by 23146 on 2026/11/19.
//
#ifndef BSP_CAN_H
#define BSP_CAN_H
#include <stdint.h>

#include "cmsis_os2.h"

/* RX frame queue: the ISR pushes every received frame here,
 * CANRxTask pops and parses them. */
extern osMessageQueueId_t can_rxQueueHandle;

typedef struct {
    uint32_t id;
    uint8_t  data[8];
    uint8_t  data_len;
} CAN_RX_t;

int bsp_can_init(void);
int bsp_can_send(uint32_t id, const uint8_t *data, uint8_t len);

#endif
