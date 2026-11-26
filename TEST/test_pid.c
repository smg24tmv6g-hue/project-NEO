//
// Created by 23146 on 2026/11/26.
//
#include <stdio.h>
#include <math.h>
#include "../Component/Algorithm/PID/pid.h"

static int g_total = 0, g_fail = 0;
#define EPS 1e-4f

/* 断言一个 float 结果，失败只记录不中断，方便一次看全部 */
static void check_close(const char *name, float got, float want)
{
    g_total++;
    if (fabsf(got - want) > EPS) {
        printf("[FAIL] %-28s got=%.6f want=%.6f\n", name, got, want);
        g_fail++;
    } else {
        printf("[ OK ] %-28s got=%.6f\n", name, got);
    }
}

static void test_p_only(void)
{
    pid_ctrl_t p;
    pid_init(&p, 2.0f, 0.0f, 0.0f, -100.0f, 100.0f, -50.0f, 50.0f);
    check_close("P: kp=2, e=6", pid_update(&p, 10.0f, 4.0f, 0.01f), 12.0f);
    check_close("P: e=0 -> 0", pid_update(&p, 5.0f, 5.0f, 0.01f), 0.0f);
}
static void test_i_only(void)
{
    pid_ctrl_t p;
    pid_init(&p, 0.0f, 2.0f, 0.0f,-100.0f, 100.0f, -50.0f, 50.0f);
    check_close("I: first update",pid_update(&p, 10.0f, 5.0f, 0.1f),1.0f);
    check_close("I: second update",pid_update(&p, 10.0f, 5.0f, 0.1f),2.0f);
}
/* ... 其余用例 ... */
static void test_d_only(void)
{
    pid_ctrl_t p;
    pid_init(&p, 0.0f, 0.0f, 1.0f,-1000.0f, 1000.0f, -50.0f, 50.0f);
    pid_update(&p, 10.0f, 0.0f, 0.1f);
    check_close("D: error change",pid_update(&p, 5.0f, 0.0f, 0.1f),-50.0f);
}
static void test_clamp(void)
{
    pid_ctrl_t p;
    pid_init(&p, 1.0f,0.0f, 0.0f, -100.0f,100.0f,-50.0f,50.0f);
    float o = pid_update(&p,1000.0f, 0.0f,0.01f);
    check_close("clamp: return",o,100.0f);
    check_close("clamp: p.out",p.out,100.0f);
}
static void test_anti_windup(void)
{
    pid_ctrl_t p;
    pid_init(&p, 1.0f,1.0f,0.0f,-10.0f,10.0f,-50.0f,50.0f);
    for (int i = 0; i < 50; i++)
        pid_update(&p, 1000.0f, 0.0f, 0.01f);       /* 持续大误差、一直饱和 */
    check_close("anti-windup: out",p.out,10.0f);
    check_close("anti-windup: integral",p.integral,0.0f);   /* 被否决，没累计 */
}
static void test_bad_dt(void)
{
    pid_ctrl_t p;
    pid_init(&p, 1.0f, 1.0f, 0.0f, -100.0f, 100.0f, -50.0f, 50.0f);
    pid_update(&p, 10.0f, 0.0f, 0.01f);             /* 正常一拍：out=10, integral=0.1 */
    float before_i = p.integral, before_out = p.out;
    check_close("bad dt=0: return", pid_update(&p, 10.0f, 0.0f, 0.0f), before_out);
    check_close("bad dt=0: integral", p.integral, before_i);
    check_close("bad dt=1: return", pid_update(&p, 10.0f, 0.0f, 1.0f), before_out);
}
static void test_two_instances(void)
{
    pid_ctrl_t a, b;
    pid_init(&a, 1.0f,0.0f,0.0f,-100.0f,100.0f, -50.0f, 50.0f);
    pid_init(&b, 2.0f, 0.0f, 0.0f,-100.0f, 100.0f, -50.0f,50.0f);
    check_close("two: a out", pid_update(&a,10.0f,0.0f,0.01f),10.0f);
    check_close("two: b out", pid_update(&b,10.0f,0.0f,0.01f),20.0f);
}
int main(void)
{
    test_p_only();
    test_i_only();
    test_d_only();
    test_clamp();
    test_anti_windup();
    test_bad_dt();
    test_two_instances();
    printf("\n==== %d/%d passed ====\n", g_total - g_fail, g_total);
    return (g_fail == 0) ? 0 : 1;
}
