//
// Created by 23146 on 2026/11/3.
//
#include "bsp_uart.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
extern UART_HandleTypeDef huart1; // 引用 main.c 的串口句柄
uint8_t rx_data[10];
void BSP_UART_Init(void) {
    HAL_UART_Receive_IT(&huart1, rx_data, 1);
}
// 发送给 VOFA+ 的波形数据（FireWater 文本协议）
void BSP_UART_SendWave(float data1, float data2, float data3, float data4) {
    char tx_buf[64];
    sprintf(tx_buf, "%d,%d,%d,%d\n",
            (int)data1,
            (int)data2,
            (int)data3,
            (int)data4);

    HAL_UART_Transmit(&huart1, (uint8_t *)tx_buf, strlen(tx_buf), 100);
}
