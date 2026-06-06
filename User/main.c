#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "LED.h"
#include "Timer.h"
//#include "Key.h"
#include "Motor.h"
#include "Encoder.h"
//#include "Serial.h"
#include "BlueSerial.h"
#include "AD.h"
#include "battery.h"
#include "MPU6050.h"
#include "public.h"
#include "PWM.h"
#include "Delay.h"
#include <string.h> 
#include "app_control.h"

///*LED测试*/
//int main(void)
//{
//	/*模块初始化*/
//	OLED_Init();		//OLED初始化
//	LED_Init();
//	
//	/*OLED显示*/

//	
//	while (1)
//	{
//		Delay_ms(500);
//		LED_Turn();
//		
//		
//		
//	}
//}


///*定时器测试*/
//uint16_t Count;
//int main(void)
//{
//	OLED_Init();
//	Timer_Init();
//	
//	while(1)
//	{
//		OLED_Printf(0,0,OLED_8X16,"Count:%05d",Count);
//		OLED_Update();
//		
//	}
//}

//void TIM1_UP_IRQHandler(void)
//{
//	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
//	{
//		Count++;
//		
//		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
//	}
//}




///*直流电机加测速测试*/
//uint8_t KeyNum;
//int8_t PWML=0,PWMR=0;
//float encoder_l_pos=0; 
//float encoder_r_pos=0; 

//int main(void)
//{
//	OLED_Init();
//	Key_Init();
//	Motor_Init();
//	App_Encoder_Init();//初始化编码器（包括初始化引脚和配置开启中断）
//	Timer_Init();
//	SysTick_US_Init();
//	Serial_Init();
//	
//	while(1)
//	{
////		KeyNum=Key_GetNum();
////		if(KeyNum == 1)
////		{
////			PWML += 10;
////			if (PWML > 100) {PWML = 100;}
////		}
////		if(KeyNum == 2)
////		{
////			PWML -= 10;
////			if (PWML < -100){PWML = -100;}
////		}
////		if(KeyNum == 3)
////		{
////			PWMR += 10;
////			if (PWMR > 100) {PWMR = 100;}
////		}
////		if(KeyNum == 4)
////		{
////			PWMR -= 10;
////			if (PWMR < -100) {PWMR = -100;}
////		}
////		
////		Motor_SetPWM(1,PWML);//设置左电机的PWM占空比进行调速
////		Motor_SetPWM(2,PWMR);//设置右电机的PWM占空比进行调速
//		OLED_Clear();
//		
//		encoder_l_pos = App_Encoder_GetPos_L();
//		encoder_r_pos = App_Encoder_GetPos_R();
//		
////		OLED_Printf(0,0,OLED_8X16,"PWML:%+04d",PWML);
////		OLED_Printf(0,16,OLED_8X16,"PWMR:%+04d",PWMR);
////		OLED_Printf(0,32,OLED_8X16,"SpdL:%5.1f",encoder_l_pos);
////		OLED_Printf(0,48,OLED_8X16,"SpdR:%5.1f",encoder_r_pos);

//		Serial_Printf("%f,%f\n",encoder_l_pos,encoder_r_pos);
//		
//		OLED_Update();
//		
//	}
//}

//void TIM1_UP_IRQHandler(void)
//{
//	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
//	{
//		Key_Tick();
//		
////		Serial_Printf("%f,%f\n",encoder_l_pos,encoder_r_pos);
//		
//		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
//	}
//}







/*ADC获取电源电压测试*/
////①显示电量
//int main(void)
//{
//	AD_Init();	//初始化AD。只要完成了AD的初始化，每次要获取AD值，就直接执行函数AD_GetValue()获取即可
//	OLED_Init();
//	int Battery_Capacity=0;
//	while(1)
//	{
//		Battery_Capacity = Show_Battery();
//		OLED_Printf(0,0,OLED_8X16,"Battery:%+04d",Battery_Capacity);

//		OLED_Update();
//	}
//}
//
////②显示电压
//int main(void)
//{
//	AD_Init();	//初始化AD。只要完成了AD的初始化，每次要获取AD值，就直接执行函数AD_GetValue()获取即可
//	OLED_Init();
//	float Battery_Capacity=0;
//	while(1)
//	{
//		Battery_Capacity = Get_vbat();
//		OLED_Printf(0,0,OLED_8X16,"Battery:%+2.2fV",Battery_Capacity);

