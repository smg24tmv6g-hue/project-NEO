//
// Created by 23146 on 2026/11/10.
//

#ifndef BSP_UART_H
#define BSP_UART_H

#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

void bsp_uart_init(void);
int  bsp_uart_send(const uint8_t *data, uint16_t length);
int  bsp_uart_receive(uint8_t *buf, uint16_t length, uint32_t timeout);
int  bsp_uart_read(uint8_t *buf, uint16_t rx_size);
int  __io_putchar(int ch);
uint16_t bsp_uart_available(void);

#endif // BSP_UART_H
