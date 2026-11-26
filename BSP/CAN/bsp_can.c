//
// Created by 23146 on 2026/11/19.
//
#include "bsp_can.h"
#include "main.h"
#include <stddef.h>

extern FDCAN_HandleTypeDef hfdcan1;

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
    return (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &h, (uint8_t *)data) == HAL_OK) ? 0 : -2;
}

/* debug counters (volatile: written in ISR, read in task) */
volatile uint32_t g_can_isr_cnt = 0;
volatile uint32_t g_can_rx_ok = 0;
volatile uint32_t g_can_rx_drop = 0;
volatile uint32_t g_can_rx_fail = 0;

/* ISR: fetch frames and push them into the queue only.
 * No parsing, no printing, no blocking.
 * IMPORTANT: always drain the RX FIFO. If we leave frames in it, the FIFO
 * becomes full, RF0N stops being set and the interrupt never fires again. */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hcan, uint32_t RxFifo0ITs)
{
    if (hcan->Instance != FDCAN1) return;
    g_can_isr_cnt++;

    FDCAN_RxHeaderTypeDef rh;
    uint8_t d[8];
    uint32_t guard = 0;

    while (guard++ < 8U)
    {
        if (HAL_FDCAN_GetRxMessage(hcan, FDCAN_RX_FIFO0, &rh, d) != HAL_OK) {
            g_can_rx_fail++;
            break;
        }
        g_can_rx_ok++;

        if (can_rxQueueHandle != NULL) {
            CAN_RX_t f;
            f.id = rh.Identifier;
            f.data_len = (uint8_t)rh.DataLength;
            for (int i = 0; i < 8; i++) f.data[i] = d[i];
            if (osMessageQueuePut(can_rxQueueHandle, &f, 0, 0) != osOK) {
                g_can_rx_drop++;
            }
        } else {
            g_can_rx_drop++;              /* queue not created yet: discard, but keep FIFO empty */
        }
    }
}
