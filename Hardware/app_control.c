#include "app_control.h"
#include "pid.h"
#include "Delay.h"
#include "MPU6050.h"
#include "qmath.h"
#include "Motor.h"
#include "Encoder.h"
#include "BlueSerial.h"





/*定义角度环和速度环PID**********************************************/
static PID_TypeDef pid_angle;	//角度环的PID
static PID_TypeDef pid_speed;	//速度环的PID
static PID_TypeDef pid_turn;	//转向环的PID
static PID_TypeDef pid_constant;	//稳速环的PID
/**********************************************定义角度环和速度环PID*/



/*函数：初始化平衡车的控制系统---------------------------------------------------------------------*/

void app_control_init(void)
{
	//初始化PID
	PID_Init(&pid_angle);
	PID_Init(&pid_speed);
	PID_Init(&pid_turn);
		
	//给角度环PID赋初值
	pid_angle.Kp=7.00f;
	pid_angle.Ki=0.08f;
	pid_angle.Kd=27.50f;
	pid_angle.outoffset = 5.5f;//输出偏移量，默认为4
	//给速度环PID赋初值
//	pid_speed.Kp=0.8f;
//	pid_speed.Ki=0.05f;
//	pid_speed.Kd=-1.50f;
	pid_speed.Kp=1.5f;
	pid_speed.Ki=0.001f;
	pid_speed.Kd=-1.50f;
	pid_speed.Target=0.0f;//默认速度为0
	pid_speed.Speed_Target=0.0f;//默认速度为0
	//给转向环PID赋初值
	pid_turn.Kp=70.00f;
	pid_turn.Ki=0.00f;
	pid_turn.Kd=-50.00f;
	pid_turn.Target=0.00f;
	
	//给速度环输出限幅	速度环的输出为目标角度	最多20°
	pid_speed.UpperLimit=20;
	pid_speed.LowerLimit=-20;
	
	//给转向环输出限幅	速度环的输出为差分PWM	最多±50
	pid_turn.UpperLimit=50;
	pid_turn.LowerLimit=-50;
}

/*---------------------------------------------------------------------函数：初始化平衡车的控制系统*/






/*函数：角度环控制系统的进程函数（在主函数的while循环中调用）--------------------------------------*/

/*角度环
		输入：目标角度Angle（执行完速度环获取） || 实际角度Angle（从MPU6050传感器获取）
		输出：用于平均速度的电机电压占空比 AvePWM
*/

static uint64_t last_time = 0;
float LeftPWM=0,RightPWM=0;//左右轮的PWM
float DifPWM=0;//差分PWM
float AvePWM=0;//平均PWM

void app_angle_proc(float Angle)

{
	// #1.将Angle输入角度环的实际值
	pid_angle.Actual=Angle;
	
	// #2.角度环PID运算得到控制量（两个轮子用于直走的PWM占空比）
	PID_Update(&pid_angle);
	AvePWM = pid_angle.Out;
	
	// #3.把控制量加载到电机
	LeftPWM = (AvePWM + DifPWM)/2.0f;
	RightPWM = (AvePWM - DifPWM)/2.0f;
	//限幅
	if(LeftPWM > 100){LeftPWM = 100;}else if(LeftPWM < -100){LeftPWM = -100;}
	if(RightPWM > 100){RightPWM = 100;}else if(RightPWM < -100){RightPWM = -100;}	
	
	// #4.将用于直走的PWM占空比直接加在电机上
	Motor_SetPWM(1,LeftPWM);
	Motor_SetPWM(2,RightPWM);

	
	/*test*/
//	//蓝牙的OLED屏打印输出PID值
//	Serial_Printf("[display,0,0,Angle:%2.2f]",Angle);
//	Serial_Printf("[display,0,20,Kp:%2.2f Ki:%2.2f Kd%2.2f]",pid_angle.Kp,pid_angle.Ki,pid_angle.Kd);
//	Serial_Printf("[display,0,40,LeftPWM:%3.3f]",LeftPWM);
//	Serial_Printf("[display,0,60,RightPWM:%3.3f]",RightPWM);
//	Serial_Printf("[display,0,80,AvePWM:%3.3f]",AvePWM);
//	Serial_Printf("[display,0,140,Target:%3.3f]",pid_angle.Target);
	
	
}
/*--------------------------------------函数：角度环控制系统的进程函数（在主函数的while循环中调用）*/





