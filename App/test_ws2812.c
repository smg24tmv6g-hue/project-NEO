//
// Created by 23146 on 2026/11/8.
//
#include <stdio.h>
#include <stdint.h>
#include "ws2812_encode.h"

static int fails = 0;
#define CHECK(cond, msg) do { if (!(cond)) { printf("FAIL: %s\n", msg); fails = 1; } } while (0)

int main(void)
{
    uint8_t out[24];

    /* 向量1 纯红(r=255)：G 段全 0xC0、R 段全 0xF0、B 段全 0xC0 —— 专杀 GRB/RGB 顺序病 */
    ws2812_encode(0, 255, 0, out);
    for (int i = 0; i < 8; i++)  CHECK(out[i] == 0xC0, "red: G bytes");
    for (int i = 8; i < 16; i++) CHECK(out[i] == 0xF0, "red: R bytes");
    for (int i = 16; i < 24; i++)CHECK(out[i] == 0xC0, "red: B bytes");

    /* 向量2 g=0x80(仅最高位1)：out[0]=0xF0、out[1]=0xC0 —— 专杀 LSB-first 病 */
    ws2812_encode(0x80, 0, 0, out);
    CHECK(out[0] == 0xF0, "g=0x80: byte0");
    CHECK(out[1] == 0xC0, "g=0x80: byte1");

    /* 向量3 全0：24 字节全 0xC0 —— 基线 */
    ws2812_encode(0, 0, 0, out);
    for (int i = 0; i < 24; i++) CHECK(out[i] == 0xC0, "zero baseline");

    /* 向量4 你自己加：选一个颜色，先手算 24 字节里前 4 个该是什么，再断言 */
    if (fails == 0) printf("PASS: all vectors\n");
    return fails;
}