//		OLED_Update();
//	}
//}




///*MPU6050获取欧拉姿态角测试*/
//axis_info_t mpu6050;
//int main(void)
//{
//	Serial_Init();
//	MPU6050_Init();
//	
//	
//	while(1)
//	{
//		MPU6050_GetData(&mpu6050.ax,&mpu6050.ay,&mpu6050.az,&mpu6050.gx,&mpu6050.gy,&mpu6050.gz,&mpu6050.temperature);	
//		Serial_Printf("%f,%f,%f,%f,%f,%f,%d\n",mpu6050.ax,mpu6050.ay,mpu6050.az,mpu6050.gx,mpu6050.gy,mpu6050.gz,mpu6050.temperature);
//		Delay_ms(100);
//	}
//}




///*MPU6050欧拉角测试*/

//static void Serial_Proc(void);

//int main(void)
//{
//	Delay(200);//等待MPU6050上电
//	MPU6050_Init();
//	Serial_Init();
//	
//	Serial_Printf("Nihao!");
//	
//	while(1)
//	{
//		MPU6050_Proc();
//		Serial_Proc();
//	}
//}

////函数：每隔10ms把欧拉角的计算结果发送给电脑
//static void Serial_Proc(void)
//{
//	static uint32_t nxt=0;//下次程序运行时间
//	if(GetTick() < nxt) return;
//	
////	float ax = App_MPU6050_Get_ax();
////	float ay = App_MPU6050_Get_ay();
////	float az = App_MPU6050_Get_az();
////	float gx = App_MPU6050_Get_gx();
//	float gy_deg = App_MPU6050_Get_gy();
//	float gy_rad = App_MPU6050_Get_gy() * 0.0174533f;
////	float gz = App_MPU6050_Get_gz();
////	float temperature = App_MPU6050_Get_temperature();
//	
////	float yaw = App_MPU6050_Get_Yaw();
////	float pitch = App_MPU6050_Get_Pitch();
//	float roll = App_MPU6050_Get_Roll();
////	
////	Serial_Printf("%f,%f,%f\r\n",gy_deg,gy_rad,roll);
//	
////	Serial_Printf("%f,%f,%f\r\n",pitch,gy_deg,gy_rad);
//	Serial_Printf("[plot,%f,%f,%f]\r\n",roll,gy_deg,gy_rad);
//	
//	nxt += 10;
//}





///*平衡车通过三级闭环系统达到车身平衡状态的测试*/

//int main(void)
//{
//	MPU6050_Init();//开启MPU6050
//	
//	
//	while(1)
//	{
//		MPU6050_Proc();//MPU6050运作，后面用MPU6050读取姿势角，作为角度环的反馈
//		
//	}


//}





///*通过蓝牙串口接收自锁指令控制PWM开关（STBY引脚）功能测试*/

//void On_Off_Flag_Proc(void)//对蓝牙串口接收到指令并对标志位做出写入后，对开关标志位判断并执行对应程序的进程函数
//{
////	/*test-----------------------------------------*/
////	if(Key1 == 1)//手机中的Key1被按住（自锁住）
////	{
////		OLED_Clear();
////		OLED_Printf(1,1,OLED_8X16,"HaiFeng");
////	}
////	else if(Key1 == 0)
////	{
////		OLED_Clear();
////		OLED_Printf(1,1,OLED_8X16,"None");
////	}
////	/*-----------------------------------------test*/
//	
//	
//	if(Key1 == 1)//手机中的Key1被按住（自锁住）
//	{
//		PWM_Cmd(1);
//	}
//	else if(Key1 == 0)
//	{
//		PWM_Cmd(0);
//	}
//}

//int main(void)
//{
//	Serial_Init();
//	PWM_Init();
//	OLED_Init();

//	while(1)
//	{
//		BlueSeiral_Proc();	//运行解析蓝牙收到的指令的进程函数（在这个函数里，已进行了判断并且执行对各个标志位的写入）
//		On_Off_Flag_Proc();
//		OLED_Update();
//		
//	}
//	
//}



