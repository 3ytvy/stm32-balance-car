#ifndef __PUBLIC_H
#define __PUBLIC_H

#include <stdint.h> 

// ========================
// 全局通用结构体（整个项目都能用）
// ========================
typedef struct {
    float ax;
    float ay;
    float az;
		float gx;
    float gy;
    float gz;
		float temperature;
}axis_info_t;


typedef struct{
	float Kp; //比例系数
	float Ki; //积分项系数
	float Kd; //微分项系数
	
	float Speed_Target; //速度环专属目标值（蓝牙在控制的速度环的目标值），正常运行时，蓝牙控制的它直接赋给Target，缓冲时候，蓝牙控制的它要进行缓冲以后再赋给Target
	float Target; //目标值，用户自设定
	float Actual; //实际值，从传感器读取
	float Actual1; //上一次的实际值
	float Out;		//输出值，作用于执行器
	
	float err_k;			//err ， 本次误差
	float err_k_1; //err[k-1]，上次误差
	float err_int; //误差积分
	
	float UpperLimit;//Ua上限
	float LowerLimit;//Ua下限

	float outoffset;//输出偏移
}PID_TypeDef;



#endif
