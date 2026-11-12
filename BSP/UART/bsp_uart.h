//
// Created by 23146 on 2026/11/10.
//

#ifndef BSP_UART_H
#define BSP_UART_H
#include "main.h"
#include <stdint.h>

#include "stm32h7xx_hal_uart.h"

void BSP_UART_Init(UART_HandleTypeDef* huart);
void BSP_UART_Transmit(float data1,float data2,float data3,float data4);
#endif //BSP_UART_H
