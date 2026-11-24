//
// Created by 23146 on 2026/11/10.
//
#include "bsp_uart.h"
#include <string.h>
#include <stddef.h>
#include "main.h"
#include "cmsis_os2.h"
#include "stdarg.h"
#define BSP_UART_HANDLE (&huart1)
#define BSP_UART_TIMEOUT 100
#define BSP_UART_RX_SIZE 128
#define BSP_UART_RX_SIZE_MASK (BSP_UART_RX_SIZE - 1)
extern UART_HandleTypeDef huart1;

static osMutexId_t uart_mutex=NULL;

typedef struct {
    uint8_t rx_byte;
    volatile uint16_t rx_head;
    volatile uint16_t rx_tail;
    UART_HandleTypeDef *huart;
    volatile uint8_t rx_buf[BSP_UART_RX_SIZE];
}UART_DATA;
static UART_DATA uart1_data;


void bsp_uart_init(void) {
    uart1_data.rx_head = 0;
    uart1_data.rx_tail = 0;
    uart1_data.huart=BSP_UART_HANDLE;
    HAL_UART_Receive_IT(uart1_data.huart, &(uart1_data.rx_byte), 1);

}

void bsp_uart_rtos_init(void)
{
    uart_mutex = osMutexNew(NULL);
}

int bsp_uart_send(const uint8_t *data, uint16_t length) {
    if (data == NULL || length == 0) {
        return -1;
    }
    if (uart_mutex) {
        osMutexAcquire(uart_mutex, osWaitForever);
    }
    HAL_StatusTypeDef st= HAL_UART_Transmit(BSP_UART_HANDLE, (uint8_t *)data, length,BSP_UART_TIMEOUT);
    if (uart_mutex) {
        osMutexRelease(uart_mutex);
        return (st==HAL_OK)?0:-2;
    }
}

int __io_putchar(int ch) {
    uint8_t c=(uint8_t)ch;
    bsp_uart_send(&c,1);
    return ch;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        uint16_t next=(uint16_t)((uart1_data.rx_head+1)&BSP_UART_RX_SIZE_MASK);
        if (next!=uart1_data.rx_tail) {
            uart1_data.rx_buf[uart1_data.rx_head]=uart1_data.rx_byte;
            uart1_data.rx_head=next;
        }
        HAL_UART_Receive_IT(BSP_UART_HANDLE, &uart1_data.rx_byte, 1);
    }
}

int bsp_uart_getc(void) {
    if (uart1_data.rx_head==uart1_data.rx_tail) return (-1);
        uint8_t c=uart1_data.rx_buf[uart1_data.rx_tail];
        uart1_data.rx_tail=(uart1_data.rx_tail+1)&BSP_UART_RX_SIZE_MASK;
        return (int)c;

}

uint16_t bsp_uart_available(void) {
    return (uart1_data.rx_head-uart1_data.rx_tail)&BSP_UART_RX_SIZE_MASK;
}

int bsp_uart_read(uint8_t *buf, uint16_t rx_size) {
    if (buf == NULL||rx_size == 0) return -1;
    uint16_t n = 0;
    while (n < rx_size && uart1_data.rx_tail != uart1_data.rx_head) {
        buf[n++] = uart1_data.rx_buf[uart1_data.rx_tail];
        uart1_data.rx_tail = (uint16_t)((uart1_data.rx_tail + 1) & BSP_UART_RX_SIZE_MASK);
    }
    return (int)n;
}

int bsp_uart_printf(const char * format, ...) {
    if (format == NULL) return -1;
    char buf[128];
    va_list ap;
    va_start(ap, format);
    int n =vsnprintf(buf, sizeof(buf), format, ap);
    va_end(ap);
    if (n<0) return -1;
    if (n>=(int)sizeof(buf)) n=(int)sizeof(buf)-1;
    return bsp_uart_send((const uint8_t*)buf,(uint16_t) n);
}//代替__io_putchar调用bsp_uart_send,避免字符串被截断