///*最终主函数程序*/

////备注：读电池电量以后输出那一步忘记了什么

///*本程序设计思路是通过把整个过程“系统化”
//整套系统分成 单片机内部 和 物理世界 
//其中物理世界部分是自然规律
//单片机部分则对应由系统内部设定的运作思路得到程序
//由“系统”（大系统下面包括小系统）得到系统框图，根据系统框图可以方便理解程序*/

//void On_Off_Flag_Proc(void);

//int main(void)
//{
//	
///*初始化部分---------------------------------------------------------------------------------------------------*/

////初始化蓝牙串口
//	
//	Serial_Init();	
//	Serial_Printf("Blue_Serial initiated successfully!\r\n");
//	
///*（每个初始化成功都给手机发一个成功接收的指令并且发现在对应的值进行确认）*/
//	
////初始化OLED
//	OLED_Init();
//	Serial_Printf("OLED initiated successfully!\r\n");
//	
////初始化MPU6050
//	MPU6050_Init();
//								//这一步内容包括了：
//								//		1.初始化了底层的I2C（包括I2C要用到引脚SCL - PB10 || SDA - PB11）
//								//		2.初始化（配置）了MPU6050内部的寄存器配置参数（通过I2C通信）
//	Serial_Printf("MyI2C initialized successfully!\r\n");
//	Serial_Printf("MPU6050 initiated successfully!\r\n");
//	
////初始化左右电机
//	Motor_Init();
//								//这一步内容包括了：
//								//		1.初始化了左右电机PID调速系统引脚AIN1、AIN2、BIN1、BIN2(对应PA12、PA13、PB14、PB15）
//								//		2.初始化了直流电机的底层PWM
//														/*这一步内容包括了：
//																2.1初始化了TIM计时器的通道输出PWM信号的引脚PA0和PA1（对应TB6612的PWMA、PWMB两个端口）
//																2.2初始化了用于连接TB6612的休眠引脚STBY的PB1，并且设其初值为0，即默认是休眠*/
//	Serial_Printf("PWM initiated successfully!\r\n");	
//	Serial_Printf("Motor initiated successfully!\r\n");

//	
////初始化编码器（包括初始化引脚和配置开启中断）
//	App_Encoder_Init();
//								//这一步内容包括了：
//								//		1.初始化了左右两个编码器要用到的接收TB6612传输过来的A相和B相的引脚
//								//		2.开启了针对左右两个编码器A相的中断
//														/*这一步内容包括了：
//																2.1初始化了TIM计时器的通道输出PWM信号的引脚PA0和PA1（对应TB6612的PWMA、PWMB两个端口）
//																2.2初始化了用于连接TB6612的休眠引脚STBY的PB1，并且设其初值为0，即默认是休眠*/
//	Serial_Printf("Encoder initiated successfully!\r\n");
//	
//	
////初始化AD（用于读取电池电量）
//	AD_Init();
//								//这一步内容包括了：
//								//		1.初始化了AD输出引脚PA6
//								//		2.对单片机的要用的AD通道进行初始化配置
//	Serial_Printf("AD initiated successfully!\r\n");

////初始化初始化平衡车的控制系统（两个系统：速度环系统 和 角度、角速度、电机控速系统 组成的串级系统）
//	app_control_init();
//								//这一步内容包括了：
//								//		1.初始化各个环的PID的值
//								//		2.初始化各个环的PID的输出限幅

///*---------------------------------------------------------------------------------------------------初始化部分*/
//	

//	while(1)
//	{

//		BlueSeiral_Proc();	//进程函数：运行解析蓝牙收到的指令
//		
//		On_Off_Flag_Proc();	//对电机开还是关进行判断并且在打开电机后执行对应后续程序的进程函数
//		
//		

//	
//		
//		OLED_Update();
//		
//	}
//	
//}

