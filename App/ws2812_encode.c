#include <stdint.h>
#include "ws2812_encode.h"
//
// Created by 23146 on 2026/11/7.
//
#define WS2812_BIT0 0xC0u//n=2自己推出来的
#define WS2812_BIT1 0xF0u//n=4自己推出来的

void ws2812_encode(uint8_t g, uint8_t r, uint8_t b, uint8_t out[24])
{
    const uint8_t color[3] = {g,r,b};
    for (uint8_t byte = 0; byte < 3; byte++)
    {
        for (int8_t bit=7;bit>=0;bit--) {
            uint8_t on = (color[byte]>>bit)&1u;
            out[byte * 8 + (7 - bit)] = on ? WS2812_BIT1 : WS2812_BIT0;
        }



        }




}