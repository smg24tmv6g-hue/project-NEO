//
// Created by 23146 on 2026/11/20.
//

#ifndef BSP_IMU_H
#define BSP_IMU_H
#include "stdint.h"

int bsp_imu_init(void);                                      /* 总线 + chip id 过门 */
int bsp_imu_config(void);                                    /* 复位/使能/量程/ODR，内置回读自检 */
int bsp_imu_read_raw(int16_t acc[3], int16_t gyro[3]);       /* 原始 LSB 计数 */
int bsp_imu_read_scaled(float acc_mg[3], float gyro_dps[3]); /* 带单位的换算值 */


#endif //BSP_IMU_H
