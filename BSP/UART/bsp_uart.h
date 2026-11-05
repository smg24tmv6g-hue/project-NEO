//
// Created by 23146 on 2026/11/3.
//

#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern UART_HandleTypeDef huart7;

extern UART_HandleTypeDef huart1;

extern UART_HandleTypeDef huart10;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_UART7_Init(void);
void MX_USART1_UART_Init(void);
void MX_USART10_UART_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */