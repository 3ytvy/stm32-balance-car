#include "stm32f10x.h"                  // Device header
#include "stdio.h"
#include "stdarg.h"
#include "string.h"
#include "math.h"
#include <stdlib.h>
#include "app_control.h"
#include "MPU6050.h"
#include "Delay.h"

uint8_t Key1 = 0;
float omega_ref_test = 0.0f;

uint8_t test_open_flag=0;//extern

uint8_t turn_flag = 0;//extern

uint8_t run_to_calm_cnt=0;//用于计数，运动→静止，累积到一定数量才能触发是静止，防止中途莫名切换

//此程序的接收的逻辑：
//开启USART的中断，每次接收到数据，就产生中断
//先定义一个接收到了数据的标志位变量和接收数据的暂存区变量
//在中段处理函数里，软件置标志位变量为1，并且把接收到的数据存到暂存区变量里
//有一个读取标志位变量的函数，用这个函数读取到标志位变量为1时就置其为0
//系统里开启一个计时器TIM，每隔一段短时间读取标志位变量是否为1，如果读到为1，就把暂存区的数据转到要用的地方去
//其中中断函数里读取数据包的流程是用状态机来写的（还用到了static关键字），看看就能理解


//如果一开始就知道从串口读取到的数据要放到哪里，直接在串口USART的中断里把这个数据放到那里就可以了
//这样避免计时器TIM跟不上读取数据的速度从而会漏掉一些数据


uint8_t Serial_RxFlag;	//接收到了数据的标志位
char Serial_RxPacket[100];	//接收数据的暂存区

uint8_t pid_adjust_flag=0;//某个pid被修改了的标志位

void Serial_Init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

	//PA9是输出
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	//PA10是输入
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; //上拉输入
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStructure);
	
	USART_InitTypeDef USART_InitStructure;
	USART_InitStructure.USART_BaudRate=9600;//波特率
	USART_InitStructure.USART_HardwareFlowControl=USART_HardwareFlowControl_None;//流控
	USART_InitStructure.USART_Mode=USART_Mode_Tx | USART_Mode_Rx;//模式：发送or接收，如果既要又要，可以用 | 给或起来
	USART_InitStructure.USART_Parity=USART_Parity_No;//校验位
	USART_InitStructure.USART_StopBits=USART_StopBits_1;//停止位
	USART_InitStructure.USART_WordLength=USART_WordLength_8b;//数据字长（包括校验位）
	USART_Init(USART1,&USART_InitStructure);
	
	
	
	
	//开启串口USART的中断（每接收到一个数据就产生中断）
	
	USART_ITConfig(USART1,USART_IT_RXNE,ENABLE);//开启RXNE这一位的中断开关
	
	//配置RXNE接下来要走的中断通道
	NVIC_InitTypeDef NVIC_InitStructure;
	NVIC_InitStructure.NVIC_IRQChannel=USART1_IRQn; //RXNE所在的通道是USART1_IRQn
	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority=1;//子优先级
	NVIC_Init(&NVIC_InitStructure);
	
	
	
	USART_Cmd(USART1,ENABLE);
}



//发送一个字节
void Serial_SendByte(uint8_t Byte)
{
	//SendData时，TXE自动清零
	USART_SendData(USART1,Byte);//直接调用USART的库函数，把Byte写入USART的TDR（发送数据寄存器）里面
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);//等待 发送数据寄存器 空
	
}


//发送数组
void Serial_SendArray(uint8_t *Array,uint16_t Length)//注意，这里Array是uint8_t数组类型
{
	uint16_t i;
	for(i=0;i<Length;i++)
	{
		Serial_SendByte(Array[i]);
	}
	
}

//发送字符串
void Serial_SendString(char *String)//注意，这里String是char类型
{
	uint8_t i;
	for(i=0;String[i]!='\0';i++)
	{
		Serial_SendByte(String[i]);
	}
	
	
}


uint32_t Serial_Pow(uint32_t X,uint32_t Y)//这是次方函数，返回值是X的Y次方
{
	uint32_t Result=1;
	while(Y--)//循环Y次
	{
		Result *= X;
		
	}
	return Result;
}

