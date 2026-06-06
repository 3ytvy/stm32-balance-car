#ifndef __APP_CONTROL_H
#define __APP_CONTROL_H

#include "stm32f10x.h"                  // Device header
#include "public.h"

extern float roll_test;
extern float yaw_test;
extern float pitch_test;
extern float omega_ref_test_running;

extern float theta_dot_test;
extern float theta_dot_ref_test;

extern float theta_SP_test;



extern float if_speed_for_angle_flag;

void app_control_init(void);

void app_angle_proc(float Angle);
void app_speed_proc(void);
void app_turn_proc(void);

void Get_Speed(void);
void app_angle_reset(void);

/*角度环PID蓝牙调控*/
void app_adjust_Angle_Kp_pid(float Kp);
void app_adjust_Angle_Ki_pid(float Ki);
void app_adjust_Angle_Kd_pid(float Kd);
void app_adjust_angle_target(float Target);
float app_get_angle_p(void);
float app_get_angle_i(void);
float app_get_angle_d(void);
float app_get_angle_target(void);
float app_get_angle_err_k(void);
float app_get_angle_err_k_1(void);
float app_get_angle_err_int(void);
float app_get_angle_out(void);
float app_get_angle_actual(void);

/*速度环PID蓝牙调控*/
void app_adjust_speed_Kp_pid(float Kp);
void app_adjust_speed_Ki_pid(float Ki);
void app_adjust_speed_Kd_pid(float Kd);
void app_adjust_speed_target(float Target);
void app_adjust_speed_speed_target(float Target);
float app_get_speed_p(void);
float app_get_speed_i(void);
float app_get_speed_d(void);
float app_get_speed_target(void);
float app_get_speed_err_k(void);
float app_get_speed_err_k_1(void);
float app_get_speed_err_int(void);
float app_get_speed_out(void);
float app_get_speed_actual(void);

/*转向环PID蓝牙调控*/
void app_adjust_turn_Kp_pid(float Kp);
void app_adjust_turn_Ki_pid(float Ki);
void app_adjust_turn_Kd_pid(float Kd);
void app_adjust_turn_target(float Target);
float app_get_turn_p(void);
float app_get_turn_i(void);
float app_get_turn_d(void);
float app_get_turn_target(void);
float app_get_turn_out(void);
float app_get_turn_actual(void);
float app_get_turn_err_k(void);
float app_get_turn_err_k_1(void);
float app_get_turn_err_int(void);


		
void app_adjust_outoffset(float outoffset);
float app_get_outoffset(void);
void app_adjust_difpwm(float val);




#endif
