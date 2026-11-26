//
// Created by 23146 on 2026/11/24.
//
#include "stdint.h"
#include "stddef.h"
#include "pid.h"

#define PID_DT_MIN 0.0005f
#define PID_DT_MAX 0.5f

void pid_init(pid_ctrl_t *p, float kp, float ki, float kd,float out_min, float out_max, float i_min, float i_max) {
        if (p==NULL) return;
        p->kp = kp;
        p->ki = ki;
        p->kd = kd;//存参
        p->out_min = out_min;
        p->out_max = out_max;//输出限幅
        p->i_min = i_min;
        p->i_max = i_max;//积分限幅
        p->integral=0.0f;
        p->last_error= 0.0f;
        p->out= 0.0f;//清空历史计算
        p->inited= 1;
}
void pid_reset(pid_ctrl_t *p) {
        if (p==NULL) return;
        p->integral= 0;
        p->last_error= 0.0f;
        p->out= 0.0f;
}

float pid_update(pid_ctrl_t *p,float target,float feedback,float dt) {
        if (p==NULL||!p->inited) return 0.0f;
        if (dt<PID_DT_MIN||dt>PID_DT_MAX) return p->out;
        float error=target-feedback;
        float p_term=p->kp*error;
        float d_term=p->kd*(error-p->last_error)/dt;
        float new_integral=p->integral+p->ki*error*dt;
        if (new_integral>p->i_max) new_integral=p->i_max;
        if (new_integral<p->i_min) new_integral=p->i_min;
        float candidate_out=p_term+d_term+new_integral;
        if (!((candidate_out>=p->out_max&&error>0.0f)||(candidate_out<=p->out_min&&error<0.0f))) {
                p->integral=new_integral;
        }
        float out=p_term+p->integral+d_term;
        if (out>p->out_max) out=p->out_max;
        if (out<p->out_min) out=p->out_min;
        p->last_error=error;
        p->out=out;
        return out;
}