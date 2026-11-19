//
// Created by 23146 on 2026/11/10.
//
#include "bsp_uart.h"
#include <string.h>
#include <stddef.h>
#include "main.h"

#define BSP_UART_HANDLE (&huart1)
#define BSP_UART_TIMEOUT 100
#define BSP_UART_RX_SIZE 128
#define BSP_UART_RX_SIZE_MASK (BSP_UART_RX_SIZE - 1)
extern UART_HandleTypeDef huart1;
static volatile uint16_t rx_head=0, rx_tail=0;
static uint8_t rx_buf[BSP_UART_RX_SIZE];
static  uint8_t rx_byte;






void bsp_uart_init(void) {
    rx_head=rx_tail=0;
    HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
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

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        uint16_t next=(uint16_t)(rx_head+1)&BSP_UART_RX_SIZE_MASK;
        if (next != rx_tail) {
            rx_buf[rx_head]=rx_byte;
            rx_head=next;
        }
        HAL_UART_Receive_IT(&huart1, &rx_byte, 1);
    }
}

int bsp_uart_getc(void) {
    if (rx_head==rx_tail) return (-1);
        uint8_t c=rx_buf[rx_tail];
        rx_tail=(rx_tail+1)&BSP_UART_RX_SIZE_MASK;
        return (int)c;

}

uint16_t bsp_uart_available(void) {
    return (rx_head-rx_tail)&BSP_UART_RX_SIZE_MASK;
}
int bsp_uart_read(uint8_t *buf, uint16_t rx_size) {
    if (buf == NULL||rx_size == 0) return -1;
    uint16_t n = 0;
    while (n < rx_size && rx_tail != rx_head) {
        buf[n++] = rx_buf[rx_tail];
        rx_tail = (uint16_t)((rx_tail + 1) & BSP_UART_RX_SIZE_MASK);
    }
    return (int)n;
}