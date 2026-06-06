#ifndef __MOTOR_H
#define __MOTOR_H

#include "public.h"

#define FILTER_N 6 //采样窗口数据数量
extern float filter_buf[FILTER_N]; //采样窗口数据存放的数组

//extern float battery_capacity_test;
//extern float battery_capacity_app;
//extern float omega_l_test;
//extern float ua_l_test;
//extern uint8_t omega_ref_test_flag;

//void motor_reset(void);

//extern float		  motor_l_Kp_test;
//extern float		  motor_l_Ki_test;
//extern float		  motor_l_Kd_test;
//extern float		  motor_l_SP_test;
//extern uint64_t 	t_k_1_test;
//extern float 			err_k_1_test;
//extern float 			err_int_k_1_test;
//extern float ua_l_filter_test;

////函数：获取左右两个电机的PID结构体里的值（以赋值给全局变量的形式）
//void App_Get_motor_PID_value(void);

void Motor_Init(void);
void Motor_SetPWM(uint8_t n,int8_t PWM);
//void App_Motor_SetOmega_L(float Omega);
//void App_Motor_SetOmega_R(float Omega);
void Motor_Cmd(uint8_t On);

//void App_Motor_Proc(void);//函数：电机通过PID调速系统进行调速的进程函数
//void app_adjust_pid(float Kp,float Ki,float Kd);//调PID的值
//void MeanFliter_Clear(void);

#endif