//static uint8_t On_Off_Flag = 0;//电机是否开启的标志位
//static uint8_t count = 0;
//void On_Off_Flag_Proc(void)//对蓝牙串口接收到指令并对标志位做出写入后，对开关标志位判断并执行对应程序的进程函数
//{
////	/*test-----------------------------------------*/
////	if(Key1 == 1)//手机中的Key1被按住（自锁住）
////	{
////		OLED_Clear();
////		OLED_Printf(1,1,OLED_8X16,"HaiFeng");
////	}
////	else if(Key1 == 0)//手机中的Key1被松开（解除自锁）
////	{
////		OLED_Clear();
////		OLED_Printf(1,1,OLED_8X16,"None");
////	}
////	/*-----------------------------------------test*/
//	
////对于自锁按键的理解：
////自锁按键分三个情况：
//	//1.按键未自锁，初始状态下，按键被按下：这就是对应电机没开启，第一次打开电机
//	//2.按键被自锁：这就是对应电机已被开启
//	//3.按键被解除自锁：这就是对应电机被关闭
////因此除了检测按键的状态（按住（即自锁）、松开（即未自锁））以外，应该加一个状态位，来区分前两种情况，这里加的是On_Off_Flag这个标志位

//	
//	if(Key1 == 1 && On_Off_Flag == 0)//手机中的Key1被按住（自锁住），并且On_Off_Flag=0，说明现在是第一次按下，电机还没开，现在要开启电机
//	{
//		Motor_Cmd(1);//打开电机（内容是：通过解除STBY休眠引脚去打开PWM，并且初始化电机PID的结构体）
//		On_Off_Flag = 1;
//	}
//	if(Key1 == 1 && On_Off_Flag == 1)//手机中的Key1被按住（自锁住），并且On_Off_Flag=1，说明电机已启动，这里去执行电机开启后的程序（加了个On_Off_Flag状态位，避免重复开启电机）
//	{
//		/**************在这里执行电机开启后的程序（进程函数）***************/
//		count++;
//		if(count > 9)
//		{
//			count = 0;
//		}
//		app_angle_proc((float)count * 10.0f);//控制系统的进程函数（每5ms执行一次）
//		App_Motor_Proc();//（每5ms执行一次）这个程序电机通过PID调速系统进行调速
//		OLED_Clear();
//		OLED_Printf(0,0,OLED_8X16,"Battery:%+2.2fV",battery_capacity_test);
//		OLED_Printf(0,16,OLED_8X16,"Runing!");
//	}
//	else if(Key1 == 0)//手机中的Key1被松开（解除自锁）
//	{
//		Motor_Cmd(0);//关闭电机（内容是：通过解除STBY休眠引脚去关闭PWM）
//		On_Off_Flag = 0;
//	}
//}



/*************************************************试验1**************************************************************/

///*最终主函数程序*/

////备注：读电池电量以后输出那一步忘记了什么

///*本程序设计思路是通过把整个过程“系统化”
//整套系统分成 单片机内部 和 物理世界 
//其中物理世界部分是自然规律
//单片机部分则对应由系统内部设定的运作思路得到程序
//由“系统”（大系统下面包括小系统）得到系统框图，根据系统框图可以方便理解程序*/

//void On_Off_Flag_Proc(void);

//static float omega_ref_test_last = 0;

//int main(void)
//{
//	
///*初始化部分---------------------------------------------------------------------------------------------------*/

////初始化蓝牙串口
//	
//	Serial_Init();	
//	Serial_Printf("Blue_Serial initiated successfully!\r\n");
//	
///*（每个初始化成功都给手机发一个成功接收的指令并且发现在对应的值进行确认）*/
//	
//	Timer_Init();
//	
//	
////初始化OLED
//	OLED_Init();
//	Serial_Printf("OLED initiated successfully!\r\n");
//	
////初始化MPU6050
//	MPU6050_Init();
//								//这一步内容包括了：
//								//		1.初始化了底层的I2C（包括I2C要用到引脚SCL - PB10 || SDA - PB11）
//								//		2.初始化（配置）了MPU6050内部的寄存器配置参数（通过I2C通信）
//	Serial_Printf("MyI2C initialized successfully!\r\n");
//	Serial_Printf("MPU6050 initiated successfully!\r\n");
//	
////初始化左右电机
//	Motor_Init();
//								//这一步内容包括了：
//								//		1.初始化了左右电机PID调速系统引脚AIN1、AIN2、BIN1、BIN2(对应PA12、PA13、PB14、PB15）
//								//		2.初始化了直流电机的底层PWM
//														/*这一步内容包括了：
//																2.1初始化了TIM计时器的通道输出PWM信号的引脚PA0和PA1（对应TB6612的PWMA、PWMB两个端口）
//																2.2初始化了用于连接TB6612的休眠引脚STBY的PB1，并且设其初值为0，即默认是休眠*/
//	Serial_Printf("PWM initiated successfully!\r\n");	
//	Serial_Printf("Motor initiated successfully!\r\n");