void Serial_SendNumber(uint32_t Number,uint8_t Length)
{
	uint8_t i;
	for(i=0;i<Length;i++)
	{
		Serial_SendByte(Number/Serial_Pow(10,Length - i - 1)%10 + 0x30);//+0x30是ASCLL的偏移
	}
}


//printf重移植
int fputc(int ch,FILE *f)
{
	Serial_SendByte(ch);
	return ch;
}


//sprintf封装函数
void Serial_Printf(char *format,...)
{
	char String[100];
	va_list arg;
	va_start(arg,format);
	vsprintf(String,format,arg);
	va_end(arg);
	Serial_SendString(String);

}

uint8_t Serial_GetRxFlag(void)
{
	if(Serial_RxFlag==1)
	{
		Serial_RxFlag=0;
		return 1;
	}
	return 0;
}




//数据包格式：
//		[数据]


void USART1_IRQHandler(void)
{
	if(USART_GetITStatus(USART1,USART_IT_RXNE)==SET)
	{
		static uint8_t RxState=0;//状态机标志 静态变量RxState
		static uint8_t pRxPacket;//这个是已接收数据的数量
		
		uint8_t RxData=USART_ReceiveData(USART1);
		
		//根据状态机逻辑来写：
		if(RxState == 0)
		{
			if(RxData=='[' && Serial_RxFlag==0)//状态1：等待包头	Serial_RxFlag=0说明上一个流程全部走完
			{
				RxState=1;
				pRxPacket=0;
			}
		}
		else if(RxState == 1)//状态2：接收数据 | 等待包尾
		{
			if(RxData==']')//判断是否为包尾
			{
				RxState=0;
				Serial_RxPacket[pRxPacket]='\0';//给字符串最后一位写结束字符\0
				Serial_RxFlag=1;//主程序一直在查询 if(Serial_RxFlag	== 1)，这里流程结束了，给RxFlag置1，让主程序检测		
			}
			else//如果不是包尾，持续将读取到的数据放到数据暂存区Serial_RxPacket[]里
			{
				Serial_RxPacket[pRxPacket]=RxData;
				pRxPacket ++;				
			}
		}

		
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);//清除中断标志位
	}
}


