//
// Created by 23146 on 2026/11/20.
//
#include "main.h"
#include "bsp_imu.h"
#include "stdio.h"
//CS控制层
#define ACC_CS_LOW() HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_RESET)
#define ACC_CS_HIGH() HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_SET)
#define GYRO_CS_LOW() HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,GPIO_PIN_RESET)
#define GYRO_CS_HIGH() HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,GPIO_PIN_SET)
#define BSP_IMU_TIMEOUT 100



extern SPI_HandleTypeDef hspi2;

static int acc_read_region(uint8_t region,uint8_t *buf,uint16_t len) {
    uint8_t tx[16+2]={0};
    uint8_t rx[16+2]={0};
    if (len>16) return -1;
    tx[0]=region|0x80;
    ACC_CS_LOW();
    HAL_StatusTypeDef st=HAL_SPI_TransmitReceive(&hspi2,tx,rx,len+2,BSP_IMU_TIMEOUT);
    ACC_CS_HIGH();
    if (st!=HAL_OK) return -2;
    for (uint16_t i=0;i<len;i++) {
        buf[i]=rx[i+2];
    }
    return 0;
}

static int gyro_read_region(uint8_t region, uint8_t *buf, uint16_t len)
{
    uint8_t tx[16+1]={0};
    uint8_t rx[16+1]={0};
    if (len>16) return -1;
    tx[0]=region|0x80;
    GYRO_CS_LOW();
    HAL_StatusTypeDef st=HAL_SPI_TransmitReceive(&hspi2,tx,rx, len+1, BSP_IMU_TIMEOUT);
    GYRO_CS_HIGH();
    if (st!=HAL_OK) return -2;
    for (uint16_t i=0; i<len; i++) {
        buf[i]=rx[i+1];
    }
    return 0;
}

int bsp_imu_init(void) {
    uint8_t id=0;
    ACC_CS_HIGH();
    GYRO_CS_HIGH();
    if (acc_read_region(0x00, &id, 1) != 0) { printf("acc spi err\r\n"); return -1; }
    printf("acc id = 0x%02X (expect 0x1E)\r\n", id);
    if (id != 0x1E) return -2;

    if (gyro_read_region(0x00, &id, 1) != 0) { printf("gyro spi err\r\n"); return -3; }
    printf("gyro id = 0x%02X (expect 0x0F)\r\n", id);
    if (id != 0x0F) return -4;

    return 0;

}