/*函数：获取平均速度和差分速度的进程函数--------------------------------------*/
/*函数用法：在使用速度环和转向环的同时，这个函数也在进程中，速度环和转向环就能通过两个变量AveSpeed和DifSpeed来得到速度*/

float AveSpeed=0;//平均速度（控制直走）
float DifSpeed=0;//差分速度（控制转向）



void Get_Speed(void)
{
	//获取传感器的值
	float speed_left = Encoder_Get(1) / 26.0f / 20.0f / 0.025f;	//0.025f是进入这个函数时间间隔
	//26.0f是线数，encoder_l / 26.0f得到的是磁环转过的圈数
	//20.0f是减速比，encoder_l / 26.0f / 20.0f得到的是转轴（车轮）转过的圈数
	//0.025f是进入这个函数时间间隔
	//单位：转/秒
	float speed_right = Encoder_Get(2) / 26.0f / 20.0f / 0.025f;	//0.025f是进入这个函数时间间隔		

	//计算后赋值给速度值变量
	AveSpeed=(speed_left+speed_right)/2.0f;
//	DifSpeed=(speed_left - speed_right)/25.0f;
	DifSpeed=(speed_right - speed_left)/25.0f;

//	//目标差分速度是：pid_turn.target
//	Serial_Printf("[plot,%f,%f]\r\n",speed_left,speed_right);
//	
//	//Serial_Printf("[plot,%2.2f,%2.2f]\r\n",pid_turn.Target,l_r_temp);
}





/*--------------------------------------函数：获取平均速度和差分速度的进程函数*/



//float if_speed_for_angle_flag = 1;//判断是否要把速度环PID的输出给角度环当输入	extern

/*函数：速度环控制系统的进程函数（在主函数的while循环中调用）--------------------------------------*/

/*速度环
		输入：目标直行速度AveSpeed（实际是电机转速）（在蓝牙串口中摇杆获取） || 实际直行速度（从编码器获取并通过计算得到）
		输出：目标角度Angle
*/
int target_temp=0;
//定义取绝对值的宏
#define ABS(x)  ((x) >= 0 ? (x) : -(x))
void app_speed_proc(void)
{	
	if(pid_speed.Speed_Target == 0 )
	{
		target_temp = pid_speed.Target * 0.95;
		pid_speed.Target = target_temp;
		if(ABS(target_temp) < 1.0)
		{target_temp=0;}
	}
	
	//将传感器得到的值作为pid的实际值actual
	pid_speed.Actual=AveSpeed;

		
	//计算速度环的pid（速度环的目标值在蓝牙串口中摇杆获取）
	PID_Update(&pid_speed);

	
	//速度环pid的输出给角度环pid当输入
	pid_angle.Target=pid_speed.Out;

}

/*--------------------------------------函数：速度环控制系统的进程函数（在主函数的while循环中调用）*/


/*函数：转向环控制系统的进程函数（在主函数的while循环中调用）--------------------------------------*/

void app_turn_proc(void)
{
	if(turn_flag == 1)
	{
		//将传感器得到的值（DifSpeed = speed_left - speed_right）作为pid的实际值actual
		pid_turn.Actual=DifSpeed;
		
		//计算转向环的pid（转向环的目标值在蓝牙串口中摇杆获取）
		PID_Update(&pid_turn);
	
		//输出作为DifPWM
		DifPWM = pid_turn.Out;
		
		if(pid_speed.Target == 0 && pid_speed.Kp != 1.5)//静止态：蓝灯	速度要归零
		{
			pid_speed.Kp=1.5f;	pid_speed.Kd=-1.50f;
			GPIO_WriteBit(GPIOA,GPIO_Pin_5,Bit_SET);
			GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_RESET);
		}
		else if(pid_speed.Target != 0 && pid_speed.Kp != 1.0)//运动态：红灯	速度要归零
		{
			pid_speed.Kp=1.0f;	pid_speed.Kd=-1.30f;
			GPIO_WriteBit(GPIOA,GPIO_Pin_5,Bit_SET);
			GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_SET);
		}
		

	}
	
}