/*蓝牙串口模块的进程函数*/
void BlueSeiral_Proc(void)
{
	
		
		
/*注意：在每个if(Serial_RxFlag	== 1)
							{
								......
							}
				的最后一定要补上 Serial_RxFlag=0;		//处理完成后，需要将接收数据包标志位清零，否则将无法接收后续数据包
*/	
		
		
//测试1：捕获字符串--------------------------------------------------------------------------------------------------//				
		
//		//（记住手机端蓝牙发送的内容文本是格式是 [文本]  ，不要忘记中括号，因为在串口是按这种格式捕获数据包的）
//		
//		if(Serial_RxFlag	== 1)
//		{
//			OLED_ShowString(4,1,"                ");
//			OLED_ShowString(4,1,Serial_RxPacket);
//			Serial_Printf("%s\r\n",Serial_RxPacket);
//			
//			Serial_RxFlag=0;		//处理完成后，需要将接收数据包标志位清零，否则将无法接收后续数据包
//		}
		
//--------------------------------------------------------------------------------------------------测试1：捕获字符串//		
		
		
		
		
		
		
//测试2：捕获字符串，通过得到的字符串判断是否为指令，若为指令则执行指令---------------------------------------------//
		
//	if(Serial_RxFlag	== 1)
//	{
//		OLED_Printf(4,1,OLED_8X16,"                ");//先擦除第四行
//		OLED_Printf(4,1,OLED_8X16,Serial_RxPacket);
//		if(strcmp(Serial_RxPacket,"LED_ON")==0)//比较两个字符串是否一样
//		{
//			LED_ON();
//			Serial_SendString("LED_ON_OK\r\n");
//			OLED_Printf(2,1,OLED_8X16,"                ");//先擦除第四行
//			OLED_Printf(2,1,OLED_8X16,"LED_ON_OK\n");
//		}
//		else if(strcmp(Serial_RxPacket,"LED_OFF")==0)
//		{
//			LED_OFF();
//			Serial_SendString("LED_OFF_OK\r\n");
//			OLED_Printf(2,1,OLED_8X16,"                ");//先擦除第四行
//			OLED_Printf(2,1,OLED_8X16,"LED_OFF_OK\n");
//		}
//		else
//		{
//			Serial_SendString("ERROR_COMMAND\r\n");
//			OLED_Printf(2,1,OLED_8X16,"                ");//先擦除第四行
//			OLED_Printf(2,1,OLED_8X16,"ERROR_COMMAND\r\n");
//		}

//			Serial_RxFlag=0;	//处理完成后，需要将接收数据包标志位清零，否则将无法接收后续数据包
//		}				
	
//---------------------------------------------测试2：捕获字符串，通过得到的字符串判断是否为指令，若为指令则执行指令//



		
//测试2：判断切割字符串---------------------------------------------------------------------------------------------//		
		
		//蓝牙小程序里，按键、滑杆、摇杆输出的数据包的内容是固定的
		//通过判断数据包的第一个子串是key、slider还是，来判断是哪个发来的数据包
		//然后再进一步解析		
		if(Serial_RxFlag	== 1)
		{
			char *substr1=strtok(Serial_RxPacket,",");//对接收到的字符串进行对逗号,“的切割，这个是第一个子串
//			char *substr2=strtok(NULL,",");//对接收到的字符串进行对逗号,“的切割，这个是第二个子串
//			char *substr3=strtok(NULL,",");//对接收到的字符串进行对逗号,“的切割，这个是第三个子串
			//注意第一次要第一个参数要填接收到字符串是哪个，后面就用NULL就行了，否则会认为是这个字符串的第一个子串
			//本来是按上面注释掉的那两句写下去，把第二、三个子串赋进去的
			//但是因为比如按键和滑动杆接收的数据是三个数据两个逗号，所以用两个
			//而摇杆有四个数据三个逗号
			//所以后面的子串在先判断完是哪个发来的以后再写入变量
			//先定义指针变量
			char *substr2;//这个是第二个子串
			char *substr3;//这个是第三个子串
			
			//用指针的原因，下面用到了“strcmp(substr2,"1") == 0”
			//这里strcmp的参数要用一个字符（注意不是字符串）的指针
			//所以用字符的指针
			//而摇杆的数据是直接接收，不做strcmp的判断，所以不用指针
			
					
//按键			
			if(strcmp(substr1,"key") == 0)//如果第一个子串是"key"，如果是，就是按键输出的数据包
			{
				//因为是按键，所以还有两个数据
				substr2 = strtok(NULL,",");	//对接收到的字符串进行对逗号,“的切割，这个是第二个子串
				substr3 = strtok(NULL,",");	//对接收到的字符串进行对逗号,“的切割，这个是第三个子串
				
				if(strcmp(substr2,"1") == 0 && strcmp(substr3,"down") == 0)
				{
					Serial_Printf("Key,1,down\r\n");
					//***在这里执行按键1按下要执行的指令***
					Key1 = 1;
					app_angle_reset();//清零平衡车系统各pid的暂存值
					//读取三环PID值
//					App_Get_motor_PID_value();
//					App_Get_tehta_PID_value();
//					App_Get_theta_dot_PID_value();
				}
				
				
				if(strcmp(substr2,"1") == 0 && strcmp(substr3,"up") == 0)
				{
					Serial_Printf("Key,1,up\r\n");
					//***在这里执行按键1松开要执行的指令***
					Key1 = 0;
					app_angle_reset();//清零平衡车系统各pid的暂存值
					float err_k=app_get_angle_err_k();
					float err_k_1=app_get_angle_err_k_1();
					float err_int=app_get_angle_err_int();
					float target = app_get_angle_target();
					float out = app_get_angle_out();
					float actual = app_get_angle_actual();
					Serial_Printf("\nerr_k=%2.2f,err_k_1=%2.2f,err_int=%2.2f\ntarget=%2.2f,actual=%2.2f,actual=%2.2f",err_k,err_k_1,err_int,target,out,actual);
					
				}
				else if(strcmp(substr2,"2") == 0 && strcmp(substr3,"down") == 0)
				{
					Serial_Printf("Key,2,down\r\n");
					Delay(200);
					get_gy_raw_bias();
					//***在这里执行按键2按下要执行的指令***
				}
				
				else if(strcmp(substr2,"dqangle") == 0 && strcmp(substr3,"down") == 0)
				{
					float p = app_get_angle_p();
					float i = app_get_angle_i();
					float d = app_get_angle_d();
					float target = app_get_angle_target();
					float Alpha = app_get_Alpha();
					float gy_raw_bias = app_get_gy_raw_bias();
					float outoffset = app_get_outoffset();

					Serial_Printf("\nangle:\nkp:%2.2f ki:%2.2f kd:%2.2f\r\n\n",p,i,d);
					Serial_Printf("\ntarget:%2.2f  gy_raw:%2.2f  Alpha:%2.2f\nout_offset:%2.2f\n",target,gy_raw_bias,Alpha,outoffset);
					//***在这里执行按键 测试前查看角度环数据 按下要执行的指令***
				}
				else if(strcmp(substr2,"dqspeed") == 0 && strcmp(substr3,"down") == 0)
				{
					float p = app_get_speed_p();
					float i = app_get_speed_i();
					float d = app_get_speed_d();
					float target = app_get_speed_target();

					Serial_Printf("\nspeed:\nkp:%2.2f ki:%2.2f kd:%2.2f",p,i,d);
					//***在这里执行按键 测试前查看速度环数据 按下要执行的指令***
				}
				
					else if(strcmp(substr2,"dqturn") == 0 && strcmp(substr3,"up") == 0)
				{
					float p = app_get_turn_p();
					float i = app_get_turn_i();
					float d = app_get_turn_d();
					float target = app_get_turn_target();

					Serial_Printf("\nturn:\nkp:%2.2f ki:%2.2f kd:%2.2f\n",p,i,d);
					//***在这里执行按键 测试前查看速度环数据 按下要执行的指令***
				}
				//按键 开启速度环输出给角度环当输入 是自锁按键
//				else if(strcmp(substr2,"openspeed") == 0 && strcmp(substr3,"down") == 0)
//				{
//					if_speed_for_angle_flag=1;
//					//***在这里执行按键 开启速度环输出给角度环当输入 按下要执行的指令***
//				}
//				else if(strcmp(substr2,"openspeed") == 0 && strcmp(substr3,"up") == 0)
//				{
//					if_speed_for_angle_flag=0;
//					//***在这里执行按键 开启速度环输出给角度环当输入 松开要执行的指令***
//				}

				else if(strcmp(substr2,"openangletest") == 0 && strcmp(substr3,"up") == 0)
				{
					if(test_open_flag == 0){test_open_flag=1;}else if(test_open_flag == 1){test_open_flag=0;}
					//***在这里执行按键 开启角度环测试 松开要执行的指令***
				}
			}
			
			
//滑杆						
			else if(strcmp(substr1,"slider") == 0)//如果第一个子串是"slider"，如果是，就是滑杆输出的数据包
			{
				//因为接收到滑杆的数据是字符串，而非整数
				//所以要#include <stdlib.h>
				//然后用atoi(字符串)得到整数,atof(字符串)得到浮点数
				
				//下面滑杆1接收到，用字符串打印
				//滑杆2接收到，先把字符串转为浮点数，存放到一个浮点数型变量，然后打印这个浮点数型变量
				//滑杆3接收到，先把字符串转为整数，存放到一个整数型变量，然后打印这个整数型变量
				
				//因为是滑杆，所以还有两个数据
				substr2 = strtok(NULL,",");	//对接收到的字符串进行对逗号,“的切割，这个是第二个子串
				substr3 = strtok(NULL,",");	//对接收到的字符串进行对逗号,“的切割，这个是第三个子串
				
				if(strcmp(substr2,"AngleKp") == 0)
				{
					float Kp = atof(substr3);
					app_adjust_Angle_Kp_pid(Kp);
					//***在这里执行接收到滑杆AngleKp数据要执行的指令***
				}
				else if(strcmp(substr2,"AngleKi") == 0)
				{
					float Ki = atof(substr3);
					app_adjust_Angle_Ki_pid(Ki);
					//***在这里执行接收到滑杆AngleKi数据要执行的指令***
				}
				else if(strcmp(substr2,"AngleKd") == 0)
				{
					float Kd = atof(substr3);
					app_adjust_Angle_Kd_pid(Kd);
					//***在这里执行接收到滑杆AngleKd数据要执行的指令***
				}
				else if(strcmp(substr2,"AngleTarget") == 0)
				{
					float Target = atof(substr3);
					app_adjust_angle_target(Target);
					//***在这里执行接收到滑杆AngleTarget数据要执行的指令***
				}
				else if(strcmp(substr2,"AdjustAlpha") == 0)
				{
					float Alpha_adjust = atof(substr3);
					app_adjust_Alpha(Alpha_adjust);
					//***在这里执行接收到滑杆AdjustAlpha数据要执行的指令***
				}
				else if(strcmp(substr2,"SpeedKp") == 0)
				{
					float Kp = atof(substr3);
					app_adjust_speed_Kp_pid(Kp);
					//***在这里执行接收到滑杆SpeedKp数据要执行的指令***
				}
				else if(strcmp(substr2,"SpeedKi") == 0)
				{
					float Ki = atof(substr3);
					app_adjust_speed_Ki_pid(Ki);
					//***在这里执行接收到滑杆SpeedKi数据要执行的指令***
				}
				else if(strcmp(substr2,"SpeedKd") == 0)
				{
					float Kd = atof(substr3);
					app_adjust_speed_Kd_pid(Kd);
					//***在这里执行接收到滑杆SpeedKd数据要执行的指令***
				}
				else if(strcmp(substr2,"mechanical") == 0)
				{
					float value = atof(substr3);
					app_adjust_mechanical_median(value);
					//***在这里执行接收到滑杆mechanical数据要执行的指令***
				}
				else if(strcmp(substr2,"TurnKp") == 0)
				{
					float Kp = atof(substr3);
					app_adjust_turn_Kp_pid(Kp);
					//***在这里执行接收到滑杆TurnKp数据要执行的指令***
				}
				else if(strcmp(substr2,"TurnKi") == 0)
				{
					float Ki = atof(substr3);
					app_adjust_turn_Ki_pid(Ki);
					//***在这里执行接收到滑杆TurnKi数据要执行的指令***
				}
				else if(strcmp(substr2,"TurnKd") == 0)
				{
					float Kd = atof(substr3);
					app_adjust_turn_Kd_pid(Kd);
					//***在这里执行接收到滑杆TurnKd数据要执行的指令***
				}
				else if(strcmp(substr2,"adjustout") == 0)
				{
					float outoffset = atof(substr3);
					app_adjust_outoffset(outoffset);
					//***在这里执行接收到滑杆adujstout数据要执行的指令***
				}							
			}

//摇杆	
			else if(strcmp(substr1,"joystick") == 0)//如果第一个子串是"joystick"，如果是，就是摇杆输出的数据包
			{
				int8_t LH = atoi(strtok(NULL,","));//第二个子串，是 左摇杆 横向值 ，转成整数型后赋给变量 LH
				int8_t LV = atoi(strtok(NULL,","));//第三个子串，是 左摇杆 纵向值 ，转成整数型后赋给变量 LV
				int8_t RH = atoi(strtok(NULL,","));//第四个子串，是 右摇杆 横向值 ，转成整数型后赋给变量 RH
				int8_t RV = atoi(strtok(NULL,","));//第五个子串，是 右摇杆 纵向值 ，转成整数型后赋给变量 RV
				
				/*
				RV：直线速度
				LH：左右转向
				*/
				
//				if(RV==0)//目标值为0
//				{
//					if(run_to_calm_cnt!=2)
//					{
//						run_to_calm_cnt++;
//					}
//					else
//					{
//						run_to_calm_cnt=0;
//						app_adjust_speed_target(0);//速度环目标值：右摇杆 纵向值
//					}
//				}
//				else
//				{
//////////					app_adjust_speed_target((float)RV/10.0);//速度环目标值：右摇杆 纵向值
//				}
				
				if(RV != 0)
				{
					app_adjust_speed_target((float)RV/10.0);//速度环目标值：右摇杆 纵向值
					app_adjust_speed_speed_target((float)RV/10.0);//速度环目标值：右摇杆 纵向值	
				}
				else
				{
					app_adjust_speed_speed_target((float)RV/10.0);//速度环目标值：右摇杆 纵向值		
				}
				
				
					
				if(LH == 0)//转向环目的是0
				{
					turn_flag=1;
					GPIO_WriteBit(GPIOA,GPIO_Pin_3,Bit_RESET);
				}
				else
				{
					turn_flag=0;
					GPIO_WriteBit(GPIOA,GPIO_Pin_3,Bit_SET);
					app_adjust_difpwm((float)LH); //转向环目标值：左摇杆 横向值					
				}
				
				
			}
			
			
			
		
		
////调节三环PID
//			//电机控速环
//			else if(strcmp(substr1,"m") == 0)//如果第一个子串是"m"，如果是，就是调节电机的PID的数据包
//		{
//				float motor_Kp = atof(strtok(NULL,","));//第二个子串，是Kp
//				float motor_Ki = atof(strtok(NULL,","));//第三个子串，是Ki
//				float motor_Kd = atof(strtok(NULL,","));//第四个子串，是Kd
//				app_adjust_pid(motor_Kp,motor_Ki,motor_Kd);	
//				App_Get_motor_PID_value();
//			
//		}
//		
//			//角度环
//			else if(strcmp(substr1,"t") == 0)//如果第一个子串是"t"，如果是，就是调节电机的PID的数据包
//		{
//				float theta_Kp = atof(strtok(NULL,","));//第二个子串，是Kp
//				float theta_Ki = atof(strtok(NULL,","));//第三个子串，是Ki
//				float theta_Kd = atof(strtok(NULL,","));//第四个子串，是Kd
//				float theta_SP = atof(strtok(NULL,","));//第五个子串，是角度环的SP
//				app_adjust_theta_pid(theta_Kp,theta_Ki,theta_Kd,theta_SP);	
//				App_Get_tehta_PID_value();
//			
//				app_control_reset();//复位一下控制系统
//			
//				pid_adjust_flag=1;//pid被修改标志位 置一
//		}
//		
//			//角速度环
//			else if(strcmp(substr1,"td") == 0)//如果第一个子串是"theta"，如果是，就是调节电机的PID的数据包
//		{
//				pid_adjust_flag=1;//pid被修改标志位 置一
//				float theta_dot_Kp = atof(strtok(NULL,","));//第二个子串，是Kp
//				float theta_dot_Ki = atof(strtok(NULL,","));//第三个子串，是Ki
//				float theta_dot_Kd = atof(strtok(NULL,","));//第四个子串，是Kd
//				app_adjust_theta_dot_pid(theta_dot_Kp,theta_dot_Ki,theta_dot_Kd);	
//				App_Get_theta_dot_PID_value();
//				//复位一下控制系统
//				app_control_reset();
//				motor_reset();
//		}
//		
////读取三环PID
//			else if(strcmp(substr1,"dqangle") == 0)
//			{
////				App_Get_motor_PID_value();
////				App_Get_tehta_PID_value();
////				App_Get_theta_dot_PID_value();
//				app_show_angle_pid();
//			}
			
			Serial_RxFlag=0;	//处理完成后，需要将接收数据包标志位清零，否则将无法接收后续数据包
		}
}		

//---------------------------------------------------------------------------------------------测试2：判断切割字符串//				
		
		
		
		

