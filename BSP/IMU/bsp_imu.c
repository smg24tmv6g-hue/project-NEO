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
#define ACC_LSB_PER_G 5460.0f//bmi088datasheet中6g量程的系数
#define GYRO_LSB_PER_DPS 16.384f//2000dps的系数




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

static int acc_write_reg(uint8_t reg, uint8_t val)
{
    uint8_t tx[2]={(uint8_t)(reg & 0x7F),val};
    ACC_CS_LOW();
    HAL_StatusTypeDef st=HAL_SPI_Transmit(&hspi2,tx,2,BSP_IMU_TIMEOUT);
    ACC_CS_HIGH();
    return (st==HAL_OK)?0:-2;
}
static int gyro_write_reg(uint8_t reg, uint8_t val)
{
    uint8_t tx[2]={(uint8_t)(reg & 0x7F),val};
    GYRO_CS_LOW();
    HAL_StatusTypeDef st=HAL_SPI_Transmit(&hspi2,tx,2,BSP_IMU_TIMEOUT);
    GYRO_CS_HIGH();
    return (st==HAL_OK)?0:-2;
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

int bsp_imu_config(void)
{
    uint8_t v = 0;
    gyro_write_reg(0x14,0xB6);//陀螺仪复位
    HAL_Delay(30);
    gyro_write_reg(0x0F,0x00);//gyro量程-+2000dps
    gyro_write_reg(0x10,0x07);//ODR100hz和gyro传输带宽
    acc_write_reg(0x7E,0xB6);//acc复位
    HAL_Delay(80);
    acc_write_reg(0x7C,0x00);//激活acc
    HAL_Delay(2);
    acc_write_reg(0x7D,0x04);//acc使能
    HAL_Delay(2);
    acc_write_reg(0x7D,0x04);//二次使能
    HAL_Delay(10);
    acc_write_reg(0x41, 0x01);//acc量程-+6g
    HAL_Delay(1);
    acc_write_reg(0x40, 0xA8);
    HAL_Delay(10);//ODR100hz和acc带宽
    acc_read_region(0x41,&v,1);
    printf("acc_range=0x%02X(expect 0x01)\r\n",v);
    acc_read_region(0x40,&v,1);
    printf("acc_conf=0x%02X\r\n",v);
    acc_read_region(0x7C,&v,1);
    printf("acc_pwr_conf=0x%02X (expect 0x00)\r\n",v);
    acc_read_region(0x7D,&v,1);
    printf("acc_pwr_ctrl=0x%02X(expect 0x04)\r\n",v);
    gyro_read_region(0x0F, &v, 1);
    printf("gyro_range=0x%02X(expect 0x00)\r\n",v);
    return 0;
}

int bsp_imu_read_raw(int16_t acc[3], int16_t gyro[3])
{
    uint8_t b[6];
    if (acc_read_region(0x12, b, 6) != 0) return -1;      /* X_LSB..Z_MSB */
    for (int i = 0; i < 3; i++) acc[i] = (int16_t)(((uint16_t)b[2*i+1] << 8) | b[2*i]);
    if (gyro_read_region(0x02, b, 6) != 0) return -2;
    for (int i = 0; i < 3; i++) gyro[i] = (int16_t)(((uint16_t)b[2*i+1] << 8) | b[2*i]);
    return 0;
}

int bsp_imu_read_scaled(float acc_mg[3], float gyro_dps[3])
{
    int16_t a[3], g[3];
    if (bsp_imu_read_raw(a,g) != 0) {
        return -1;
    }
    for (int i = 0; i < 3; i++) {
        acc_mg[i]=a[i]*1000.0f/ACC_LSB_PER_G;
        gyro_dps[i]=g[i]/GYRO_LSB_PER_DPS;
    }
    return 0;
}