/*--------------------------------------函数：转向环控制系统的进程函数（在主函数的while循环中调用）*/


///*函数：新 ● 转向环控制系统的进程函数（仅针对直线行走）（在主函数的while循环中调用）--------------------------------------*/

////定义取绝对值的宏
//#define ABS(x)  ((x) >= 0 ? (x) : -(x))
//void app_turn_proc(void)
//{
//	//得到传感器的值（DifSpeed = speed_right - speed_left）来做判断
//	if(ABS(DifSpeed) > 0.1 && pid_turn.Target == 0)
//	{
//		DifPWM = DifSpeed * pid_turn.Kp;
//	}
//	
//	
//	
//}

///*--------------------------------------函数：新 ● 转向环控制系统的进程函数（仅针对直线行走）（在主函数的while循环中调用）*/



/*函数：平衡车系统各PID复位函数（每次重开都要清除复位）--------------------------------------*/

void app_angle_reset(void)
{
	pid_angle.err_int=0;
	pid_angle.err_k=0;
	pid_angle.err_k_1=0;
	pid_angle.Out=0;
	pid_angle.Target=0;
	pid_angle.Actual=0;
	pid_angle.Actual1=0;
	Motor_SetPWM(1,0);
	Motor_SetPWM(2,0);
	
	AvePWM=0;
	DifPWM=0;
	LeftPWM=0;
	RightPWM=0;
	
	pid_speed.err_int=0;
	pid_speed.err_k=0;
	pid_speed.err_k_1=0;
	pid_speed.Out=0;
	pid_speed.Target=0;
	pid_speed.Actual=0;
	pid_speed.Actual1=0;
	
	pid_turn.err_int=0;
	pid_turn.err_k=0;
	pid_turn.err_k_1=0;
	pid_turn.Out=0;
	pid_turn.Target=0;
	pid_turn.Actual=0;	
	pid_turn.Actual1=0;		
	
}


/*--------------------------------------函数：平衡车系统各PID复位函数（每次重开都要清除复位）*/


/*函数：蓝牙串口滑杆调参相关-------------------------------------------------------------------------------------*/

/*角度环*/

		//改角度环Kp
		void app_adjust_Angle_Kp_pid(float Kp)
		{
			pid_angle.Kp = Kp;
		}
		
		//改角度环Ki
		void app_adjust_Angle_Ki_pid(float Ki)
		{
			pid_angle.Ki = Ki;
		}
		
		//改角度环Kd
		void app_adjust_Angle_Kd_pid(float Kd)
		{
			pid_angle.Kd = Kd;
		}
		
		/*函数：给外部调用的修改角度环设定值*/
		void app_adjust_angle_target(float Target)
		{
			pid_angle.Target = Target;
		}
			
		/*函数：给外部调用的获取角度环的pid数值*/
		float app_get_angle_p(void)
		{
			return pid_angle.Kp;
		}
		
		float app_get_angle_i(void)
		{
			return pid_angle.Ki;
		}
		
		float app_get_angle_d(void)
		{
			return pid_angle.Kd;
		}

		
		
		/*函数：给外部调用的获取角度环的pid结构体的暂存值*/
		
		float app_get_angle_target(void)
		{
			return pid_angle.Target;
		}
		
		float app_get_angle_out(void)
		{
			return pid_angle.Out;
		}
		
		float app_get_angle_actual(void)
		{
			return pid_angle.Actual;
		}
		
		float app_get_angle_err_k(void)
		{
			return pid_angle.err_k;
		}
		
		float app_get_angle_err_k_1(void)
		{
			return pid_angle.err_k_1;
		}
		
		float app_get_angle_err_int(void)
		{
			return pid_angle.err_int;
		}
		
