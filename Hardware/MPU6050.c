#include "stm32f10x.h"                  // Device header
#include "MyI2C.h"
#include "MPU6050_Reg.h"
#include "Delay.h"
#include "public.h"
#include "MPU6050.h"
#include "math.h"
#include "qmath.h"
#include "MPU6050.h"
#include "BlueSerial.h"

float Alpha;//互补滤波系数，默认为0.01，可自行修改

/*定义机械中值（可蓝牙修改）*******************************************************/
static float mechanical_median = 0.5;  
/*******************************************************定义机械中值（可蓝牙修改）*/

static uint8_t firstCompute = 1;//给进程函数判断是否第一次进进程函数，如果是第一次进，第一次的roll和pitch是直接用的是加速度解算出来的，第二次开始才用融合的，注意，在别的地方重启MPU6050的时候也要让这个为1，我在MPU6050_Init那里写其为1

static int16_t gx_bias = 0, gy_bias = 0, gz_bias = 0;


static int16_t ax_raw,ay_raw,az_raw,gx_raw,gy_raw,gz_raw,temperature_raw;//原始的16位的数据
static float ax=0,ay=0,az=0,gx=0,gy=0,gz=0,temperature=0;
static float yaw=0,pitch=0,roll=0;


#define MPU6050_ADDRESS		0xD0		//MPU6050的I2C从机地址

//
// @函数：gy_raw上电零偏补偿
//	返回值：gy_raw的零偏误差
//

static int16_t gy_raw_bias=-18;//静差默认为-18
//有需要再更新

static int16_t gyro_offset=0;
int16_t gy_raw_adjust(void)
{
	int32_t sum=0;
	for(int i=0;i<500;i++)
	{
		MPU6050_GetRaw();
		sum += gy_raw;
		Delay(1);
	}
	gyro_offset = sum / 500;
		
	return gyro_offset;
}


/**
  * 函    数：MPU6050写寄存器
  * 参    数：RegAddress 寄存器地址，范围：参考MPU6050手册的寄存器描述
  * 参    数：Data 要写入寄存器的数据，范围：0x00~0xFF
  * 返 回 值：无
  */
void MPU6050_WriteReg(uint8_t RegAddress, uint8_t Data)
{
	MyI2C_Start();						//I2C起始
	MyI2C_SendByte(MPU6050_ADDRESS);	//发送从机地址，读写位为0，表示即将写入
	MyI2C_ReceiveAck();					//接收应答
	MyI2C_SendByte(RegAddress);			//发送寄存器地址
	MyI2C_ReceiveAck();					//接收应答
	MyI2C_SendByte(Data);				//发送要写入寄存器的数据
	MyI2C_ReceiveAck();					//接收应答
	MyI2C_Stop();						//I2C终止
}

/**
  * 函    数：MPU6050读寄存器
  * 参    数：RegAddress 寄存器地址，范围：参考MPU6050手册的寄存器描述
  * 返 回 值：读取寄存器的数据，范围：0x00~0xFF
  */
uint8_t MPU6050_ReadReg(uint8_t RegAddress)
{
	uint8_t Data;
	
	MyI2C_Start();						//I2C起始
	MyI2C_SendByte(MPU6050_ADDRESS);	//发送从机地址，读写位为0，表示即将写入
	MyI2C_ReceiveAck();					//接收应答
	MyI2C_SendByte(RegAddress);			//发送寄存器地址
	MyI2C_ReceiveAck();					//接收应答
	
	MyI2C_Start();						//I2C重复起始
	MyI2C_SendByte(MPU6050_ADDRESS | 0x01);	//发送从机地址，读写位为1，表示即将读取
	MyI2C_ReceiveAck();					//接收应答
	Data = MyI2C_ReceiveByte();			//接收指定寄存器的数据
	MyI2C_SendAck(1);					//发送应答，给从机非应答，终止从机的数据输出
	MyI2C_Stop();						//I2C终止
	
	return Data;
}


/**
  * 函    数：MPU6050读多个字节
  * 参    数：RegAddress 寄存器地址（首地址），DataArray 数据暂存数组，Count 读取字节数量
  * 返 回 值：无
  */
