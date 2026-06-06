#include "stm32f10x.h"                  // Device header
#include "AD.h"

#define BATTERY_FULL  12.6f	//定义满电电压
#define BATTERY_EMPTY 9.0f	//定义无电电压

#define FILTER_SAMPLE_COUNT 5000	//滤波采样次数

#define ADC_MAX 4095
#define REF_VOLTAGE 3300 // 3.3V 放大1000倍为整数
#define ADC_DIV_RATIO_INT 11 // 根据原理图电路可知，分压比11（整数）

uint16_t ADValue;//存放读取到的AD值的变量
float VBAT;//存放读取到的实际电池电压的变量

//函数：获取电池当前电压
float Get_vbat(void)
{
//	// #1.初始化AD，在主函数里已进行了一次初始化，不要重复初始化
//	//	AD_Init();
	
	// #2.获取当前AD的值并进行滤波
	int sum=0;//在这里（函数里面）定义变量，每次重新执行这个函数时候，这个变量又归零
	for(int i=0;i<FILTER_SAMPLE_COUNT;i++)//获取FILTER_SAMPLE_COUNT次AD值
	{
		ADValue = AD_GetValue();
		sum+=ADValue;
	}
	ADValue = sum / FILTER_SAMPLE_COUNT; //进行均值滤波
	
	// #3.将滤波后的AD值转为电压
	uint32_t vbat_mv= (uint32_t)ADValue * REF_VOLTAGE * ADC_DIV_RATIO_INT / ADC_MAX;
	VBAT=(float)vbat_mv / 1000.0f;	//实际电池电压的变量
	
	// #4.返回电压值
	return VBAT;
}


//定义电池满电是12.6V，无电是9.0V
int Show_Battery(void)
{
	int Battery_Capacity = 0;//电池电量的变量
	int sum=0;//在这里（函数里面）定义变量，每次重新执行这个函数时候，这个变量又归零
	for(int i=0;i<FILTER_SAMPLE_COUNT;i++)//获取FILTER_SAMPLE_COUNT次AD值
	{
		ADValue = AD_GetValue();
		sum+=ADValue;
	}
	ADValue = sum / FILTER_SAMPLE_COUNT; //进行均值滤波
	uint32_t vbat_mv= (uint32_t)ADValue * REF_VOLTAGE * ADC_DIV_RATIO_INT / ADC_MAX;
	VBAT=(float)vbat_mv / 1000.0f;	//实际电池电压的变量
	if(VBAT<=9)
	{
		Battery_Capacity = 0;
	}
	else
	{
		Battery_Capacity = (int)(100.0f * ((VBAT - 9.0f) / 3.6f)); //电量计算公式（百分比）
//		if(Battery_Capacity > 100) Battery_Capacity = 100;
//    if(Battery_Capacity < 0)   Battery_Capacity = 0;
	}
	
	return Battery_Capacity;
}