//	
////初始化编码器（包括初始化引脚和配置开启中断）
//	App_Encoder_Init();
//								//这一步内容包括了：
//								//		1.初始化了左右两个编码器要用到的接收TB6612传输过来的A相和B相的引脚
//								//		2.开启了针对左右两个编码器A相的中断
//														/*这一步内容包括了：
//																2.1初始化了TIM计时器的通道输出PWM信号的引脚PA0和PA1（对应TB6612的PWMA、PWMB两个端口）
//																2.2初始化了用于连接TB6612的休眠引脚STBY的PB1，并且设其初值为0，即默认是休眠*/
//	Serial_Printf("Encoder initiated successfully!\r\n");
//	
//	
////初始化AD（用于读取电池电量）
//	AD_Init();
//								//这一步内容包括了：
//								//		1.初始化了AD输出引脚PA6
//								//		2.对单片机的要用的AD通道进行初始化配置
//	Serial_Printf("AD initiated successfully!\r\n");

////初始化初始化平衡车的控制系统（两个系统：速度环系统 和 角度、角速度、电机控速系统 组成的串级系统）
//	app_control_init();
//								//这一步内容包括了：
//								//		1.初始化各个环的PID的值
//								//		2.初始化各个环的PID的输出限幅

///*---------------------------------------------------------------------------------------------------初始化部分*/
//	

//	while(1)
//	{
//		
//		BlueSeiral_Proc();	//进程函数：运行解析蓝牙收到的指令
//		
//		On_Off_Flag_Proc();	//对电机开还是关进行判断并且在打开电机后执行对应后续程序的进程函数
//		
////		if(omega_ref_test_last != omega_ref_test)
////		{
////			omega_ref_test_last = omega_ref_test;
////			omega_ref_test_flag=1;
////			MeanFliter_Clear();
////		}

//	
//		
////		OLED_Update();
//		
//	}
//	
//}

//static uint8_t On_Off_Flag = 0;//电机是否开启的标志位

//static uint32_t count = 0;
//static float omega_test=0;

//static float encoder_left_test;
//static float encoder_right_test;

////#define omega_ref_test 30.0f

//void On_Off_Flag_Proc(void)//对蓝牙串口接收到指令并对标志位做出写入后，对开关标志位判断并执行对应程序的进程函数
//{
////	/*test-----------------------------------------*/
////	if(Key1 == 1)//手机中的Key1被按住（自锁住）
////	{
////		OLED_Clear();
////		OLED_Printf(1,1,OLED_8X16,"HaiFeng");
////	}
////	else if(Key1 == 0)//手机中的Key1被松开（解除自锁）
////	{
////		OLED_Clear();
////		OLED_Printf(1,1,OLED_8X16,"None");
////	}
////	/*-----------------------------------------test*/
//	
////对于自锁按键的理解：
////自锁按键分三个情况：
//	//1.按键未自锁，初始状态下，按键被按下：这就是对应电机没开启，第一次打开电机
//	//2.按键被自锁：这就是对应电机已被开启
//	//3.按键被解除自锁：这就是对应电机被关闭
////因此除了检测按键的状态（按住（即自锁）、松开（即未自锁））以外，应该加一个状态位，来区分前两种情况，这里加的是On_Off_Flag这个标志位

