//
// Created by 23146 on 2026/11/10.
//
#include "bsp_uart.h"
#include <string.h>
#include <stdio.h>
#include "main.h"

#define BSP_UART_HANDLE &huart1
#define BSP_UART_TIMEOUT 100
#define BSP_UART_RX_SIZE 128
extern UART_HandleTypeDef huart1;
static volatile uint16_t rx_head, rx_tail;
static uint8_t rx_buf[BSP_UART_RX_SIZE];
static volatile uint8_t rx_byte;


void bsp_uart_init(void) {

}
int bsp_uart_send(const uint8_t *data, uint16_t length) {
    if (data == NULL || length == 0) {
        return -1;
    }
    if (HAL_UART_Transmit(BSP_UART_HANDLE,(uint8_t *)data,length,BSP_UART_TIMEOUT)!= HAL_OK) {
        return -2;
    }
    return 0;
}
int bsp_uart_receive(uint8_t *data, uint16_t length, uint32_t timeout) {
    if (data == NULL || length == 0) {
        return -1;
    }
    if (HAL_UART_Receive(BSP_UART_HANDLE,(uint8_t *)data,length,timeout)!= HAL_OK) {
        return -2;
    }
    return 0;
}
int __io_putchar(int ch) {
    uint8_t c=(uint8_t)ch;
    bsp_uart_send(&c,1);
    return ch;
}
int bsp_uart_read(uint8_t *buf, uint16_t sise) {

}
