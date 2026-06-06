#include "stm32f10x.h"                  // Device header
#include "PWM.h"
#include "public.h"
#include "pid.h"
#include "Delay.h"
#include "Encoder.h"
#include "battery.h"
#include "motor.h"
#include "app_control.h"
#include "public.h"
#include "Motor.h"
#include "BlueSerial.h"
#include <string.h> 

/**
  * 函    数：直流电机初始化
  * 参    数：无
  * 返 回 值：无
  */
	
	
	//PB12和PB13、PB14和PB15各两个共四个引脚是用来设置两个电机的转动方向为正转还是反转的
	//PWM输出作为设置占空比
	//初始化左右电机PID调速系统
	
	//规定方向引脚接法：
	//AIN1 - PB13
	//AIN2 - PB12
	//BIN1 - PB14
	//BIN2 - PB15
	//注意，要使平衡车往同一个方向，两个轮的转动方向刚好相反
	

static PID_TypeDef pid_motor_l,pid_motor_r;
	
void Motor_Init(void)
{
	/*开启时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);		//开启GPIOB的时钟
	
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);						//将PB12和PB13、PB14和PB15初始化为推挽输出	
	GPIO_ResetBits(GPIOB, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
	
	PWM_Init();													//初始化直流电机的底层PWM
	
//	PID_Init(&pid_motor_l,0.16f,0.32f,-0.0085f);//赋电机控速系统的PID的值（左电机）
//	PID_Init(&pid_motor_r,0.16f,0.32f,-0.0085f);//赋电机控速系统的PID的值（右电机）
//	PID_LimitConfig(&pid_motor_l,12.6f,-12.6f);//对电机控速系统PID输出（Ua）进行限幅（左电机）
//	PID_LimitConfig(&pid_motor_r,12.6f,-12.6f);//对电机控速系统PID输出（Ua）进行限幅（右电机）


	
}


//函数：开关电机
void Motor_Cmd(uint8_t On)
{
	if(On == 1)//打开电机
	{
		PWM_Cmd(1);
	}
	else if(On == 0)
	{
		PWM_Cmd(0);
	}
		
}


/**
  * 函    数：输出占空比给TB6612（电机设置速度）
	* 参    数：n 决定控制的是左电机还是右电机。n=1是控制左电机，n=2是控制右电机
							Speed 要设置的速度，范围：-100~100
  * 返 回 值：无
  */
void Motor_SetPWM(uint8_t n,int8_t PWM)
{
	if(n==1)
	{
//			if (PWM >= 0)							//如果设置正转的速度值
//		{
//			GPIO_SetBits(GPIOB, GPIO_Pin_12);	//PB12置高电平
//			GPIO_ResetBits(GPIOB, GPIO_Pin_13);	//PB13置低电平，设置方向为正转
//			PWM_SetCompare1(PWM);				//左电机对应的是TIM2的通道1，PWM设置为速度值
//		}
//		else if(PWM<0)								//否则，即设置反转的速度值
//		{
//			GPIO_ResetBits(GPIOB, GPIO_Pin_12);	//PB12置低电平
//			GPIO_SetBits(GPIOB, GPIO_Pin_13);	//PB13置高电平，设置方向为反转
//			PWM_SetCompare1(-PWM);			//左电机对应的是TIM2的通道1，PWM设置为负的速度值，因为此时速度值Speed为负数，而PWM只能给正数
//		}
		
				if (PWM >= 0)							//如果设置正转的速度值
		{
			GPIO_ResetBits(GPIOB, GPIO_Pin_12);	//PB12置高电平
			GPIO_SetBits(GPIOB, GPIO_Pin_13);	//PB13置低电平，设置方向为正转
			PWM_SetCompare1(PWM);				//左电机对应的是TIM2的通道1，PWM设置为速度值
		}
		else if(PWM<0)								//否则，即设置反转的速度值
		{
			GPIO_SetBits(GPIOB, GPIO_Pin_12);	//PB12置低电平
			GPIO_ResetBits(GPIOB, GPIO_Pin_13);	//PB13置高电平，设置方向为反转
			PWM_SetCompare1(-PWM);			//左电机对应的是TIM2的通道1，PWM设置为负的速度值，因为此时速度值Speed为负数，而PWM只能给正数
		}	
		
}
	
	else if(n==2)
	{
			if (PWM >= 0)							//如果设置正转的速度值
		{
			GPIO_SetBits(GPIOB, GPIO_Pin_14);	//PB14置高电平
			GPIO_ResetBits(GPIOB, GPIO_Pin_15);	//PB15置低电平，设置方向为正转
			PWM_SetCompare2(PWM);				//右电机对应的是TIM2的通道2，PWM设置为速度值
		}
		else if(PWM<0)								//否则，即设置反转的速度值
		{
			GPIO_ResetBits(GPIOB, GPIO_Pin_14);	//PB14置低电平
			GPIO_SetBits(GPIOB,GPIO_Pin_15);	//PB15置高电平，设置方向为反转
			PWM_SetCompare2(-PWM);			//右电机对应的是TIM2的通道2，PWM设置为负的速度值，因为此时速度值Speed为负数，而PWM只能给正数
		}
	
	}	
}



float battery_capacity_test = 0;
float battery_capacity_app = 0;
float omega_l_test = 0;
uint8_t omega_ref_test_flag=0;

float ua_l_test = 0;
float duty_l_test = 0;

float ua_l_filter_test=0;

float omega_r_test = 0;
float ua_r_test = 0;
float duty_r_test = 0;

////函数：电机通过PID调速系统进行调速的进程函数（在主函数的while循环中调用）
//void App_Motor_Proc(void)
//{
//	static uint32_t nxt=0;//下次程序运行时间
//	if(GetTick() < nxt) return;
//	
//	// #1.获取左右电机当前的旋转角速度
//	float omega_l = App_Encoder_GetSpeed_L();
//	float omega_r = App_Encoder_GetSpeed_R();
//	omega_l_test = omega_l;
//	omega_r_test = omega_r;
//	
//	// #2.计算PID控制器的输出，赋给Ua（电机两端的目标电压值）
//	float ua_l = PID_Compute(&pid_motor_l,omega_l);
//	float ua_r = PID_Compute(&pid_motor_r,omega_r);
//	ua_l_test = ua_l;
//	ua_r_test = ua_r;
//	
//	// #3.引进bettery.h，获取当前电池电压VBAT
//	float vbat = Get_vbat();
//	battery_capacity_test = Get_vbat();
//	
//	// #4.通过电压Ua和当前电池电压VBAT计算要转为所要输出给TB6612两个PWM输入端口的PWM占空比
//	float duty_l = ua_l / vbat * 100.0f;
//	float duty_r = ua_r / vbat * 100.0f;
//	duty_l_test = duty_l;
//	duty_r_test = duty_r;
//	
//	// #5.将占空比输出给轮趣TB6612
//	Motor_SetPWM(1,duty_l);
//	Motor_SetPWM(2,duty_r);
//	
//	nxt += 5;	
//}

/*√test1*/

//float MeanFliter_Update(float new_val);


//void App_Motor_Proc(void)
//{
//	static uint32_t nxt=0;//下次程序运行时间
//	if(GetTick() < nxt) return;
//		
//	// #1.获取左右电机当前的旋转角速度
//	float omega_l = App_Encoder_GetSpeed_L();
//	float omega_r = App_Encoder_GetSpeed_R();
//	omega_l_test = omega_l;
//	omega_r_test = omega_r;
//	
//	// #2.计算PID控制器的输出，赋给Ua（电机两端的目标电压值）
//	float ua_l = PID_Compute(&pid_motor_l,omega_l);
//	float ua_r = PID_Compute(&pid_motor_r,omega_r);
//	ua_l_test = ua_l;
//	ua_r_test = ua_r;
////	float ua_l_filter = MeanFliter_Update(ua_l);//同一目标速度的情况下，对ua_l进行均值滤波
////	float ua_r_filter = MeanFliter_Update(ua_r);//同一目标速度的情况下，对ua_r进行均值滤波
////	ua_l_filter_test = ua_l_filter;
//	
//	// #3.引进bettery.h，获取当前电池电压VBAT
//	float vbat = Get_vbat();
//	battery_capacity_test = Get_vbat();
//	
//	// #4.通过电压Ua和当前电池电压VBAT计算要转为所要输出给TB6612两个PWM输入端口的PWM占空比
//	float duty_l = ua_l / vbat * 100.0f;
//	float duty_r = ua_r / vbat * 100.0f;
//	duty_l_test = duty_l;
//	duty_r_test = duty_r;
//	
//	// #5.将占空比输出给轮趣TB6612
//	Motor_SetPWM(1,(int)duty_l);
//	Motor_SetPWM(2,(int)duty_r);
//	
//	nxt += 5;	
//}

///*√test1*/


//float filter_buf[FILTER_N]; //采样窗口数据存放的数组
//static int index = 0;
//uint8_t buf_index = 0;
//float sum=0.0f;
////函数：对ua_l进行均值滤波的函数
//float MeanFliter_Update(float new_val)
//{
//	//对ua_l进行均值滤波：
//	sum=0.0f;
//	uint8_t i;
//	
//	filter_buf[buf_index]=new_val;
//	buf_index++;
//	if(buf_index>=FILTER_N){buf_index=0;}
//	
//	//求和
//	for(i=0;i<FILTER_N;i++)
//	{sum+=filter_buf[i];}
//	
//	return sum/FILTER_N;
//}
////函数：均值滤波的复位函数
//void MeanFliter_Clear(void)
//{
//	memset(filter_buf, 0, sizeof(filter_buf));// 清零采样缓冲区
//	buf_index=0;//采样缓冲区位置索引归零
//}



///*test2*/

//void App_Motor_Proc(void)
//{
//	static uint32_t nxt=0;//下次程序运行时间
//	if(GetTick() < nxt) return;
//	
//	// #1.获取左右电机当前的旋转角速度w
//	float omega_l = App_Encoder_GetSpeed_L();
//	float omega_r = App_Encoder_GetSpeed_R();
//	omega_l_test = omega_l;
//	omega_r_test = omega_r;
//	
//	// #2.计算电机系统的负反馈l_w_fb和r_w_fb
//	float l_w_fb = omega_l * 0.176f;
//	float r_w_fb = omega_r * 0.176f;
//	
//	// #2.求出
//	float ua_l = PID_Compute(&pid_motor_l,omega_l);
//	float ua_r = PID_Compute(&pid_motor_r,omega_r);
//	ua_l_test = ua_l;
//	ua_r_test = ua_r;
//	
//	// #3.引进bettery.h，获取当前电池电压VBAT
//	float vbat = Get_vbat();
//	battery_capacity_test = Get_vbat();
//	
//	// #4.通过电压Ua和当前电池电压VBAT计算要转为所要输出给TB6612两个PWM输入端口的PWM占空比
//	float duty_l = ua_l / 12.0 * 100.0f;
//	float duty_r = ua_r / 12.0 * 100.0f;
//	duty_l_test = duty_l;
//	duty_r_test = duty_r;
//	
//	// #5.将占空比输出给轮趣TB6612
//	Motor_SetPWM(1,(int)duty_l);
//	Motor_SetPWM(2,duty_r);
//	
//	nxt += 5;	
//}

///*test2*/



////函数：调节左电机转速Omega的设定值SP
//void App_Motor_SetOmega_L(float Omega)
//{
//	PID_ChangeSP(&pid_motor_l,Omega);
//}

////函数：调节右电机转速Omega的设定值SP
//void App_Motor_SetOmega_R(float Omega)
//{
//	PID_ChangeSP(&pid_motor_r,Omega);
//}

//void motor_reset(void);


///*电机调速系统复位函数*/
//void motor_reset(void)
//{
//	//对PID控制器进行复位
//	PID_Reset(&pid_motor_l);
//	PID_Reset(&pid_motor_r);
//}


//float		  motor_l_Kp_test =0.0f;
//float		  motor_l_Ki_test=0.0f;
//float		  motor_l_Kd_test=0.0f;
//float		  motor_l_SP_test=0.0f;
//uint64_t 	t_k_1_test=0;
//float 			err_k_1_test=0.0f;
//float 			err_int_k_1_test=0.0f;


////函数：获取左右两个电机的PID结构体里的值（以赋值给全局变量的形式）
//void Get_motor_PID_value(PID_TypeDef *PID)
//{
//	//获取
//	motor_l_Kp_test  =   PID->Kp; 
//	motor_l_Ki_test  =   PID->Ki; 
//	motor_l_Kd_test  =   PID->Kd; 
//	motor_l_SP_test  =   PID->SP; 
//	//展示
//	Serial_Printf("motor:kp=%f,ki=%f,kd=%f,SP:%f\r\n",motor_l_Kp_test,motor_l_Ki_test,motor_l_Kd_test,motor_l_SP_test);
//}

//void App_Get_motor_PID_value(void)
//{
//	Get_motor_PID_value(&pid_motor_l);
//}


////函数：改变左右两个电机的PID的数值
//void adjust_pid(PID_TypeDef *PID,float Kp,float Ki,float Kd)
//{
//	PID_Init(&pid_motor_l,Kp,Ki,Kd);//赋电机控速系统的PID的值（左电机）
//	PID_Init(&pid_motor_r,Kp,Ki,Kd);//赋电机控速系统的PID的值（右电机）
//}

//void app_adjust_pid(float Kp,float Ki,float Kd)
//{
//	adjust_pid(&pid_motor_l,Kp,Ki,Kd);
//	adjust_pid(&pid_motor_r,Kp,Ki,Kd);
//}