//	
//	if(Key1 == 1 && On_Off_Flag == 0)//手机中的Key1被按住（自锁住），并且On_Off_Flag=0，说明现在是第一次按下，电机还没开，现在要开启电机
//	{
//		Motor_Cmd(1);//打开电机（内容是：通过解除STBY休眠引脚去打开PWM，并且初始化左右两个电机PID的结构体）
//		On_Off_Flag = 1;
//		App_Get_motor_PID_value();//函数：获取左右两个电机的PID结构体里的值（以赋值给全局变量的形式）
//		Serial_Printf("\n\nMotor has been opened successfully!\nKp:%f,Ki:%f,Kd:%f,SP:%f\r\n",motor_l_Kp_test,motor_l_Ki_test,motor_l_Kd_test,motor_l_SP_test);
//		Delay(100);
//	}
//	else if(Key1 == 1 && On_Off_Flag == 1)//手机中的Key1被按住（自锁住），并且On_Off_Flag=1，说明电机已启动，这里去执行电机开启后的程序（加了个On_Off_Flag状态位，避免重复开启电机）
//	{
//		/**************在这里执行电机开启后的程序（进程函数）***************/
//		
////		App_Motor_SetOmega_R(omega_test);//这个是改变电机调速系统的PID的设定值omega
//		App_Motor_SetOmega_L(omega_ref_test);
//		App_Motor_Proc();//（每5ms执行一次）这个程序电机通过PID调速系统进行调速
//		encoder_left_test = App_Encoder_GetSpeed_L();
//		encoder_right_test = App_Encoder_GetSpeed_R();
//		
//		Serial_Printf("[plot,%f,%f,%f,%f]\r\n",ua_l_test,encoder_left_test,omega_ref_test,battery_capacity_test);//回传：实际加载电压ua_l，实际转速，目标转速，实际电池电压,真正加载均值电压ua_l

//		
////		Serial_Printf("%f,%f,%f\r\n",omega_test,omega_l_test,battery_capacity_test);
//		
//		
////		OLED_Clear();
////		OLED_Printf(0,0,OLED_8X16,"Battery:%+2.2fV",battery_capacity_test);
////		OLED_Printf(0,16,OLED_8X16,"Runing!");
//	}
//	else if(Key1 == 0)//手机中的Key1被松开（解除自锁）
//	{
//		Motor_Cmd(0);//关闭电机（内容是：通过解除STBY休眠引脚去关闭PWM）
//		On_Off_Flag = 0;
//	}
//}


//void TIM1_UP_IRQHandler(void)//1ms中断一次
//{
//	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
//	{
//		count++;
//		if(count > 1000)//1000ms归零一次
//		{
//			omega_test += 10;
//			if(omega_test>100)
//			{omega_test=0;}
//			count = 0;
//		}
//		
//		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
//	}
//}



/*最终主函数程序*/

/*本程序设计思路是通过把整个过程“系统化”
整套系统分成 单片机内部 和 物理世界 
其中物理世界部分是自然规律
单片机部分则对应由系统内部设定的运作思路得到程序
由“系统”（大系统下面包括小系统）得到系统框图，根据系统框图可以方便理解程序*/

void On_Off_Flag_Proc(void);

static float omega_ref_test_last = 0;

static uint8_t angle_proc_flag = 0;//计时器内自刷新的是否执行角度环进程函数的标志位
static uint8_t speed_proc_flag = 0;//计时器内自刷新的是否执行速度环进程函数的标志位
static uint8_t turn_proc_flag = 0;//计时器内自刷新的是否执行角度环进程函数的标志位