void MPU6050_ReadRegs(uint8_t RegAddress,uint8_t *DataArray,uint8_t Count)
{
	uint8_t i;
	
	MyI2C_Start();						//I2C起始
	MyI2C_SendByte(MPU6050_ADDRESS);	//发送从机地址，读写位为0，表示即将写入
	MyI2C_ReceiveAck();					//接收应答
	MyI2C_SendByte(RegAddress);			//发送寄存器地址
	MyI2C_ReceiveAck();					//接收应答
	
	MyI2C_Start();						//I2C重复起始
	MyI2C_SendByte(MPU6050_ADDRESS | 0x01);	//发送从机地址，读写位为1，表示即将读取
	MyI2C_ReceiveAck();					//接收应答
	for(i=0;i<Count;i++)
	{
		DataArray[i] = MyI2C_ReceiveByte();			//接收指定寄存器的数据
		if(i<Count-1)
		{
			MyI2C_SendAck(0);					//发送应答，给从机应答，继续接收从机的数据输出
		}
		else{
			MyI2C_SendAck(1);					//发送应答，给从机非应答，终止从机的数据输出
		}
	}
	
	MyI2C_Stop();						//I2C终止
	
}


/**
  * 函    数：MPU6050获取ID号
  * 参    数：无
  * 返 回 值：MPU6050的ID号
  */
uint8_t MPU6050_GetID(void)
{
	return MPU6050_ReadReg(MPU6050_WHO_AM_I);		//返回WHO_AM_I寄存器的值
}



/**
  * 函    数：MPU6050初始化
  * 参    数：无
  * 返 回 值：无
  */
void MPU6050_Init(void)
{
	MyI2C_Init();									//先初始化底层的I2C
	
	/*MPU6050寄存器初始化，需要对照MPU6050手册的寄存器描述配置，此处仅配置了部分重要的寄存器*/
	MPU6050_WriteReg(MPU6050_PWR_MGMT_1, 0x01);		//电源管理寄存器1，取消睡眠模式，选择时钟源为X轴陀螺仪
	MPU6050_WriteReg(MPU6050_PWR_MGMT_2, 0x00);		//电源管理寄存器2，保持默认值0，所有轴均不待机
	MPU6050_WriteReg(MPU6050_SMPLRT_DIV, 0x07);		//采样率分频寄存器，配置采样率
	MPU6050_WriteReg(MPU6050_CONFIG, 0x03);   //滤波，有滤波就有延迟，这里小滤波
	MPU6050_WriteReg(MPU6050_GYRO_CONFIG, 0x18);
	MPU6050_WriteReg(MPU6050_ACCEL_CONFIG, 0x18);	
	Alpha = 0.01f; 
}


/**
  * 函    数：外部调用的计算静差gy_raw_bias的函数（给蓝牙调用）
  * 参    数：无
  * 返 回 值：无
  */
void get_gy_raw_bias(void)
{
	gy_raw_bias = gy_raw_adjust();//获取gy_raw的零偏误差		
	Serial_Printf("\n gy_raw_bias:%d \r\n",gy_raw_bias);
}


/**
  * 函    数：MPU6050通过读取寄存器的多个字节来获取数据，将原始数据数据写到对应的16位的存储变量X_raw里
  * 参    数：无
  * 返 回 值：无
  */
void MPU6050_GetRaw(void)
{
	uint8_t Data[14];
	
	MPU6050_ReadRegs(MPU6050_ACCEL_XOUT_H,Data,14);
	
	ax_raw = (Data[0] << 8) | Data[1];
	ay_raw = (Data[2] << 8) | Data[3];
	az_raw = (Data[4] << 8) | Data[5];
	
	temperature_raw = (Data[6] << 8) | Data[7];
	
	gx_raw = (Data[8] << 8) | Data[9];
	gy_raw = (Data[10] << 8) | Data[11];
	gz_raw = (Data[12] << 8) | Data[13];
}


//
// @函数：MPU6050的进程函数（通过陀螺仪的角速度去计算欧拉角）
//

uint8_t out_flag=0;//检测到角度过大（小车倒下）直接关掉电机
float AngleACC=0;
float AngleGyro=0;
float Angle=0;

