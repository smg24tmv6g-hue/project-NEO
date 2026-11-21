//
// Created by 23146 on 2026/11/20.
//

#ifndef BSP_IMU_H
#define BSP_IMU_H
#include "stdint.h"

int bsp_imu_init(void);//读取从机ID acc_id==0x1E gyro_id==0x0F匹配

int bsp_imu_read_accel(int16_t raw[30]);
int bsp_imu_read_gyro(int16_t raw[30]);
int bsp_imu_read_temp(int16_t *raw);//BMI088的温度
#endif //BSP_IMU_H