void Init(void)
{
	/*初始化部分---------------------------------------------------------------------------------------------------*/

//	
//	//初始化MPU6050
//	Delay(200);//等待MPU6050上电
//	MPU6050_Init();
//	Delay(150);//等待MPU6050上电
//								//这一步内容包括了：
//								//		1.初始化了底层的I2C（包括I2C要用到引脚SCL - PB10 || SDA - PB11）
//								//		2.初始化（配置）了MPU6050内部的寄存器配置参数（通过I2C通信）
////	Serial_Printf("MyI2C initialized successfully!\r\n");
////	Serial_Printf("MPU6050 initiated successfully!\r\n");
//	
	
//初始化蓝牙串口
	
	Serial_Init();	
	Serial_Printf("Blue_Serial initiated successfully!\r\n");
	
/*（每个初始化成功都给手机发一个成功接收的指令并且发现在对应的值进行确认）*/
//	
//	Timer_Init();
	
	
////初始化OLED
//	OLED_Init();
//	Serial_Printf("OLED initiated successfully!\r\n");
	

//初始化左右电机
	Motor_Init();
	Delay(5);
								//这一步内容包括了：
								//		1.初始化了左右电机PID调速系统引脚AIN1、AIN2、BIN1、BIN2(对应PA12、PA13、PB14、PB15）
								//		2.初始化了直流电机的底层PWM
														/*这一步内容包括了：
																2.1初始化了TIM计时器的通道输出PWM信号的引脚PA0和PA1（对应TB6612的PWMA、PWMB两个端口）
																2.2初始化了用于连接TB6612的休眠引脚STBY的PB1，并且设其初值为0，即默认是休眠*/
	Serial_Printf("PWM initiated successfully!\r\n");	
	Serial_Printf("Motor initiated successfully!\r\n");

	
//初始化编码器（包括初始化引脚和配置开启中断）
	Encoder_Init();
	Serial_Printf("Encoder initiated successfully!\r\n");
	
//	
////初始化AD（用于读取电池电量）
//	AD_Init();
//								//这一步内容包括了：
//								//		1.初始化了AD输出引脚PA6
//								//		2.对单片机的要用的AD通道进行初始化配置
//	Serial_Printf("AD initiated successfully!\r\n");


//初始化计时器TIM1
	Timer_Init();

//初始化MPU6050
	Delay(100);
	MPU6050_Init();


//初始化显示用的LED灯引脚
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// 1. 开 GPIOA 时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	// 2. 配置 PA5 和 PA4
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_4 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;    // 推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;  // 速度
	
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// 3. 初始化后是静态
	GPIO_WriteBit(GPIOA,GPIO_Pin_5,Bit_SET);
	GPIO_WriteBit(GPIOA,GPIO_Pin_4,Bit_RESET);
	GPIO_WriteBit(GPIOA,GPIO_Pin_3,Bit_RESET);

/*---------------------------------------------------------------------------------------------------初始化部分*/
}

static float angle_test=0;
static float angle_ref=0;
static float test_flag=0;

int main(void)
{
	
	Init();
	
	app_control_init();//不再Init()里面，只初始化这一次，防止自更新的PID值在重新初始化工程的时候被初始值覆盖


	while(1)
	{

		
		BlueSeiral_Proc();	//进程函数：运行解析蓝牙收到的指令

		On_Off_Flag_Proc();	//对电机开还是关进行判断并且在打开电机后执行对应后续程序的进程函数


		
//		/*test*/
//		if(test_flag == 1 && test_open_flag == 1)//每30ms进一次
//		{	
//			test_flag=0;
//			angle_ref = app_get_angle_target();
//			Serial_Printf("[plot,%2.2f,%2.2f]",angle_ref,angle_test);
//		}
		
		
	}
	
}

static uint8_t On_Off_Flag = 0;//电机是否开启的标志位



//#define omega_ref_test 30.0f

