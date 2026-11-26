//
// Created by 23146 on 2026/11/14.
//
#include "bsp_led.h"
#include "main.h"
#include "../../Component/ENCODE_WS2812/ws2812_encode.h"
extern SPI_HandleTypeDef hspi6;

void bsp_led_boot(void) {
        for (int i = 0; i <2; i++) {
                bsp_led_set(0,255,0);
                HAL_Delay(100);
                bsp_led_set(0,0,0);
                HAL_Delay(100);
        }
}

void bsp_led_set(uint8_t r, uint8_t g, uint8_t b) {
        uint8_t spi_buf[24];
        ws2812_encode(g, r, b, spi_buf);//顺序grb
        HAL_SPI_Transmit(&hspi6,spi_buf,24,100);
        uint32_t t0=HAL_GetTick();
      while(HAL_GetTick()-t0<2) {}
}