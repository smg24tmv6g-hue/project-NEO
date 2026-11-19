//
// Created by 23146 on 2026/11/19.
//

#ifndef BSP_CAN_H
#define BSP_CAN_H
#include "main.h"
#include <stdint.h>

#define FDCAN_MAX_DATA_LEN 64

typedef struct {
    uint32_t ID;
    uint8_t Data[FDCAN_MAX_DATA_LEN];
    uint32_t DataLenCan;
}FDCAN_RxMessage_t;

void FDCAN_Init(void);
HAL_StatusTypeDef FDCAN_User(uint32_t ID, uint8_t *Data, uint32_t DataLen, uint32_t Timeout);
#endif //BSP_CAN_H
