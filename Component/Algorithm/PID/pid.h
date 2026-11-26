//
// Created by 23146 on 2026/11/24.
//

#ifndef PID_H
#define PID_H

typedef struct {
    float kp,ki,kd;
    float out_min,out_max;//输出限幅
    float i_min,i_max;//积分项限幅
    float integral;//积分贡献
    float last_error;//上次误差
    float out;//上次输出   三个状态值
    int inited;
} pid_ctrl_t;

void  pid_init(pid_ctrl_t *p, float kp,float ki,float kd,float out_min,float out_max,float i_min,float i_max);
float pid_update(pid_ctrl_t *p,float target,float feedback,float dt);
void  pid_reset(pid_ctrl_t *p);
#endif //PID_H