void On_Off_Flag_Proc(void)//对蓝牙串口接收到指令并对标志位做出写入后，对开关标志位判断并执行对应程序的进程函数
{
//	/*test-----------------------------------------*/
//	if(Key1 == 1)//手机中的Key1被按住（自锁住）
//	{
//		OLED_Clear();
//		OLED_Printf(1,1,OLED_8X16,"HaiFeng");
//	}
//	else if(Key1 == 0)//手机中的Key1被松开（解除自锁）
//	{
//		OLED_Clear();
//		OLED_Printf(1,1,OLED_8X16,"None");
//	}
//	/*-----------------------------------------test*/
	
//对于自锁按键的理解：
//自锁按键分三个情况：
	//1.按键未自锁，初始状态下，按键被按下：这就是对应电机没开启，第一次打开电机
	//2.按键被自锁：这就是对应电机已被开启
	//3.按键被解除自锁：这就是对应电机被关闭
//因此除了检测按键的状态（按住（即自锁）、松开（即未自锁））以外，应该加一个状态位，来区分前两种情况，这里加的是On_Off_Flag这个标志位

	
//状态一：手机中的Key1被按下（还未自锁，即将就要自锁）（电机还没开，现在要开启电机）
	if(Key1 == 1 && On_Off_Flag == 0)
	{
		Motor_Cmd(1);//打开电机（内容是：通过解除STBY休眠引脚去打开PWM，并且初始化左右两个电机PID的结构体）
		On_Off_Flag = 1;
		
		Serial_Printf("\nMotor has been opened successfully!\n");
	}

	
//状态二：手机中的Key1被按住（自锁住）（电机已启动，这里可以去执行各种进程函数，避免重复开启电机））
	else if(Key1 == 1 && On_Off_Flag == 1)
	{
		/************************************************************************************************/
		/************************************************************************************************/
		/****************************在这里执行电机开启后的程序（进程函数）******************************/
		
		if(out_flag == 0)//小车没倒地，正常运行各个进程函数
		{
			/*转向环执行周期：30ms，第一位执行*/
			if(turn_proc_flag == 1)//到点执行转向环进程函数了
			{
				turn_proc_flag=0;//清除掉进程函数执行标志位先
				
					/*转向环进程函数执行的内容--------------------------*/
					app_turn_proc();//注意这个函数里有关进程函数时间间隔T！！！
					/*--------------------------转向环进程函数执行的内容*/				
			}
			
			
			/*速度环执行周期：25ms，第二位执行*/
			if(speed_proc_flag == 1)//到点执行速度环进程函数了
			{
				speed_proc_flag=0;//清除掉进程函数执行标志位先
				
					/*速度环进程函数执行的内容--------------------------*/
					Get_Speed();
					app_speed_proc();//注意这个函数里有关进程函数时间间隔T！！！		//执行速度环进程函数		
					/*--------------------------速度环进程函数执行的内容*/				
			}
			
			/*角度环执行周期：7ms，第三位执行*/
			if(angle_proc_flag == 1)//到点执行角度环进程函数了
			{
				angle_proc_flag=0;//清除进程函数执行标志位先
			
					/*角度环进程函数执行的内容--------------------------*/
					float Angle = MPU6050_Proc();//注意这个函数里有关进程函数时间间隔T！！！	//通过MPU6050_Proc()进程函数获取角度angle
					app_angle_proc(Angle);
					/*--------------------------角度环进程函数执行的内容*/
			}
			
		}
		else if(out_flag == 1)//小车倒地，改自锁按键标志位从而跳出此状态（相当于自己按下key1按键）
			{
				out_flag=0;
				Key1=0;
			}
		}
	
//状态三：手机中的Key1被松开（解除自锁）	
	else if(Key1 == 0)
	{
		Motor_Cmd(0);//关闭电机（内容是：通过解除STBY休眠引脚去关闭PWM）
		On_Off_Flag = 0;
	}
}


static uint8_t angle_time_count=0;
static uint8_t speed_time_count=0;
static uint8_t turn_time_count=0;
static uint8_t test_time_count=0;
static float test_time_count1=0;
static uint8_t test_time_count2=0;
//计时器TIM1的中断函数（每1ms进一次）
void TIM1_UP_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
	{
		TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
		
		
//角度环的进程标志位计时器变量		
	angle_time_count++;
	if(angle_time_count>=7)//这里每7ms进一次
	{
		angle_time_count=0;
		__disable_irq();
		angle_proc_flag = 1;//到点置角度环进程函数标志位为1，让主循环去执行一次
		__enable_irq();
	}
	
	
//速度环的进程标志位计时器变量		
	speed_time_count++;
	if(speed_time_count>=25)//这里25ms进一次
	{
		speed_time_count=0;
		__disable_irq();
		speed_proc_flag	=	1;//到点置速度环进程函数标志位为1，让主循环去执行一次		
		__enable_irq();

	}
	
//转向环的进程标志位计时器变量			
	turn_time_count++;
	if(turn_time_count>=50)//这里50ms进一次
	{
		turn_time_count=0;
		__disable_irq();
		turn_proc_flag	=	1;//到点置转向环进程函数标志位为1，让主循环去执行一次
		__enable_irq();

	}	
	
	test_time_count++;
	if(test_time_count>=15)
	{
		test_time_count=0;
		test_flag	=	1;
	}
	


	
		/*中断函数退出前，再次检查标志位*/
		if (TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
		{
//			Serial_Printf("\n1\n");
			/*标志位又置1了，说明中断函数执行时间超过了定时时间（1ms）*/
			/*清标志位，避免中断连续触发，导致主函数完全无法执行*/
			TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
		}

	}
}