/*角度环*/
		
		
/*速度环*/
		//改速度环Kp
		void app_adjust_speed_Kp_pid(float Kp)
		{
			pid_speed.Kp = Kp;
		}
		
		//改速度环Ki
		void app_adjust_speed_Ki_pid(float Ki)
		{
			pid_speed.Ki = Ki;
		}
		
		//改速度环Kd
		void app_adjust_speed_Kd_pid(float Kd)
		{
			pid_speed.Kd = Kd;
		}
		
		/*函数：给外部调用的修改速度环设定值*/
		void app_adjust_speed_target(float Target)
		{
			pid_speed.Target = Target;
		}
		
		void app_adjust_speed_speed_target(float Target)
		{
			pid_speed.Speed_Target = Target;
		}
			
		/*函数：给外部调用的获取速度环的pid数值*/
		float app_get_speed_p(void)
		{
			return pid_speed.Kp;
		}
		
		float app_get_speed_i(void)
		{
			return pid_speed.Ki;
		}
		
		float app_get_speed_d(void)
		{
			return pid_speed.Kd;
		}
		
	/*函数：给外部调用的获取速度环的pid结构体的暂存值*/
		
		float app_get_speed_target(void)
		{
			return pid_speed.Target;
		}
		
		float app_get_speed_out(void)
		{
			return pid_speed.Out;
		}
		
		float app_get_speed_actual(void)
		{
			return pid_speed.Actual;
		}
		
		float app_get_speed_err_k(void)
		{
			return pid_speed.err_k;
		}
		
		float app_get_speed_err_k_1(void)
		{
			return pid_speed.err_k_1;
		}
		
		float app_get_speed_err_int(void)
		{
			return pid_speed.err_int;
		}
		
		
		
		
/*速度环*/
		

/*转向环*/
		//改转向环Kp
		void app_adjust_turn_Kp_pid(float Kp)
		{
			pid_turn.Kp = Kp;
		}
		
		//改转向环Ki
		void app_adjust_turn_Ki_pid(float Ki)
		{
			pid_turn.Ki = Ki;
		}
		
		//改转向环Kd
		void app_adjust_turn_Kd_pid(float Kd)
		{
			pid_turn.Kd = Kd;
		}
		
		/*函数：给外部调用的修改转向环设定值*/
		void app_adjust_turn_target(float Target)
		{
			pid_turn.Target = Target;
		}
			
		/*函数：给外部调用的获取转向环的pid数值*/
		float app_get_turn_p(void)
		{
			return pid_turn.Kp;
		}
		
		float app_get_turn_i(void)
		{
			return pid_turn.Ki;
		}
		
		float app_get_turn_d(void)
		{
			return pid_turn.Kd;
		}
		
	/*函数：给外部调用的获取转向环的pid结构体的暂存值*/
		
		float app_get_turn_target(void)
		{
			return pid_turn.Target;
		}
		
		float app_get_turn_out(void)
		{
			return pid_turn.Out;
		}
		
		float app_get_turn_actual(void)
		{
			return pid_turn.Actual;
		}
		
		float app_get_turn_err_k(void)
		{
			return pid_turn.err_k;
		}
		
		float app_get_turn_err_k_1(void)
		{
			return pid_turn.err_k_1;
		}
		
		float app_get_turn_err_int(void)
		{
			return pid_turn.err_int;
		}
		
		

/*转向环*/
		
//函数：给外部调用的修改角度环输出偏移的值
void app_adjust_outoffset(float outoffset)
{
	pid_angle.outoffset = outoffset;
}
//函数：给外部调用的读取角度环输出偏移的值
float app_get_outoffset(void)
{
	return pid_angle.outoffset;
}		

//函数：给外部调用的直接修改DifPWM的值
void app_adjust_difpwm(float val)
{
	DifPWM = val;
}		


/*-------------------------------------------------------------------------------------函数：蓝牙串口滑杆调参相关*/