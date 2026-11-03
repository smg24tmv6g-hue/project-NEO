//
// Created by 23146 on 2026/11/3.
//

#ifndef _BAP_UART_H
#define _BAP_UART_H
#include "firsttry/projectneo/BSP/UART/bap_uart.h"

void bap_uart_init(void);
void bap_uart_Sendwave(float data1,float data2,float data3,float data4);//向串口发送的数据，一次四字节
#endif //_BAP_UART_H
