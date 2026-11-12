//
// Created by 23146 on 2026/11/10.
//
#include "bsp_uart.h"
#include <string.h>
#include <stdio.h>


extern UART_HandleTypeDef huart1;
uint8_t rx_buf;
void BSP_UART_Init(UART_HandleTypeDef* huart) {
  HAL_UART_Receive_IT(&huart1,&rx_buf,1);
}

void BSP_UART_Transmit(float data1,float data2,float data3,float data4) {
    char tx_buf[64];
    sprintf(tx_buf,"%d %d %d %d",(int)data1,(int)data2,(int)data3,(int)data4);
      HAL_UART_Transmit(&huart1,tx_buf,strlen(tx_buf),100);
}//向上位机发送数据