float MPU6050_Proc(void)
{
//	static uint32_t nxt=0;//下次程序运行时间
//	if(GetTick() < nxt) return Angle;
	
	MPU6050_GetRaw(); 
//	Serial_Printf("%d,%d,%d,%d,%d,%d\n",ax_raw,ay_raw,az_raw,gx_raw,gy_raw,gz_raw);
	gy_raw -= gy_raw_bias;
	
	
	// #1.算出pitch要用到的加速度解算的angle和陀螺仪解算的angle
	AngleACC = -qatan2(ax_raw,az_raw) / 3.14159 * 180;//单位°	这个进行取反，极性不一样，因为进行了比值关系，所以不用缩放ax和az
	AngleGyro = Angle + gy_raw / 32768.0f * 2000.0f * 0.007f; //单位°	进行了缩放	注意，最后的×零点几，这个proc函数的间隔是多少（秒）就乘多少
	//特别注意这里 ↑ 是Angle！！！！！！而不是AngleGyro！！！！！！
	
	// #2.进行互补滤波
	//互补滤波系数Alpha会改的
	Angle = Alpha * AngleACC + (1 - Alpha) * AngleGyro;
	
//test:		Serial_Printf("[plot,%d,%f]\n",gy_raw,Angle);
	
//	// #3.检测角度，倒地关电机
	if(Angle >= 85 || Angle <= -85)
	{
		out_flag = 1;
		//关机前发送当前角度值
		Serial_Printf("\n now angle:%f \r\n",Angle);
	}
	
	// #4.打印测试
//	Serial_Printf("[display,0,100,Angle:%3.3f]",Angle);
//	Serial_Printf("[display,0,120,gy_bias:%d]",gy_raw_bias);
//	Serial_Printf("[plot,%f]",Angle);
//	nxt += 5;//改完这个记得改上面的AngleGyro = Angle + gy_raw / 32768.0 * 2000 * 0.00里的最后一个数
//	
//	Serial_Printf("%f\r\n",Angle);
	
	
	return Angle - mechanical_median;//返回最终值Angle
}



//
// @函数：外部调用来修改互补滤波系数Alpha的函数
//
void app_adjust_Alpha(float Alpha_adjust)
{
	Alpha = Alpha_adjust;
}


//
// @函数：外部调用来获取互补滤波系数Alpha的函数
//
float app_get_Alpha(void)
{
	return Alpha;
}


//
// @函数：外部调用来获取gy静差gy_raw_bias的函数
//
float app_get_gy_raw_bias(void)
{
	return gy_raw_bias;
}



//
// @函数：外部调用来修改机械中值mechanical_median的函数
//
float app_adjust_mechanical_median(float value)
{
	mechanical_median = value;
}
















/**
  * 函    数：将16位的原始数据转换成实际结果后保存到float型的实际数据变量中
							ax ay az 加速度计的实际结果（经过计算），单位 g，使用输出参数的形式返回
							gx gy gz 陀螺仪的实际结果（经过计算），单位 °/s，使用输出参数的形式返回
							temperature 温度实际结果（经过计算），单位 摄氏度，使用输出参数的形式返回
  * 返 回 值：无
  */
void MPU6050_GetData(void)
{
	MPU6050_GetRaw();
	
	ax = (ax_raw) * 6.1035e-5f;
	ay = (ay_raw) * 6.1035e-5f;
	az = (az_raw) * 6.1035e-5f;
	
	
	gx = (gx_raw - gx_bias) * 0.0609756097561f;
  gy = (gy_raw - gy_bias) * 0.0609756097561f;
  gz = (gz_raw - gz_bias) * 0.0609756097561f;
	
	temperature = 0;
	
	
}


//
// @函数：获取ax
//
float App_MPU6050_Get_ax(void)
{
	return ax;
}

//
// @函数：获取ay
//
float App_MPU6050_Get_ay(void)
{
	return ay;
}

// @函数：获取az
//
float App_MPU6050_Get_az(void)
{
	return az;
}

//
// @函数：获取gx获取gz（暂不用，若要用，参考gy）
//
float App_MPU6050_Get_gx(void)
{
	return gx;
}

//
// @函数：获取gy
//
float App_MPU6050_Get_gy(void)
{
	//注意：每次都是在执行完 void MPU6050_Proc(void) 读取到roll以后读取这个gy，而在 void MPU6050_Proc(void) 中已经执行了一次 MPU6050_GetData();
//	MPU6050_GetData();
	return gy;
}

// @函数：获取gz（暂不用，若要用，参考gy）
//
float App_MPU6050_Get_gz(void)
{
	return gz;
}

// @函数：获取temperature
//
float App_MPU6050_Get_temperature(void)
{
	MPU6050_GetData();
	return temperature;
}

//
// @函数：获取Yaw偏航角 //单位 °
//
float App_MPU6050_Get_Yaw(void)
{
	return yaw;
}

//
// @函数：获取Pitch俯仰角 //单位 °
//
float App_MPU6050_Get_Pitch(void)
{
	return pitch;
}

//
// @函数：获取Roll翻滚角 //单位 °
//
float App_MPU6050_Get_Roll(void)
{
	return roll;
}




