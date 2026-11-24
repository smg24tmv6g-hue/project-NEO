//
// Created by 23146 on 2026/11/19.
//
#include "bsp_can.h"
#include "main.h"
#include <stddef.h>
#include <stdio.h>
extern FDCAN_HandleTypeDef hfdcan1;

#define BSP_CAN_MAX_HANDLERS 8
static struct { uint32_t id; bsp_can_handler_t fn; } s_tab[BSP_CAN_MAX_HANDLERS];

int bsp_can_init(void)
{
    if (HAL_FDCAN_ConfigGlobalFilter(&hfdcan1,
            FDCAN_ACCEPT_IN_RX_FIFO0, FDCAN_ACCEPT_IN_RX_FIFO0,
            FDCAN_FILTER_REMOTE, FDCAN_FILTER_REMOTE) != HAL_OK) return -2;
    if (HAL_FDCAN_Start(&hfdcan1) != HAL_OK) return -1;
    if (HAL_FDCAN_ActivateNotification(&hfdcan1,
            FDCAN_IT_RX_FIFO0_NEW_MESSAGE | FDCAN_IT_BUS_OFF, 0) != HAL_OK) return -3;
    return 0;
}

int bsp_can_send(uint32_t id, const uint8_t *data, uint8_t len)
{
    if (data == NULL || len > 8) return -1;
    FDCAN_TxHeaderTypeDef h = {0};
    h.Identifier = id;
    h.IdType = FDCAN_STANDARD_ID;
    h.TxFrameType = FDCAN_DATA_FRAME;
    h.DataLength = (uint32_t)len;
    h.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    h.BitRateSwitch = FDCAN_BRS_OFF;
    h.FDFormat = FDCAN_CLASSIC_CAN;
    h.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    h.MessageMarker = 0;
    return (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &h, (uint8_t *)data) == HAL_OK)?0:-2;
}

int bsp_can_register(uint32_t id, bsp_can_handler_t handler)
{
    for (int i = 0; i < BSP_CAN_MAX_HANDLERS; i++)
        if (s_tab[i].fn == NULL) {
            s_tab[i].id = id; s_tab[i].fn = handler;
            return 0;
        }
    return -1;
}

void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hcan, uint32_t RxFifo0ITs) {
    if (hcan->Instance != FDCAN1) {
        return;
    }
    if (!(RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE)) {
        return;
    }
    FDCAN_RxHeaderTypeDef rh;
    uint8_t d[8];
    uint32_t guard=0;
    while ((guard++<32U)&&(HAL_FDCAN_GetRxMessage(hcan, FDCAN_RX_FIFO0, &rh, d) == HAL_OK))
    {
        for (int i = 0; i<BSP_CAN_MAX_HANDLERS;i++) {
            if (s_tab[i].fn && s_tab[i].id == rh.Identifier) {
                s_tab[i].fn(rh.Identifier, d, (uint8_t)rh.DataLength);
                break;
            }
        }
    }
}