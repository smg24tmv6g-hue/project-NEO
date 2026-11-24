//
// Created by 23146 on 2026/11/10.
//

#ifndef BSP_UART_H
#define BSP_UART_H

#include <stdint.h>
#include <stdio.h>

void bsp_uart_init(void);
int  bsp_uart_send(const uint8_t *data, uint16_t length);
void bsp_uart_rtos_init(void) ;
int  bsp_uart_read(uint8_t *buf, uint16_t rx_size);
uint16_t bsp_uart_available(void);
int  bsp_uart_getc(void);
int  bsp_uart_printf(const char * format, ...);//替代__io_putchar
int __io_putchar(int ch);
#endif // BSP_UART_H
