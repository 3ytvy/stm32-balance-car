#ifndef __MPU6050_H
#define __MPU6050_H
#include "public.h"



static uint8_t firstCompute;

void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data);
uint8_t MPU6050_ReadReg(uint8_t RegAddress);

void MPU6050_Init(void);
uint8_t MPU6050_GetID(void);
void MPU6050_GetRaw(void);
void MPU6050_GetData(void);

float App_MPU6050_Get_ax(void);
float App_MPU6050_Get_ay(void);
float App_MPU6050_Get_az(void);
float App_MPU6050_Get_temperature(void);
float App_MPU6050_Get_gx(void);
float App_MPU6050_Get_gy(void);
float App_MPU6050_Get_gz(void);

float App_MPU6050_Get_Yaw(void);
float App_MPU6050_Get_Pitch(void);
float App_MPU6050_Get_Roll(void);

float MPU6050_Proc(void);
void app_adjust_Alpha(float Alpha_adjust);

float app_get_Alpha(void);
float app_get_gy_raw_bias(void);

extern uint8_t out_flag;

void get_gy_raw_bias(void);

float app_adjust_mechanical_median(float value);


#endif

